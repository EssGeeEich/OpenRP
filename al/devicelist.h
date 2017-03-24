#ifndef LUAAL_DEVICELIST_H
#define LUAAL_DEVICELIST_H
#include "shared.h"
#include "device.h"

namespace LuaApi {
	// Global "Audio" in Lua
    class DeviceList {
        std::vector<Device> m_devices;
        
        DeviceList(DeviceList const&) =delete;
        DeviceList& operator= (DeviceList const&) =delete;
    public:
        DeviceList();
        
        std::vector<Device>& Devices();
        Device* Default();
        
        Lua::Array<std::string> LuaDevices() const;
        std::string LuaDefaultDevice() const;
        Lua::ReturnValues LuaOpenDevice(std::string const&);
    };
}

template <> struct MetatableDescriptor<LuaApi::DeviceList> {
    static char const* name() { return "devicelist_mt"; }
    static char const* luaname() { return ""; }
    static char const* constructor() { return ""; }
    static bool construct(LuaApi::DeviceList*) { return false; }
    static void metatable(Lua::member_function_storage<LuaApi::DeviceList>& mt) {
        mt["List"] = Lua::Transform(&LuaApi::DeviceList::LuaDevices);
        mt["Default"] = Lua::Transform(&LuaApi::DeviceList::LuaDefaultDevice);
        mt["Open"] = Lua::Transform(&LuaApi::DeviceList::LuaOpenDevice);
    }
};

#endif
