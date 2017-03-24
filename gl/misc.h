#ifndef LUAGL_MISC_H
#define LUAGL_MISC_H
#include "shared.h"
#include <chrono>

namespace LuaApi {
    class TimerImpl {
    public:
        typedef std::chrono::high_resolution_clock clock;
    private:
        bool m_running;
        clock::time_point m_start;
        clock::time_point m_time;
    public:
        TimerImpl();
        void update();
        std::uint64_t timei() const;
        float timef() const;
        
        void start();
        void stop();
        void reset();
        bool running() const;
    };
    
    typedef RefCounted<TimerImpl> Timer;
}

template <> struct MetatableDescriptor<LuaApi::TimerImpl> {
    static char const* name() { return "timer_mt"; }
    static char const* luaname() { return "Timer"; }
    static char const* constructor() { return "New"; }
    static bool construct(LuaApi::TimerImpl* v) { return Lua::DefaultConstructor(v); }
    static void metatable(Lua::member_function_storage<LuaApi::TimerImpl>& mt) {
        mt["Reset"] = Lua::Transform(&LuaApi::TimerImpl::reset);
        mt["Running"] = Lua::Transform(&LuaApi::TimerImpl::running);
        mt["Start"] = Lua::Transform(&LuaApi::TimerImpl::start);
        mt["Stop"] = Lua::Transform(&LuaApi::TimerImpl::stop);
        mt["TimeI"] = Lua::Transform(&LuaApi::TimerImpl::timei);
        mt["TimeF"] = Lua::Transform(&LuaApi::TimerImpl::timef);
        mt["Update"] = Lua::Transform(&LuaApi::TimerImpl::update);
    }
};

#endif
