#ifndef METAMETHOD_H
#define METAMETHOD_H

#include <string>

#include "variant.h"
#include "metatype.h"
#include "event.h"

using namespace std;

class Object;

class NEXT_LIBRARY_EXPORT MetaMethod {
public:


    enum MethodType {
        Method                  = 0,
        Signal,
        Slot
    };

    struct Table {
        typedef void            (*InvokeMem)                (Object *, int argc, const Variant *, Variant &);
        typedef void            (*AddressMem)               (char *ptr, int size);

        MethodType              type;
        const char             *name;
        InvokeMem               invoker;
        AddressMem              address;
        int                     argc;
        const MetaType::Table **types;
    };

public:
    MetaMethod                  (const Table *table);

    bool                        isValid                     () const;

    const char                 *name                        () const;
    string                      signature                   () const;

    MethodType                  type                        () const;
    MetaType                    returnType                  () const;
    int                         parameterCount              () const;
    MetaType                    parameterType               (int index) const;

    bool                        invoke                      (Object *object, Variant &returnValue, int argc, const Variant *args) const;

    const Table                *table                       () const;

private:
    const Table                *m_pTable;

};

class MethodCallEvent : public Event {
public:
    MethodCallEvent             (int32_t method, Object *senderm, const Variant &args);

    Object                     *sender                      () const;

    int32_t                     method                      () const;

    const Variant              *args                        () const;

protected:
    Object                     *m_pSender;

    int32_t                     m_Method;

    Variant                     m_Args;
};

namespace unpack {
    template<unsigned...> struct indices {};

    template<unsigned N, unsigned... Is>
    struct indices_gen : indices_gen < N - 1, N - 1, Is... > {};

    template<unsigned... Is>
    struct indices_gen<0, Is...> : indices < Is... > {};
}

//Base template for function and methods invocation
template<typename Signature>
struct Invoker;

//Free functions
template<typename Return, typename... Args>
struct Invoker<Return(*)(Args...)> {
    // Workaround for the Visual Studio bug
    typedef Return(*Fun)(Args...);

    inline static int argCount() {
        return sizeof...(Args);
    }

