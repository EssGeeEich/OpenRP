#ifndef LUAGL_DRAWABLE_H
#define LUAGL_DRAWABLE_H
#include "shared.h"
#include "texture.h"
#include "shader.h"

namespace LuaApi {
	class DrawableState {
        friend class DrawableBase;
    public:
        enum { MAX_TEXTURES = 16 };
    protected:
        Texture m_fallback;
        Shader m_shader;
        Texture m_textures[MAX_TEXTURES];
        GLenum m_mode = GL_FILL;
        
        bool Apply();
        void UnApply();
    public:
        DrawableState() =default;
        DrawableState(DrawableState const&) =default;
        DrawableState(DrawableState&&) =default;
        DrawableState& operator= (DrawableState const&) =default;
        DrawableState& operator= (DrawableState&&) =default;
        
        void SetShader(Shader shd);
        void SetTexture(Texture tex, std::size_t index);
        void SetFallbackTexture(Texture fb);
        void SetDrawMode(GLenum mode);
        
        Shader shader() const;
        Texture texture(std::size_t) const;
        Texture FallbackTexture() const;
        GLenum DrawMode() const;
    };

    class DrawableBase {
    protected:
        virtual bool OnInitialize();
        virtual bool OnPreDraw();
        virtual void OnDraw();
        virtual void OnPostDraw();
        
        virtual DrawableState GetDrawState();
        virtual void Draw();
    };
}

#endif
