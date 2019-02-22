include(GenerateExportHeader)

set(SRC_FILES
    src/analytics/profiler.cpp
    src/commandbuffer.cpp
    src/components/actor.cpp
    src/components/animationcontroller.cpp
    src/components/baselight.cpp
    src/components/basemesh.cpp
    src/components/camera.cpp
    src/components/component.cpp
    src/components/directlight.cpp
    src/components/nativebehaviour.cpp
    src/components/particlerender.cpp
    src/components/pointlight.cpp
    src/components/spotlight.cpp
    src/components/scene.cpp
    src/components/spritemesh.cpp
    src/components/staticmesh.cpp
    src/components/textmesh.cpp
    src/components/transform.cpp
    src/engine.cpp
    src/file.cpp
    src/input.cpp
    src/log.cpp
    src/resources/animationclip.cpp
    src/resources/atlas.cpp
    src/resources/font.cpp
    src/resources/material.cpp
    src/resources/mesh.cpp
    src/resources/particleeffect.cpp
    src/resources/pipeline.cpp
    src/resources/rendertexture.cpp
    src/resources/text.cpp
    src/resources/texture.cpp
    src/timer.cpp
    src/utils.cpp
    includes/adapters/desktopadaptor.h
    includes/adapters/iplatformadaptor.h
    includes/adapters/androidfile.h
    includes/adapters/mobileadaptor.h
    includes/analytics/profiler.h
    includes/commandbuffer.h
    includes/components/actor.h
    includes/components/animationcontroller.h
    includes/components/baselight.h
    includes/components/basemesh.h
    includes/components/camera.h
    includes/components/component.h
    includes/components/directlight.h
    includes/components/nativebehaviour.h
    includes/components/particlerender.h
    includes/components/pointlight.h
    includes/components/spotlight.h
    includes/components/scene.h
    includes/components/spritemesh.h
    includes/components/staticmesh.h
    includes/components/textmesh.h
    includes/components/transform.h
    includes/engine.h
    includes/engine_export.h
    includes/file.h
    includes/input.h
    includes/log.h
    includes/module.h
    includes/platform.h
    includes/resources/animationclip.h
    includes/resources/atlas.h
    includes/resources/font.h
    includes/resources/material.h
    includes/resources/mesh.h
    includes/resources/particleeffect.h
    includes/resources/pipeline.h
    includes/resources/rendertexture.h
    includes/resources/text.h
    includes/resources/texture.h
    includes/system.h
    includes/timer.h
    includes/utils.h
)
set(CONVERTER_FILES
    src/converters/builder.cpp    
    includes/converters/builder.h    
    src/converters/converter.cpp
    includes/converters/converter.h    
)
list(APPEND SRC_FILES src/adapters/desktopadaptor.cpp)
add_library(engine STATIC ${SRC_FILES})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes includes/components includes/resources ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(engine PUBLIC next freetype glfw physfs Qt5::Core)
target_compile_definitions(engine PUBLIC ENGINE_STATIC_DEFINE) # since both builds use engine_export, this marks the export/import macro as no-op
set_target_properties(engine PROPERTIES AUTOMOC ON)

add_library(engine-editor SHARED ${SRC_FILES} ${CONVERTER_FILES})
set_target_properties(engine-editor PROPERTIES AUTOMOC ON)
target_include_directories(engine-editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes includes/components includes/resources)
target_link_libraries(engine-editor PUBLIC next-editor freetype-editor glfw-editor physfs-editor Qt5::Core)
target_compile_definitions(engine-editor PRIVATE NEXT_SHARED) # using `next` library in shared form ( expose dllimport's on windows)

install(TARGETS engine-editor
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION lib
)
install(DIRECTORY includes/ DESTINATION include/engine
    PATTERN "adapters" EXCLUDE
)