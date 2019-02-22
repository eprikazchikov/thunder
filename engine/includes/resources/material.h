#ifndef SHADER
#define SHADER

#include "engine.h"
#include "texture.h"

class Material;

class ENGINE_EXPORT MaterialInstance {
public:
    struct Info {
        uint32_t                type;

        uint32_t                count;

        const void             *ptr;
    };

    typedef unordered_map<string, Info>             InfoMap;

public:
    MaterialInstance            (Material *material);

    ~MaterialInstance           ();

    Material                   *material            () const;

    const Texture              *texture             (const char *name);

    InfoMap                     params              () const;

    void                        setInteger          (const char *name, const int32_t *value, uint32_t count = 1);

    void                        setFloat            (const char *name, const float   *value, uint32_t count = 1);
    void                        setVector2          (const char *name, const Vector2 *value, uint32_t count = 1);
    void                        setVector3          (const char *name, const Vector3 *value, uint32_t count = 1);
    void                        setVector4          (const char *name, const Vector4 *value, uint32_t count = 1);

    void                        setMatrix4          (const char *name, const Matrix4 *value, uint32_t count = 1);

    void                        setTexture          (const char *name, const Texture *value, uint32_t count = 1);

protected:
    Material                   *m_pMaterial;

    InfoMap                     m_Info;
};

class ENGINE_EXPORT Material : public Object {
    A_REGISTER(Material, Object, Resources)

public:
    enum MaterialType {
        Surface,
        PostProcess,
        LightFunction
    };

    /*! \enum LightModelType */
    enum LightModelType {
        Unlit,
        Lit,
        Subsurface
    };

    /*! \enum BlendType */
    enum BlendType {
        Opaque,
        Additive,
        Translucent
    };

    enum SurfaceType {
        Static                  = (1<<0),
        Skinned                 = (1<<1),
        Billboard               = (1<<2),
        Oriented                = (1<<3)
    };

    typedef map<string, const Texture *>    TextureMap;

public:
    Material                    ();

    virtual ~Material           ();

    virtual void                clear                       ();

    void                        loadUserData                (const VariantMap &data);

    MaterialType                materialType                () const;

    LightModelType              lightModel                  () const;

    BlendType                   blendMode                   () const;

    bool                        isDoubleSided               () const;
    void                        setDoubleSided              (bool flag);

    bool                        isDepthTest                 () const;
    void                        setDepthTest                (bool flag);

    void                        setTexture                  (const string &name, const Texture *texture);

    uint8_t                     surfaces                    () const;

    virtual MaterialInstance   *createInstance              ();

protected:
    BlendType                   m_BlendMode;

    LightModelType              m_LightModel;

    MaterialType                m_MaterialType;

    bool                        m_DoubleSided;

    bool                        m_DepthTest;

    TextureMap                  m_Textures;

    uint8_t                     m_Surfaces;

};
typedef vector<MaterialInstance*>   MaterialArray;

#endif // SHADER

