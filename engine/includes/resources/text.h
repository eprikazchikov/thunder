#ifndef TEXT_H
#define TEXT_H

#include "engine.h"

class ENGINE_EXPORT Text : public Object {
    A_REGISTER(Text, Object, Resources)

public:
    Text                        ();

    virtual ~Text               ();

    const int8_t               *data                        () const;
    uint32_t                    size                        () const;

    string                      text                        () const;

protected:
    void                        loadUserData                (const VariantMap &data);

protected:
    ByteArray                   m_Data;

};

#endif // TEXT_H
