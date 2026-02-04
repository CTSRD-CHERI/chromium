#!/usr/bin/env bash

set -e
set -o pipefail
if [ "$VERBOSE" ]; then
  set -x
fi
set -u

# Setup the installation directory hierarchy in the package staging area.
prep_staging_freebsd() {
  install -m 755 -d "${STAGEDIR}/${PREFIX}/bin" \
    "${STAGEDIR}/${PREFIX}/share/applications" \
    "${STAGEDIR}/${PREFIX}/share/${PACKAGE}" \
    "${STAGEDIR}/${PREFIX}/share/man/man1" \
    "${STAGEDIR}/${PREFIX}/share/metainfo"
}

# Put the package contents in the staging area.
stage_install_freebsd() {
  local USR_BIN_SYMLINK_NAME="${PACKAGE}"
  local PACKAGE_ORIG="${PACKAGE}"
  local INSTALLDIR="${INSTALLDIR}"
  prep_staging_freebsd
  SHLIB_PERMS=644
  log_cmd echo "Staging FreeBSD install files in '${STAGEDIR}'..."

  # Note: Changes here may also need to be applied to ChromeOS's
  # chromite/lib/chrome_util.py.

  # Note: This only supports static binaries and does not work when the GN
  # is_component_build flag is true.

  # app
  STRIPPEDFILE="${OUTPUTDIR}/${PROGNAME}.stripped"
  install -m 755 "${STRIPPEDFILE}" "${STAGEDIR}/${INSTALLDIR}/${PROGNAME}"

  # crashpad
  #strippedfile="${OUTPUTDIR}/chrome_crashpad_handler.stripped"
  #install -m 755 "${strippedfile}" "${STAGEDIR}/${INSTALLDIR}/chrome_crashpad_handler"

  # Final permissions for the chrome-management-service will be set in
  # postinst chrome_management_service_setup().
  #strippedfile="${OUTPUTDIR}/chrome_management_service.stripped"
  #install -m 755 "${strippedfile}" "${STAGEDIR}/${INSTALLDIR}/chrome-management-service"

  # resources
  install -m 644 "${OUTPUTDIR}/resources.pak" "${STAGEDIR}/${INSTALLDIR}/"
  # TODO(mmoss): This has broken a couple times on adding new .pak files. Maybe
  # we should flag all installer files in FILES.cfg and get them from there, so
  # there's only one place people need to keep track of such things (and in
  # only the public repository).
  if [ -r "${OUTPUTDIR}/chrome_100_percent.pak" ]; then
    install -m 644 "${OUTPUTDIR}/chrome_100_percent.pak" "${STAGEDIR}/${INSTALLDIR}/"
    install -m 644 "${OUTPUTDIR}/chrome_200_percent.pak" "${STAGEDIR}/${INSTALLDIR}/"
  else
    install -m 644 "${OUTPUTDIR}/theme_resources_100_percent.pak" "${STAGEDIR}/${INSTALLDIR}/"
    install -m 644 "${OUTPUTDIR}/ui_resources_100_percent.pak" "${STAGEDIR}/${INSTALLDIR}/"
  fi

  # ICU data file; Necessary when the GN icu_use_data_file flag is true.
  #install -m 644 "${OUTPUTDIR}/icudtl.dat" "${STAGEDIR}/${INSTALLDIR}/"

  # V8 snapshot files; Necessary when the GN v8_use_external_startup_data flag
  # is true.
  # Use v8_context_snapshot.bin instead of snapshot_blob.bin if it is available.
  # TODO(crbug.com/764576): Unship snapshot_blob.bin on ChromeOS and drop this branch
  #if [ -f "${OUTPUTDIR}/v8_context_snapshot.bin" ]; then
  #  install -m 644 "${OUTPUTDIR}/v8_context_snapshot.bin" "${STAGEDIR}/${INSTALLDIR}/"
  #else
  #  install -m 644 "${OUTPUTDIR}/snapshot_blob.bin" "${STAGEDIR}/${INSTALLDIR}/"
  #fi

  # sandbox
  # Rename sandbox binary with hyphen instead of underscore because that's what
  # the code looks for. Originally, the SCons build system may have had a bug
  # where it did not support hyphens, so this is stuck as is to avoid breaking
  # anyone who expects the build artifact to have the underscore.
  # the code looks for, but the build targets can't use hyphens (scons bug?)
  #strippedfile="${OUTPUTDIR}/${PROGNAME}_sandbox.stripped"
  #install -m 4755 "${strippedfile}" "${STAGEDIR}/${INSTALLDIR}/chrome-sandbox"

  # l10n paks
  install -m 755 -d "${STAGEDIR}/${INSTALLDIR}/locales/"
  find "${OUTPUTDIR}/locales" -type f -name '*.pak' -exec \
    cp -a '{}' "${STAGEDIR}/${INSTALLDIR}/locales/" \;
  find "${STAGEDIR}/${INSTALLDIR}/locales" -type f -exec chmod 644 '{}' \;

  # TODO(https://crbug.com/1077934): The below conditions check for the
  # existence of files to determine if they should be copied to the staging
  # directory.  However, these may be stale if the build config no longer
  # builds these files.  The build config should be obtained from gn rather than
  # guessed based on the presence of files.

  # MEI Preload
  if [ -f "${OUTPUTDIR}/MEIPreload/manifest.json" ]; then
    install -m 755 -d "${STAGEDIR}/${INSTALLDIR}/MEIPreload/"
    install -m 644 "${OUTPUTDIR}/MEIPreload/manifest.json" "${STAGEDIR}/${INSTALLDIR}/MEIPreload/"
    install -m 644 "${OUTPUTDIR}/MEIPreload/preloaded_data.pb" "${STAGEDIR}/${INSTALLDIR}/MEIPreload/"
  fi

  # Widevine CDM.
  if [ -d "${OUTPUTDIR}/WidevineCdm" ]; then
    # No need to strip; libwidevinecdm.so starts out stripped.
    cp -a "${OUTPUTDIR}/WidevineCdm" "${STAGEDIR}/${INSTALLDIR}/"
    find "${STAGEDIR}/${INSTALLDIR}/WidevineCdm" -type d -exec chmod 755 '{}' \;
    find "${STAGEDIR}/${INSTALLDIR}/WidevineCdm" -type f -exec chmod 644 '{}' \;
    find "${STAGEDIR}/${INSTALLDIR}/WidevineCdm" -name libwidevinecdm.so \
      -exec chmod ${SHLIB_PERMS} '{}' \;
  fi

  # ANGLE
  if [ -f "${OUTPUTDIR}/libEGL.so" ]; then
    for file in libEGL.so libGLESv2.so; do
      strippedfile="${OUTPUTDIR}/${file}.stripped"
      install -m ${SHLIB_PERMS} "${strippedfile}" "${STAGEDIR}/${INSTALLDIR}/${file}"
    done
  fi

  # ANGLE's libvulkan library
  if [ -f "${OUTPUTDIR}/libvulkan.so.1" ]; then
    file="libvulkan.so.1"
    strippedfile="${OUTPUTDIR}/${file}.stripped"
    install -m ${SHLIB_PERMS} "${strippedfile}" "${STAGEDIR}/${INSTALLDIR}/${file}"
  fi

  # SwiftShader VK
  if [ -f "${OUTPUTDIR}/libvk_swiftshader.so" ]; then
    install -m 755 -d "${STAGEDIR}/${INSTALLDIR}/"
    file="libvk_swiftshader.so"
    strippedfile="${OUTPUTDIR}/${file}.stripped"
    install -m ${SHLIB_PERMS} "${strippedfile}" "${STAGEDIR}/${INSTALLDIR}/${file}"
    # Install the ICD json file to point ANGLE to libvk_swiftshader.so
    install -m 644 "${OUTPUTDIR}/vk_swiftshader_icd.json" "${STAGEDIR}/${INSTALLDIR}/"
  fi

  # Optimization Guide Internal
  if [ -f "${OUTPUTDIR}/liboptimization_guide_internal.so" ]; then
    file="liboptimization_guide_internal.so"
    strippedfile="${OUTPUTDIR}/${file}.stripped"
    install -m ${SHLIB_PERMS} "${strippedfile}" "${STAGEDIR}/${INSTALLDIR}/${file}"
  fi

  # QT shim
  if [ -f "${OUTPUTDIR}/libqt5_shim.so" ]; then
    file="libqt5_shim.so"
    strippedfile="${OUTPUTDIR}/${file}.stripped"
    install -m ${SHLIB_PERMS} "${strippedfile}" "${STAGEDIR}/${INSTALLDIR}/${file}"
  fi
  if [ -f "${OUTPUTDIR}/libqt6_shim.so" ]; then
    file="libqt6_shim.so"
    strippedfile="${OUTPUTDIR}/${file}.stripped"
    install -m ${SHLIB_PERMS} "${strippedfile}" "${STAGEDIR}/${INSTALLDIR}/${file}"
  fi

  # default apps
  if [ -d "${OUTPUTDIR}/default_apps" ]; then
    cp -a "${OUTPUTDIR}/default_apps" "${STAGEDIR}/${INSTALLDIR}/"
    find "${STAGEDIR}/${INSTALLDIR}/default_apps" -type d -exec chmod 755 '{}' \;
    find "${STAGEDIR}/${INSTALLDIR}/default_apps" -type f -exec chmod 644 '{}' \;
  fi

  # launcher script and symlink
  process_template "${OUTPUTDIR}/installer/freebsd/wrapper" \
    "${STAGEDIR}/${INSTALLDIR}/${PACKAGE}"
  chmod 755 "${STAGEDIR}/${INSTALLDIR}/${PACKAGE}"
  if [ ! -z "${PACKAGE_ORIG}" ]; then
    if [ ! -f "${STAGEDIR}/${INSTALLDIR}/${PACKAGE_ORIG}" ]; then
      ln -sn "${INSTALLDIR}/${PACKAGE}" \
        "${STAGEDIR}/${INSTALLDIR}/${PACKAGE_ORIG}"
    fi
  fi
  if [ ! -z "${USR_BIN_SYMLINK_NAME}" ]; then
    ln -snf "${INSTALLDIR}/${PACKAGE}" \
      "${STAGEDIR}/usr/local/bin/${USR_BIN_SYMLINK_NAME}"
  fi

  local icon_regex=".*product_logo_[0-9]\+\."
  LOGO_RESOURCES_PNG=$(find "${OUTPUTDIR}/installer/theme/" \
    -regextype sed -regex "${icon_regex}png" -printf "%f ")
  LOGO_RESOURCE_XPM=$(find "${OUTPUTDIR}/installer/theme/" \
    -regextype sed -regex "${icon_regex}xpm" -printf "%f")
  for logo in ${LOGO_RESOURCES_PNG} ${LOGO_RESOURCE_XPM}; do
    install -m 644 \
      "${OUTPUTDIR}/installer/theme/${logo}" \
      "${STAGEDIR}/${INSTALLDIR}/"
  done
 
  # desktop integration
  install -m 755 "${OUTPUTDIR}/xdg-mime" "${STAGEDIR}/${INSTALLDIR}/"
  install -m 755 "${OUTPUTDIR}/xdg-settings" "${STAGEDIR}/${INSTALLDIR}/"

  install -m 644 "${OUTPUTDIR}/installer/freebsd/chromium-browser.appdata.xml" \
    "${STAGEDIR}/${PREFIX}/share/metainfo/${PACKAGE}.appdata.xml"

  process_template "${OUTPUTDIR}/installer/freebsd/desktop.template" \
    "${STAGEDIR}/usr/local/share/applications/${PACKAGE}.desktop"
  chmod 644 "${STAGEDIR}/usr/local/share/applications/${PACKAGE}.desktop"
  #process_template "${OUTPUTDIR}/installer/common/default-app.template" \
  #  "${STAGEDIR}/usr/share/gnome-control-center/default-apps/${PACKAGE}.xml"
  #chmod 644 "${STAGEDIR}/usr/share/gnome-control-center/default-apps/${PACKAGE}.xml"
  #process_template "${OUTPUTDIR}/installer/common/default-app-block.template" \
  #  "${STAGEDIR}${INSTALLDIR}/default-app-block"
  #chmod 644 "${STAGEDIR}${INSTALLDIR}/default-app-block"

  # documentation
  process_template "${OUTPUTDIR}/installer/freebsd/manpage.1.in" \
    "${STAGEDIR}/usr/local/share/man/man1/${USR_BIN_SYMLINK_NAME}.1"
  gzip -9n "${STAGEDIR}/usr/local/share/man/man1/${USR_BIN_SYMLINK_NAME}.1"
  chmod 644 "${STAGEDIR}/usr/local/share/man/man1/${USR_BIN_SYMLINK_NAME}.1.gz"
  # The stable channel allows launching the app without the "-stable"
  # suffix like the other channels.  Create a linked man page for the
  # app-without-the-channel case.
  #if [ ! -f "${STAGEDIR}/usr/share/man/man1/${PACKAGE}.1.gz" ]; then
  #  ln -s "${USR_BIN_SYMLINK_NAME}.1.gz" \
  #    "${STAGEDIR}/usr/local/share/man/man1/${PACKAGE}.1.gz"
  #fi
  
  # Process the pkg manifest 
  process_template "${OUTPUTDIR}/installer/freebsd/manifest.template" \
    "${STAGEDIR}/manifest"

  # Create the pkg-plist
  find "${STAGEDIR}/usr/local" -type f,l -printf "%P\n" -mindepth 1 | tr ' ' '\n' > "${STAGEDIR}/pkg-plist"

  # Check to make sure all the ELF binaries are stripped.
  UNSTRIPPED=$(find "${STAGEDIR}/${INSTALLDIR}/" -type f | xargs file |
      grep ELF | grep -c "not stripped" || true)
  if [ "${UNSTRIPPED}" != "0" ]; then
    echo "ERROR: Found ${UNSTRIPPED} unstripped ELF files." 1>&2
    #exit 1
  fi

  # Check to make sure no ELF binaries set RPATH.
  if [ "${TARGET_OS}" != "chromeos" ]; then
    RPATH_BINS=
    for elf in $(find "${STAGEDIR}/${INSTALLDIR}/" -type f | xargs file |
                   grep ELF | awk '{print $1;}' | sed 's/:$//'); do
      if readelf -d ${elf} | grep "(RPATH)" >/dev/null; then
        RPATH_BINS="${RPATH_BINS} $(basename ${elf})"
      fi
    done
    if [ -n "${RPATH_BINS}" ]; then
      echo "ERROR: Found binaries with RPATH set:${RPATH_BINS}" 1>&2
      #exit 1
    fi
  fi

  # Make sure ELF binaries live in INSTALLDIR exclusively.
  ELF_OUTSIDE_INSTALLDIR=$(find "${STAGEDIR}/" -not -path \
    "${STAGEDIR}${INSTALLDIR}/*" -type f | xargs file -b |
                             grep -ce "^ELF" || true)
  if [ "${ELF_OUTSIDE_INSTALLDIR}" -ne 0 ]; then
    echo "ERROR: Found ${ELF_OUTSIDE_INSTALLDIR} ELF binaries" \
      "outside of ${INSTALLDIR}" 1>&2
    exit 1
  fi

  # Verify file permissions.
  for file in $(find "${STAGEDIR}" -mindepth 1); do
    local actual_perms=$(stat -c "%a" "${file}")
    local file_type="$(file -b "${file}")"
    local base_name=$(basename "${file}")
    if [[ "${file_type}" = "directory"* ]]; then
      local expected_perms=755
    elif [[ "${file_type}" = *"symbolic link"* ]]; then
      if [[ "$(readlink ${file})" = "/"* ]]; then
        # Absolute symlink.
        local expect_exists="${STAGEDIR}/$(readlink "${file}")"
      else
        # Relative symlink.
        local expect_exists="$(dirname "${file}")/$(readlink "${file}")"
      fi
      if [ ! -f "${expect_exists}" ]; then
        echo "Broken symlink: ${file}" 1>&2
        exit 1
      fi
      local expected_perms=777
    elif [ "${base_name}" = "chrome-management-service" ]; then
      local expected_perms=755
    elif [ "${base_name}" = "chrome-sandbox" ]; then
      local expected_perms=4755
    elif [[ "${base_name}" = "nacl_irt_"*".nexe" ]]; then
      local expected_perms=644
    elif [[ "${file_type}" = *"shell script"* ]]; then
      local expected_perms=755
    elif [[ "${file_type}" = ELF* ]]; then
      if [[ "${base_name}" = *".so" || "${base_name}" = *".so."[[:digit:]]* ]]; then
        local expected_perms=${SHLIB_PERMS}
      else
        local expected_perms=755
      fi
    else
      # Regular data file.
      local expected_perms=644
    fi
    if [ ${expected_perms} -ne ${actual_perms} ]; then
      echo Expected permissions on ${base_name} to be \
        ${expected_perms}, but they were ${actual_perms} 1>&2
      exit 1
    fi
  done
}

