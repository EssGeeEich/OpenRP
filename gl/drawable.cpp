#include "drawable.h"
#include "gamewindow.h"
#include <QOpenGLFunctions>

namespace LuaApi {

// DrawableState
bool DrawableState::Apply() {
    if(!(m_shader.IsValid() && m_shader->good()))
        return false;
    
    for(std::size_t i = 0; i < DrawableState::MAX_TEXTURES; ++i)
    {
        if(m_textures[i].IsValid() && m_textures[i]->good())
        {
            m_textures[i]->texture()->bind(i);
        }
    }
    
    if(m_mode != GL_FILL)
    {
        glPolygonMode(GL_FRONT_AND_BACK, m_mode);
    }
    return true;
}
void DrawableState::UnApply() {
    if(m_mode != GL_FILL)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void DrawableState::SetShader(Shader shd) { m_shader = shd; }
void DrawableState::SetTexture(Texture tex, std::size_t index) {
    if(index >= DrawableState::MAX_TEXTURES)
        return;
    m_textures[index] = (tex.IsValid() && tex->good()) ? tex : m_fallback;
}
void DrawableState::SetFallbackTexture(Texture fb) {
    m_fallback = fb;
    for(int i = 0; i < DrawableState::MAX_TEXTURES; ++i)
    {
        if(!(m_textures[i].IsValid() && m_textures[i]->good()))
            m_textures[i] = m_fallback;
    }
}
void DrawableState::SetDrawMode(GLenum mode) {
    switch(mode)
    {
    case GL_POINT:
    case GL_LINE:
    case GL_FILL:
        m_mode = mode;
    }
}
Shader DrawableState::shader() const { return m_shader; }
Texture DrawableState::texture(std::size_t ix) const { if(ix >= DrawableState::MAX_TEXTURES) return m_fallback; return m_textures[ix]; }
Texture DrawableState::FallbackTexture() const { return m_fallback; }
GLenum DrawableState::DrawMode() const { return m_mode; }

// DrawableBase
bool DrawableBase::OnInitialize() { return true; }
bool DrawableBase::OnPreDraw() {
    return GetDrawState().Apply();
}
void DrawableBase::OnDraw() {}
void DrawableBase::OnPostDraw() {
    GetDrawState().UnApply();
}
DrawableState DrawableBase::GetDrawState() { return DrawableState(); }
void DrawableBase::Draw() {
    if(OnPreDraw())
    {
        OnDraw();
        OnPostDraw();
    }
}

}
