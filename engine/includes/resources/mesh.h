#ifndef MESH_H
#define MESH_H

#include <memory>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <amath.h>

#include "engine.h"
#include "material.h"

class ENGINE_EXPORT Mesh : public Object {
    A_REGISTER(Mesh, Object, Resources)

public:
    enum Attributes {
        ATTRIBUTE_COLOR         = (1<<0),
        ATTRIBUTE_UV0           = (1<<1),
        ATTRIBUTE_UV1           = (1<<2),
        ATTRIBUTE_NORMALS       = (1<<3),
        ATTRIBUTE_TANGENTS      = (1<<4),
        ATTRIBUTE_ANIMATED      = (1<<5),
    };

    enum Modes {
        MODE_TRIANGLES          = 0,
        MODE_LINES,
        MODE_TRIANGLE_STRIP,
        MODE_LINE_STRIP
    };

    typedef vector<uint32_t>    IndexVector;

    struct Lod {
        IndexVector             indices;

        Vector2Vector           uv0;

        Vector2Vector           uv1;

        Vector3Vector           normals;

        Vector3Vector           tangents;

        Vector3Vector           vertices;

        Vector4Vector           colors;

        Vector4Vector           weights;

        Vector4Vector           bones;

        Material               *material;
    };
    typedef deque<Lod>          LodQueue;

    struct Surface {
        LodQueue                lods;
        /// Special tag indicating that a given surface in calculation of collision
        bool                    collision;

        Modes                   mode;

        AABBox                  aabb;
    };
    typedef deque<Surface>      SurfaceQueue;

public:
    Mesh                        ();
    virtual ~Mesh               ();

    virtual void                apply               ();
    virtual void                clear               ();

    void                        makeDynamic         ();

    Material                   *material            (uint32_t surface, uint32_t lod) const;

    Vector3Vector               vertices            (uint32_t surface, uint32_t lod) const;

    IndexVector                 indices             (uint32_t surface, uint32_t lod) const;

    uint32_t                    surfacesCount       () const;

    uint32_t                    lodsCount           (uint32_t surface) const;

    uint32_t                    vertexCount         (uint32_t surface, uint32_t lod) const;

    uint32_t                    indexCount          (uint32_t surface, uint32_t lod) const;

    AABBox                      bound               () const;

    AABBox                      bound               (uint32_t surface) const;

    Modes                       mode                (uint32_t surface) const;

    uint8_t                     flags               () const;

    void                        setFlags            (uint8_t flags);

    void                        addSurface          (const Surface &surface);

    void                        setSurface          (uint32_t index, Surface &surface);

    void                        loadUserData        (const VariantMap &data);

protected:
    bool                        m_Dynamic;

    uint8_t                     m_Flags;

    SurfaceQueue                m_Surfaces;

    AABBox                      m_Box;
};

#endif // MESH_H
