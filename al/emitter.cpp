#include "emitter.h"

namespace LuaApi {

// SoundEmitterImpl
bool SoundEmitterImpl::setFormat(SEB_BufferFormat fmt) {
    if(m_format == SBF_INVALID)
    {
        m_format = fmt;
        return true;
    }
    return m_format == fmt;
}
bool SoundEmitterImpl::setFrequency(std::size_t i) {
    if(m_frequency == 0)
    {
        m_frequency = i;
        return true;
    }
    return m_frequency == i;
}

SoundEmitterImpl::SoundEmitterImpl() : m_source(0), m_format(SBF_INVALID), m_frequency(0) {}
SoundEmitterImpl::~SoundEmitterImpl() { Release(); }

bool SoundEmitterImpl::queueData(SEB_BufferFormat fmt, const void * ptr, std::size_t sz, std::size_t freq)
{
    if(!m_source ||
        !setFormat(fmt) ||
        !setFrequency(freq))
        return false;
    
    ALuint buf = 0;
    alGenBuffers(1,&buf);
    if(!buf)
        return false;
    alBufferData(buf,static_cast<ALenum>(fmt),ptr,sz,freq);
    alSourceQueueBuffers(m_source,1,&buf);
    m_buffers.push_back(buf);
    return true;
}

bool SoundEmitterImpl::Init() {
    Release();
    alGenSources(1,&m_source);
    return (m_source != 0);
}

bool SoundEmitterImpl::IsValid() const {
    return m_source != 0;
}

void SoundEmitterImpl::Release() {
    if(m_source)
    {
        alDeleteSources(1,&m_source);
        m_source = 0;
        alDeleteBuffers(m_buffers.size(), m_buffers.data());
        m_buffers.clear();
    }
}

void SoundEmitterImpl::Play()
{
    alSourcePlay(m_source);
}
void SoundEmitterImpl::Pause()
{
    alSourcePause(m_source);
}
void SoundEmitterImpl::Stop()
{
    alSourceStop(m_source);
    Rewind();
}
void SoundEmitterImpl::Rewind()
{
    alSourceRewind(m_source);
    alSourcei(m_source, AL_BUFFER, 0);
    alSourceQueueBuffers(m_source, m_buffers.size(), m_buffers.data());
}
Lua::ReturnValues SoundEmitterImpl::LuaQueryDirection() const
{
    float x,y,z;
    queryDirection(x,y,z);
    return Lua::Return(x,y,z);
}
Lua::ReturnValues SoundEmitterImpl::LuaQueryPosition() const
{
    float x,y,z;
    queryPosition(x,y,z);
    return Lua::Return(x,y,z);
}
Lua::ReturnValues SoundEmitterImpl::LuaQueryVelocity() const
{
    float x,y,z;
    queryVelocity(x,y,z);
    return Lua::Return(x,y,z);
}

#define GET_GENERIC_PROPERTY(FuncName, EnumName, public_type, internal_type, function_name, which_object)\
    public_type SoundEmitterImpl::query##FuncName() const { internal_type rv = 0; function_name(which_object, EnumName, &rv); return static_cast<public_type>(rv); }
#define SET_GENERIC_PROPERTY(FuncName, EnumName, public_type, internal_type, function_name, which_object)\
    void SoundEmitterImpl::set##FuncName(public_type val) { function_name(which_object, EnumName, static_cast<internal_type>(val)); }

#define GET_GENERIC_PROPERTY3(FuncName, EnumName, public_type, internal_type, function_name, which_object)\
    void SoundEmitterImpl::query##FuncName(public_type& a, public_type& b, public_type& c) const { internal_type d[3]; function_name(which_object, EnumName, d+0, d+1, d+2); a = d[0]; b = d[1]; c = d[2]; }
#define SET_GENERIC_PROPERTY3(FuncName, EnumName, public_type, internal_type, function_name, which_object)\
    void SoundEmitterImpl::set##FuncName(public_type a, public_type b, public_type c) { function_name(which_object, EnumName, static_cast<internal_type>(a), static_cast<internal_type>(b), static_cast<internal_type>(c)); }

#define GET_BOOL_PROPERTY(FuncName, EnumName, internal_type, function_name, which_object)\
    bool SoundEmitterImpl::query##FuncName() const { internal_type rv; function_name(which_object, EnumName, &rv); return (rv != 0); }
#define SET_BOOL_PROPERTY(FuncName, EnumName, internal_type, function_name, which_object)\
    void SoundEmitterImpl::set##FuncName(bool val) { function_name(which_object, EnumName, static_cast<internal_type>(val ? 1 : 0)); }

#define BUFFER_PROPERTY_i(FuncName, EnumName)\
    GET_GENERIC_PROPERTY(FuncName, EnumName, int, ALint, alGetBufferi, m_buffers.front())

#define SOURCE_PROPERTY_i(FuncName, EnumName)\
    GET_GENERIC_PROPERTY(FuncName, EnumName, int, ALint, alGetSourcei, m_source)\
    SET_GENERIC_PROPERTY(FuncName, EnumName, int, ALint, alSourcei, m_source)

#define SOURCE_PROPERTY_f(FuncName, EnumName)\
    GET_GENERIC_PROPERTY(FuncName, EnumName, float, ALfloat, alGetSourcef, m_source)\
    SET_GENERIC_PROPERTY(FuncName, EnumName, float, ALfloat, alSourcef, m_source)

#define SOURCE_PROPERTY_3f(FuncName, EnumName)\
    GET_GENERIC_PROPERTY3(FuncName, EnumName, float, ALfloat, alGetSource3f, m_source)\
    SET_GENERIC_PROPERTY3(FuncName, EnumName, float, ALfloat, alSource3f, m_source)

#define SOURCE_PROPERTY_b(FuncName, EnumName)\
    GET_BOOL_PROPERTY(FuncName, EnumName, ALint, alGetSourcei, m_source)\
    SET_BOOL_PROPERTY(FuncName, EnumName, ALint, alSourcei, m_source)

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

GET_GENERIC_PROPERTY(SourceType, AL_SOURCE_TYPE, SEB_SourceType, ALint, alGetSourcei, m_source)
GET_GENERIC_PROPERTY(SourceState, AL_SOURCE_STATE, SEB_SourceState, ALint, alGetSourcei, m_source)
GET_GENERIC_PROPERTY(OffsetSeconds, AL_SEC_OFFSET, float, ALfloat, alGetSourcef, m_source)
GET_GENERIC_PROPERTY(OffsetSamples, AL_SAMPLE_OFFSET, int, ALint, alGetSourcei, m_source)
GET_GENERIC_PROPERTY(OffsetBytes, AL_BYTE_OFFSET, int, ALint, alGetSourcei, m_source)
GET_GENERIC_PROPERTY(QueuedBuffers, AL_BUFFERS_QUEUED, int, ALint, alGetSourcei, m_source)
GET_GENERIC_PROPERTY(ProcessedBuffers, AL_BUFFERS_PROCESSED, int, ALint, alGetSourcei, m_source)

}
