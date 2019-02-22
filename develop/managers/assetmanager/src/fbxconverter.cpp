#include "fbxconverter.h"

#include "resources/mesh.h"
#include "file.h"
#include "log.h"
#include "converters/converter.h"
#include "projectmanager.h"

#include <fbxsdk.h>
#include <bson.h>

#include <cstring>

#include <QFileInfo>
#include <QTime>

#define HEADER  "Header"
#define DATA    "Data"

Matrix3 gInvert;

struct index_data {
    uint32_t        vIndex;
    uint32_t        uIndex;
    uint32_t        xIndex;

    bool operator== (const index_data &right) {
        return (vIndex == right.vIndex) && (uIndex == right.uIndex);
    }
};
typedef vector<index_data>  indexVector;

VariantMap MeshSerial::saveUserData() const {
    VariantMap result;

    VariantList header;
    header.push_back(m_Flags);
    result[HEADER]  = header;

    VariantList surfaces;
    for(auto s : m_Surfaces) {
        VariantList surface;
        surface.push_back(s.collision);
        surface.push_back(s.mode);

        for(auto l : s.lods) {
            VariantList lod;
            string path = Engine::reference(l.material);
            // Push material
            lod.push_back(path);
            uint32_t vCount = l.vertices.size();
            lod.push_back((int)vCount);
            lod.push_back((int)l.indices.size() / 3);

            { // Required field
                ByteArray buffer;
                buffer.resize(sizeof(Vector3) * vCount);
                memcpy(&buffer[0], &l.vertices[0], sizeof(Vector3) * vCount);
                lod.push_back(buffer);
            }
            { // Required field
                ByteArray buffer;
                buffer.resize(sizeof(uint32_t) * l.indices.size());
                memcpy(&buffer[0], &l.indices[0], sizeof(uint32_t) * l.indices.size());
                lod.push_back(buffer);
            }
            if(m_Flags & ATTRIBUTE_COLOR) { // Optional field
                ByteArray buffer;
                buffer.resize(sizeof(Vector4) * vCount);
                memcpy(&buffer[0], &l.colors[0], sizeof(Vector4) * vCount);
                lod.push_back(buffer);
            }
            if(m_Flags & ATTRIBUTE_UV0) { // Optional field
                ByteArray buffer;
                buffer.resize(sizeof(Vector2) * vCount);
                memcpy(&buffer[0], &l.uv0[0], sizeof(Vector2) * vCount);
                lod.push_back(buffer);
            }
            if(m_Flags & ATTRIBUTE_UV1) { // Optional field
                ByteArray buffer;
                buffer.resize(sizeof(Vector2) * vCount);
                memcpy(&buffer[0], &l.uv1[0], sizeof(Vector2) * vCount);
                lod.push_back(buffer);
            }

            if(m_Flags & ATTRIBUTE_NORMALS) { // Optional field
                ByteArray buffer;
                buffer.resize(sizeof(Vector3) * vCount);
                memcpy(&buffer[0], &l.normals[0], sizeof(Vector3) * vCount);
                lod.push_back(buffer);
            }
            if(m_Flags & ATTRIBUTE_TANGENTS) { // Optional field
                ByteArray buffer;
                buffer.resize(sizeof(Vector3) * vCount);
                memcpy(&buffer[0], &l.tangents[0], sizeof(Vector3) * vCount);
                lod.push_back(buffer);
            }
            if(m_Flags & ATTRIBUTE_ANIMATED) { // Optional field
                {
                    ByteArray buffer;
                    buffer.resize(sizeof(Vector4) * vCount);
                    memcpy(&buffer[0], &l.weights[0], sizeof(Vector4) * vCount);
                    lod.push_back(buffer);
                }
                {
                    ByteArray buffer;
                    buffer.resize(sizeof(Vector4) * vCount);
                    memcpy(&buffer[0], &l.bones[0], sizeof(Vector4) * vCount);
                    lod.push_back(buffer);
                }
            }
            surface.push_back(lod);
        }
        surfaces.push_back(surface);
    }
    result[DATA]    = surfaces;

    return result;
}

FBXConverter::FBXConverter() {
    gInvert[0]    =-1.0;
    gInvert[1]    = 0.0;
    gInvert[2]    = 0.0;

    gInvert[3]    = 0.0;
    gInvert[4]    = 0.0;
    gInvert[5]    = 1.0;

    gInvert[6]    = 0.0;
    gInvert[7]    = 1.0;
    gInvert[8]    = 0.0;

}

