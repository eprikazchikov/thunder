#ifndef ENGINE_H
#define ENGINE_H

#include <cstdint>
#include <string>
#include <map>

#include <aobject.h>
#include <aobjectsystem.h>

class IFile;

class IModule;
class IController;

class EnginePrivate;

class NEXT_LIBRARY_EXPORT Engine : public AObjectSystem {
public:
    enum ModeTypes {
        EDIT                    = (1<<0),
        GAME                    = (1<<1)
    };

public:
    Engine                      (IFile *file, int argc, char **argv);
    ~Engine                     ();
/*
    Main system
*/
    bool                        init                        ();

    int32_t                     exec                        ();
/*
    Settings
*/
    static AVariant             value                       (const string &key, const AVariant &defaultValue = AVariant());

    static void                 setValue                    (const string &key, const AVariant &value);
/*
    Resource management
*/
    static AObject             *loadResource                (const string &path = string());

    template<typename T>
    static T                   *loadResource                (const string &path) {
        return dynamic_cast<T *>(loadResource(path));
    }

    static string               reference                   (AObject *object);

    static void                 reloadBundle                ();
/*
    Misc
*/
    void                        addModule                   (IModule *mode);

    bool                        createWindow                ();

    IController                *controller                  ();
    /*!
        Get FileIO object.

        @return                 Pointer to file system object.
    */
    IFile                      *file                        ();

    static string               locationAppDir              ();

    static string               locationConfig              ();

    static string               locationAppConfig           ();

    string                      applicationName             () const;

    void                        setApplicationName          (const string &name);

    string                      organizationName            () const;

    void                        setOrganizationName         (const string &name);

private:
    EnginePrivate              *p_ptr;

};

#endif // ENGINE_H
