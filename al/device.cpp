#include "device.h"
#include "context.h"

namespace LuaApi {

    DeviceImpl::DeviceImpl(std::string name)
        : m_device(nullptr), m_name(std::move(name)), m_lastContext(0)
    {
    }
    
    DeviceImpl::~DeviceImpl() { Release(); }
    
    bool DeviceImpl::Init() {
        if(m_device)
            return true;
        m_device = alcOpenDevice(m_name.c_str());
        if(!m_device)
            return false;
        return true;
    }
    
    bool DeviceImpl::IsValid() const { return m_device != nullptr; }
    
    void DeviceImpl::Release() {
        if(alcCloseDevice(m_device) != ALC_TRUE)
            throw std::runtime_error("Unable to release DeviceImpl");
        
        m_device = nullptr;
    }
    
    std::string DeviceImpl::Name() const {
        return m_name;
    }
    
    Context* DeviceImpl::createContext() {
        if(!m_device)
            return nullptr;
        Context* pSelContext = m_contexts + (m_lastContext++);
        if(m_lastContext >= DeviceImpl::MAX_CONTEXTS)
            m_lastContext = 0;
        if(!pSelContext->IsValid()) {
            pSelContext->Init(this, nullptr);
        }
        return pSelContext;
    }
    
    Lua::ReturnValues DeviceImpl::LuaOpenContext()
    {
        Context* pCtx = createContext();
        if(!pCtx)
            return Lua::Return();
        return Lua::Return(*pCtx);
    }
}
