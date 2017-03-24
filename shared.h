#ifndef SHARED_H
#define SHARED_H

#include <QOpenGLFunctions_4_4_Core>
#include <QFile>

#include <exception>
#include <memory>
#include <type_traits>
#include "state.h"
#include "library.h"

typedef QOpenGLFunctions_4_4_Core GL_t;
class GameWindow;

enum {
    KILOBYTE = 1024 * 1,
    MEGABYTE = 1024 * KILOBYTE,
    GIGABYTE = 1024 * MEGABYTE
};

namespace LuaApi {
    typedef Lua::lua_exception gl_error;
    typedef Lua::lua_exception al_error;

    template <typename T>
    class RefCounted {
        std::shared_ptr<T> m_data;
    public:
        RefCounted() {}
        RefCounted(RefCounted const&) =default;
        RefCounted(RefCounted &&) =default;
        RefCounted& operator= (RefCounted const&) =default;
        RefCounted& operator= (RefCounted &&) =default;
        
        template <typename ... Args>
        void Init(Args&& ... args)
        {
            m_data = std::make_shared<T>(std::forward<Args>(args)...);
        }
    
        bool IsValid() const {
            return m_data != nullptr;
        }
        
        void SoftRelease() {
            m_data.reset();
        }
        
        T& Get() const {
            if(m_data == nullptr)
                throw std::runtime_error(std::string("Usage of invalid entity: ") + MetatableDescriptor<T>::name());
            return *m_data;
        }
        
        T* operator -> () const {
            if(m_data == nullptr)
                throw std::runtime_error(std::string("Usage of invalid entity: ") + MetatableDescriptor<T>::name());
            return m_data.get();
        }
        
        T& operator * () const {
            return Get();
        }
        
         //Problem is around here.
         //I'm unable to correctly pass lua_State*, most likely.
        int RunFunction(lua_State* state, Lua::ClassMemberFunctor<T> fnc) {
            return fnc(Get(),state);
        }
        
        int IsValidFnc(lua_State* state, Lua::ClassMemberFunctor<T> fnc) {
            if(!IsValid())
            {
                lua_pushboolean(state,0);
                return 1;
            }
            return fnc(Get(),state);
        }
    };
}

template <typename T> struct MetatableDescriptor<LuaApi::RefCounted<T>> {
    static char const* name() { return MetatableDescriptor<T>::name(); }
    static char const* luaname() { return MetatableDescriptor<T>::luaname(); }
    static char const* constructor() { return MetatableDescriptor<T>::constructor(); }
    
    typedef decltype(MetatableDescriptor<T>::construct(std::declval<T*>())) valid_subclass;
    struct bad {};
    
    static bool construct(typename std::conditional<std::is_default_constructible<T>::value, LuaApi::RefCounted<T>, bad>::type *location) {
        if(!Lua::DefaultConstructor(location))
            return false;
        location->Init();
        return location->IsValid();
    }
    
    static bool construct(typename std::conditional<!std::is_default_constructible<T>::value, LuaApi::RefCounted<T>, bad>::type*) {
        return false;
    }

    /*static bool construct(LuaApi::RefCounted<T>* location) {
        if(!Lua::DefaultConstructor(location))
            return false;
        location->Init();
        return location->IsValid();
    }*/

    static void metatable(Lua::member_function_storage<LuaApi::RefCounted<T>>& mt) {
        Lua::member_function_storage<T> fnc;
        MetatableDescriptor<T>::metatable(fnc);
        
        for(auto it = fnc.begin(); it != fnc.end(); ++it)
        {
            std::string const& name = it->first;
            Lua::ClassMemberFunctor<T> const& fnc = it->second;
            mt[name] = Lua::ToFnc([=](LuaApi::RefCounted<T>& m, lua_State* s) -> int {
                return m.RunFunction(s,fnc);
            });
        }
        
        auto it = fnc.find("IsValid");
        if(it != fnc.end())
        {
            Lua::ClassMemberFunctor<T> const& fnc = it->second;
            mt["IsValid"] = Lua::ToFnc([=](LuaApi::RefCounted<T>& m, lua_State* s) -> int {
                return m.IsValidFnc(s,fnc);
            });
        }
        else
            mt["IsValid"] = Lua::Transform(&LuaApi::RefCounted<T>::IsValid);
    }
};

#endif // SHARED_H
