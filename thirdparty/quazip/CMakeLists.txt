set(SRC_FILES
src/unzip.c
src/zip.c
src/JlCompress.cpp
src/qioapi.cpp
src/quaadler32.cpp
src/quacrc32.cpp
src/quagzipfile.cpp
src/quaziodevice.cpp
src/quazip.cpp
src/quazipdir.cpp
src/quazipfile.cpp
src/quazipfileinfo.cpp
src/quazipnewinfo.cpp
src/crypt.h
src/ioapi.h
src/JlCompress.h
src/quaadler32.h
src/quachecksum32.h
src/quacrc32.h
src/quagzipfile.h
src/quaziodevice.h
src/quazipdir.h
src/quazipfile.h
src/quazipfileinfo.h
src/quazip_global.h
src/quazip.h
src/quazipnewinfo.h
src/unzip.h
src/zip.h
)

add_library(quazip STATIC ${SRC_FILES})
target_include_directories(quazip PUBLIC src)
target_link_libraries(quazip PUBLIC zlib Qt5::Core)
target_compile_definitions(quazip PUBLIC QUAZIP_STATIC)
set_target_properties(quazip PROPERTIES AUTOMOC ON)

add_library(quazip-editor SHARED ${SRC_FILES})
target_include_directories(quazip-editor PUBLIC src)
target_compile_definitions(quazip-editor PRIVATE QUAZIP_BUILD)
target_link_libraries(quazip-editor PUBLIC zlib-editor Qt5::Core)
set_target_properties(quazip-editor PROPERTIES AUTOMOC ON)

install(TARGETS quazip-editor
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION lib
)
