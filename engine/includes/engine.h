#ifndef ENGINE_H
#define ENGINE_H

#include "engine_export.h"

#include <cstdint>
#include <string>
#include <map>

#include <objectsystem.h>
#include <file.h>

class IModule;

class EnginePrivate;

class Actor;
class Scene;

class ENGINE_EXPORT Engine : public ObjectSystem {
public:
    Engine                      (IFile *file, int argc, char **argv);
    ~Engine                     ();
/*
    Main system
*/
    bool                        init                        ();

    bool                        start                       ();

    void                        resize                      ();

    void                        update                      ();
/*
    Settings
*/
    static Variant              value                       (const string &key, const Variant &defaultValue = Variant());

    static void                 setValue                    (const string &key, const Variant &value);
/*
    Resource management
*/
    static Object              *loadResourceImpl            (const string &path);

    static void                 unloadResource              (const string &path);

    template<typename T>
    static T                   *loadResource                (const string &path) {
        return dynamic_cast<T *>(loadResourceImpl(path));
    }

    static string               reference                   (Object *object);

    static void                 reloadBundle                ();

/*
    Misc
*/
    static bool                 isGameMode                  ();

    static void                 setGameMode                 (bool game);

    void                        addModule                   (IModule *mode);

    Scene                      *scene                       ();

    static IFile               *file                        ();

    static string               locationAppDir              ();

    static string               locationConfig              ();

    static string               locationAppConfig           ();

    string                      applicationName             () const;

    void                        setApplicationName          (const string &name);

    string                      organizationName            () const;

    void                        setOrganizationName         (const string &name);

    void                        updateScene                 (Scene *scene);

    static void                 setResource                 (Object *object, string &uuid);

private:
    EnginePrivate              *p_ptr;

};

#endif // ENGINE_H