uint8_t FBXConverter::convertFile(IConverterSettings *settings) {
    MeshSerial mesh;

    importFBX(settings->source(), mesh);

    //exportAnimation(dst);

    for(auto &surface : mesh.m_Surfaces) {
        for(auto &lod : surface.lods) {
            lod.material    = nullptr;
        }
    }

    QFile file(settings->absoluteDestination());
    if(file.open(QIODevice::WriteOnly)) {
        ByteArray data  = Bson::save(Engine::toVariant(&mesh));
        file.write((const char *)&data[0], data.size());
        file.close();
        return 0;
    }

    return 1;
}

void FBXConverter::importFBX(const string &src, Mesh &mesh) {
    FbxManager *lSdkManager = FbxManager::Create();
    // Create an IOSettings object.
    FbxIOSettings *ios      = FbxIOSettings::Create( lSdkManager, IOSROOT );
    lSdkManager->SetIOSettings(ios);

    FbxImporter *lImporter  = FbxImporter::Create( lSdkManager, "" );
    // Initialize the importer.
    if(lImporter->Initialize(src.c_str(), -1, lSdkManager->GetIOSettings()) == false) {
        Log(Log::ERR) << "Call to KFbxImporter::Initialize() failed." ;
        return;
    }
    // Create a new scene so it can be populated by the imported file.
    FbxScene *lScene    = FbxScene::Create(lSdkManager, "Scene");
    // Import the contents of the file into the scene.
    lImporter->Import(lScene);

    // File format version numbers to be populated.
    int lFileMajor, lFileMinor, lFileRevision;
    // Populate the FBX file format version numbers with the import file.
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    importDynamic(lScene->GetRootNode(), mesh);

    lScene->Destroy(true);
    lImporter->Destroy();
    ios->Destroy();
    lSdkManager->Destroy();
}

