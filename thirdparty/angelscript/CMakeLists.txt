set(SRC_FILES
    source/as_atomic.cpp
    source/as_callfunc_ppc_64.cpp
    source/as_callfunc_x86.cpp
    source/as_gc.cpp
    source/as_outputbuffer.cpp
    source/as_scriptnode.cpp
    source/as_typeinfo.cpp
    source/as_builder.cpp
    source/as_callfunc_ppc.cpp
    source/as_callfunc_xenon.cpp
    source/as_generic.cpp
    source/as_parser.cpp
    source/as_scriptobject.cpp
    source/as_variablescope.cpp
    source/as_bytecode.cpp
    source/as_callfunc_sh4.cpp
    source/as_compiler.cpp
    source/as_globalproperty.cpp
    source/as_restore.cpp
    source/as_string.cpp
    source/as_callfunc_arm.cpp
    source/as_callfunc_x64_gcc.cpp
    source/as_configgroup.cpp
    source/as_memory.cpp
    source/as_scriptcode.cpp
    source/as_string_util.cpp
    source/as_callfunc.cpp
    source/as_callfunc_x64_mingw.cpp
    source/as_context.cpp
    source/as_module.cpp
    source/as_scriptengine.cpp
    source/as_thread.cpp
    source/as_callfunc_mips.cpp
    source/as_callfunc_x64_msvc.cpp
    source/as_datatype.cpp
    source/as_objecttype.cpp
    source/as_scriptfunction.cpp
    source/as_tokenizer.cpp    
)

add_library(angelscript
    STATIC ${SRC_FILES})
target_include_directories(angelscript PRIVATE include)
target_compile_definitions(angelscript PUBLIC ANGELSCRIPT_EXPORT AS_NO_COMPILER)

if (MSVC AND (CMAKE_SIZEOF_VOID_P EQUAL 8))
    enable_language (ASM_MASM)
	list(APPEND SRC_FILES source/as_callfunc_x64_msvc_asm.asm)
endif()
add_library(angelscript-editor
    SHARED ${SRC_FILES})
target_include_directories(angelscript-editor PRIVATE include)
target_compile_definitions(angelscript-editor PUBLIC ANGELSCRIPT_EXPORT)
install(TARGETS angelscript
        ARCHIVE DESTINATION lib
)
install(TARGETS angelscript-editor
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION lib
)
