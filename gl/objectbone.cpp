#include "objectbone.h"

namespace LuaApi {

ModelBoneImpl::ModelBoneImpl()
    : m_boneId(static_cast<std::uint32_t>(-1))
{
    m_model.Init();
    m_material.Init();
}

void ModelBoneImpl::SetName(std::string name) { m_name = std::move(name); }
void ModelBoneImpl::SetBoneId(std::uint32_t n) { m_boneId = n; }
std::string ModelBoneImpl::Name() const { return m_name; }
std::uint32_t ModelBoneImpl::Bone() const { return m_boneId; }
ObjectMaterial ModelBoneImpl::Material() { return m_material; }
void ModelBoneImpl::SetMaterial(ObjectMaterial m) { m_material = std::move(m); }

}
