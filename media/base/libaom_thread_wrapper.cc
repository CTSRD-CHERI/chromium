// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "media/base/codec_worker_impl.h"
#include "media/base/libvpx_thread_wrapper.h"
#if !BUILDFLAG(IS_BSD)
#include "third_party/libaom/source/libaom/aom_util/aom_thread.h"
#endif

namespace media {

void InitLibAomThreadWrapper() {
#if !BUILDFLAG(IS_BSD)
  const AVxWorkerInterface interface =
      CodecWorkerImpl<AVxWorkerInterface, AVxWorkerImpl, AVxWorker,
                      AVxWorkerStatus, AVX_WORKER_STATUS_NOT_OK,
                      AVX_WORKER_STATUS_OK,
                      AVX_WORKER_STATUS_WORKING>::GetCodecWorkerInterface();
  CHECK(aom_set_worker_interface(&interface));
#endif
}

}  // namespace media