void FBXConverter::importDynamic(FbxNode *lRootNode, Mesh &mesh) {
    vector<FbxCluster *> bones;

    QTime t;
    t.start();

    for(int n = 0; n < lRootNode->GetChildCount(); n++) {
        FbxNode *node               = lRootNode->GetChild(n);
        FbxNodeAttribute *attrib    = node->GetNodeAttribute();
        if(attrib && attrib->GetAttributeType() == FbxNodeAttribute::eMesh) {
            FbxMesh *m = (FbxMesh *)attrib;
/*
            bool valid  = false;
            for(int d = 0; d < m->GetDeformerCount(); d++) {
                KFbxDeformer *deformer	= m->GetDeformer(d);
                if(deformer->GetDeformerType() == KFbxDeformer::eSKIN) {
                    mesh.type       = MESH_ANIMATED;
                    KFbxSkin *skin  = (KFbxSkin *)deformer;
                    // Export bone hierarchy
                    for(int s = 0; s < skin->GetClusterCount(); s++) {
                        KFbxCluster *bone   = skin->GetCluster(s);
                        int j;
                        for(j = 0; j < bones.size(); j++) {
                            if(bones[j] == bone) break;
                        }
                        if(j != bones.size())
                            continue;

                        bones.push_back(bone);
                    }
                    valid	= true;
                    break;
                }
            }
            if(!valid)
                continue;
*/
            Mesh::Surface s;
            Mesh::Lod l;

            mesh.setFlags(0);

            s.collision	= false;
            s.mode      = Mesh::MODE_TRIANGLES;

            indexVector indices;

            // Export
            FbxVector4 *verts                   = m->GetControlPoints();
            FbxGeometryElementUV *uv            = m->GetElementUV();
            if(uv) {
                mesh.setFlags(mesh.flags() | Mesh::ATTRIBUTE_UV0);
            } else {
                Log(Log::WRN) << "No uv exist";
            }

            FbxGeometryElementNormal *normals   = m->GetElementNormal();
            if(normals) {
                mesh.setFlags(mesh.flags() | Mesh::ATTRIBUTE_NORMALS);
            } else {
                Log(Log::WRN) << "No normals exist";
            }

            FbxGeometryElementTangent *tangents = m->GetElementTangent();
            if(tangents) {
                mesh.setFlags(mesh.flags() | Mesh::ATTRIBUTE_TANGENTS);
            } else {
                Log(Log::WRN) << "No tangents exist";
            }

            int tCount  = m->GetPolygonCount();
            for(int triangle = 0; triangle < tCount; triangle++) {
                int size    = m->GetPolygonSize(triangle);
                for(int h = 0; h < ((size == 4) ? 2 : 1); h++) {
                    for(int k = 0; k < 3; k++) {
                        index_data data;
                        data.vIndex = m->GetPolygonVertex(triangle, (k == 0) ? k : k + h);
                        data.uIndex	= m->GetTextureUVIndex(triangle, (k == 0) ? k : k + h);
                        data.xIndex = m->GetPolygonVertexIndex(triangle) + ((k == 0) ? k : k + h);

                        int count	= indices.size();

                        bool create	= true;
                        for(int i = 0; i < count; i++) {
                            if(indices[i] == data) {
                                // Vertex already exist. Just add it into indices list
                                l.indices.push_back(i);
                                create	= false;
                                break;
                            }
                        }

                        // Add vertex to arrays
                        if(create) {
                            indices.push_back(data);

                            FbxVector4 v    = verts[data.vIndex];
                            l.vertices.push_back(gInvert * Vector3(v[0], v[1], v[2]));
                            l.indices.push_back(count);

                            if(normals) {
                                switch (normals->GetReferenceMode()) {
                                    case FbxLayerElement::eDirect: {
                                        v   = normals->GetDirectArray().GetAt(data.xIndex);
                                    } break;
                                    case FbxLayerElement::eIndexToDirect: {
                                        int index   = normals->GetIndexArray().GetAt(data.xIndex);
                                        v           = normals->GetDirectArray().GetAt(index);
                                    } break;
                                    default: {
                                        Log(Log::ERR) << "Invalid normals reference mode";
                                    } break;
                                }
                                l.normals.push_back(gInvert * Vector3(v[0], v[1], v[2]));
                            }

                            if(tangents) {
                                switch (tangents->GetReferenceMode()) {
                                    case FbxLayerElement::eDirect: {
                                        v       = tangents->GetDirectArray().GetAt(data.xIndex);
                                    } break;
                                    case FbxLayerElement::eIndexToDirect: {
                                        int index   = tangents->GetIndexArray().GetAt(data.xIndex);
                                        v           = tangents->GetDirectArray().GetAt(index);
                                    } break;
                                    default: {
                                        Log(Log::ERR) << "Invalid tangents reference mode";
                                    } break;

                                }
                                l.tangents.push_back(gInvert * Vector3(v[0], v[1], v[2]));
                            }

                            if(uv) {
                                v       = uv->GetDirectArray().GetAt(data.uIndex);
                                l.uv0.push_back(Vector2(v[0], v[1]));
                            }
    /*
                            if(mesh.isAnimated()) {
                                int w	= 0;
                                for(int b = 0; b < bones.size(); b++) {
                                    int *indices	= bones[b]->GetControlPointIndices();
                                    double *weights = bones[b]->GetControlPointWeights();
                                    for(int i = 0; i < bones[b]->GetControlPointIndicesCount(); i++) {
                                        if(indices[i] == data.vIndex) {
                                            //KFbxNode *bone  = bones[b]->GetLink();

                                            vert.index[w]	= (float)b;
                                            vert.weight[w]	= (float)weights[i];

                                            w++;
                                        }
                                    }
                                }
                            }
    */
                        } // end Creation
                    }
                }
            }
            s.lods.push_back(l);
            mesh.addSurface(s);
        }
    }

    Log(Log::INF) << "Mesh imported in:" << t.elapsed() << "msec";
/*
    if(pMesh->type == MESH_ANIMATED) {
        pMesh->jCount			= bones.size();
        pMesh->aAnim->priority	= new char[pMesh->jCount];

        pMesh->jArray			= new joint_data[pMesh->jCount];
        for(int s = 0; s < pMesh->jCount; s++) {
            KFbxNode *bone		= bones[s]->GetLink();
            KFbxNode *parent	= bone->GetParent();
            int j;
            for(j = 0; j < pMesh->jCount; j++)
                if(bones[j]->GetLink() == parent) break;

            // Getting Joint priority (Using with animation blending)
            int prop	= 100;
            pMesh->aAnim->priority[s]		= prop;

            if(j == bones.size()) {
                pMesh->jArray[s].parent		= 0; // Root Joint
                pMesh->jArray[s].iparent	= ROOT;
            } else {
                pMesh->jArray[s].parent		= &pMesh->jArray[j];
                pMesh->jArray[s].iparent	= j;
            }

            memset(pMesh->jArray[s].name, 0, 32);
            strncpy(pMesh->jArray[s].name, bone->GetName(), 31);

            int proxy	= 0;
            pMesh->jArray[s].proxy		= proxy;

            int emitter	= 0;
            pMesh->jArray[s].emitter	= emitter;
        }
    }
*/
}

