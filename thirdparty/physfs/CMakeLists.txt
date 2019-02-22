set(SRC_FILES
src/physfs_byteorder.c
src/physfs.c
src/archivers/dir.c
src/archivers/grp.c
src/archivers/hog.c
src/archivers/mvl.c
src/archivers/qpak.c
src/archivers/wad.c
src/archivers/zip.c
src/acconfig.h
src/physfs.h
src/physfs_internal.h
)
if(WIN32)
    list(APPEND SRC_FILES
        src/platform/win32.c
    )
else()
    list(APPEND SRC_FILES
        src/platform/posix.c
        src/platform/unix.c
    )
endif()
add_library(physfs
    STATIC ${SRC_FILES})
add_library(physfs-editor
    SHARED ${SRC_FILES})

target_include_directories(physfs PUBLIC src)
target_include_directories(physfs-editor PUBLIC src)
target_link_libraries(physfs PUBLIC zlib)
target_link_libraries(physfs-editor PUBLIC zlib)
target_compile_definitions(physfs PUBLIC PHYSFS_SUPPORTS_ZIP PHYSFS_NO_CDROM_SUPPORT)
target_compile_definitions(physfs-editor PUBLIC PHYSFS_SUPPORTS_ZIP PHYSFS_NO_CDROM_SUPPORT)
if(APPLE)
    target_compile_definitions(physfs PUBLIC PHYSFS_DARWIN)
    target_compile_definitions(physfs-editor PUBLIC PHYSFS_DARWIN)
endif()
if(WIN32)
    target_link_libraries(physfs PUBLIC advapi32)
    target_link_libraries(physfs-editor PUBLIC advapi32)
endif()
install(TARGETS physfs
        ARCHIVE DESTINATION lib
)
install(TARGETS physfs-editor
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION lib
)
