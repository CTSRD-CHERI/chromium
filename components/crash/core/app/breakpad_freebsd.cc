// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/crash/core/app/breakpad_freebsd.h"

#if defined(__CHERI_PURE_CAPABILITY__) && defined(__aarch64__)

#include <fcntl.h>
#include <pthread_np.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>

#include "base/compiler_specific.h"
#include "base/debug/stack_trace.h"
#include "base/files/scoped_file.h"
#include "base/posix/eintr_wrapper.h"
#include "google_breakpad/common/minidump_format.h"

namespace crash_reporter {
namespace {

// The normal ARM64 context records integer addresses. Preserve the original
// Morello register encodings and their out-of-band tags in a private stream.
constexpr uint32_t kMorelloStream = 0x43484552;
struct MorelloContext {
  uint32_t version;
  uint32_t register_count;
  uint64_t tags;
  std::array<std::array<unsigned char, 16>, 34> registers;
};

struct SingleThreadList {
  uint32_t count;
  MDRawThread thread;
};

struct Minidump {
  MDRawHeader header;
  std::array<MDRawDirectory, 4> directory;
  MDRawSystemInfo system;
  SingleThreadList threads;
  MDRawExceptionStream exception;
  MDRawContextARM64 context;
  MorelloContext morello;
};

int dump_directory = -1;
std::array<char, 48> dump_filename;
std::atomic_flag dump_started;

bool WriteAll(int fd, const void* data, size_t size) {
  const auto* bytes = static_cast<const char*>(data);
  while (size != 0) {
    const ssize_t written = HANDLE_EINTR(write(fd, bytes, size));
    if (written <= 0) {
      return false;
    }
    bytes = UNSAFE_BUFFERS(bytes + written);
    size -= static_cast<size_t>(written);
  }
  return true;
}

bool CaptureMinidump(int signal, siginfo_t* info, void* context) {
  if (!context || dump_started.test_and_set(std::memory_order_relaxed)) {
    return false;
  }

  const auto& mc = static_cast<const ucontext_t*>(context)->uc_mcontext;
  Minidump dump{};
  dump.header.signature = MD_HEADER_SIGNATURE;
  dump.header.version = MD_HEADER_VERSION;
  dump.header.stream_count = dump.directory.size();
  dump.header.stream_directory_rva = offsetof(Minidump, directory);
  dump.directory = {{
      {MD_SYSTEM_INFO_STREAM, {sizeof(dump.system), offsetof(Minidump, system)}},
      {MD_THREAD_LIST_STREAM,
       {sizeof(dump.threads), offsetof(Minidump, threads)}},
      {MD_EXCEPTION_STREAM,
       {sizeof(dump.exception), offsetof(Minidump, exception)}},
      {kMorelloStream, {sizeof(dump.morello), offsetof(Minidump, morello)}},
  }};
  dump.system.platform_id = MD_OS_UNIX;
  dump.system.processor_architecture = MD_CPU_ARCHITECTURE_ARM64;
  dump.system.number_of_processors = 1;
  dump.threads.count = 1;
  dump.threads.thread.thread_id = pthread_getthreadid_np();
  dump.threads.thread.thread_context = {sizeof(dump.context),
                                        offsetof(Minidump, context)};
  dump.exception.thread_id = dump.threads.thread.thread_id;
  dump.exception.thread_context = dump.threads.thread.thread_context;
  dump.exception.exception_record.exception_code = signal;
  dump.exception.exception_record.number_parameters = 1;
  dump.exception.exception_record.exception_information[0] = info->si_code;
  if (info->si_code > SI_NOINFO && info->si_code < SI_USER) {
    dump.exception.exception_record.exception_address =
        reinterpret_cast<uintptr_t>(info->si_addr);
    if (signal == SIGPROT) {
      dump.exception.exception_record.number_parameters = 2;
      dump.exception.exception_record.exception_information[1] = info->si_capreg;
    }
  }

  dump.context.context_flags =
      MD_CONTEXT_ARM64_CONTROL | MD_CONTEXT_ARM64_INTEGER;
  dump.context.cpsr = mc.mc_spsr;
  dump.morello.version = 1;
  // Native purecap signal frames always contain the inline register block.
  size_t index = 0;
  auto save_register = [&](const __uintcap_t& value) {
    static_assert(sizeof(value) == 16);
    std::memcpy(dump.morello.registers[index].data(), &value, sizeof(value));
    if (__builtin_cheri_tag_get(value)) {
      dump.morello.tags |= uint64_t{1} << index;
    }
    if (index < MD_CONTEXT_ARM64_GPR_COUNT) {
      UNSAFE_BUFFERS(dump.context.iregs[index]) = static_cast<uint64_t>(value);
    }
    ++index;
  };
  for (const auto& reg : mc.mc_capregs.cap_x) {
    save_register(reg);
  }
  save_register(mc.mc_capregs.cap_lr);
  save_register(mc.mc_capregs.cap_sp);
  save_register(mc.mc_capregs.cap_elr);
  save_register(mc.mc_capregs.cap_ddc);
  dump.morello.register_count = index;

  const int fd = openat(dump_directory, dump_filename.data(),
                        O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
  if (fd >= 0) {
    const auto stack = mc.mc_capregs.cap_sp;
    const size_t offset = __builtin_cheri_offset_get(stack);
    const size_t length = __builtin_cheri_length_get(stack);
    if (__builtin_cheri_tag_get(stack) && offset < length) {
      // Copy up to one page from the saved stack capability. The kernel
      // validates the user buffer, so an inaccessible stack still leaves a
      // dump with register context.
      const size_t size = std::min(size_t{4096}, length - offset);
      const ssize_t written = HANDLE_EINTR(
          pwrite(fd, reinterpret_cast<const void*>(stack), size, sizeof(dump)));
      if (written > 0) {
        dump.threads.thread.stack.start_of_memory_range =
            static_cast<uint64_t>(stack);
        dump.threads.thread.stack.memory = {static_cast<uint32_t>(written),
                                            sizeof(dump)};
      }
    }
    const bool complete = WriteAll(fd, &dump, sizeof(dump));
    close(fd);
    if (!complete) {
      unlinkat(dump_directory, dump_filename.data(), 0);
    }
  }
  // Let the stack-dump handler print and re-raise the original signal.
  return false;
}

}  // namespace

void InitializeFreeBSDMinidumps() {
  const char* path = std::getenv("CHROMIUM_FREEBSD_MINIDUMP_DIR");
  if (!path || !*path) {
    return;
  }
  base::ScopedFD directory(open(path, O_RDONLY | O_DIRECTORY | O_CLOEXEC));
  if (!directory.is_valid()) {
    return;
  }
  const std::string filename = "chromium-" + std::to_string(getpid()) + ".dmp";
  if (filename.size() >= dump_filename.size()) {
    return;
  }
  filename.copy(dump_filename.data(), filename.size());
  dump_directory = directory.release();
  base::debug::SetStackDumpFirstChanceCallback(&CaptureMinidump);
}

}  // namespace crash_reporter

#endif  // defined(__CHERI_PURE_CAPABILITY__) && defined(__aarch64__)
