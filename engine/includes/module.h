#ifndef IMODULE_H
#define IMODULE_H

#include <engine.h>
#include <file.h>

class ISystem;
class IConverter;

#if defined(NEXT_SHARED) && defined(_WIN32)
    #define MODULE_EXPORT __declspec(dllexport)
#else
    #define MODULE_EXPORT
#endif


class ENGINE_EXPORT IModule {
public:
    enum PluginTypes {
        SYSTEM                      = (1<<0),
        EXTENSION                   = (1<<1),
        CONVERTER                   = (1<<2)
    };

public:
    IModule                         () {}

    virtual ~IModule                () {}

    virtual const char             *description             () const = 0;
    virtual const char             *version                 () const = 0;
    virtual uint8_t                 types                   () const = 0;

    virtual ISystem                *system                  () { return nullptr; }

    virtual IConverter             *converter               () { return nullptr; }

    virtual StringList              components              () const { return StringList(); }
};

#endif // IMODULE_H
