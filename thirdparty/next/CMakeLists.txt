set(SRC_FILES
    src/core/bson.cpp
    src/core/invalid.cpp
    src/core/metamethod.cpp
    src/core/metaproperty.cpp
    src/core/object.cpp
    src/core/threadpool.cpp
    src/core/variant.cpp
    src/core/event.cpp
    src/core/json.cpp
    src/core/metaobject.cpp
    src/core/metatype.cpp
    src/core/objectsystem.cpp
    src/core/uri.cpp
    src/math/aabb.cpp
    src/math/matrix3.cpp
    src/math/obb.cpp
    src/math/quaternion.cpp
    src/math/vector2.cpp
    src/math/vector4.cpp
    src/math/math.cpp
    src/math/matrix4.cpp
    src/math/plane.cpp
    src/math/ray.cpp
    src/math/vector3.cpp
    src/anim/animation.cpp
    src/anim/animationcurve.cpp
    src/anim/propertyanimation.cpp
    src/anim/variantanimation.cpp
)
set(INC_FILES
    inc/anim/animationcurve.h
    inc/core/bson.h
    inc/core/metamethod.h
    inc/core/object.h
    inc/core/variant.h
    inc/math/matrix3.h
    inc/math/quaternion.h
    inc/math/vector4.h
    inc/anim/animation.h
    inc/core/event.h
    inc/core/metaobject.h
    inc/core/objectsystem.h
    inc/global.h
    inc/math/matrix4.h
    inc/math/ray.h
    inc/anim/propertyanimation.h
    inc/core/invalid.h
    inc/core/metaproperty.h
    inc/core/threadpool.h
    inc/math/aabb.h
    inc/math/obb.h
    inc/math/vector2.h
    inc/anim/variantanimation.h
    inc/core/json.h
    inc/core/metatype.h
    inc/core/uri.h
    inc/math/amath.h
    inc/math/plane.h
    inc/math/vector3.h
)
add_library(next STATIC ${SRC_FILES} ${INC_FILES})
target_include_directories(next PUBLIC inc)
target_include_directories(next PUBLIC inc/core)
target_include_directories(next PUBLIC inc/math)
target_include_directories(next PUBLIC inc/anim)

add_library(next-editor SHARED ${SRC_FILES} ${INC_FILES})
target_include_directories(next-editor PUBLIC inc)
target_include_directories(next-editor PUBLIC inc/core)
target_include_directories(next-editor PUBLIC inc/math)
target_include_directories(next-editor PUBLIC inc/anim)
target_compile_definitions(next-editor PRIVATE NEXT_LIBRARY)
target_compile_definitions(next-editor PUBLIC NEXT_SHARED)

install(TARGETS next-editor
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION lib
)
install(DIRECTORY inc/ DESTINATION include/next)