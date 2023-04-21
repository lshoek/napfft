# - Try to find KISSFFT
# Once done this will define
# 
# KISSFFT_FOUND - System has KISSFFT
# KISSFFT_INCLUDE_DIR - The KISSFFT include directory
# KISSFFT_LIB - The library needed to use KISSFFT
# KISSFFT_DLL - The dll needed to use KISSFFT

if(WIN32)
    find_path(KISSFFT_DIR
        NAMES source/kiss_fft.h
        HINTS ${THIRDPARTY_DIR}/kissfft
    )
    find_path(KISSFFT_INCLUDE_DIR 
        NAMES kiss_fft.h
        HINTS ${KISSFFT_DIR}/source
    )
    find_path(KISSFFT_LIB_DIR 
        NAMES kissfft-float.lib
        HINTS ${KISSFFT_DIR}/msvc/x86_64/Release
    )         
    set(KISSFFT_LIB ${KISSFFT_LIB_DIR}/kissfft-float.lib)
    set(KISSFFT_DLL ${KISSFFT_LIB_DIR}/kissfft-float.dll)

    find_library(KISSFFT_LIB
        PATHS ${KISSFFT_DIR}/bin
        NO_DEFAULT_PATH
    )

    mark_as_advanced(KISSFFT_INCLUDE_DIR)
    mark_as_advanced(KISSFFT_LIB_DIR)

    add_library(kissfft SHARED IMPORTED)
    set_target_properties(kissfft PROPERTIES
        IMPORTED_CONFIGURATIONS "Debug;Release;"
        IMPORTED_LOCATION_RELEASE ${KISSFFT_DLL}
        IMPORTED_LOCATION_DEBUG ${KISSFFT_DLL}
    )

    set_target_properties(kissfft PROPERTIES
        IMPORTED_IMPLIB_RELEASE ${KISSFFT_LIB}
        IMPORTED_IMPLIB_DEBUG ${KISSFFT_LIB}
    )

    # handle the QUIETLY and REQUIRED arguments
    # if all listed variables are TRUE
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(kissfft REQUIRED_VARS KISSFFT_DIR) 

    # Copy the dynamic linked lib into the build directory
    macro(copy_kissfft_dll)
        add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:kissfft>
                "$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY_$<UPPER_CASE:$<CONFIG>>>"
            )
    endmacro()
endif()
