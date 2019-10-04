#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "resource.h"

class TranslatorPrivate;

class NEXT_LIBRARY_EXPORT Translator : public Resource {
    A_REGISTER(Translator, Resource, Resources)

public:
    Translator ();
    ~Translator () override;

    string translate(const string &source) const;

private:
    void loadUserData (const VariantMap &data) override;

private:
    TranslatorPrivate *p_ptr;

};

#endif // TRANSLATOR_H
