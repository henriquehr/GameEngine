cmake_minimum_required(VERSION 3.8)

project("ashwga")

find_package(Vulkan REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo")

file(GLOB_RECURSE GLSL_SOURCE_FILES "shaders/*.glsl")
file(GLOB_RECURSE GLSL_HEADER_FILES "shaders/*.h")
file(GLOB_RECURSE CPP_SOURCE_FILES "src/*.h" "src/*.cpp")

add_executable(ashwga
        ${CPP_SOURCE_FILES}
        ${GLSL_SOURCE_FILES}
        ${GLSL_HEADER_FILES}
        )

set_target_properties(ashwga PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)

target_compile_definitions(ashwga PRIVATE GLM_FORCE_XYZW_ONLY GLM_FORCE_QUAT_DATA_XYZW GLM_FORCE_RADIANS GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_compile_definitions(ashwga PRIVATE TINYOBJLOADER_IMPLEMENTATION)
target_compile_definitions(ashwga PRIVATE VK_USE_PLATFORM_WIN32_KHR)
target_compile_definitions(ashwga PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

add_library(glm INTERFACE)
add_library(sdl2 INTERFACE)
add_library(tinyobjloader INTERFACE)

target_include_directories(glm INTERFACE external/glm)
target_include_directories(tinyobjloader INTERFACE external/tinyobjloader)

set(sdl2_DIR "external/SDL2")
target_include_directories(sdl2 INTERFACE ${sdl2_DIR}/include ${sdl2_image_DIR}/include)
target_link_directories(sdl2 INTERFACE ${sdl2_DIR}/lib/x64 ${sdl2_image_DIR}/lib/x64)
target_link_libraries(sdl2 INTERFACE SDL2 SDL2main)

target_link_libraries(ashwga
        tinyobjloader
        glm
        Vulkan::Vulkan
        sdl2
        )

set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslangValidator.exe")
set(SPIRV_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/shaders/compiled/")
foreach (GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(STEM ${GLSL} NAME_WLE)
    set(SPIRV "${SPIRV_OUTPUT_DIR}${STEM}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
            COMMAND ${GLSL_VALIDATOR} -V --target-env vulkan1.0 ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL} ${GLSL_HEADER_FILES})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach ()

add_custom_target(compile_shaders DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(ashwga compile_shaders)

file(COPY "external/SDL2/lib/x64/SDL2.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
file(COPY "external/SDL2/lib/x64/SDL2.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")