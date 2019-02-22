set(SRC_FILES
    src/adler32.c
    src/compress.c
    src/crc32.c
    src/deflate.c
    src/gzio.c
    src/infback.c
    src/inffast.c
    src/inflate.c
    src/inftrees.c
    src/trees.c
    src/uncompr.c
    src/zutil.c
)

add_library(zlib STATIC ${SRC_FILES})
target_include_directories(zlib PUBLIC src)

add_library(zlib-editor SHARED ${SRC_FILES})
target_include_directories(zlib-editor PUBLIC src)
target_compile_definitions(zlib-editor PUBLIC ZLIB_DLL)

install(TARGETS zlib
        ARCHIVE DESTINATION lib
)
install(TARGETS zlib-editor
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION lib
)
