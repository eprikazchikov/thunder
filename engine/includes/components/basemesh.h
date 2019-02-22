#ifndef BASEMESH_H
#define BASEMESH_H

#include "mesh.h"
#include "material.h"

#include "nativebehaviour.h"

#include <array>

class ENGINE_EXPORT BaseMesh : public NativeBehaviour {
    A_REGISTER(BaseMesh, NativeBehaviour, General);

    A_METHODS(
        A_METHOD(Material *, BaseMesh::material),
        A_METHOD(void, BaseMesh::setMaterial)
    )

public:
    BaseMesh                    ();

    void                        draw                    (ICommandBuffer &buffer, int8_t layer);

    Mesh                       *mesh                    () const;

    virtual void                setMesh                 (Mesh *mesh);

    MaterialArray               materials               () const;

    virtual void                setMaterials            (const MaterialArray &material);

    Material                   *material                (int index = 0) const;

    virtual void                setMaterial             (Material *material, int index = 0);

    uint32_t                    materialCount           () const;

    void                        loadUserData            (const VariantMap &data);

    VariantMap                  saveUserData            () const;

protected:
    Mesh                       *m_pMesh;

    MaterialArray               m_Materials;

};

#endif // BASEMESH_H