# Generate the package file
do_package() {
  log_cmd echo "Packaging ${ARCHITECTURE}..."
  /local/scratch/gcj21/pkg/src/pkg create \
    -M ${STAGEDIR}/manifest \
    -p ${STAGEDIR}/pkg-plist \
    -r ${STAGEDIR} \
    -o ${OUTPUTDIR} ${PACKAGE}
}

cleanup() {
  log_cmd echo "Cleaning..."
  rm -rf "${STAGEDIR}"
}

usage() {
  echo "usage: $(basename $0) [-a target_arch] -c channel -d branding"
  echo "                      [-f] [-o 'dir'] -s 'dir' -t target_os"
  echo "-a arch      pkg package architecture"
  echo "-d brand     either chromium or google_chrome"
  echo "-f           indicates that this is an official build"
  echo "-h           this help message"
  echo "-o dir       package output directory [${OUTPUTDIR}]"
  echo "-s dir       /path/to/sysroot"
  echo "-t platform  target platform"
}

process_opts() {
  while getopts ":a:b:d:fho:s:t:" OPTNAME
  do
    case $OPTNAME in
      a )
        ARCHITECTURE="$OPTARG"
        ;;
      b )
        ABI="$OPTARG"
        ;;
      d )
        BRANDING="$OPTARG"
        ;;
      f )
        IS_OFFICIAL_BUILD=1
        ;;
      h )
        usage
        exit 0
        ;;
      o )
        OUTPUTDIR=$(readlink -f "${OPTARG}")
        mkdir -p "${OUTPUTDIR}"
        ;;
      s )
        SYSROOT="$OPTARG"
        ;;
      t )
        TARGET_OS="$OPTARG"
        ;;
     \: )
        echo "'-$OPTARG' needs an argument."
        usage
        exit 1
        ;;
      * )
        echo "invalid command-line option: $OPTARG"
        usage
        exit 1
        ;;
    esac
  done
}

#=========
# MAIN
#=========

SCRIPTDIR=$(readlink -f "$(dirname "$0")")
OUTPUTDIR="${PWD}"

# call cleanup() on exit
trap cleanup 0
process_opts "$@"

STAGEDIR="${OUTPUTDIR}/pkg-staging"
mkdir -p "${STAGEDIR}"

source ${OUTPUTDIR}/installer/freebsd/installer.include

get_version_info
VERSIONFULL="${VERSION}-${PACKAGE_RELEASE}"

source "${OUTPUTDIR}/installer/freebsd/chromium-browser.info"

stage_install_freebsd

do_package
