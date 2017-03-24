#ifndef LUAAL_DEVICE_H
#define LUAAL_DEVICE_H
#include "shared.h"
#include "context.h"

namespace LuaApi {
    class ContextImpl;
    class DeviceListImpl;
    
    class DeviceImpl {
        DeviceImpl(DeviceImpl const&) =delete;
        DeviceImpl& operator= (DeviceImpl const&) =delete;
    public:
        enum { MAX_CONTEXTS = 16 };
    private:
        friend class ::LuaApi::ContextImpl;
        friend class ::LuaApi::DeviceListImpl;
        
        ALCdevice* m_device;
        std::string m_name;
        Context m_contexts[MAX_CONTEXTS];
        std::size_t m_lastContext;
    public:
        DeviceImpl(std::string);
        ~DeviceImpl();
        
        bool IsValid() const;
        bool Init();
        void Release();
        std::string Name() const;
        Context* createContext();
        
        Lua::ReturnValues LuaOpenContext();
    };
    
    typedef RefCounted<DeviceImpl> Device;
}
template <> struct MetatableDescriptor<LuaApi::DeviceImpl> {
    static char const* name() { return "device_mt"; }
    static char const* luaname() { return ""; }
    static char const* constructor() { return ""; }
    static bool construct(LuaApi::DeviceImpl*) { return false; }
    static void metatable(Lua::member_function_storage<LuaApi::DeviceImpl>& mt) {
        mt["Name"] = Lua::Transform(&LuaApi::DeviceImpl::Name);
        mt["CreateContext"] = Lua::Transform(&LuaApi::DeviceImpl::LuaOpenContext);
        mt["IsValid"] = Lua::Transform(&LuaApi::DeviceImpl::IsValid);
    }
};
#endif
