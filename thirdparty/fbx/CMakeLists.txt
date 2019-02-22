add_library(fbx SHARED IMPORTED GLOBAL)

target_include_directories(fbx INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
#target_compile_definitions(fbx INTERFACE FBXSDK_SHARED)
if(WIN32)
set_property(TARGET fbx PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libfbxsdk.dll)
set_property(TARGET fbx PROPERTY IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/libfbxsdk.lib)
elseif(APPLE)
set_property(TARGET fbx PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libfbxsdk.dylib") 
else()
set_property(TARGET fbx PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libfbxsdk.so") 
endif()

get_target_property(BIN_LIB fbx IMPORTED_LOCATION)

install(FILES ${BIN_LIB} DESTINATION .)