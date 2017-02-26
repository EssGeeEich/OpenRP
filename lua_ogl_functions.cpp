#include <cmath>
#include <chrono>
#include <thread>

#include "lua_ogl_link.h"
#include "gamewindow.h"
#include <QOpenGLFunctions>

namespace LuaApi {

namespace impl {
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

    static float const inv_1000 = static_cast<float>(1.0 / 1000.0);
    float timef() {
        return static_cast<float>(timei()) * inv_1000;
    }
    
    float clampf(float val, float min, float max) {
        if(val < min)
            return min;
        if(val > max)
            return max;
        return val;
    }
}

bool Storage::InitAll(GL_t* gl, GameWindow* gw, Lua::State& state)
{
    {
        Texture fallback;
        if(!fallback.load(":/textures/error.png",Lua::Arg<bool>::ToCopy(false)))
            return false;
        fallback.texture()->setMagnificationFilter(QOpenGLTexture::Nearest);
        fallback.texture()->setMinificationFilter(QOpenGLTexture::Nearest);
        m_ds.SetFallbackTexture(fallback);
    }
    
    if(!m_square.Initialize(gl,gw))
        return false;
    
#define REG_NAMED_FUNC(name, fnc) state.luapp_add_translated_function( #name, Lua::Transform(fnc) )
#define REG_LAMBDA(name, fnc) REG_NAMED_FUNC(name, Lua::ToFnc(fnc))
#define REG_FUNC(fnc) state.luapp_add_translated_function( #fnc, Lua::Transform(fnc))
#define REG_NAMED_MEM_FUNC(name, var, type, fnc) state.luapp_add_translated_function( #name, Lua::Transform(var, &type::fnc))
#define REG_MEM_FUNC(var, type, fnc) state.luapp_add_translated_function( #fnc, Lua::Transform(var, &type::fnc))
#define REG_NAMED_CONST(name, var) { state.pushinteger(var); state.setglobal( #name ); }
#define REG_CONST(var) { state.pushinteger(var); state.setglobal( #var ); }
#define REG_NAMED_CONSTF(name, var) { state.pushnumber(var); state.setglobal( #name ); }
#define REG_CONSTF(var) { state.pushnumber(var); state.setglobal( #var ); }
    
        // Standard constants
        REG_CONST(RAND_MAX);
        REG_NAMED_CONSTF(PI, M_PI)
        
        // Standard Functions
        REG_NAMED_FUNC(rand, std::rand);
        REG_NAMED_FUNC(sin, (float(*)(float))std::sin);
        
        // Custom Functions
        REG_NAMED_FUNC(tick, impl::timei);
        REG_NAMED_FUNC(time, impl::timef);
        REG_NAMED_FUNC(clampf, impl::clampf);
        
        // Render Functions
        REG_MEM_FUNC(m_square, SquareShape, DrawRaw);
        REG_MEM_FUNC(m_square, SquareShape, DrawRawCentered);
        REG_MEM_FUNC(m_square, SquareShape, DrawCorrected);
        REG_MEM_FUNC(m_ds, DrawableState, SetTexture);
        
        REG_NAMED_MEM_FUNC(data, *gw, GameWindow, DataPath);
        
        /*REG_LAMBDA(data, []()->void{
            gw->
        });*/
        
    return true;
}

// Storage
Storage::Storage() : m_square(m_ds) {}

}
