#ifndef V8_PLATFORM_CHERI_H_
#define V8_PLATFORM_CHERI_H_

namespace v8 {
namespace base {
extern thread_local bool t_cheri_madvise;

bool CheriShouldMadvise();

struct CheriMadviseScope {
  CheriMadviseScope(bool condition);
  ~CheriMadviseScope();

  CheriMadviseScope() = delete;
  CheriMadviseScope(const CheriMadviseScope& other) = delete;
  CheriMadviseScope& operator=(const CheriMadviseScope& other) = delete;
};
}  // namespace base
}  // namespace v8

#endif  // V8_PLATFORM_CHERI_H_
