#ifndef LUAGL_OBJECT_H
#define LUAGL_OBJECT_H
#include "shared.h"
#include "objectbone.h"

namespace LuaApi {
	class ModelImpl {
        std::vector<ModelBone> m_bones;
    public:
        bool load(std::string const&);
        
        std::size_t BoneCount() const;
        Lua::ReturnValues GetBoneByNumber(std::size_t);
        Lua::ReturnValues GetBoneByName(std::string const&);
    };
    typedef LuaApi::RefCounted<ModelImpl> Model;
}

template <> struct MetatableDescriptor<LuaApi::ModelImpl> {
    static char const* name() { return "object_mt"; }
    static char const* luaname() { return "Model"; }
    static char const* constructor() { return "New"; }
    static bool construct(LuaApi::ModelImpl* v) { return Lua::DefaultConstructor(v); }
    static void metatable(Lua::member_function_storage<LuaApi::ModelImpl>& mt) {
        mt["LoadFile"] = Lua::Transform(&LuaApi::ModelImpl::load);
        mt["BoneCount"] = Lua::Transform(&LuaApi::ModelImpl::BoneCount);
        mt["BoneByName"] = Lua::Transform(&LuaApi::ModelImpl::GetBoneByName);
        mt["BoneByIndex"] = Lua::Transform(&LuaApi::ModelImpl::GetBoneByNumber);
    }
};
#endif
