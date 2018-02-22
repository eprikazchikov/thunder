import qbs

Project {
    id: thunder

    property string COMPANY_NAME: "FrostSpear"
    property string EDITOR_NAME: "WorldEditor"
    property string BUILDER_NAME: "Builder"
    property string SDK_VERSION: "2.0"
    property string COPYRIGHT_YEAR: "2018"
    property string COPYRIGHT_AUTHOR: "Evgeniy Prikazchikov"

    property string PLATFORM: {
        var arch = qbs.architecture;
        if(qbs.targetOS.contains("osx")) {
            arch = "x86_64"
        }

        return qbs.targetOS[0] + "/" + arch;
    }
    property string RESOURCE_ROOT: "../worldeditor/bin"

    property string PREFIX: "_PACKAGE"
    property string LAUNCHER_PATH: "launcher"
    property string SDK_PATH: "sdk/" + SDK_VERSION
    property string PLATFORM_PATH: SDK_PATH + "/" + PLATFORM
    property string BIN_PATH: PLATFORM_PATH + "/bin"
    property string LIB_PATH: PLATFORM_PATH + "/lib"
    property string INC_PATH: SDK_PATH + "/include"

    property bool desktop: !qbs.targetOS.contains("android") && !qbs.targetOS.contains("ios")
    property string bundle: {
        if(qbs.targetOS.contains("osx")) {
            return EDITOR_NAME + ".app/Contents/MacOS/"
        }
        return "";
    }

    references: [
        "thirdparty/thirdparty.qbs",
        "engine/engine.qbs",
        "modules/renders/rendergl/rendergl.qbs",
        "worldeditor/worldeditor.qbs",
        "builder/builder.qbs",
        "build/install.qbs"
    ]
}

