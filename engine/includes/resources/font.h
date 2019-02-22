#ifndef FONT_H
#define FONT_H

#include "engine.h"
#include "atlas.h"

class FT_FaceRec_;

class ENGINE_EXPORT Font : public Atlas {
    A_REGISTER(Font, Atlas, Resources)

public:
    Font                        ();

    virtual ~Font               ();

    virtual void                clear                       ();

    uint32_t                    size                        () const;

    uint32_t                    atlasIndex                  (uint32_t character, uint32_t size = 0) const;

    void                        requestCharacters           (const u32string &characters, uint32_t size = 0);

    void                        setFontName                 (const string &name);

    uint32_t                    length                      (const u32string &characters) const;

    uint32_t                    spaceWidth                  (uint32_t size = 0) const;

    uint32_t                    lineHeight                  (uint32_t size = 0) const;

protected:
    void                        loadUserData                (const VariantMap &data);

protected:
    uint32_t                    m_Size;

    string                      m_FontName;

    FT_FaceRec_                *m_pFace;

    typedef unordered_map<uint32_t, uint32_t>   GlyphMap;

    GlyphMap                    m_GlyphMap;

    typedef unordered_map<uint32_t, Vector2>    SpecialMap;

    ByteArray                   m_Data;

};


#endif // FONT_H
