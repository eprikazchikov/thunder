set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SRC_FILES
    
    #    res/app-Info.plist
    #    res/icon.rc
    #    res/icons/thunder.icns
        src/editors/componentbrowser/componentbrowser.ui
        src/editors/contentbrowser/contentbrowser.ui
        src/editors/contentbrowser/contentselect.ui
        src/editors/materialedit/materialedit.ui
        src/editors/meshedit/meshedit.ui
        src/editors/meshedit/skeletalmeshedit.ui
        src/editors/meshedit/staticmeshedit.ui
        src/editors/objecthierarchy/hierarchybrowser.ui
        src/editors/particleedit/particleedit.ui
        src/editors/propertyedit/editors/Actions.ui
        src/editors/propertyedit/editors/StringEdit.ui
        src/editors/propertyedit/editors/VectorEdit.ui
        src/editors/propertyedit/propertyeditor.ui
        src/editors/scenecomposer/aboutdialog.ui
        src/editors/scenecomposer/scenecomposer.ui
        src/editors/textureedit/textureedit.ui
        src/editors/timeline/timeline.ui
        src/managers/asseteditormanager/importqueue.ui
        src/managers/configmanager/configdialog.ui
        src/managers/consolemanager/consolemanager.ui
        src/managers/pluginmanager/plugindialog.ui
        src/managers/projectmanager/projectdialog.ui
    #    worldeditor.qbs

    
    res/WorldEditor.qrc
    src/controllers/cameractrl.cpp
    src/controllers/cameractrl.h
    src/controllers/objectctrl.cpp
    src/controllers/objectctrl.h
    src/controllers/objectctrlpipeline.cpp
    src/controllers/objectctrlpipeline.h
    src/controllers/widgetctrl.cpp
    src/controllers/widgetctrl.h
    src/editors/componentbrowser/componentbrowser.cpp
    src/editors/componentbrowser/componentbrowser.h
    src/editors/componentbrowser/componentmodel.cpp
    src/editors/componentbrowser/componentmodel.h
    src/editors/contentbrowser/contentbrowser.cpp
    src/editors/contentbrowser/contentbrowser.h
    src/editors/contentbrowser/contentlist.cpp
    src/editors/contentbrowser/contentlist.h
    src/editors/contentbrowser/contentselect.cpp
    src/editors/contentbrowser/contentselect.h
    src/editors/contentbrowser/contenttree.cpp
    src/editors/contentbrowser/contenttree.h
    src/editors/materialedit/materialedit.cpp
    src/editors/materialedit/materialedit.h
    src/editors/meshedit/meshedit.cpp
    src/editors/meshedit/meshedit.h
    src/editors/objecthierarchy/hierarchybrowser.cpp
    src/editors/objecthierarchy/hierarchybrowser.h
    src/editors/objecthierarchy/objecthierarchymodel.cpp
    src/editors/objecthierarchy/objecthierarchymodel.h
    src/editors/particleedit/particleedit.cpp
    src/editors/particleedit/particleedit.h
    src/editors/propertyedit/custom/AssetProperty.cpp
    src/editors/propertyedit/custom/AssetProperty.h
    src/editors/propertyedit/custom/BoolProperty.cpp
    src/editors/propertyedit/custom/BoolProperty.h
    src/editors/propertyedit/custom/ColorProperty.cpp
    src/editors/propertyedit/custom/ColorProperty.h
    src/editors/propertyedit/custom/EnumProperty.cpp
    src/editors/propertyedit/custom/EnumProperty.h
    src/editors/propertyedit/custom/FilePathProperty.cpp
    src/editors/propertyedit/custom/FilePathProperty.h
    src/editors/propertyedit/custom/Property.cpp
    src/editors/propertyedit/custom/Property.h
    src/editors/propertyedit/custom/StringProperty.cpp
    src/editors/propertyedit/custom/StringProperty.h
    src/editors/propertyedit/custom/Vector3DProperty.cpp
    src/editors/propertyedit/custom/Vector3DProperty.h
    src/editors/propertyedit/editors/Actions.cpp
    src/editors/propertyedit/editors/Actions.h
    src/editors/propertyedit/editors/ArrayEdit.cpp
    src/editors/propertyedit/editors/ArrayEdit.h
    src/editors/propertyedit/editors/ColorEdit.cpp
    src/editors/propertyedit/editors/ColorEdit.h
    src/editors/propertyedit/editors/PathEdit.cpp
    src/editors/propertyedit/editors/PathEdit.h
    src/editors/propertyedit/editors/PathEdit.ui
    src/editors/propertyedit/editors/StringEdit.cpp
    src/editors/propertyedit/editors/StringEdit.h
    src/editors/propertyedit/editors/VectorEdit.cpp
    src/editors/propertyedit/editors/VectorEdit.h
    src/editors/propertyedit/nextobject.cpp
    src/editors/propertyedit/nextobject.h
    src/editors/propertyedit/propertyeditor.cpp
    src/editors/propertyedit/propertyeditor.h
    src/editors/propertyedit/propertymodel.cpp
    src/editors/propertyedit/propertymodel.h
    src/editors/scemeeditor/schemeeditor.cpp
    src/editors/scemeeditor/schemeeditor.h
    src/editors/scenecomposer/aboutdialog.cpp
    src/editors/scenecomposer/aboutdialog.h
    src/editors/scenecomposer/scenecomposer.cpp
    src/editors/scenecomposer/scenecomposer.h
    src/editors/textureedit/textureedit.cpp
    src/editors/textureedit/textureedit.h
    src/editors/timeline/animationclipmodel.cpp
    src/editors/timeline/animationclipmodel.h
    src/editors/timeline/timeline.cpp
    src/editors/timeline/timeline.h
    src/graph/editors/chartview.cpp
    src/graph/editors/chartview.h
    src/graph/editors/curveeditor.cpp
    src/graph/editors/curveeditor.h
    src/graph/editors/graphwidget.cpp
    src/graph/editors/graphwidget.h
    src/graph/handles.cpp
    src/graph/handles.h
    src/graph/handletools.cpp
    src/graph/handletools.h
    src/graph/sceneview.cpp
    src/graph/sceneview.h
    src/graph/viewport.cpp
    src/graph/viewport.h
    src/main.cpp
    src/managers/asseteditormanager/iconrender.cpp
    src/managers/asseteditormanager/iconrender.h
    src/managers/asseteditormanager/importqueue.cpp
    src/managers/asseteditormanager/importqueue.h
    src/managers/configmanager/configdialog.cpp
    src/managers/configmanager/configdialog.h
    src/managers/consolemanager/consolemanager.cpp
    src/managers/consolemanager/consolemanager.h
    src/managers/consolemanager/logmodel.cpp
    src/managers/consolemanager/logmodel.h
    src/managers/pluginmanager/plugindelegate.cpp
    src/managers/pluginmanager/plugindelegate.h
    src/managers/pluginmanager/plugindialog.cpp
    src/managers/pluginmanager/plugindialog.h
    src/managers/projectmanager/projectdialog.cpp
    src/managers/projectmanager/projectdialog.h
    src/managers/toolwindowmanager/private/qtoolwindowmanager_p.h
    src/managers/toolwindowmanager/private/qtoolwindowmanagerarea_p.h
    src/managers/toolwindowmanager/private/qtoolwindowmanagerwrapper_p.h
    src/managers/toolwindowmanager/qabstracttoolwindowmanagerarea.cpp
    src/managers/toolwindowmanager/qabstracttoolwindowmanagerarea.h
    src/managers/toolwindowmanager/qtoolwindowmanager.cpp
    src/managers/toolwindowmanager/qtoolwindowmanager.h
    src/managers/toolwindowmanager/qtoolwindowmanagerarea.cpp
    src/managers/toolwindowmanager/qtoolwindowmanagerwrapper.cpp
    src/managers/undomanager/undomanager.cpp
    src/managers/undomanager/undomanager.h
    src/qlog.h
    src/settings.cpp
    src/settings.h
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
add_executable(WorldEditor ${SRC_FILES})
target_link_libraries(WorldEditor PRIVATE engine-editor rendergl-editor Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Quick Qt5::QuickWidgets fbx glsl spirvcross
    zlib
    )
target_include_directories(WorldEditor PRIVATE src
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/develop/managers/assetmanager/include
    ${PROJECT_SOURCE_DIR}/develop/managers/projectmanager/include
    ${PROJECT_SOURCE_DIR}/develop/managers/pluginmanager/include
    ${PROJECT_SOURCE_DIR}/develop/models/include/
    )
foreach(def ${PROJECT_DEFINES})
	target_compile_definitions(WorldEditor PUBLIC ${def})
endforeach()

install(TARGETS WorldEditor
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION lib
)
install(DIRECTORY bin/ DESTINATION resources)
