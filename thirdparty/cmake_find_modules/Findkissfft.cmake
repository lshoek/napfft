find_path(KISSFFT_DIR
    NAMES src/kiss_fft.h
    HINTS ${NAP_ROOT}/modules/napfft/thirdparty/kissfft
)

mark_as_advanced(KISSFFT_DIR)
set(KISSFFT_INCLUDE_DIR ${KISSFFT_DIR}/src)
set(KISSFFT_DLL ${KISSFFT_DIR}/${NAP_THIRDPARTY_PLATFORM_DIR}/${ARCH}/bin/)
file(GLOB KISSFFT_LICENSE_FILES ${KISSFFT_DIR}/LICENSES/*)

find_library(
    KISSFFT_LIB
    NAMES kissfft-float
    PATHS ${KISSFFT_DIR}/${NAP_THIRDPARTY_PLATFORM_DIR}/${ARCH}/bin
    NO_DEFAULT_PATH
)

# promote package for find
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(kissfft REQUIRED_VARS KISSFFT_DIR KISSFFT_INCLUDE_DIR KISSFFT_LICENSE_FILES)

add_library(kissfft SHARED IMPORTED)
