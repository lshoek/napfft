find_path(KISSFFT_DIR
    NAMES src/kiss_fft.h
    HINTS ${NAP_ROOT}/modules/napfft/thirdparty/kissfft
)

mark_as_advanced(KISSFFT_DIR)
set(KISSFFT_NAME kissfft-float)
set(KISSFFT_LIB_FILENAME ${KISSFFT_NAME}.lib)
set(KISSFFT_DLL_FILENAME ${KISSFFT_NAME}.dll)

set(KISSFFT_INCLUDE_DIR ${KISSFFT_DIR}/src)

set(KISSFFT_RELEASE_DLL ${KISSFFT_DIR}/${NAP_THIRDPARTY_PLATFORM_DIR}/${ARCH}/bin/Release/${KISSFFT_DLL_FILENAME})
set(KISSFFT_RELEASE_LIB ${KISSFFT_DIR}/${NAP_THIRDPARTY_PLATFORM_DIR}/${ARCH}/bin/Release/${KISSFFT_LIB_FILENAME})

set(KISSFFT_DEBUG_DLL ${KISSFFT_DIR}/${NAP_THIRDPARTY_PLATFORM_DIR}/${ARCH}/bin/Debug/${KISSFFT_DLL_FILENAME})
set(KISSFFT_DEBUG_LIB ${KISSFFT_DIR}/${NAP_THIRDPARTY_PLATFORM_DIR}/${ARCH}/bin/Debug/${KISSFFT_LIB_FILENAME})

file(GLOB KISSFFT_LICENSE_FILES ${KISSFFT_DIR}/LICENSES/*)

find_library(
    KISSFFT_LIB_FILENAME
    NAMES ${KISSFFT_NAME}
    PATHS ${KISSFFT_DIR}/${NAP_THIRDPARTY_PLATFORM_DIR}/${ARCH}/bin/Release
    NO_DEFAULT_PATH
)

# promote package for find
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(kissfft REQUIRED_VARS KISSFFT_DIR KISSFFT_INCLUDE_DIR KISSFFT_LICENSE_FILES)

add_library(kissfft SHARED IMPORTED)
