CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

project(thunder)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(Qt5 COMPONENTS Core Gui Widgets Quick QuickWidgets)
find_package(Threads REQUIRED)
if(WIN32)
  add_definitions(-D_UNICODE -DUNICODE)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Output all built binaries (exe/dll) to a single directory
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/deploy)
########################## Prepare global data and defines ####################
set(PROJECT_DEFINES)
list(APPEND PROJECT_DEFINES 
    COMPANY_NAME="FrostSpear"
    EDITOR_NAME="WorldEditor"
    PRODUCT_NAME="Thunder Engine"
    COPYRIGHT_AUTHOR="Evgeniy Prikazchikov"
)
file(READ ${PROJECT_SOURCE_DIR}/sponsors SPONSORS)
file(READ ${PROJECT_SOURCE_DIR}/legal LEGAL)
string(TIMESTAMP COPYRIGHT_YEAR "%Y")
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
list(APPEND PROJECT_DEFINES
    SPONSORS="${SPONSORS}" 
    LEGAL="${LEGAL}"
    COPYRIGHT_YEAR="${COPYRIGHT_YEAR}"
    REVISION="${GIT_BRANCH}"
    BUILDER_NAME="Builder"
    SDK_VERSION="1.0"
    LAUNCHER_VERSION="1.0"
)
############################ add project component directories

add_subdirectory(thirdparty)

add_subdirectory(engine)
add_subdirectory(modules)
add_subdirectory(worldeditor)

add_subdirectory(builder)
