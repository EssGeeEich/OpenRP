#ifndef LUAAL_CONTEXT_H
#define LUAAL_CONTEXT_H
#include "shared.h"
#include "enums.h"

namespace LuaApi {
    struct SoundContextProperties {
        // SCP_DistanceModel
        
        static float queryDopplerFactor();
        static float querySoundSpeed();
        static std::uint32_t queryDistanceModel();
        
        static void setDopplerFactor(float=1.f);
        static void setSoundSpeed(float=343.3f);
        static void setDistanceModel(std::uint32_t=SDM_INVERSE_DISTANCE_CLAMPED);
        
        // Listener
        static float queryListenerGain();
        static void queryListenerPosition(float&, float&, float&);
        static void queryListenerVelocity(float&, float&, float&);
        static void queryListenerOrientation(float&, float&, float&,
                                     float&, float&, float&);
        
        static void setListenerGain(float=1.f);
        static void setListenerPosition(float=0.f, float=0.f, float=0.f);
        static void setListenerVelocity(float=0.f, float=0.f, float=0.f);
        static void setListenerOrientation(float, float, float,
                                   float, float, float);
        
        static Lua::ReturnValues luaQueryListenerPosition();
        static Lua::ReturnValues luaQueryListenerVelocity();
        static Lua::ReturnValues luaQueryListenerOrientation();
    };

    class DeviceImpl;
    class ContextImpl {
        friend class ::LuaApi::DeviceImpl;
        
        ALCcontext* m_context;
        
        ContextImpl(ContextImpl const&) =delete;
        ContextImpl& operator= (ContextImpl const&) =delete;
    public:
        ContextImpl();
        ~ContextImpl();
        
        ContextImpl(DeviceImpl*, ALCint*);
        ContextImpl(DeviceImpl*, std::initializer_list<ALCint> const& args);
        
        bool Init(DeviceImpl*, std::initializer_list<ALCint> const&);
        bool Init(DeviceImpl*, ALCint*);
        bool IsValid() const;
        void Release();
        bool Bind() const;
        
        Lua::ReturnValues LuaLoadFile(std::string const&);
    };
    
    typedef LuaApi::RefCounted<ContextImpl> Context;
}
template <> struct MetatableDescriptor<LuaApi::ContextImpl> {
    static char const* name() { return "context_mt"; }
    static char const* luaname() { return ""; }
    static char const* constructor() { return ""; }
    static bool construct(LuaApi::ContextImpl*) { return false; }
    static void metatable(Lua::member_function_storage<LuaApi::ContextImpl>& mt) {
        mt["Bind"] = Lua::Transform(&LuaApi::ContextImpl::Bind);
        mt["LoadFile"] = Lua::Transform(&LuaApi::ContextImpl::LuaLoadFile);
        mt["IsValid"] = Lua::Transform(&LuaApi::ContextImpl::IsValid);
    }
};
#endif
