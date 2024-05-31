#!/usr/bin/env python3
#
# Copyright 2019 The Chromium Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Creates config files for building dav1d."""

from __future__ import print_function

import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile

BASE_DIR = os.path.abspath(os.path.dirname(__file__))
CHROMIUM_ROOT_DIR = os.path.abspath(os.path.join(BASE_DIR, '..', '..'))

sys.path.append(os.path.join(CHROMIUM_ROOT_DIR, 'build'))
import gn_helpers

MESON = ['meson']

DEFAULT_BUILD_ARGS = [
    '-Denable_tools=false', '-Denable_tests=false', '-Ddefault_library=static',
    '--buildtype', 'release'
]

WINDOWS_BUILD_ARGS = ['-Dc_winlibs=']


def PrintAndCheckCall(argv, *args, **kwargs):
    print('\n-------------------------------------------------\nRunning %s' %
          ' '.join(argv))
    c = subprocess.check_call(argv, *args, **kwargs)


def RewriteFile(path, search_replace):
    with open(path) as f:
        contents = f.read()
    with open(path, 'w') as f:
        for search, replace in search_replace:
            contents = re.sub(search, replace, contents)

        # Cleanup trailing newlines.
        f.write(contents.strip() + '\n')


def CopyConfigs(src_dir, dest_dir):
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)

    shutil.copy(os.path.join(src_dir, 'config.h'), dest_dir)

    # The .asm file will not be present for all configurations.
    asm_file = os.path.join(src_dir, 'config.asm')
    if os.path.exists(asm_file):
        shutil.copy(asm_file, dest_dir)


def GenerateConfig(config_dir, env, special_args=[]):
    temp_dir = tempfile.mkdtemp()
    PrintAndCheckCall(MESON + DEFAULT_BUILD_ARGS + special_args + [temp_dir],
                      cwd='libdav1d',
                      env=env)

    RewriteFile(
        os.path.join(temp_dir, 'config.h'),
        [
            # We don't want non-visible log strings polluting the official binary.
            (r'(#define CONFIG_LOG .*)',
             r'// \1 -- Logging is controlled by Chromium'),

            # The Chromium build system already defines this.
            (r'(#define _WIN32_WINNT .*)',
             r'// \1 -- Windows version is controlled by Chromium'),

            # Android doesn't have pthread_{get,set}affinity_np.
            (r'(#define HAVE_PTHREAD_(GET|SET)AFFINITY_NP \d{1,2})',
             r'// \1 -- Controlled by Chomium'),
        ])

    config_asm_path = os.path.join(temp_dir, 'config.asm')
    if (os.path.exists(config_asm_path)):
        RewriteFile(config_asm_path,
                    # Clang LTO doesn't respect stack alignment, so we must use
                    # the platform's default stack alignment;
                    # https://crbug.com/928743.
                    [(r'(%define STACK_ALIGNMENT \d{1,2})',
                      r'; \1 -- Stack alignment is controlled by Chromium')])

    CopyConfigs(temp_dir, config_dir)

    shutil.rmtree(temp_dir)


def CopyVersions(src_dir, dest_dir):
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)

    shutil.copy(os.path.join(src_dir, 'include', 'dav1d', 'version.h'),
                dest_dir)
    shutil.copy(os.path.join(src_dir, 'include', 'vcs_version.h'), dest_dir)


def GenerateVersion(version_dir, env):
    temp_dir = tempfile.mkdtemp()
    PrintAndCheckCall(MESON + DEFAULT_BUILD_ARGS + [temp_dir],
                      cwd='libdav1d',
                      env=env)
    PrintAndCheckCall(['ninja', '-C', temp_dir, 'include/vcs_version.h'],
                      cwd='libdav1d',
                      env=env)

    CopyVersions(temp_dir, version_dir)

    shutil.rmtree(temp_dir)


def main():
    linux_env = os.environ
    linux_env['CC'] = 'clang'

    noasm_dir = 'config/linux-noasm/arm64'
    GenerateConfig(noasm_dir, linux_env, ['-Denable_asm=false'])

    GenerateVersion('version', linux_env)


if __name__ == '__main__':
    main()
