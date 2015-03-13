import qbs
Project {
    CppApplication {
        property string version: "0.0.1"

        name: "dDeflect"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: [
            "src/*/*.cpp",
            "src/*/*.h",
            "src/*/*/*.cpp",
            "src/*/*/*.h",
            "src/*/*/*/*.cpp",
            "src/*/*/*/*.h",
            "src/*/*/*/*/*.cpp",
            "src/*/*/*/*/*.h"
        ]

        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.install: true
            qbs.installDir: "bin"
        }

        Group {
            qbs.install: true
            qbs.installDir: "bin"
            files: [
                "src/core/descriptions/",
            ]
        }

        Group {
            qbs.install: true
            qbs.installDir: "bin/data"
            files: [
                "src/core/detection/",
                "src/core/handlers/",
                "src/core/helper_func/",
            ]
        }

        Depends { name: "Qt"; submodules: ["core", "widgets", "quick", "gui"] }
        cpp.warningLevel: "all"
        cpp.includePaths: ["src"]
        cpp.cxxFlags: ["-std=c++11","-Wno-unknown-pragmas","-Wno-reorder","-Wno-unused-local-typedefs"]
        cpp.defines: ["PROJECT_VERSION=\"" + version + "\""]
    }
    /*
    CppApplication {
        name: "tester"
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        files: [
            "src/core/adding_methods/wrappers/*.cpp",
            "src/core/adding_methods/wrappers/*.h",
            "src/helper/json_parser/djsonparser.cpp",
            "src/helper/json_parser/djsonparser.h",
            "src/helper/settings_parser/dsettings.h",
            "src/helper/settings_parser/dsettings.cpp",
            "src/helper/logger/dlogger.h",
            "src/helper/logger/dlogger.cpp",
            "tests/src/*.cpp",
            "tests/src/*.h",
            "src/core/file_types/*.cpp",
            "src/core/file_types/*.h",
        ]
        Depends { name: "Qt"; submodules: ["core", "widgets"] }
        cpp.warningLevel: "all"
        cpp.cxxFlags: ["-Wold-style-cast", "-std=c++11"]
        cpp.includePaths: ["src"]

        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.install: true
            qbs.installDir: "bin"
        }
    }
    */
}
