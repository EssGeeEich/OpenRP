#include "objectbone.h"

namespace LuaApi {

ObjectBoneImpl::ObjectBoneImpl()
    : m_boneId(static_cast<std::uint32_t>(-1))
{
    m_model.Init();
    m_material.Init();
}

void ObjectBoneImpl::SetName(std::string name) { m_name = std::move(name); }
void ObjectBoneImpl::SetBoneId(std::uint32_t n) { m_boneId = n; }
std::string ObjectBoneImpl::Name() const { return m_name; }
std::uint32_t ObjectBoneImpl::Bone() const { return m_boneId; }
ObjectMaterial ObjectBoneImpl::Material() { return m_material; }
void ObjectBoneImpl::SetMaterial(ObjectMaterial m) { m_material = std::move(m); }

}
