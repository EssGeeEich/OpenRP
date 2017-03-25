#include "material.h"

namespace LuaApi {

void ObjectMaterialImpl::UpdateUBO()
{
}

ObjectMaterialImpl::ObjectMaterialImpl()
    : m_ubo(static_cast<QOpenGLBuffer::Type>(GL_UNIFORM_BUFFER))
{
    m_ubo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_diffuse.m_x = 1.f;
    m_diffuse.m_y = 1.f;
    m_diffuse.m_z = 1.f;
    m_specular.m_x = 0.f;
    m_specular.m_y = 0.f;
    m_specular.m_z = 0.f;
    m_ambient.m_x = 0.f;
    m_ambient.m_y = 0.f;
    m_ambient.m_z = 0.f;
    m_emissive.m_x = 0.f;
    m_emissive.m_y = 0.f;
    m_emissive.m_z = 0.f;
    
    m_opacity.m_x = 1.f;
    m_shininess.m_x = 0.f;
    m_shininessStrength.m_x = 0.f;
    
    m_diffuse.m_uv =
            m_specular.m_uv =
            m_ambient.m_uv =
            m_emissive.m_uv =
            m_normals.m_uv =
            m_height.m_uv =
            m_opacity.m_uv =
            m_shininess.m_uv =
            m_displacement.m_uv =
            m_lightmap.m_uv =
            m_reflection.m_uv = 0;
}

#define COLPROP3(Name, Prop)\
    void ObjectMaterialImpl::Set##Name(float x, float y, float z) {\
        Property3& p = m_##Prop;\
        p.m_x = x; p.m_y = y; p.m_z = z;\
    }\
    Lua::ReturnValues ObjectMaterialImpl::Name() const {\
        Property3 const& p = m_##Prop;\
        return Lua::Return(p.m_x, p.m_y, p.m_z);\
    }
#define COLPROP1(Name, Prop)\
    void ObjectMaterialImpl::Set##Name(float x) {\
        m_##Prop.m_x = x;\
    }\
    float ObjectMaterialImpl::Name() const {\
        return m_##Prop.m_x;\
    }

#define TEXPROP(Name, Prop)\
    void ObjectMaterialImpl::Set##Name##Texture(Texture t) {\
        m_##Prop.m_texture = std::move(t);\
    }\
    void ObjectMaterialImpl::Set##Name##UV(std::size_t i) {\
        if(i >= 16) return;\
        m_##Prop.m_uv = i;\
    }\
    Texture ObjectMaterialImpl::Name##Texture() const {\
        return m_##Prop.m_texture;\
    }\
    std::size_t ObjectMaterialImpl::Name##UV() const {\
        return m_##Prop.m_uv;\
    }

COLPROP3(DiffuseColor, diffuse)
COLPROP3(SpecularColor, specular)
COLPROP3(AmbientColor, ambient)
COLPROP3(EmissiveColor, emissive)
COLPROP1(Opacity, opacity)
COLPROP1(Shininess, shininess)
COLPROP1(ShininessStrength, shininessStrength)


TEXPROP(Diffuse, diffuse)
TEXPROP(Specular, specular)
TEXPROP(Ambient, ambient)
TEXPROP(Emissive, emissive)
TEXPROP(Normals, normals)
TEXPROP(Height, height)
TEXPROP(Opacity, opacity)
TEXPROP(Shininess, shininess)
TEXPROP(Displacement, displacement)
TEXPROP(Lightmap, lightmap)
TEXPROP(Reflection, reflection)

}
