#ifndef LUAGL_SHADER_H
#define LUAGL_SHADER_H
#include "shared.h"
#include "camera.h"

namespace LuaApi {
	class ShaderImpl {
        std::shared_ptr<QOpenGLShaderProgram> m_program;
    public:
        ShaderImpl() =default;
        
        bool load(std::string const&, std::string const&, Lua::Arg<std::string> const&);
        bool loadfromfile(std::string const&, std::string const&, Lua::Arg<std::string> const&);
        void unload();
        QOpenGLShaderProgram* shader() const;
        bool good() const;
        
        void SetCamera(Camera);
    };
    
    typedef RefCounted<ShaderImpl> Shader;
}

template <> struct MetatableDescriptor<LuaApi::ShaderImpl> {
    static char const* name() { return "shader_mt"; }
    static char const* luaname() { return "Shader"; }
    static char const* constructor() { return "New"; }
    static bool construct(LuaApi::ShaderImpl* v) { return Lua::DefaultConstructor(v); }
    static void metatable(Lua::member_function_storage<LuaApi::ShaderImpl>& mt) {
        mt["Load"] = Lua::Transform(&LuaApi::ShaderImpl::load);
        mt["LoadFile"] = Lua::Transform(&LuaApi::ShaderImpl::loadfromfile);
        mt["IsValid"] = Lua::Transform(&LuaApi::ShaderImpl::good);
        mt["Unload"] = Lua::Transform(&LuaApi::ShaderImpl::unload);
    }
};
#endif
