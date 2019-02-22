#ifndef TEXTMESH_H
#define TEXTMESH_H

#include "basemesh.h"

#include "font.h"

class Mesh;
class MaterialInstance;

class ENGINE_EXPORT TextMesh : public BaseMesh {
    A_REGISTER(TextMesh, BaseMesh, Components)

    A_PROPERTIES(
        A_PROPERTY(string, Text, TextMesh::text, TextMesh::setText),
        A_PROPERTY(Font, Font_Name, TextMesh::font, TextMesh::setFont),
        A_PROPERTY(int, Font_Size, TextMesh::fontSize, TextMesh::setFontSize),
        A_PROPERTY(Color, Color, TextMesh::color, TextMesh::setColor)
    )
    A_NOMETHODS()

public:
    TextMesh                    ();

    string                      text                () const;

    void                        setText             (const string &text);

    Font                       *font                () const;

    void                        setFont             (Font *font);

    int                         fontSize            () const;

    void                        setFontSize         (int size);

    Vector4                     color               () const;

    void                        setColor            (const Vector4 &color);

    void                        loadUserData        (const VariantMap &data);

    VariantMap                  saveUserData        () const;

protected:
    void                        composeMesh         ();

    Font                       *m_pFont;

    uint32_t                    m_Size;

    int32_t                     m_Space;

    int32_t                     m_Line;

    string                      m_Text;

    Vector4                     m_Color;

};

#endif // TEXTMESH_H
