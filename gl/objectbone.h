#ifndef LUAGL_BONE_H
#define LUAGL_BONE_H
#include "shared.h"
#include "drawable.h"
#include "model.h"
#include "material.h"

namespace LuaApi {
	class ModelBoneImpl final : public DrawableBase {
        friend class ModelImpl;
        std::string m_name;
        std::uint32_t m_boneId;
        ModelStorage m_model;
        ObjectMaterial m_material;
        
        void SetName(std::string);
        void SetBoneId(std::uint32_t);
    public:
        ModelBoneImpl();
        
        std::string Name() const;
        std::uint32_t Bone() const;
        ObjectMaterial Material();
        void SetMaterial(ObjectMaterial);
    };
    
    typedef RefCounted<ModelBoneImpl> ModelBone;
}

template <> struct MetatableDescriptor<LuaApi::ModelBoneImpl> {
    static char const* name() { return "objectbone_mt"; }
    static char const* luaname() { return ""; }
    static char const* constructor() { return ""; }
    static bool construct(LuaApi::ModelBoneImpl* v) { return Lua::DefaultConstructor(v); }
    static void metatable(Lua::member_function_storage<LuaApi::ModelBoneImpl>& mt) {
        mt["Name"] = Lua::Transform(&LuaApi::ModelBoneImpl::Name);
        mt["Bone"] = Lua::Transform(&LuaApi::ModelBoneImpl::Bone);
        mt["Material"] = Lua::Transform(&LuaApi::ModelBoneImpl::Material);
        mt["SetMaterial"] = Lua::Transform(&LuaApi::ModelBoneImpl::SetMaterial);
    }
};

#endif
