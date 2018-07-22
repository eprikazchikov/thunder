import qbs

Project {
    id: ogg
    property stringList srcFiles: [
        "src/*.c"
    ]

    property stringList incPaths: [
        "src",
        "src/ogg"
    ]

    DynamicLibrary {
        name: "ogg-editor"
        condition: ogg.desktop
        files: ogg.srcFiles
        Depends { name: "cpp" }
        bundle.isBundle: false

        cpp.defines: ["LIBOGG_LIBRARY"]
        cpp.includePaths: ogg.incPaths
        cpp.libraryPaths: [ ]
        cpp.dynamicLibraries: [ ]
        cpp.sonamePrefix: "@executable_path"

        Properties {
            condition: qbs.targetOS.contains("windows")
            cpp.linkerFlags: ["/DEF:" + path + "/src/ogg.def"]
        }

        Group {
            name: "Install Dynamic Ogg"
            fileTagsFilter: ["dynamiclibrary", "dynamiclibrary_import"]
            qbs.install: true
            qbs.installDir: ogg.BIN_PATH + "/" + ogg.bundle
            qbs.installPrefix: ogg.PREFIX
        }
    }

    StaticLibrary {
        name: "ogg"
        files: ogg.srcFiles
        Depends { name: "cpp" }
        bundle.isBundle: false

        cpp.defines: [  ]
        cpp.includePaths: ogg.incPaths

        Group {
            name: "Install Static Ogg"
            condition: ogg.desktop
            fileTagsFilter: product.type
            qbs.install: true
            qbs.installDir: ogg.SDK_PATH + "/" + qbs.targetOS[0] + "/" + qbs.architecture + "/lib"
            qbs.installPrefix: ogg.PREFIX
        }
    }
}
