#include "link.h"
#include "gamewindow.h"

#define REG_NAMED_FUNC(name, fnc) state.luapp_add_translated_function( #name, Lua::Transform(fnc) )
#define REG_FUNC(fnc) state.luapp_add_translated_function( #fnc, Lua::Transform(fnc))
#define REG_NAMED_MEM_FUNC(name, var, type, fnc) state.luapp_add_translated_function( #name, Lua::Transform(var, &type::fnc))
#define REG_MEM_FUNC(var, type, fnc) state.luapp_add_translated_function( #fnc, Lua::Transform(var, &type::fnc))
#define REG_NAMED_CONST(name, var) { state.pushinteger(var); state.setglobal( #name ); }
#define REG_CONST(var) { state.pushinteger(var); state.setglobal( #var ); }
#define REG_NAMED_CONSTF(name, var) { state.pushnumber(var); state.setglobal( #name ); }
#define REG_CONSTF(var) { state.pushnumber(var); state.setglobal( #var ); }
#define REG_NAMED_GL_FUNC(name, var) state.luapp_add_translated_function( #var, Lua::Transform(*gl, &GL_t::name ))
#define REG_GL_FUNC(name) state.luapp_add_translated_function( #name, Lua::Transform(*gl, &GL_t::name ))

namespace LuaApi {

namespace impl {
    static float const inv_1000 = static_cast<float>(1.0 / 1000.0);
    static thread_local std::chrono::high_resolution_clock::time_point start;
    static thread_local bool is_first_run = true;
    std::uint64_t timei() {
        if(is_first_run)
        {
            is_first_run = false;
            start = std::chrono::high_resolution_clock::now();
        }
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    }
    float timef() {
        return static_cast<float>(timei()) * inv_1000;
    }
}

Link::Link() : m_square(m_drawState) {}

bool Link::Init(GL_t* gl, GameWindow* gw, Lua::State& state)
{
    registerEnums(state);
    state.luapp_register_object<LuaApi::Texture>();
    state.luapp_register_object<LuaApi::Shader>();
    state.luapp_register_object<LuaApi::Object>();
    state.luapp_register_object<LuaApi::Timer>();
    state.luapp_register_object<LuaApi::DeviceList>();
    state.luapp_register_object<LuaApi::Device>();
    state.luapp_register_object<LuaApi::Context>();
    state.luapp_register_object<LuaApi::SoundEmitterBase>();
    
    state.luapp_push_object<LuaApi::DeviceList>();
    state.setglobal("Audio");
    
    {
        Texture fallback;
        if(!fallback.load(":/textures/error.png",Lua::Arg<bool>::ToCopy(false)))
            return false;
        fallback.texture()->setMagnificationFilter(QOpenGLTexture::Nearest);
        fallback.texture()->setMinificationFilter(QOpenGLTexture::Nearest);
        m_drawState.SetFallbackTexture(fallback);
    }
    
    if(!m_square.Initialize(gl,gw))
        return false;
        
    // Custom Functions
    REG_NAMED_FUNC(TimeI, impl::timei);
    REG_NAMED_FUNC(TimeF, impl::timef);
    REG_NAMED_MEM_FUNC(Data, *gw, GameWindow, DataPath);
    
    // Render Functions
    REG_MEM_FUNC(m_square, SquareShape, DrawRaw);
    REG_MEM_FUNC(m_square, SquareShape, DrawRawCentered);
    REG_MEM_FUNC(m_square, SquareShape, DrawCorrected);
    REG_MEM_FUNC(m_drawState, DrawableState, SetTexture);
    
    // OpenGL Functions
    REG_GL_FUNC(glEnable);
    REG_GL_FUNC(glDisable);
    REG_GL_FUNC(glDepthFunc);
    REG_GL_FUNC(glClearColor);
    REG_GL_FUNC(glClear);
    
    // OpenAL Functions
       
    return true;
}

}
