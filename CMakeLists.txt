cmake_minimum_required(VERSION 3.18.4)

# Exclude for Android
if(ANDROID)
    return()
endif()

project(mod_napfft)

find_package(kissfft REQUIRED)

file(GLOB_RECURSE sources src/*.cpp src/*.h)

set(INCLUDES src ${KISSFFT_INCLUDE_DIR})

# Get our NAP modules dependencies from module.json
module_json_to_cmake()

add_library(${PROJECT_NAME} SHARED ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Modules)
# Remove lib prefix on Unix libraries
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES})

target_link_libraries(${PROJECT_NAME} napcore ${DEPENDENT_NAP_MODULES} optimized ${KISSFFT_LIB} )
target_link_libraries(${PROJECT_NAME} napcore ${DEPENDENT_NAP_MODULES})

target_compile_definitions(${PROJECT_NAME} PRIVATE NAP_SHARED_LIBRARY)
target_compile_definitions(${PROJECT_NAME} PRIVATE MODULE_NAME=${PROJECT_NAME})

# link with external libs
if(WIN32)
  target_link_libraries(${PROJECT_NAME} napcore ${DEPENDENT_NAP_MODULES} kissfft Ws2_32.lib)
else()
  target_link_libraries(${PROJECT_NAME} napcore ${DEPENDENT_NAP_MODULES} kissfft)
endif()

if(WIN32)
    # Copy DLL to build directory
    if(NOT TARGET kissfft)
        find_package(kissfft REQUIRED)
    endif()
    copy_kissfft_dll()
endif()

# Deploy module.json as MODULENAME.json alongside module post-build
copy_module_json_to_bin()

# Package into platform release
package_module()

# Package kissfft into platform release
#install(FILES ${KISSFFT_DIR}/LICENSE.txt DESTINATION thirdparty/kissfft)
#install(FILES ${KISSFFT_DIR}/README.md DESTINATION thirdparty/kissfft)
#install(FILES ${KISSFFT_DIR}/VERSION DESTINATION thirdparty/kissfft)

install(DIRECTORY ${KISSFFT_INCLUDE_DIR} DESTINATION thirdparty/kissfft FILES_MATCHING PATTERN "*.h")

if(WIN32)
    install(FILES ${KISSFFT_LIB} DESTINATION thirdparty/kissfft/msvc)
    install(FILES ${KISSFFT_DLL} DESTINATION thirdparty/kissfft/msvc)
endif()
