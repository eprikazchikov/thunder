set(SRC_FILES
    src/context.c
    src/init.c
    src/input.c
    src/monitor.c
    src/vulkan.c
    src/window.c

    src/internal.h
    include/GLFW/glfw3.h
    include/GLFW/glfw3native.h
)
if(WIN32)
    list(APPEND SRC_FILES
            src/win32_init.c
            src/win32_joystick.c
            src/win32_monitor.c
            src/win32_time.c
            src/win32_tls.c
            src/win32_window.c
            src/wgl_context.c
            src/egl_context.c

            src/win32_platform.h
            src/win32_joystick.h
            src/wgl_context.h
            src/egl_context.h
    )
elseif(UNIX)
    find_package(X11 REQUIRED)
    list(APPEND SRC_FILES
            src/x11_init.c
            src/linux_joystick.c
            src/xkb_unicode.c
            src/x11_monitor.c
            src/posix_time.c
            src/posix_tls.c
            src/x11_window.c
            src/glx_context.c
            src/egl_context.c

            src/x11_platform.h
            src/linux_joystick.h
            src/glx_context.h
            src/egl_context.h
    )
elseif(APPLE)
    list(APPEND SRC_FILES
            src/cocoa_init.m
            src/cocoa_joystick.m
            src/cocoa_monitor.m
            src/cocoa_time.c
            src/posix_tls.c
            src/cocoa_window.m
            src/nsgl_context.m

            src/cocoa_platform.h
            src/cocoa_joystick.h
            src/nsgl_context.h
    )
endif()

add_library(glfw
    STATIC ${SRC_FILES})
target_include_directories(glfw PUBLIC include)
add_library(glfw-editor
    SHARED ${SRC_FILES})
target_include_directories(glfw-editor PUBLIC include)
target_compile_definitions(glfw-editor PUBLIC _GLFW_BUILD_DLL)

if(WIN32)
    target_compile_definitions(glfw PUBLIC _GLFW_WIN32)
    target_compile_definitions(glfw-editor PUBLIC _GLFW_WIN32)
endif()
if(UNIX)
    target_compile_definitions(glfw PUBLIC _GLFW_X11)
    target_compile_definitions(glfw-editor PUBLIC _GLFW_X11)
    set(x11_libs 
        ${X11_LIBRARIES} 
        ${X11_Xrandr_LIB} 
        ${X11_Xcursor_LIB}
        ${X11_Xinerama_LIB}
    )
    target_link_libraries(glfw PUBLIC ${x11_libs} ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(glfw-editor PUBLIC ${x11_libs} ${CMAKE_THREAD_LIBS_INIT})
endif()

install(TARGETS glfw
        ARCHIVE DESTINATION lib
)
install(TARGETS glfw-editor
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION lib
)
