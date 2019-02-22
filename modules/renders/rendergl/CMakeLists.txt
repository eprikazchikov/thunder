set(SRC_FILES
    src/commandbuffergl.cpp
    src/postprocess/aambientocclusiongl.cpp
    src/postprocess/abloomgl.cpp
    src/renderglsystem.cpp
    src/resources/ameshgl.cpp
    src/resources/atexturegl.cpp
    src/filters/ablurgl.cpp
    src/postprocess/aantialiasinggl.cpp
    src/rendergl.cpp
    src/resources/amaterialgl.cpp
    src/resources/arendertexturegl.cpp
)
set(INC_FILES
    includes/agl.h
    includes/postprocess/aambientocclusiongl.h
    includes/postprocess/apostprocessor.h
    includes/resources/amaterialgl.h
    includes/resources/atexturegl.h
    includes/commandbuffergl.h
    includes/postprocess/aantialiasinggl.h
    includes/rendergl.h
    includes/resources/ameshgl.h
    includes/filters/ablurgl.h
    includes/postprocess/abloomgl.h
    includes/renderglsystem.h
    includes/resources/arendertexturegl.h
)
add_library(rendergl STATIC ${SRC_FILES} ${INC_FILES})
target_include_directories(rendergl PUBLIC includes)
target_link_libraries(rendergl PUBLIC engine glad)
install(TARGETS rendergl
        ARCHIVE DESTINATION lib
)
add_library(rendergl-editor SHARED ${SRC_FILES} ${INC_FILES})
target_include_directories(rendergl-editor PUBLIC includes)
target_link_libraries(rendergl-editor PUBLIC engine-editor glad)
target_compile_definitions(rendergl-editor PRIVATE NEXT_SHARED) # TODO: use cmake support for export header generation

install(DIRECTORY includes/ DESTINATION include/engine)
install(TARGETS rendergl-editor
        RUNTIME DESTINATION plugins/
        LIBRARY DESTINATION plugins/
        ARCHIVE DESTINATION lib
)