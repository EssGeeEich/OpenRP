#ifndef LUAGL_BONE_H
#define LUAGL_BONE_H
#include "shared.h"
#include "drawable.h"
#include "model.h"
#include "material.h"

namespace LuaApi {
	class ObjectBoneImpl final : public DrawableBase {
        friend class ObjectImpl;
        std::string m_name;
        std::uint32_t m_boneId;
        Model m_model;
        ObjectMaterial m_material;
        
        void SetName(std::string);
        void SetBoneId(std::uint32_t);
    public:
        ObjectBoneImpl();
        
        std::string Name() const;
        std::uint32_t Bone() const;
        ObjectMaterial Material();
        void SetMaterial(ObjectMaterial);
    };
    
    typedef RefCounted<ObjectBoneImpl> ObjectBone;
}

template <> struct MetatableDescriptor<LuaApi::ObjectBoneImpl> {
    static char const* name() { return "objectbone_mt"; }
    static char const* luaname() { return ""; }
    static char const* constructor() { return ""; }
    static bool construct(LuaApi::ObjectBoneImpl* v) { return Lua::DefaultConstructor(v); }
    static void metatable(Lua::member_function_storage<LuaApi::ObjectBoneImpl>& mt) {
        mt["Name"] = Lua::Transform(&LuaApi::ObjectBoneImpl::Name);
        mt["Bone"] = Lua::Transform(&LuaApi::ObjectBoneImpl::Bone);
        mt["Material"] = Lua::Transform(&LuaApi::ObjectBoneImpl::Material);
        mt["SetMaterial"] = Lua::Transform(&LuaApi::ObjectBoneImpl::SetMaterial);
    }
};

#endif
