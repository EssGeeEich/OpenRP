#ifndef LUAGL_OBJECT_H
#define LUAGL_OBJECT_H
#include "shared.h"
#include "objectbone.h"

namespace LuaApi {
	class ObjectImpl {
        std::vector<ObjectBone> m_bones;
    public:
        bool load(std::string const&);
        
        std::size_t BoneCount() const;
        Lua::ReturnValues GetBoneByNumber(std::size_t);
        Lua::ReturnValues GetBoneByName(std::string const&);
    };
    typedef LuaApi::RefCounted<ObjectImpl> Object;
}

template <> struct MetatableDescriptor<LuaApi::ObjectImpl> {
    static char const* name() { return "object_mt"; }
    static char const* luaname() { return "Object"; }
    static char const* constructor() { return "New"; }
    static bool construct(LuaApi::ObjectImpl* v) { return Lua::DefaultConstructor(v); }
    static void metatable(Lua::member_function_storage<LuaApi::ObjectImpl>& mt) {
        mt["LoadFile"] = Lua::Transform(&LuaApi::ObjectImpl::load);
        mt["BoneCount"] = Lua::Transform(&LuaApi::ObjectImpl::BoneCount);
        mt["BoneByName"] = Lua::Transform(&LuaApi::ObjectImpl::GetBoneByName);
        mt["BoneByIndex"] = Lua::Transform(&LuaApi::ObjectImpl::GetBoneByNumber);
    }
};
#endif