void FBXConverter::importAFAnimation(FbxScene *lScene) {
/*
    KTime gPeriod, gStart, gStop, gCurrent;
    gPeriod.SetTime(0, 0, 0, 1, 0, lScene->GetGlobalSettings().GetTimeMode());

    KTimeSpan lTimeLineTimeSpan;
    lScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
    gStart			= lTimeLineTimeSpan.GetStart();
    gStop			= lTimeLineTimeSpan.GetStop();

    pMesh->aAnim.fCount = static_cast<int>((gStop - gStart).GetSecondDouble() / gPeriod.GetSecondDouble());
    pMesh->aAnim.frames	= new ASFrame * [pMesh->jCount];

    int joint;
    for(joint = 0; joint < pMesh->jCount; joint++) {
        KFbxNode *bone	= pMesh->bones[joint]->GetLink();
        pMesh->aAnim.frames[joint]	= new ASFrame[pMesh->aAnim.fCount + 1];

        int cur_frame	= 0;
        for(gCurrent = gStart; gCurrent < gStop; gCurrent += gPeriod) {
            KFbxXMatrix mTransform	= bone->EvaluateGlobalTransform(gCurrent);

            Matrix3 transform;
            transform.mat[0]	= mTransform.mData[0].mData[0];
            transform.mat[1]	= mTransform.mData[0].mData[1];
            transform.mat[2]	= mTransform.mData[0].mData[2];

            transform.mat[3]	= mTransform.mData[1].mData[0];
            transform.mat[4]	= mTransform.mData[1].mData[1];
            transform.mat[5]	= mTransform.mData[1].mData[2];

            transform.mat[6]	= mTransform.mData[2].mData[0];
            transform.mat[7]	= mTransform.mData[2].mData[1];
            transform.mat[8]	= mTransform.mData[2].mData[2];

            Matrix3 m;
            Vector3 vec;
            Quaternion  quat;

            KFbxVector4	p		= mTransform.GetT();
            vec					= invert * Vector3(p.GetAt(0), p.GetAt(1), p.GetAt(2));
            pMesh->aAnim.frames[joint][cur_frame].vector		= vec;

            m					= transform;
            m.orthonormalize();

            m					= invert * m;
            m					= m.inverse();

            quat				= Quaternion (m);
            pMesh->aAnim.frames[joint][cur_frame].quaternion	= quat;

            cur_frame++;
        }
    }

    for(joint = 0; joint < pMesh->jCount; joint++) {
        for(int frame = 0; frame < pMesh->aAnim.fCount; frame++) {
            int iparent	= pMesh->jArray[joint].iparent;
            Vector3 parent	= Vector3(0, 0, 0);

            if(iparent != -1) {
                parent	= pMesh->aAnim.frames[iparent][frame].vector;
            }

            pMesh->aAnim.frames[joint][frame].relative	= pMesh->aAnim.frames[joint][frame].vector - parent;
        }
    }
*/
}

void FBXConverter::saveAnimation(const string &dst) {
/*
    FILE *fp	= fopen(filename, "wb");
    if(fp) {
        fprintf(fp, AB_SIGNATURE);

        fwrite(&pMesh->aAnim.fCount,	sizeof(short), 1, fp);
        // Save joint animation priority
        fwrite(pMesh->aAnim.priority,	sizeof(char) * pMesh->jCount,	1, fp);
        // Save count of joint animation keys
//		fwrite(pMesh->aAnim.kCount,		sizeof(short) * pMesh->jCount,	1, fp);

        for(int b = 0; b < pMesh->jCount; b++) {
            // Save name of bone
            fwrite(pMesh->jArray[b].name, 32, 1, fp);
            for (int cur_frame = 0; cur_frame < pMesh->aAnim.fCount; cur_frame++) { // pMesh->aAnim.kCount[b]
//				fwrite(&pMesh->aAnim.frames[b][cur_frame].fPos,			sizeof(short),			1, fp);
                fwrite(&pMesh->aAnim.frames[b][cur_frame].quaternion,	sizeof(Quaternion ),	1, fp);

                if(ABSOLUTE_POS) {
                    fwrite(&pMesh->aAnim.frames[b][cur_frame].vector,	sizeof(Vector3),		1, fp);
                } else {
                    fwrite(&pMesh->aAnim.frames[b][cur_frame].relative,	sizeof(Vector3),		1, fp);
                }
            }
        }
        fclose(fp);
    }
*/
}
