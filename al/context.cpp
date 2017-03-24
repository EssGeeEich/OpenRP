#include "context.h"
#include "emitter.h"
#include "loader.h"
#include "device.h"

namespace LuaApi {

float SoundContextProperties::queryDopplerFactor() {
    return alGetFloat(AL_DOPPLER_FACTOR);
}
float SoundContextProperties::querySoundSpeed() {
    return alGetFloat(AL_SPEED_OF_SOUND);
}
std::uint32_t SoundContextProperties::queryDistanceModel() {
    return alGetInteger(AL_DISTANCE_MODEL);
}
void SoundContextProperties::setDopplerFactor(float f) {
    alDopplerFactor(f);
}
void SoundContextProperties::setSoundSpeed(float f) {
    alSpeedOfSound(f);
}
void SoundContextProperties::setDistanceModel(std::uint32_t v) {
    alDistanceModel(v);
}
float SoundContextProperties::queryListenerGain() {
    float v = 0.f;
    alGetListenerf(AL_GAIN,&v);
    return v;
}
void SoundContextProperties::queryListenerPosition(float& a, float& b, float& c) {
    alGetListener3f(AL_POSITION,&a,&b,&c);
}
void SoundContextProperties::queryListenerVelocity(float& a, float& b, float& c) {
    alGetListener3f(AL_VELOCITY,&a,&b,&c);
}
void SoundContextProperties::queryListenerOrientation(float& a, float& b, float& c, float& d, float& e, float& f) {
    ALfloat buf[6];
    alGetListenerfv(AL_ORIENTATION,buf);
    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];
    e = buf[4];
    f = buf[5];
}
void SoundContextProperties::setListenerGain(float f)
{
    alListenerf(AL_GAIN,f);
}
void SoundContextProperties::setListenerPosition(float a, float b, float c)
{
    alListener3f(AL_POSITION,a,b,c);
}
void SoundContextProperties::setListenerVelocity(float a, float b, float c)
{
    alListener3f(AL_VELOCITY,a,b,c);
}
void SoundContextProperties::setListenerOrientation(float a, float b, float c, float d, float e, float f)
{
    ALfloat buf[6];
    buf[0] = a;
    buf[1] = b;
    buf[2] = c;
    buf[3] = d;
    buf[4] = e;
    buf[5] = f;
    alListenerfv(AL_ORIENTATION,buf);
}

Lua::ReturnValues SoundContextProperties::luaQueryListenerPosition()
{
    float d[3];
    queryListenerPosition(d[0],d[1],d[2]);
    return Lua::Return(d[0],d[1],d[2]);
}
Lua::ReturnValues SoundContextProperties::luaQueryListenerVelocity()
{
    float d[3];
    queryListenerVelocity(d[0],d[1],d[2]);
    return Lua::Return(d[0],d[1],d[2]);
}
Lua::ReturnValues SoundContextProperties::luaQueryListenerOrientation()
{
    float d[6];
    queryListenerOrientation(d[0],d[1],d[2],d[3],d[4],d[5]);
    return Lua::Return(d[0],d[1],d[2],d[3],d[4],d[5]);
}

ContextImpl::ContextImpl() : m_context(nullptr) {}
ContextImpl::~ContextImpl() { Release(); }

ContextImpl::ContextImpl(DeviceImpl* d, ALCint* l) : ContextImpl() { Init(d,l); }
ContextImpl::ContextImpl(DeviceImpl* d, const std::initializer_list<ALCint> &args) { Init(d,args); }

bool ContextImpl::Init(DeviceImpl* device, const std::initializer_list<ALCint>& flags) {
    std::vector<ALCint> data;
    data.reserve(flags.size() + 1);
    for(auto it = flags.begin(); it != flags.end(); ++it)
    {
        data.push_back(*it);
    }
    data.push_back(0);
    return Init(device, data.data());
}

bool ContextImpl::Init(DeviceImpl* device, ALCint* l) {
    if(!device || !device->m_device)
        return false;
    m_context = alcCreateContext(device->m_device, l);
    if(m_context && !Bind())
    {
        Release();
        return false;
    }
    return true;
}
bool ContextImpl::IsValid() const { return m_context != nullptr; }
void ContextImpl::Release() {
    if(!m_context)
        return;
    if(alcGetCurrentContext() == m_context)
        alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    m_context = nullptr;
}
bool ContextImpl::Bind() const {
    if(!m_context)
        throw std::runtime_error("Attemting to bind an invalid Context!");
    return alcMakeContextCurrent(m_context) == ALC_TRUE;
}

Lua::ReturnValues ContextImpl::LuaLoadFile(std::string const& fn)
{
    SoundEmitter base = SoundLoader::LoadFile(fn);
    if(base.IsValid() && base->IsValid())
    {
        return Lua::Return(std::move(base));
    }
    return Lua::Return();
}

}
