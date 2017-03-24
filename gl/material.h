#ifndef LUAGL_MATERIAL_H
#define LUAGL_MATERIAL_H
#include "shared.h"
#include "texture.h"

namespace LuaApi {
	class ObjectMaterialImpl {
        QOpenGLBuffer m_ubo;
        
        void UpdateUBO();
    public:
        ObjectMaterialImpl();
        ObjectMaterialImpl(ObjectMaterialImpl&&) =default;
        ObjectMaterialImpl(ObjectMaterialImpl const&) =default;
        ObjectMaterialImpl& operator=(ObjectMaterialImpl&&) =default;
        ObjectMaterialImpl& operator=(ObjectMaterialImpl const&) =default;
        
        struct PropertyTex {
            inline PropertyTex() { m_texture.Init(); }
            std::size_t m_uv;
            Texture m_texture;
        };
        struct Property3 : public PropertyTex {
            float m_x;
            float m_y;
            float m_z;
        };
        struct Property1 : public PropertyTex {
            float m_x;
        };
        struct LoneProperty {
            float m_x;
        };
        
        Property3 m_diffuse;
        Property3 m_specular;
        Property3 m_ambient;
        Property3 m_emissive;
        PropertyTex m_normals;
        PropertyTex m_height;
        Property1 m_opacity;
        Property1 m_shininess;
        LoneProperty m_shininessStrength;
        PropertyTex m_displacement;
        PropertyTex m_lightmap;
        PropertyTex m_reflection;
        
        
        void SetDiffuseColor(float, float, float);
        void SetSpecularColor(float, float, float);
        void SetAmbientColor(float, float, float);
        void SetEmissiveColor(float, float, float);
        void SetOpacity(float);
        void SetShininess(float);
        void SetShininessStrength(float);
        
        Lua::ReturnValues DiffuseColor() const;
        Lua::ReturnValues SpecularColor() const;
        Lua::ReturnValues AmbientColor() const;
        Lua::ReturnValues EmissiveColor() const;
        float Opacity() const;
        float Shininess() const;
        float ShininessStrength() const;
        
        void SetDiffuseTexture(Texture);
        void SetDiffuseUV(std::size_t);
        void SetSpecularTexture(Texture);
        void SetSpecularUV(std::size_t);
        void SetAmbientTexture(Texture);
        void SetAmbientUV(std::size_t);
        void SetEmissiveTexture(Texture);
        void SetEmissiveUV(std::size_t);
        void SetNormalsTexture(Texture);
        void SetNormalsUV(std::size_t);
        void SetHeightTexture(Texture);
        void SetHeightUV(std::size_t);
        void SetOpacityTexture(Texture);
        void SetOpacityUV(std::size_t);
        void SetShininessTexture(Texture);
        void SetShininessUV(std::size_t);
        void SetDisplacementTexture(Texture);
        void SetDisplacementUV(std::size_t);
        void SetLightmapTexture(Texture);
        void SetLightmapUV(std::size_t);
        void SetReflectionTexture(Texture);
        void SetReflectionUV(std::size_t);
        
        
        Texture DiffuseTexture() const;
        std::size_t DiffuseUV() const;
        Texture SpecularTexture() const;
        std::size_t SpecularUV() const;
        Texture AmbientTexture() const;
        std::size_t AmbientUV() const;
        Texture EmissiveTexture() const;
        std::size_t EmissiveUV() const;
        Texture NormalsTexture() const;
        std::size_t NormalsUV() const;
        Texture HeightTexture() const;
        std::size_t HeightUV() const;
        Texture OpacityTexture() const;
        std::size_t OpacityUV() const;
        Texture ShininessTexture() const;
        std::size_t ShininessUV() const;
        Texture DisplacementTexture() const;
        std::size_t DisplacementUV() const;
        Texture LightmapTexture() const;
        std::size_t LightmapUV() const;
        Texture ReflectionTexture() const;
        std::size_t ReflectionUV() const;
    };
    
    typedef RefCounted<ObjectMaterialImpl> ObjectMaterial;
}

template <> struct MetatableDescriptor<LuaApi::ObjectMaterialImpl> {
    static char const* name() { return "material_mt"; }
    static char const* luaname() { return "Material"; }
    static char const* constructor() { return "New"; }
    static bool construct(LuaApi::ObjectMaterialImpl* v) { return Lua::DefaultConstructor(v); }
    static void metatable(Lua::member_function_storage<LuaApi::ObjectMaterialImpl>& mt) {
#define REG_FNC(name) mt[ #name ] = Lua::Transform(&LuaApi::ObjectMaterialImpl::name)
#define REG_TXFNC(prop) REG_FNC(prop##Texture); REG_FNC(prop##UV); REG_FNC(Set##prop##Texture); REG_FNC(Set##prop##UV);
        REG_FNC(SetDiffuseColor);
        REG_FNC(SetSpecularColor);
        REG_FNC(SetAmbientColor);
        REG_FNC(SetEmissiveColor);
        REG_FNC(SetOpacity);
        REG_FNC(SetShininess);
        REG_FNC(SetShininessStrength);
        REG_FNC(DiffuseColor);
        REG_FNC(SpecularColor);
        REG_FNC(AmbientColor);
        REG_FNC(EmissiveColor);
        REG_FNC(Opacity);
        REG_FNC(Shininess);
        REG_FNC(ShininessStrength);
        
        REG_TXFNC(Diffuse);
        REG_TXFNC(Specular);
        REG_TXFNC(Ambient);
        REG_TXFNC(Emissive);
        REG_TXFNC(Normals);
        REG_TXFNC(Height);
        REG_TXFNC(Opacity);
        REG_TXFNC(Shininess);
        REG_TXFNC(Displacement);
        REG_TXFNC(Lightmap);
        REG_TXFNC(Reflection);
#undef REG_FNC
#undef REG_TXFNC
    }
};

#endif
