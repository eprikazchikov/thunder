#include "sprite.h"

#include "engine.h"
#include "texture.h"

#include "atlas.h"

#include <cstring>

#define HEADER  "Header"
#define DATA    "Data"
#define MESHES  "Meshes"

typedef deque<Texture *>  Textures;
typedef deque<Mesh *>  Meshes;

static Vector3Vector vertEmpty;
static IndexVector trisEmpty;

class SpritePrivate {
public:
    SpritePrivate() :
        m_pTexture(nullptr),
        m_pRoot(new AtlasNode) {

    }
    Meshes m_Meshes;

    Texture *m_pTexture;

    Textures m_Sources;

    AtlasNode *m_pRoot;
};

/*!
    \class Sprite
    \brief Represents 2D sprite.
    \inmodule Resource

    Sprites usually used in games to display environment and characters in 2D games.
    This class also supports sprite sheets to contain several images in one container to simplify animation or handle tile maps.
*/

Sprite::Sprite() :
        p_ptr(new SpritePrivate) {

    p_ptr->m_pTexture = Engine::objectCreate<Texture>("", this);
    p_ptr->m_pTexture->setFiltering(Texture::Bilinear);
    resize(1024, 1024);
}

Sprite::~Sprite() {
    PROFILE_FUNCTION();

    clearAtlas();

    delete p_ptr;
    p_ptr = nullptr;
}
/*!
    \internal
*/
void Sprite::clearAtlas() {
    PROFILE_FUNCTION();

    for(auto it : p_ptr->m_Meshes) {
        delete it;
    }
    p_ptr->m_Meshes.clear();

    for(auto it : p_ptr->m_Sources) {
        delete it;
    }
    p_ptr->m_Sources.clear();
}
/*!
    Adds new sub \a texture as element to current sprite sheet.
    All elements will be packed to a single sprite sheet texture using Sprite::pack() method.
    Returns the id of the new element.

    \sa pack()
*/
int Sprite::addElement(Texture *texture) {
    PROFILE_FUNCTION();

    p_ptr->m_Sources.push_back(texture);

    Lod lod;
    lod.setVertices({Vector3(0.0f, 0.0f, 0.0f),
                     Vector3(0.0f, 1.0f, 0.0f),
                     Vector3(1.0f, 1.0f, 0.0f),
                     Vector3(1.0f, 0.0f, 0.0f) });

    lod.setIndices({0, 1, 2, 0, 2, 3});

    Mesh *mesh = Engine::objectCreate<Mesh>("Mesh");
    mesh->addLod(&lod);
    p_ptr->m_Meshes.push_back(mesh);

    return (p_ptr->m_Sources.size() - 1);
}
/*!
    Packs all added elements int to a single sprite sheet.
    Parameter \a padding can be used to delimit elements.

    \sa addElement()
*/
void Sprite::pack(int padding) {
    PROFILE_FUNCTION();

    for(int i = 0; i < p_ptr->m_Sources.size(); i++) {
        Texture *it = p_ptr->m_Sources[i];
        Mesh *m = mesh(i);
        Lod *lod = m->lod(0);

        int32_t width  = (it->width() + padding * 2);
        int32_t height = (it->height() + padding * 2);

        AtlasNode *n = p_ptr->m_pRoot->insert(width, height);
        if(n && lod) {
            n->fill = true;
            int32_t w = n->w - padding * 2;
            int32_t h = n->h - padding * 2;

            Vector4 uv;
            uv.x = n->x / static_cast<float>(p_ptr->m_pRoot->w);
            uv.y = n->y / static_cast<float>(p_ptr->m_pRoot->h);
            uv.z = uv.x + w / static_cast<float>(p_ptr->m_pRoot->w);
            uv.w = uv.y + h / static_cast<float>(p_ptr->m_pRoot->h);

            int8_t *src = &(it->surface(0)[0])[0];
            int8_t *dst = &(p_ptr->m_pTexture->surface(0)[0])[0];
            for(int32_t y = padding; y < h; y++) {
                memcpy(&dst[(y + n->y) * p_ptr->m_pRoot->w + n->x], &src[y * w], w);
            }

            lod->setUv0({Vector2(uv.x, uv.y),
                         Vector2(uv.z, uv.y),
                         Vector2(uv.z, uv.w),
                         Vector2(uv.x, uv.w)});
        } else {
            resize(p_ptr->m_pRoot->w * 2, p_ptr->m_pRoot->h * 2);
            pack(padding);
        }
    }

    p_ptr->m_pTexture->setDirty();
}
/*!
    \internal
    Changes current size of the sprite sheet and sets resorce state to ResourceState::ToBeUpdated.
*/
void Sprite::resize(int32_t width, int32_t height) {
    PROFILE_FUNCTION();

    if(p_ptr->m_pRoot) {
        delete p_ptr->m_pRoot;
        p_ptr->m_pRoot = new AtlasNode;
    }
    p_ptr->m_pRoot->w = width;
    p_ptr->m_pRoot->h = height;

    p_ptr->m_pTexture->resize(width, height);
}
/*!
    \internal
*/
void Sprite::loadUserData(const VariantMap &data) {
    clearAtlas();
    {
        auto it = data.find(DATA);
        if(it != data.end()) {
            Object *object = ObjectSystem::toObject(it->second);
            Texture *texture = dynamic_cast<Texture *>(object);
            if(texture) {
                delete p_ptr->m_pTexture; // May lead to crash in case of m_pTexture had references
                p_ptr->m_pTexture = texture;
                p_ptr->m_pTexture->setParent(this);
            }
        }
    }
    {
        auto it = data.find(MESHES);
        if(it != data.end()) {
            for(auto mesh : it->second.toList()) {
                Object *object = ObjectSystem::toObject(mesh);
                Mesh *m = dynamic_cast<Mesh *>(object);
                if(m) {
                    p_ptr->m_Meshes.push_back(m);
                }
            }
        }
    }

    setState(Ready);
}
/*!
    \internal
*/
VariantMap Sprite::saveUserData() const {
    VariantMap result;

    Variant data = ObjectSystem::toVariant(p_ptr->m_pTexture);
    if(data.isValid()) {
        result[DATA] = data;
    }

    if(!p_ptr->m_Meshes.empty()) {
        VariantList meshes;

        for(auto it : p_ptr->m_Meshes) {
            meshes.push_back(ObjectSystem::toVariant(it));
        }
        result[MESHES] = meshes;
    }

    return result;
}
/*!
    Returns a mesh which represents the sprite with \a index.
*/
Mesh *Sprite::mesh(int index) const {
    PROFILE_FUNCTION();

    if(index > -1 && index < p_ptr->m_Meshes.size()) {
        return p_ptr->m_Meshes[index];
    }
    return nullptr;
}
/*!
    Sets a new mesh for the sprite with \a index.
    The old mesh will be deleted and no longer available.
    \note The in case of \a create flag was set to true the spite will be created in case it doesn't exist yet.
*/
void Sprite::setMesh(int index, Mesh *mesh, bool create) {
    PROFILE_FUNCTION();

    if(index > -1 && index < p_ptr->m_Meshes.size()) {
        if(p_ptr->m_Meshes[index]) {
            delete p_ptr->m_Meshes[index];
        }
        p_ptr->m_Meshes[index] = mesh;
    } else if(create) {
        p_ptr->m_Meshes.insert(p_ptr->m_Meshes.begin() + index, mesh);
    }
}
/*!
    Returns a sprite sheet texture.
*/
Texture *Sprite::texture() const {
    PROFILE_FUNCTION();

    return p_ptr->m_pTexture;
}
/*!
    Sets a new sprite sheet \a texture.
*/
void Sprite::setTexture(Texture *texture) {
    PROFILE_FUNCTION();

    if(p_ptr->m_pTexture) {
        delete p_ptr->m_pTexture;
    }
    p_ptr->m_pTexture = texture;
    p_ptr->m_pTexture->setParent(this);
}
