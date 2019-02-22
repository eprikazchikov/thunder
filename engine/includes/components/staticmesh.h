#ifndef STATICMESH_H
#define STATICMESH_H

#include "basemesh.h"

class ENGINE_EXPORT StaticMesh : public BaseMesh {
    A_REGISTER(StaticMesh, BaseMesh, Components)

    A_PROPERTIES (
        A_PROPERTY(Mesh *, Mesh, StaticMesh::mesh, StaticMesh::setMesh),
        A_PROPERTY(MateralArray, Materials, StaticMesh::materials, StaticMesh::setMaterials)
    )
    A_NOMETHODS()

};

#endif // STATICMESH_H
