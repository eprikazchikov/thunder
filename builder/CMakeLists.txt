set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SRC_FILES
    builder.cpp    
    builder.h
    consolelog.h
    main.cpp
    
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/animconverter.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/assetmanager.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/effectconverter.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/fbxconverter.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/fontconverter.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/functionmodel.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/material/aconstvalue.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/material/acoordinates.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/material/agradient.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/material/amaterialparam.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/material/amathfunction.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/material/amathoperator.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/material/atexturesample.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/material/autils.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/materialconverter.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/prefabconverter.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/qbsbuilder.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/shaderbuilder.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/spirvconverter.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/textconverter.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include/textureconverter.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/animconverter.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/assetmanager.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/effectconverter.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/fbxconverter.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/fontconverter.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/functionmodel.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/materialconverter.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/prefabconverter.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/qbsbuilder.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/shaderbuilder.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/textconverter.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/src/textureconverter.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/pluginmanager/include/pluginmodel.h
    ${PROJECT_SOURCE_DIR}/develop/managers/pluginmanager/src/pluginmodel.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/projectmanager/include/projectmanager.h
    ${PROJECT_SOURCE_DIR}/develop/managers/projectmanager/include/projectmodel.h
    ${PROJECT_SOURCE_DIR}/develop/managers/projectmanager/src/projectmanager.cpp
    ${PROJECT_SOURCE_DIR}/develop/managers/projectmanager/src/projectmodel.cpp
    ${PROJECT_SOURCE_DIR}/develop/models/include/abstractschememodel.h
    ${PROJECT_SOURCE_DIR}/develop/models/include/baseobjectmodel.h
    ${PROJECT_SOURCE_DIR}/develop/models/src/baseobjectmodel.cpp
)
add_executable(builder ${SRC_FILES})
target_include_directories(builder PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR} # for config.h
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include
    ${PROJECT_SOURCE_DIR}/develop/managers/projectmanager/include
    ${PROJECT_SOURCE_DIR}/develop/managers/pluginmanager/include
    ${PROJECT_SOURCE_DIR}/develop/models/include/
)

target_link_libraries(builder PRIVATE 
    engine-editor rendergl-editor 
    Qt5::Core Qt5::Gui
    fbx glsl spirvcross quazip-editor
    )
target_compile_definitions(builder PRIVATE NEXT_SHARED BUILDER)
foreach(def ${PROJECT_DEFINES})
	target_compile_definitions(builder PUBLIC ${def})
endforeach()

install(TARGETS builder
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION lib
)

#    QtApplication {
#        name: builder.BUILDER_NAME
#        condition: builder.desktop
#        files: builder.srcFiles
#
#        bundle.isBundle: false
#
#        cpp.includePaths: builder.incPaths
#        cpp.libraryPaths: [
#            "../thirdparty/fbx/lib"
#        ]
#
#        property string prefix: qbs.targetOS.contains("windows") ? "lib" : ""
#        cpp.dynamicLibraries: [
#            prefix + "fbxsdk"
#        ]
#        cpp.cxxLanguageVersion: "c++14"
#
#        Properties {
#            condition: qbs.targetOS.contains("linux")
#            cpp.rpaths: "$ORIGIN"
#        }
#
#        Properties {
#            condition: qbs.targetOS.contains("darwin")
#            cpp.sonamePrefix: "@rpath"
#            cpp.rpaths: "@executable_path/../Frameworks/"
#        }
#    }
#}
