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
#define REG_NAMED_AL_FUNC(name, var) state.luapp_add_translated_function( #var, Lua::Transform(&SoundContextProperties::name))
#define REG_AL_FUNC(name) state.luapp_add_translated_function( #name, Lua::Transform(&SoundContextProperties::name))

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

Link::Link() {}

bool Link::Init(GL_t* gl, GameWindow* gw, Lua::State& state)
{
    registerEnums(state);
    
    // GL
    state.luapp_register_object<LuaApi::ObjectMaterial>();
    state.luapp_register_object<LuaApi::Texture>();
    state.luapp_register_object<LuaApi::Shader>();
    state.luapp_register_object<LuaApi::Model>();
    state.luapp_register_object<LuaApi::ObjectBone>();
    state.luapp_register_object<LuaApi::Object>();
    
    // Misc
    state.luapp_register_object<LuaApi::Timer>();
    
    // AL
    state.luapp_register_object<LuaApi::DeviceList>();
    state.luapp_register_object<LuaApi::Device>();
    state.luapp_register_object<LuaApi::Context>();
    state.luapp_register_object<LuaApi::SoundEmitter>();
    state.luapp_push_object<LuaApi::DeviceList>();
    state.setglobal("Audio");
        
    // Custom Functions
    REG_NAMED_FUNC(TimeI, impl::timei);
    REG_NAMED_FUNC(TimeF, impl::timef);
    REG_NAMED_MEM_FUNC(Data, *gw, GameWindow, DataPath);
    
    // OpenGL Functions
    REG_GL_FUNC(glEnable);
    REG_GL_FUNC(glEnablei);
    REG_GL_FUNC(glDisable);
    REG_GL_FUNC(glDisablei);
    REG_GL_FUNC(glIsEnabled);
    REG_GL_FUNC(glIsEnabledi);
    REG_GL_FUNC(glLineWidth);
    REG_GL_FUNC(glDepthFunc);
    REG_GL_FUNC(glClearColor);
    REG_GL_FUNC(glClearDepth);
    REG_GL_FUNC(glClearDepthf);
    REG_GL_FUNC(glClearStencil);
    REG_GL_FUNC(glCullFace);
    REG_GL_FUNC(glDepthMask);
    REG_GL_FUNC(glClear);
    REG_GL_FUNC(glViewport);
    REG_GL_FUNC(glDepthRange);
    REG_GL_FUNC(glDepthRangef);
    REG_GL_FUNC(glFlush);
    REG_GL_FUNC(glFinish);
    REG_GL_FUNC(glFrontFace);
    REG_GL_FUNC(glHint);
    
    // OpenAL Functions
    REG_NAMED_AL_FUNC(queryDopplerFactor, DopplerFactor);
    REG_NAMED_AL_FUNC(querySoundSpeed, SoundSpeed);
    REG_NAMED_AL_FUNC(queryDistanceModel, DistanceModel);
    REG_NAMED_AL_FUNC(setDopplerFactor, SetDopplerFactor);
    REG_NAMED_AL_FUNC(setSoundSpeed, SetSoundSpeed);
    REG_NAMED_AL_FUNC(setDistanceModel, SetDistanceModel);
    
    REG_NAMED_AL_FUNC(queryListenerGain, ListenerGain);
    REG_NAMED_AL_FUNC(luaQueryListenerPosition, ListenerPosition);
    REG_NAMED_AL_FUNC(luaQueryListenerVelocity, ListenerVelocity);
    REG_NAMED_AL_FUNC(luaQueryListenerOrientation, ListenerOrientation);
    REG_NAMED_AL_FUNC(setListenerGain, SetListenerGain);
    REG_NAMED_AL_FUNC(setListenerPosition, SetListenerPosition);
    REG_NAMED_AL_FUNC(setListenerVelocity, SetListenerVelocity);
    REG_NAMED_AL_FUNC(setListenerOrientation, SetListenerOrientation);
       
    return true;
}

}