    inline static const MetaType::Table **types(const char *typeName) {
        static const MetaType::Table *staticTypes[] = {
            Table<Return>::get(typeName),
            getTable<Args>()...
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *, F f, const Variant *args, unpack::indices<Is...>) {
        return f(args[Is]...); //any_cast<Args>(args[Is])...
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if(argc != sizeof...(Args)) {
            throw std::runtime_error("Bad argument count");
        }
        ret = invoke(obj, fun, args, unpack::indices_gen<sizeof...(Args)>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Free function with no parameters
template<typename Return>
struct Invoker<Return(*)()> {
    typedef Return(*Fun)();

    inline static int argCount() {
        return 0;
    }

    inline static const MetaType::Table **types(const char *typeName) {
        static const MetaType::Table *staticTypes[] = {
            Table<Return>::get(typeName)
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *, F f, const Variant *args, unpack::indices<Is...>) {
        return f(args[Is]...); // any_cast<Args>(args[Is])...
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if(argc != 0) {
            throw std::runtime_error("Bad argument count");
        }
        ret = invoke(obj, fun, args, unpack::indices_gen<0>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Void free function
template<typename... Args>
struct Invoker<void(*)(Args...)> {
    typedef void(*Fun)(Args...);

    inline static int argCount() {
        return sizeof...(Args);
    }

    inline static const MetaType::Table **types(const char *typeName) {
        A_UNUSED(typeName);
        static const MetaType::Table *staticTypes[] = {
            nullptr,
            getTable<Args>()...
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *, F f, const Variant *args, unpack::indices<Is...>) {
        f(args[Is]...); // any_cast<Args>(args[Is])...
        return Variant();
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if(argc != sizeof...(Args)) {
            throw std::runtime_error("Bad argument count");
        }
        ret = invoke(obj, fun, args, unpack::indices_gen<sizeof...(Args)>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Void free function with no arguments
template<>
struct Invoker<void(*)()> {
    typedef void(*Fun)();

    inline static int argCount() {
        return 0;
    }

    inline static const MetaType::Table **types(const char *typeName) {
        A_UNUSED(typeName);
        static const MetaType::Table *staticTypes[] = {
            nullptr
        };
        return staticTypes;
    }

    template<unsigned... Is>
    inline static Variant invoke(Object *, Fun f, const Variant *, unpack::indices<Is...>) {
        f();
        return Variant();
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if(argc != 0) {
            throw std::runtime_error("bad argument count");
        }
        ret = invoke(obj, fun, args, unpack::indices_gen<0>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Base method template
template<typename Class, typename Return, typename... Args>
struct Invoker<Return(Class::*)(Args...)> {
    typedef Return(Class::*Fun)(Args...);

    inline static int argCount() {
        return sizeof...(Args);
    }

    inline static const MetaType::Table **types(const char *typeName) {
        static const MetaType::Table *staticTypes[] = {
            Table<Return>::get(typeName),
            getTable<Args>()...
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *obj, F f, const Variant *args, unpack::indices<Is...>) {
        return (static_cast<Class *>(obj)->*f)(args[Is]...); // any_cast<Args>(args[Is])...
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if(argc != sizeof...(Args)) {
            throw std::runtime_error("Bad argument count");
        }
        ret = invoke(obj, fun, args, unpack::indices_gen<sizeof...(Args)>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Method with no parameters
template<typename Class, typename Return>
struct Invoker<Return(Class::*)()> {
    typedef Return(Class::*Fun)();

    inline static int argCount() {
        return 0;
    }

    inline static const MetaType::Table **types(const char *typeName) {
        static const MetaType::Table *staticTypes[] = {
            Table<Return>::get(typeName)
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *obj, F f, const Variant *, unpack::indices<Is...>) {
        return (static_cast<Class *>(obj)->*f)();
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if(argc != 0) {
            throw std::runtime_error("Bad argument count");
        }
        ret = invoke(obj, fun, args, unpack::indices_gen<0>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Void method
template<typename Class, typename... Args>
struct Invoker<void(Class::*)(Args...)> {
    typedef void(Class::*Fun)(Args...);

    inline static int argCount() {
        return sizeof...(Args);
    }

    inline static const MetaType::Table **types(const char *typeName) {
        A_UNUSED(typeName);
        static const MetaType::Table *staticTypes[] = {
            nullptr,
            getTable<Args>()...
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *obj, F f, const Variant *args, unpack::indices<Is...>) {
        (static_cast<Class *>(obj)->*f)(args[Is].value<Args>()...);
        return Variant();
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if (argc != sizeof...(Args)) {
            throw std::runtime_error("Bad argument count");
        }
        ret = invoke(obj, fun, args, unpack::indices_gen<sizeof...(Args)>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Void method with no parameters
template<typename Class>
struct Invoker<void(Class::*)()> {
    typedef void(Class::*Fun)();

    inline static int argCount() {
        return 0;
    }

    inline static const MetaType::Table **types(const char *typeName) {
        A_UNUSED(typeName);
        static const MetaType::Table *staticTypes[] = {
            nullptr
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *obj, F f, const Variant *args, unpack::indices<Is...>) {
        A_UNUSED(args);
        (static_cast<Class *>(obj)->*f)(args[Is]...); // any_cast<Args>(args[Is])...
        return Variant();
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if (argc != 0) {
            throw std::runtime_error("Bad argument count");
        }
        invoke(obj, fun, args, unpack::indices_gen<0>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Base const method template
template<typename Class, typename Return, typename... Args>
struct Invoker<Return(Class::*)(Args...)const> {
    typedef Return(Class::*Fun)(Args...)const;

    inline static int argCount() {
        return sizeof...(Args);
    }

    inline static const MetaType::Table **types(const char *typeName) {
        static const MetaType::Table *staticTypes[] = {
            Table<Return>::get(typeName),
            getTable<Args>()...
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *obj, F f, const Variant *args, unpack::indices<Is...>) {
        return (const_cast<const Class *>(static_cast<Class *>(obj))->*f)(args[Is]...); // any_cast<Args>(args[Is])...
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if (argc != sizeof...(Args)) {
            throw std::runtime_error("Bad argument count");
        }
        ret = invoke(obj, fun, args, unpack::indices_gen<sizeof...(Args)>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Const method with no parameters
template<typename Class, typename Return>
struct Invoker<Return(Class::*)()const> {
    typedef Return(Class::*Fun)()const;

    inline static int argCount() {
        return 0;
    }

    inline static const MetaType::Table **types(const char *typeName) {
        static const MetaType::Table *staticTypes[] = {
            Table<Return>::get(typeName)
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *obj, F f, const Variant *, unpack::indices<Is...>) {
        return (const_cast<const Class *>(static_cast<Class *>(obj))->*f)();
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if (argc != 0) {
            throw std::runtime_error("Bad argument count");
        }
        ret = invoke(obj, fun, args, unpack::indices_gen<0>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Void const method
template<typename Class, typename... Args>
struct Invoker<void(Class::*)(Args...)const> {
    typedef void(Class::*Fun)(Args...)const;

    inline static int argCount() {
        return sizeof...(Args);
    }

    inline static const MetaType::Table **types(const char *typeName) {
        A_UNUSED(typeName);
        static const MetaType::Table *staticTypes[] = {
            nullptr,
            getTable<Args>()...
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *obj, F f, const Variant *args, unpack::indices<Is...>) {
        (const_cast<const Class *>(static_cast<Class *>(obj))->*f)(args[Is]...); // any_cast<Args>(args[Is])...
        return Variant();
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if(argc != sizeof...(Args)) {
            throw std::runtime_error("Bad argument count");
        }
        ret = invoke(obj, fun, args, unpack::indices_gen<sizeof...(Args)>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

//Void const method with no parameters
template<typename Class>
struct Invoker<void(Class::*)()const> {
    typedef void(Class::*Fun)()const;

    inline static int argCount() {
        return 0;
    }

    inline static const MetaType::Table **types(const char *typeName) {
        A_UNUSED(typeName);
        static const MetaType::Table *staticTypes[] = {
            nullptr
        };
        return staticTypes;
    }

    template<typename F, unsigned... Is>
    inline static Variant invoke(Object *obj, F f, const Variant *args, unpack::indices<Is...>) {
        (const_cast<const Class *>(static_cast<Class *>(obj))->*f)(args[Is]...); // any_cast<Args>(args[Is])...
        return Variant();
    }

    template<Fun fun>
    static void invoke(Object *obj, int argc, const Variant *args, Variant &ret) {
        if(argc != 0) {
            throw std::runtime_error("Bad argument count");
        }
        invoke(obj, fun, args, unpack::indices_gen<0>());
    }

    template<Fun fun>
    static void address(char *ptr, int size) {
        Fun f   = fun;
        for(size_t n = 0; n < size; n++) {
            ptr[n]  = reinterpret_cast<const char *>(&f)[n];
        }
    }
};

#endif // METAMETHOD_H
