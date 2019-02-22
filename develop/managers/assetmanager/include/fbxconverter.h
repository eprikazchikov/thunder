#ifndef FBXCONVERTER_H
#define FBXCONVERTER_H

namespace fbxsdk
{
    class FbxNode;
    class FbxScene;
}

#include "converters/converter.h"

#include "resources/mesh.h"

class IFile;

class MeshSerial : public Mesh {
public:

    VariantMap                  saveUserData                () const;

protected:
    friend class FBXConverter;

};

class FBXConverter : public IConverter {
public:
    FBXConverter                ();

    QStringList suffixes() const { return {"fbx"}; }
    uint32_t                    contentType             () const { return ContentMesh; }
    uint32_t                    type                    () const { return MetaType::type<Mesh *>(); }
    uint8_t                     convertFile             (IConverterSettings *);

protected:
    void                        importFBX               (const string &src, Mesh &mesh);
    void                        importDynamic           (fbxsdk::FbxNode *lRootNode, Mesh &mesh);
    void                        importAFAnimation       (fbxsdk::FbxScene *lScene);

    void                        saveAnimation           (const string &dst);

};

#endif // FBXCONVERTER_H
