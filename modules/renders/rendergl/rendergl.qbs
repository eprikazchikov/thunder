import qbs

Project {
    id: rendergl
    property stringList srcFiles: [
        "src/*.cpp",
        "src/components/*.cpp",
        "src/postprocess/*.cpp",
        "src/resources/*.cpp",
        "src/filters/*.cpp",

        "includes/*.h",
        "includes/components/*.h",
        "includes/resources/*.h",
        "includes/postprocess/*.h",
        "includes/filters/*.h"
    ]

    property stringList incPaths: [
        "includes",
        "../../../common",
        "../../../engine/includes",
        "../../../engine/includes/resources",
        "../../../thirdparty/next/inc",
        "../../../thirdparty/next/inc/math",
        "../../../thirdparty/next/inc/core",
        "../../../thirdparty/glfw/include",
        "../../../thirdparty/glfm/include",
        "../../../thirdparty/glew/include",
        "../../../thirdparty/glad/include",
    ]

    DynamicLibrary {
        name: "rendergl-editor"
        condition: rendergl.desktop
        files: rendergl.srcFiles
        Depends { name: "cpp" }
        Depends { name: "next-editor" }
        Depends { name: "engine-editor" }
        Depends { name: "glfw-editor" }
        Depends { name: "glad" }
        bundle.isBundle: false

        cpp.defines: ["BUILD_SHARED", "NEXT_LIBRARY"]
        cpp.includePaths: rendergl.incPaths
        cpp.cxxLanguageVersion: "c++14"
        cpp.minimumMacosVersion: "10.12"
        cpp.cxxStandardLibrary: "libc++"
        cpp.sonamePrefix: "@executable_path"

        Properties {
            condition: qbs.targetOS.contains("windows")
        }

        Properties {
            condition: qbs.targetOS.contains("darwin")
            cpp.weakFrameworks: ["OpenGL"]
        }

        Group {
            name: "Install Dynamic RenderGL"
            fileTagsFilter: ["dynamiclibrary", "dynamiclibrary_import"]
            qbs.install: true
            qbs.installDir: rendergl.BIN_PATH + "/plugins/" + rendergl.bundle
            qbs.installPrefix: rendergl.PREFIX
        }
    }

    StaticLibrary {
        name: "rendergl"
        files: rendergl.srcFiles
        Depends { name: "cpp" }
        bundle.isBundle: false

        cpp.includePaths: rendergl.incPaths
        cpp.cxxLanguageVersion: "c++14"
        cpp.minimumMacosVersion: "10.12"
        cpp.minimumIosVersion: "10.0"
        cpp.minimumTvosVersion: "10.0"
        cpp.cxxStandardLibrary: "libc++"

        Properties {
            condition: !rendergl.desktop
            cpp.defines: ["THUNDER_MOBILE"]
        }

        Properties {
            condition: qbs.targetOS.contains("android")
            Android.ndk.appStl: "gnustl_shared"
        }

        Properties {
            condition: qbs.targetOS.contains("darwin")
            cpp.weakFrameworks: ["OpenGL"]
        }

        Group {
            name: "Install Static RenderGL"
            fileTagsFilter: product.type
            qbs.install: true
            qbs.installDir: rendergl.SDK_PATH + "/" + qbs.targetOS[0] + "/" + qbs.architecture + "/lib"
            qbs.installPrefix: rendergl.PREFIX
        }
    }
}
