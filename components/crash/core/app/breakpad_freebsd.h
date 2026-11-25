// Copyright 2026 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_CRASH_CORE_APP_BREAKPAD_FREEBSD_H_
#define COMPONENTS_CRASH_CORE_APP_BREAKPAD_FREEBSD_H_

namespace crash_reporter {

// Enables local crash dumps when CHROMIUM_FREEBSD_MINIDUMP_DIR names an
// existing directory. Requires Chromium's POSIX stack-dump handler.
void InitializeFreeBSDMinidumps();

}  // namespace crash_reporter

#endif  // COMPONENTS_CRASH_CORE_APP_BREAKPAD_FREEBSD_H_
