#include "rp_openal.h"
#include "gamewindow.h"

namespace LuaApi {

// SoundEmitterBase::SoundHolder

bool SoundEmitterBase::SoundHolder::free() {
    if(m_source != 0)
    {
        alDeleteSources(1,&m_source);
        m_source = 0;
        alDeleteBuffers(m_buffers.size(),m_buffers.data());
        m_buffers.clear();
    }
    return true;
}

SoundEmitterBase::SoundHolder::SoundHolder(ALuint i) :
    m_source(i),
    m_format(SBF_INVALID),
    m_frequency(0) {}

void SoundEmitterBase::SoundHolder::addBuffer(ALuint i)
{
    m_buffers.push_back(i);
}

std::vector<ALuint>& SoundEmitterBase::SoundHolder::buffers()
{
    return m_buffers;
}

ALuint SoundEmitterBase::SoundHolder::source() const
{
    return m_source;
}

SoundEmitterBase::SoundHolder::~SoundHolder()
{
    free();
}

SEB_BufferFormat SoundEmitterBase::SoundHolder::format() const
{
    return m_format;
}

bool SoundEmitterBase::SoundHolder::setFormat(SEB_BufferFormat f)
{
    if(m_format == SBF_INVALID)
        m_format = f;
    return m_format == f;
}

std::size_t SoundEmitterBase::SoundHolder::frequency() const
{
    return m_frequency;
}

bool SoundEmitterBase::SoundHolder::setFrequency(std::size_t f)
{
    if(m_frequency == 0)
        m_frequency = f;
    
    return m_frequency == f;
}

SoundEmitterBase::SoundHolder::SoundHolder(SoundHolder&& o)
    : m_source(o.m_source), m_format(o.m_format), m_frequency(o.m_frequency)
{
    std::swap(m_buffers, o.m_buffers);
    o.m_source = 0;
    o.m_format = SBF_INVALID;
    o.m_frequency = 0;
}
SoundEmitterBase::SoundHolder& SoundEmitterBase::SoundHolder::operator = (SoundEmitterBase::SoundHolder&& o) {
    free();
    m_source = o.m_source;
    m_buffers = std::move(o.m_buffers);
    m_format = o.m_format;
    m_frequency = o.m_frequency;
    
    o.m_source = 0;
    o.m_format = SBF_INVALID;
    o.m_frequency = 0;
    o.m_buffers.clear();
    return *this;
}

// SoundEmitterBase

SoundEmitterBase::SoundEmitterBase(Context* context)
    : m_context(context)
{
    ALuint src = 0;
    alGenSources(1,&src);
    if(src != 0)
    {
        m_sound = std::make_shared<SoundEmitterBase::SoundHolder>(src);
    }
}

bool SoundEmitterBase::good() const
{
    return m_sound != nullptr;
}

void SoundEmitterBase::release()
{
    m_sound.reset();
}

SoundEmitterBase::~SoundEmitterBase()
{
    release();
}

bool SoundEmitterBase::queueData(SEB_BufferFormat fmt, const void * ptr, std::size_t sz, std::size_t freq)
{
    if(!m_sound ||
        !m_sound->source() ||
        !m_sound->setFormat(fmt) ||
        !m_sound->setFrequency(freq))
        return false;
    
    ALuint buf = 0;
    alGenBuffers(1,&buf);
    if(!buf)
        return false;
    alBufferData(buf,static_cast<ALenum>(fmt),ptr,sz,freq);
    alSourceQueueBuffers(m_sound->source(),1,&buf);
    m_sound->addBuffer(buf);
    return true;
}


void SoundEmitterBase::play()
{
    alSourcePlay(m_sound->source());
}
void SoundEmitterBase::pause()
{
    alSourcePause(m_sound->source());
}
void SoundEmitterBase::stop()
{
    alSourceStop(m_sound->source());
    rewind();
}
void SoundEmitterBase::rewind()
{
    alSourceRewind(m_sound->source());
    alSourcei(m_sound->source(), AL_BUFFER, 0);
    alSourceQueueBuffers(m_sound->source(), m_sound->buffers().size(), m_sound->buffers().data());
}
Lua::ReturnValues SoundEmitterBase::LuaQueryDirection() const
{
    float x,y,z;
    queryDirection(x,y,z);
    return Lua::Return(x,y,z);
}
Lua::ReturnValues SoundEmitterBase::LuaQueryPosition() const
{
    float x,y,z;
    queryPosition(x,y,z);
    return Lua::Return(x,y,z);
}
Lua::ReturnValues SoundEmitterBase::LuaQueryVelocity() const
{
    float x,y,z;
    queryVelocity(x,y,z);
    return Lua::Return(x,y,z);
}

#define GET_GENERIC_PROPERTY(FuncName, EnumName, public_type, internal_type, function_name, which_object)\
    public_type SoundEmitterBase::query##FuncName() const { internal_type rv = 0; function_name(which_object, EnumName, &rv); return static_cast<public_type>(rv); }
#define SET_GENERIC_PROPERTY(FuncName, EnumName, public_type, internal_type, function_name, which_object)\
    void SoundEmitterBase::set##FuncName(public_type val) { function_name(which_object, EnumName, static_cast<internal_type>(val)); }

#define GET_GENERIC_PROPERTY3(FuncName, EnumName, public_type, internal_type, function_name, which_object)\
    void SoundEmitterBase::query##FuncName(public_type& a, public_type& b, public_type& c) const { internal_type d[3]; function_name(which_object, EnumName, d+0, d+1, d+2); a = d[0]; b = d[1]; c = d[2]; }
#define SET_GENERIC_PROPERTY3(FuncName, EnumName, public_type, internal_type, function_name, which_object)\
    void SoundEmitterBase::set##FuncName(public_type a, public_type b, public_type c) { function_name(which_object, EnumName, static_cast<internal_type>(a), static_cast<internal_type>(b), static_cast<internal_type>(c)); }

#define GET_BOOL_PROPERTY(FuncName, EnumName, internal_type, function_name, which_object)\
    bool SoundEmitterBase::query##FuncName() const { internal_type rv; function_name(which_object, EnumName, &rv); return (rv != 0); }
#define SET_BOOL_PROPERTY(FuncName, EnumName, internal_type, function_name, which_object)\
    void SoundEmitterBase::set##FuncName(bool val) { function_name(which_object, EnumName, static_cast<internal_type>(val ? 1 : 0)); }

#define BUFFER_PROPERTY_i(FuncName, EnumName)\
    GET_GENERIC_PROPERTY(FuncName, EnumName, int, ALint, alGetBufferi, m_sound->buffers().front())

#define SOURCE_PROPERTY_i(FuncName, EnumName)\
    GET_GENERIC_PROPERTY(FuncName, EnumName, int, ALint, alGetSourcei, m_sound->source())\
    SET_GENERIC_PROPERTY(FuncName, EnumName, int, ALint, alSourcei, m_sound->source())

#define SOURCE_PROPERTY_f(FuncName, EnumName)\
    GET_GENERIC_PROPERTY(FuncName, EnumName, float, ALfloat, alGetSourcef, m_sound->source())\
    SET_GENERIC_PROPERTY(FuncName, EnumName, float, ALfloat, alSourcef, m_sound->source())

#define SOURCE_PROPERTY_3f(FuncName, EnumName)\
    GET_GENERIC_PROPERTY3(FuncName, EnumName, float, ALfloat, alGetSource3f, m_sound->source())\
    SET_GENERIC_PROPERTY3(FuncName, EnumName, float, ALfloat, alSource3f, m_sound->source())

#define SOURCE_PROPERTY_b(FuncName, EnumName)\
    GET_BOOL_PROPERTY(FuncName, EnumName, ALint, alGetSourcei, m_sound->source())\
    SET_BOOL_PROPERTY(FuncName, EnumName, ALint, alSourcei, m_sound->source())

BUFFER_PROPERTY_i(Frequency, AL_FREQUENCY)
BUFFER_PROPERTY_i(Bits, AL_BITS)
BUFFER_PROPERTY_i(Channels, AL_CHANNELS)
BUFFER_PROPERTY_i(ByteSize, AL_SIZE)

SOURCE_PROPERTY_f(Pitch, AL_PITCH)
SOURCE_PROPERTY_f(Gain, AL_GAIN)
SOURCE_PROPERTY_f(MaxDistance, AL_MAX_DISTANCE)
SOURCE_PROPERTY_f(Rolloff, AL_ROLLOFF_FACTOR)
SOURCE_PROPERTY_f(RefDistance, AL_REFERENCE_DISTANCE)
SOURCE_PROPERTY_f(MinGain, AL_MIN_GAIN)
SOURCE_PROPERTY_f(MaxGain, AL_MAX_GAIN)
SOURCE_PROPERTY_f(ConeOuterGain, AL_CONE_OUTER_GAIN)
SOURCE_PROPERTY_f(ConeInnerAngle, AL_CONE_INNER_ANGLE)
SOURCE_PROPERTY_f(ConeOuterAngle, AL_CONE_OUTER_ANGLE)
SOURCE_PROPERTY_3f(Position, AL_POSITION)
SOURCE_PROPERTY_3f(Velocity, AL_VELOCITY)
SOURCE_PROPERTY_3f(Direction, AL_DIRECTION)
SOURCE_PROPERTY_b(RelativePosition, AL_SOURCE_RELATIVE)
SOURCE_PROPERTY_b(Looping, AL_LOOPING)

GET_GENERIC_PROPERTY(SourceType, AL_SOURCE_TYPE, SEB_SourceType, ALint, alGetSourcei, m_sound->source())
GET_GENERIC_PROPERTY(SourceState, AL_SOURCE_STATE, SEB_SourceState, ALint, alGetSourcei, m_sound->source())
GET_GENERIC_PROPERTY(OffsetSeconds, AL_SEC_OFFSET, float, ALfloat, alGetSourcef, m_sound->source())
GET_GENERIC_PROPERTY(OffsetSamples, AL_SAMPLE_OFFSET, int, ALint, alGetSourcei, m_sound->source())
GET_GENERIC_PROPERTY(OffsetBytes, AL_BYTE_OFFSET, int, ALint, alGetSourcei, m_sound->source())
GET_GENERIC_PROPERTY(QueuedBuffers, AL_BUFFERS_QUEUED, int, ALint, alGetSourcei, m_sound->source())
GET_GENERIC_PROPERTY(ProcessedBuffers, AL_BUFFERS_PROCESSED, int, ALint, alGetSourcei, m_sound->source())

Context::ContextHolder::ContextHolder(ALCcontext *c)
    : m_context(c) {}
Context::ContextHolder::ContextHolder(Context::ContextHolder&& o)
    : m_context(o.m_context) { o.m_context = nullptr; }
Context::ContextHolder& Context::ContextHolder::operator = (Context::ContextHolder&& o) {
    free();
    m_context = o.m_context;
    o.m_context = nullptr;
    return *this;
}
Context::ContextHolder::~ContextHolder() { free(); }
ALCcontext* Context::ContextHolder::context() const { return m_context; }
bool Context::ContextHolder::free() {
    if(alcGetCurrentContext() == m_context)
        alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    m_context = nullptr;
    return true;
}

Context::Context(Device* d)
    : m_owner(d), m_context(nullptr) { }
Context::Context(Device* d, ALCint* l)
    : Context(d) { doInit(l); }
void Context::doInit(ALCint* l)
{
    if(!m_owner->m_device)
        return;
    m_context = std::make_shared<Context::ContextHolder>(alcCreateContext(m_owner->m_device->device(), l));
}
bool Context::initialized() const
{
    return m_context != nullptr;
}
void Context::release()
{
    m_context.reset();
}
bool Context::bind() const
{
    if(m_context)
        return alcMakeContextCurrent(m_context->context())==ALC_TRUE ? true : false;
    return false;
}
SoundEmitterBase* Context::loadOggFile(const char* filename)
{
    OggSampleEmitter* ose = new OggSampleEmitter(this);
    try {
        if(ose->loadFile(filename))
            return ose;
    } catch(...) {
        delete ose;
        throw;
    }
    delete ose;
    return nullptr;
}
Lua::ReturnValues Context::LuaLoadFile(std::string const& fn)
{
    SoundEmitterBase* base = loadOggFile(fn.c_str());
    if(base)
    {
        if(base->good())
        {
            SoundEmitterBase sebMove = std::move(*base);
            delete base;
            return Lua::Return(std::move(sebMove));
        }
        delete base;
    }
    return Lua::Return();
}

Context::~Context()
{
    release();
}

Device::DeviceHolder::DeviceHolder(ALCdevice *d)
    : m_device(d) {}
Device::DeviceHolder& Device::DeviceHolder::operator= (Device::DeviceHolder&& o)
{
    free();
    m_device = o.m_device;
    o.m_device = nullptr;
    return *this;
}
Device::DeviceHolder::DeviceHolder(Device::DeviceHolder&& o)
    : m_device(o.m_device) { o.m_device = nullptr; }
Device::DeviceHolder::~DeviceHolder() { free(); }
bool Device::DeviceHolder::free() { if(alcCloseDevice(m_device) == ALC_TRUE) { m_device = nullptr; return true; } return false; }
ALCdevice* Device::DeviceHolder::device() const { return m_device; }

ALCdevice* Device::DeviceData::device() const { return m_device.device(); }
std::vector<Context>& Device::DeviceData::contexts() { return m_contexts; }
std::size_t& Device::DeviceData::lastContext() { return m_lastContext; }
Device::DeviceData::DeviceData(ALCdevice *dev)
    : m_device(dev) {
    m_contexts.reserve(MAX_CONTEXTS);
}

Device::Device(std::string name)
    : m_name(std::move(name)) {}
Device::~Device() { release(); }
bool Device::initialized() const {
    return m_device != nullptr;
}

bool Device::initialize() {
    release();
    ALCdevice* dev = alcOpenDevice(m_name.c_str());
    if(!dev)
        return false;
    m_device = std::make_shared<Device::DeviceData>(dev);
    return true;
}
void Device::release()
{
    m_device.reset();
}
std::string Device::name() const {
    return m_name;
}
Context* Device::createContext()
{
    if(!m_device && !initialize())
        return nullptr;
    if(m_device->contexts().size() >= DeviceData::MAX_CONTEXTS)
    {
        m_device->lastContext() %= DeviceData::MAX_CONTEXTS;
        return &m_device->contexts()[m_device->lastContext()++];
    }
    m_device->contexts().emplace_back(this,nullptr);
    if(!m_device->contexts().back().initialized())
    {
        m_device->contexts().pop_back();
        return nullptr;
    }
    return &(m_device->contexts().back());
}
Lua::ReturnValues Device::LuaOpenContext()
{
    Context* pCtx = createContext();
    if(!pCtx)
        return Lua::Return();
    Context cxMove = std::move(*pCtx);
    delete pCtx;
    return Lua::Return(std::move(cxMove));
}

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
        m_devices.push_back(Device(std::move(*it)));
    }
}

Device* DeviceList::Default()
{
    std::string strPtr = LuaDefaultDevice();
    if(strPtr.empty())
        return nullptr;
    for(auto it = m_devices.begin(); it != m_devices.end(); ++it)
    {
        if(it->m_name == strPtr)
            return &*it;
    }
    return nullptr;
}

Lua::Array<std::string> DeviceList::LuaDevices() const
{
    Lua::Array<std::string> dvcs;
    for(auto it = m_devices.begin(); it != m_devices.end(); ++it)
    {
        dvcs.m_data.push_back(it->name());
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
        if(it->name() == name)
        {
            if(it->initialized() || it->initialize())
                return Lua::Return(*it);
            return Lua::Return();
        }
    }
    return Lua::Return();
}

}
