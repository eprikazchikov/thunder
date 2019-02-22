set(SRC_FILES
    src/spirv_cfg.cpp
    src/spirv_common.hpp
    src/spirv_cross.hpp
    src/spirv_cross_parsed_ir.hpp
    src/spirv_glsl.hpp
    src/spirv_hlsl.hpp
    src/spirv_msl.cpp
    src/spirv_parser.cpp
    src/spirv_cfg.hpp
    src/spirv_cross.cpp
    src/spirv_cross_parsed_ir.cpp
    src/spirv_glsl.cpp
    src/spirv_hlsl.cpp
    src/spirv.hpp
    src/spirv_msl.hpp
    src/spirv_parser.hpp
)

add_library(spirvcross
    STATIC ${SRC_FILES})
target_include_directories(spirvcross PUBLIC src)
