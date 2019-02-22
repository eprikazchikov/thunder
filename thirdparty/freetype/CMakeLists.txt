set(SRC_FILES
        src/base/ftbase.c
        src/base/ftinit.c
        src/base/ftsystem.c
        src/base/ftbbox.c
        src/base/ftbdf.c
        src/base/ftbitmap.c
        src/base/ftcid.c
        src/base/ftfntfmt.c
        src/base/ftfstype.c
        src/base/ftgasp.c
        src/base/ftglyph.c
        src/base/ftgxval.c
        src/base/ftlcdfil.c
        src/base/ftmm.c
        src/base/ftotval.c
        src/base/ftpatent.c
        src/base/ftpfr.c
        src/base/ftstroke.c
        src/base/ftsynth.c
        src/base/fttype1.c
        src/base/ftwinfnt.c
        src/autofit/autofit.c
        src/bdf/bdf.c
        src/cff/cff.c
        src/cache/ftcache.c
        src/gzip/ftgzip.c
        src/lzw/ftlzw.c
        src/pcf/pcf.c
        src/pfr/pfr.c
        src/psaux/psaux.c
        src/pshinter/pshinter.c
        src/psnames/psmodule.c
        src/raster/raster.c
        src/sfnt/sfnt.c
        src/smooth/smooth.c
        src/truetype/truetype.c
        src/type1/type1.c
        src/cid/type1cid.c
        src/type42/type42.c
        src/winfonts/winfnt.c

)

add_library(freetype
    STATIC ${SRC_FILES})
target_include_directories(freetype PUBLIC include)
target_compile_definitions(freetype PRIVATE FT2_BUILD_LIBRARY)
add_library(freetype-editor
    SHARED ${SRC_FILES})
target_include_directories(freetype-editor PUBLIC include)
target_compile_definitions(freetype-editor PRIVATE FT2_BUILD_LIBRARY)

install(TARGETS freetype
        ARCHIVE DESTINATION lib
)
install(TARGETS freetype-editor
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION lib
)
