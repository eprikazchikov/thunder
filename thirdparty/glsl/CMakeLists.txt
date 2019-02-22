add_subdirectory(OGLCompilersDLL)
add_subdirectory(SPIRV)
add_subdirectory(glslang)

add_library(glsl INTERFACE IMPORTED GLOBAL)
target_link_libraries(glsl INTERFACE OGLCompiler SPIRV glslang ${CMAKE_THREAD_LIBS_INIT} )
target_include_directories(glsl INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})