if(NOT TARGET kissfft)
    find_package(kissfft REQUIRED)
endif()

set(dest_dir ${KISSFFT_DIR})

if(NAP_BUILD_CONTEXT MATCHES "source")
    target_include_directories(${PROJECT_NAME} PUBLIC src ${KISSFFT_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${KISSFFT_LIB})

    # additional definitions
    if(WIN32)
        target_compile_definitions(${PROJECT_NAME} PUBLIC WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0A00)

        # Copy FFmpeg DLLs to build directory
        file(GLOB KISSFFT_DLLS ${dest_dir}/${NAP_THIRDPARTY_PLATFORM_DIR}/${ARCH}/bin/*.dll)
        copy_files_to_bin(${KISSFFT_DLLS})
    endif()

    # Package kissfft into platform release
    install(FILES ${KISSFFT_LICENSE_FILES} DESTINATION ${dest_dir})
    install(DIRECTORY ${KISSFFT_INCLUDE_DIR} DESTINATION ${dest_dir})
else()
    add_include_to_interface_target(napfft ${KISSFFT_INCLUDE_DIR})
    add_define_to_interface_target(napfft KISSFFT_TOOLS=OFF)

    if(WIN32)
        # Define _WIN32_WINNT for KISSFFT
        add_define_to_interface_target(napfft WIN32_LEAN_AND_MEAN)
        add_define_to_interface_target(napfft _WIN32_WINNT=0x0A00)
    endif()

    # Install kissfft license into packaged project
    install(FILES ${KISSFFT_LICENSE_FILES} DESTINATION licenses/kissfft/)
endif()
