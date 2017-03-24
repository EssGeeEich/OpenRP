#ifndef LUAAL_EMITTER_H
#define LUAAL_EMITTER_H
#include "shared.h"
#include "enums.h"

namespace LuaApi {

    class SoundEmitterImpl {
        ALuint m_source;
        std::vector<ALuint> m_buffers;
        SEB_BufferFormat m_format;
        std::size_t m_frequency;
        
        bool setFormat(SEB_BufferFormat);
        bool setFrequency(std::size_t);
        
        SoundEmitterImpl(SoundEmitterImpl const&) =delete;
        SoundEmitterImpl& operator= (SoundEmitterImpl const&) =delete;
    public:
        SoundEmitterImpl();
        ~SoundEmitterImpl();
        
        bool queueData(SEB_BufferFormat, void const*, std::size_t, std::size_t);
        
        bool Init();
        bool IsValid() const;
        void Release();
        
        // Source actions
        void Play();
        void Pause();
        void Stop();
        void Rewind();
        bool queryLooping() const;
        void setLooping(bool=false);
        
        // Buffer properties
        int queryFrequency() const;
        int queryBits() const;
        int queryChannels() const;
        int queryByteSize() const;
        
        // Source properties
        float queryPitch() const;
        float queryGain() const;
        float queryMaxDistance() const;
        float queryRolloff() const;
        float queryRefDistance() const;
        float queryMinGain() const;
        float queryMaxGain() const;
        float queryConeOuterGain() const;
        float queryConeInnerAngle() const;
        float queryConeOuterAngle() const;
        void queryPosition(float&, float&, float&) const;
        void queryVelocity(float&, float&, float&) const;
        void queryDirection(float&, float&, float&) const;
        bool queryRelativePosition() const;
        
        void setPitch(float=1.f);
        void setGain(float=1.f);
        void setMaxDistance(float);
        void setRolloff(float);
        void setRefDistance(float);
        void setMinGain(float);
        void setMaxGain(float);
        void setConeOuterGain(float);
        void setConeInnerAngle(float);
        void setConeOuterAngle(float);
        void setPosition(float=0.f, float=0.f, float=0.f);
        void setVelocity(float=0.f, float=0.f, float=0.f);
        void setDirection(float, float, float);
        void setRelativePosition(bool);
        
        // Immutable
        SEB_SourceType querySourceType() const;
        SEB_SourceState querySourceState() const;
        int queryQueuedBuffers() const;
        int queryProcessedBuffers() const;
        float queryOffsetSeconds() const;
        int queryOffsetSamples() const;
        int queryOffsetBytes() const;
        
        Lua::ReturnValues LuaQueryPosition() const;
        Lua::ReturnValues LuaQueryVelocity() const;
        Lua::ReturnValues LuaQueryDirection() const;
    };
    
    typedef RefCounted<SoundEmitterImpl> SoundEmitter;
}

template <> struct MetatableDescriptor<LuaApi::SoundEmitterImpl> {
    static char const* name() { return "sound_mt"; }
    static char const* luaname() { return ""; }
    static char const* constructor() { return ""; }
    static bool construct(LuaApi::SoundEmitterImpl*) { return false; }
    static void metatable(Lua::member_function_storage<LuaApi::SoundEmitterImpl>& mt) {
#define OAL_PROPERTY(prop) mt[ #prop ] = Lua::Transform(&LuaApi::SoundEmitterImpl::query##prop);\
                            mt[ "Set" #prop ] = Lua::Transform(&LuaApi::SoundEmitterImpl::set##prop)
#define OAL_PROPERTY_ALT(prop) mt[ #prop ] = Lua::Transform(&LuaApi::SoundEmitterImpl::LuaQuery##prop);\
                            mt[ "Set" #prop ] = Lua::Transform(&LuaApi::SoundEmitterImpl::set##prop)
#define OAL_PROPERTY_RO(prop) mt[ #prop ] = Lua::Transform(&LuaApi::SoundEmitterImpl::query##prop)

        OAL_PROPERTY(Pitch);
        OAL_PROPERTY(Gain);
        OAL_PROPERTY(MaxDistance);
        OAL_PROPERTY(Rolloff);
        OAL_PROPERTY(RefDistance);
        OAL_PROPERTY(MinGain);
        OAL_PROPERTY(MaxGain);
        OAL_PROPERTY(ConeOuterGain);
        OAL_PROPERTY(ConeInnerAngle);
        OAL_PROPERTY(ConeOuterAngle);
        OAL_PROPERTY(RelativePosition);
        OAL_PROPERTY(Looping);
        OAL_PROPERTY_ALT(Position);
        OAL_PROPERTY_ALT(Velocity);
        OAL_PROPERTY_ALT(Direction);
        //OAL_PROPERTY_RO(SourceType);
        //OAL_PROPERTY_RO(SourceState);
        OAL_PROPERTY_RO(QueuedBuffers);
        OAL_PROPERTY_RO(ProcessedBuffers);
        OAL_PROPERTY_RO(OffsetSeconds);
        OAL_PROPERTY_RO(OffsetSamples);
        OAL_PROPERTY_RO(OffsetBytes);
        OAL_PROPERTY_RO(Frequency);
        OAL_PROPERTY_RO(Bits);
        OAL_PROPERTY_RO(Channels);
        OAL_PROPERTY_RO(ByteSize);
#undef OAL_PROPERTY
#undef OAL_PROPERTY_ALT
#undef OAL_PROPERTY_RO
        
        mt["IsValid"] = Lua::Transform(&LuaApi::SoundEmitterImpl::IsValid);
        mt["Play"] = Lua::Transform(&LuaApi::SoundEmitterImpl::Play);
        mt["Pause"] = Lua::Transform(&LuaApi::SoundEmitterImpl::Pause);
        mt["Stop"] = Lua::Transform(&LuaApi::SoundEmitterImpl::Stop);
        mt["Rewind"] = Lua::Transform(&LuaApi::SoundEmitterImpl::Rewind);
    }
};
#endif
