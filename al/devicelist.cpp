#include "devicelist.h"
#include "device.h"
#include <list>

namespace LuaApi {

DeviceList::DeviceList()
{
    ALCchar const* ptr = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
    if(!ptr)
        return;
    
    std::list<std::string> devices;
    
    std::string s = ptr;
    while(!s.empty())
    {
        devices.push_back(std::move(s));
        ptr += devices.back().size() + 1;
        s = ptr;
    }
    
    m_devices.reserve(devices.size());
    for(auto it = devices.begin(); it != devices.end(); ++it)
    {
        m_devices.emplace_back();
        m_devices.back().Init(std::move(*it));
    }
}

Device* DeviceList::Default()
{
    std::string strPtr = LuaDefaultDevice();
    if(strPtr.empty())
        return nullptr;
    for(auto it = m_devices.begin(); it != m_devices.end(); ++it)
    {
        if((*it)->Name() == strPtr)
            return &*it;
    }
    return nullptr;
}

Lua::Array<std::string> DeviceList::LuaDevices() const
{
    Lua::Array<std::string> dvcs;
    for(auto it = m_devices.begin(); it != m_devices.end(); ++it)
    {
        dvcs.m_data.push_back((*it)->Name());
    }
    return dvcs;
}

std::string DeviceList::LuaDefaultDevice() const
{
    ALCchar const* ptr = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
    if(!ptr)
        return std::string();
    return ptr;
}

Lua::ReturnValues DeviceList::LuaOpenDevice(std::string const& name)
{
    for(auto it = m_devices.begin(); it != m_devices.end(); ++it)
    {
        if((*it)->Name() == name)
        {
            if((*it)->IsValid() || (*it)->Init())
                return Lua::Return(*it);
            return Lua::Return();
        }
    }
    return Lua::Return();
}
}
