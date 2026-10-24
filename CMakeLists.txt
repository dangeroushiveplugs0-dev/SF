cmake_minimum_required(VERSION 3.28.3)
project(xpbd-softbody-simulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_options(-O3 -march=native)
endif()

cmake_policy(SET CMP0074 NEW)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(yaml-cpp REQUIRED)

# Vulkan shader sources are compiled to SPIR-V even while the existing OpenGL
# renderer remains the active runtime backend. This lets CI validate both
# shader languages before the Vulkan backend is introduced.
find_program(GLSLANG_VALIDATOR NAMES glslangValidator REQUIRED)
set(SF_SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

function(sf_compile_shader source stage language output_name)
    set(output "${SF_SHADER_OUTPUT_DIR}/${output_name}.spv")
    if(language STREQUAL "HLSL")
        add_custom_command(
            OUTPUT "${output}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${SF_SHADER_OUTPUT_DIR}"
            COMMAND "${GLSLANG_VALIDATOR}" -D -V --target-env vulkan1.1 -S "${stage}" -e main -o "${output}" "${CMAKE_CURRENT_SOURCE_DIR}/${source}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${source}"
            COMMENT "Compiling HLSL shader: ${source}"
            VERBATIM
        )
    else()
        add_custom_command(
            OUTPUT "${output}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${SF_SHADER_OUTPUT_DIR}"
            COMMAND "${GLSLANG_VALIDATOR}" -V --target-env vulkan1.1 -S "${stage}" -o "${output}" "${CMAKE_CURRENT_SOURCE_DIR}/${source}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${source}"
            COMMENT "Compiling GLSL shader: ${source}"
            VERBATIM
        )
    endif()
    set(SF_COMPILED_SHADERS ${SF_COMPILED_SHADERS} "${output}" PARENT_SCOPE)
endfunction()

sf_compile_shader("res/shaders/vulkan/glsl/mesh.vert" vert GLSL "mesh_glsl_vert")
sf_compile_shader("res/shaders/vulkan/glsl/mesh.frag" frag GLSL "mesh_glsl_frag")
sf_compile_shader("res/shaders/vulkan/hlsl/mesh.vert.hlsl" vert HLSL "mesh_hlsl_vert")
sf_compile_shader("res/shaders/vulkan/hlsl/mesh.frag.hlsl" frag HLSL "mesh_hlsl_frag")

add_custom_target(sf_vulkan_shaders ALL DEPENDS ${SF_COMPILED_SHADERS})

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
    src/*.c
    src/*.hpp
)
file(GLOB IMGUI_SRC
    lib/imgui/*.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(${PROJECT_NAME} src/main.cpp ${SRC_FILES})
add_dependencies(${PROJECT_NAME} sf_vulkan_shaders)
target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends
)

target_link_libraries(${PROJECT_NAME} glfw OpenGL::GL assimp yaml-cpp)
