if(WIN32)

find_path(KISSFFT_DIR
    NAMES source/kiss_fft.h
    HINTS ${NAP_ROOT}/modules/napfft/thirdparty/kissfft
)

mark_as_advanced(KISSFFT_DIR)
set(KISSFFT_INCLUDE_DIR ${KISSFFT_DIR}/source)
file(GLOB KISSFFT_LICENSE_FILES ${KISSFFT_DIR}/LICENSES/*)


endif()

# promote package for find
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(kissfft REQUIRED_VARS KISSFFT_DIR KISSFFT_INCLUDE_DIR KISSFFT_LICENSE_FILES)
