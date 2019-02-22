#include "resources/texture.h"

#include <variant.h>

#include <cstring>

#define HEADER  "Header"
#define DATA    "Data"

class Node {
public:
    Node() :
            fill(false),
            x(0),
            y(0),
            w(1),
            h(1) {
        child[0]    = nullptr;
        child[1]    = nullptr;
    }

    Node *insert(const Texture *texture, uint8_t padding) {
        if(child[0]) {
            Node *node  = child[0]->insert(texture, padding);
            if(node) {
                return node;
            }
            return child[1]->insert(texture, padding);
        }

        uint32_t width  = (texture->width() + padding);
        uint32_t height = (texture->height() + padding);

        if(fill || w < width || h < height) {
            return nullptr;
        }

        if(w == width && h == height) {
            return this;
        }
        // Texture is smaller then node start splitting
        uint32_t sw = w - width;
        uint32_t sh = h - height;

        child[0]    = new Node;
        child[1]    = new Node;

        if(sw > sh) {    // Horizontal
            child[0]->x = x;
            child[0]->y = y;
            child[0]->w = width;
            child[0]->h = h;

            child[1]->x = x + width;
            child[1]->y = y;
            child[1]->w = sw;
            child[1]->h = h;
        } else {    // Vertical
            child[0]->x = x;
            child[0]->y = y;
            child[0]->w = w;
            child[0]->h = height;

            child[1]->x = x;
            child[1]->y = y + height;
            child[1]->w = w;
            child[1]->h = sh;
        }

        return child[0]->insert(texture, padding);
    }

    bool            fill;

    uint32_t        x;
    uint32_t        y;
    uint32_t        w;
    uint32_t        h;

    Node           *child[2];
};

Texture::Texture() :
        m_Format(R8),
        m_Compress(Uncompressed),
        m_Type(Flat),
        m_Filtering(None),
        m_Wrap(Clamp),
        m_pRoot(nullptr) {
    setShape({ Vector2(0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f), Vector2(1.0f, 0.0f) });
}

Texture::~Texture() {
    clear();
}

void Texture::apply() {

}

void Texture::loadUserData(const VariantMap &data) {
    clear();

    {
        auto it = data.find(HEADER);
        if(it != data.end()) {
            VariantList header  = (*it).second.value<VariantList>();

            auto i      = header.begin();
            m_Width     = (*i).toInt();
            i++;
            m_Height    = (*i).toInt();
            i++;
            //Reserved
            i++;

            m_Type      = TextureType((*i).toInt());
            i++;
            m_Compress  = CompressionType((*i).toInt());
            i++;
            m_Format    = FormatType((*i).toInt());
            i++;
            m_Filtering = FilteringType((*i).toInt());
            i++;
            m_Wrap      = WrapType((*i).toInt());
            i++;
        }
    }

    {
        auto it = data.find(DATA);
        if(it != data.end()) {
            const VariantList &surfaces = (*it).second.value<VariantList>();
            for(auto s : surfaces) {
                Surface img;
                uint32_t w  = m_Width;
                uint32_t h  = m_Height;
                const VariantList &lods = s.value<VariantList>();
                for(auto l : lods) {
                    ByteArray bits = l.toByteArray();
                    uint32_t s  = size(w, h);
                    if(s && !bits.empty()) {
                        uint8_t *pixels = new uint8_t[s];
                        memcpy(pixels, &bits[0], s);
                        img.push_back(pixels);
                    }
                    w   = MAX(w / 2, 1);
                    h   = MAX(h / 2, 1);
                }
                m_Sides.push_back(img);
            }
        }
    }

    apply();
}

void Texture::addSurface(const Surface &surface) {
    m_Sides.push_back(surface);
}

void Texture::clear() {
    m_Width     = 1;
    m_Height    = 1;

    for(auto side : m_Sides) {
        for(auto lod : side) {
            delete []lod;
        }
    }
    m_Sides.clear();
    m_Shape.clear();

    if(m_pRoot) {
        delete m_pRoot;
    }
    m_pRoot     = new Node;
}

void *Texture::nativeHandle() const {
    return nullptr;
}

void Texture::readPixels(int32_t x, int32_t y, uint32_t width, uint32_t height) {
    A_UNUSED(x)
    A_UNUSED(y)
    A_UNUSED(width)
    A_UNUSED(height)
}

uint32_t Texture::getPixel(int32_t x, int32_t y) const {
    uint32_t result = 0;
    if(!m_Sides.empty() && !m_Sides[0].empty()) {
        uint8_t *ptr    = m_Sides[0][0] + (y * m_Width + x);
        memcpy(&result, ptr, sizeof(uint32_t));
    }
    return result;
}

uint32_t Texture::width() const {
    return m_Width;
}

uint32_t Texture::height() const {
    return m_Height;
}

void Texture::setWidth(uint32_t width) {
    m_Width     = width;
}

void Texture::setHeight(uint32_t height) {
    m_Height    = height;
}

uint32_t Texture::size(uint32_t width, uint32_t height) const {
    uint32_t (Texture::*sizefunc)(uint32_t, uint32_t) const;
    sizefunc    = (isCompressed() ? &Texture::sizeDXTc : &Texture::sizeRGB);

    return (this->*sizefunc)(width, height);
}

inline uint32_t Texture::sizeDXTc(uint32_t width, uint32_t height) const {
    return ((width + 3) / 4) * ((height + 3) / 4) * (m_Compress == DXT1 ? 8 : 16);
}

inline uint32_t Texture::sizeRGB(uint32_t width, uint32_t height) const {
    return width * height * components();
}

Vector2Vector Texture::shape() const {
    return m_Shape;
}

void Texture::setShape(const Vector2Vector &shape) {
    m_Shape = shape;
}

Vector4Vector Texture::pack(const Textures &textures, uint8_t padding) {
    Vector4Vector result;
    for(auto it : textures) {
        Node *n = m_pRoot->insert(it, padding);
        if(n) {
            n->fill = true;
            /// \todo can be optimized to do all copies in the end of packing
            uint8_t *src    = it->m_Sides[0][0];
            uint8_t *dst    = m_Sides[0][0];
            uint32_t w      = n->w - padding;
            uint32_t h      = n->h - padding;
            for(uint32_t y = 0; y < h; y++) {
                memcpy(&dst[(y + n->y) * m_Width + n->x], &src[y * w], w);
            }

            Vector4 res;
            res.x   = n->x / (float)m_Width;
            res.y   = n->y / (float)m_Height;
            res.z   = res.x + w / (float)m_Width;
            res.w   = res.y + h / (float)m_Height;

            result.push_back(res);
        } else {
            resize(m_Width * 2, m_Height * 2);
            return pack(textures, padding);
        }
    }
    return result;
}

void Texture::resize(uint32_t width, uint32_t height) {
    clear();

    m_Width     = width;
    m_Height    = height;

    m_pRoot->w  = m_Width;
    m_pRoot->h  = m_Height;
    int32_t length  = size(m_Width, m_Height);
    uint8_t *pixels = new uint8_t[length];
    memset(pixels, 0, length);
    Texture::Surface s;
    s.push_back(pixels);
    addSurface(s);
}

void Texture::setFormat(FormatType type) {
    m_Format    = type;
}

bool Texture::isCompressed() const {
    return m_Compress != Uncompressed;
}

bool Texture::isCubemap() const {
    return (m_Type == Cubemap);
}

uint8_t Texture::components() const {
    switch(m_Format) {
        case R8:    return 1;
        case RGB8:  return 3;
        default: break;
    }
    return 4;
}
