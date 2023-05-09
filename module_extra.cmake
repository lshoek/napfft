if(NOT TARGET kissfft)
    find_package(kissfft REQUIRED)
endif()

if(NAP_BUILD_CONTEXT MATCHES "source")
    target_include_directories(${PROJECT_NAME} PUBLIC src ${KISSFFT_INCLUDE_DIR})
    target_compile_definitions(${PROJECT_NAME} PUBLIC KISSFFT_STANDALONE)

    # additional definitions
    if(WIN32)
        target_compile_definitions(${PROJECT_NAME} PUBLIC WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0A00)
    endif()

    # Package kissfft into platform release
    set(dest_dir system_modules/${PROJECT_NAME}/thirdparty/kissfft)
    install(FILES ${KISSFFT_LICENSE_FILES} DESTINATION ${dest_dir})
    install(DIRECTORY ${KISSFFT_INCLUDE_DIR} DESTINATION ${dest_dir})
else()
    add_include_to_interface_target(napfft ${KISSFFT_INCLUDE_DIR})
    add_define_to_interface_target(napfft KISSFFT_STANDALONE)

    if(WIN32)
        # Define _WIN32_WINNT for KISSFFT
        add_define_to_interface_target(napfft WIN32_LEAN_AND_MEAN)
        add_define_to_interface_target(napfft _WIN32_WINNT=0x0A00)
    endif()

    # Install kissfft license into packaged project
    install(FILES ${KISSFFT_LICENSE_FILES} DESTINATION licenses/kissfft/)
endif()
