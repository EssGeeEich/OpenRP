#ifndef LUA_OAL_LINK_H
#define LUA_OAL_LINK_H
#include "shared.h"
#include <initializer_list>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include "state.h"
#include "library.h"

namespace LuaApi {
    typedef Lua::lua_exception al_error;

    class Context;
    class Device;
    class DeviceList;
    
    enum SEB_SourceType {
        SST_UNDETERMINED = AL_UNDETERMINED,
        SST_STATIC = AL_STATIC,
        SST_STREAMING = AL_STREAMING
    };
    
    enum SEB_SourceState {
        SSS_INITIAL = AL_INITIAL,
        SSS_PLAYING = AL_PLAYING,
        SSS_PAUSED = AL_PAUSED,
        SSS_STOPPED = AL_STOPPED
    };
    
    enum SCP_DistanceModel {
        SDM_INVERSE_DISTANCE = AL_INVERSE_DISTANCE,
        SDM_INVERSE_DISTANCE_CLAMPED = AL_INVERSE_DISTANCE_CLAMPED,
        SDM_LINEAR_DISTANCE = AL_LINEAR_DISTANCE,
        SDM_LINEAR_DISTANCE_CLAMPED = AL_LINEAR_DISTANCE_CLAMPED,
        SDM_EXPONENT_DISTANCE = AL_EXPONENT_DISTANCE,
        SDM_EXPONENT_DISTANCE_CLAMPED = AL_EXPONENT_DISTANCE_CLAMPED
    };
    
    enum SEB_BufferFormat {
        SBF_INVALID = 0,
        SBF_MONO_8 = AL_FORMAT_MONO8,
        SBF_MONO_16 = AL_FORMAT_MONO16,
        SBF_STEREO_8 = AL_FORMAT_STEREO8,
        SBF_STEREO_16 = AL_FORMAT_STEREO16
        
#ifdef AL_EXT_float32
        ,SBF_MONO_F32 = AL_FORMAT_MONO_FLOAT32,
        SBF_STEREO_F32 = AL_FORMAT_STEREO_FLOAT32
#endif
        
#ifdef AL_EXT_double
        ,SBF_MONO_DOUBLE = AL_FORMAT_MONO_DOUBLE_EXT,
        SBF_STEREO_DOUBLE = AL_FORMAT_STEREO_DOUBLE_EXT
#endif
        
#ifdef AL_EXT_MCFORMATS
        ,SBF_QUAD_8 = AL_FORMAT_QUAD8,
        SBF_QUAD_16 = AL_FORMAT_QUAD16,
        SBF_QUAD_32 = AL_FORMAT_QUAD32,
        
        SBF_REAR_8 = AL_FORMAT_REAR8,
        SBF_REAR_16 = AL_FORMAT_REAR16,
        SBF_REAR_32 = AL_FORMAT_REAR32,
        
        SBF_51CHN_8 = AL_FORMAT_51CHN8,
        SBF_51CHN_16 = AL_FORMAT_51CHN16,
        SBF_51CHN_32 = AL_FORMAT_51CHN32,
        
        SBF_61CHN_8 = AL_FORMAT_61CHN8,
        SBF_61CHN_16 = AL_FORMAT_61CHN16,
        SBF_61CHN_32 = AL_FORMAT_61CHN32,
        
        SBF_71CHN_8 = AL_FORMAT_71CHN8,
        SBF_71CHN_16 = AL_FORMAT_71CHN16,
        SBF_71CHN_32 = AL_FORMAT_71CHN32
#endif
    };
    
    struct SoundContextProperties {
        static float queryDopplerFactor();
        static float querySoundSpeed();
        static SCP_DistanceModel queryDistanceModel();
        
        static void setDopplerFactor(float=1.f);
        static void setSoundSpeed(float=343.3f);
        static void setDistanceModel(SCP_DistanceModel=SDM_INVERSE_DISTANCE_CLAMPED);
        
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
    };
    
    class SoundEmitterBase {
        Context* m_context;
        
        class SoundHolder {
            ALuint m_source;
            std::vector<ALuint> m_buffers;
            
            SEB_BufferFormat m_format;
            std::size_t m_frequency;
            
            SoundHolder(SoundHolder const&) =delete;
            SoundHolder& operator= (SoundHolder const&) =delete;
            
            bool free();
        public:
            SoundHolder(SoundHolder&& o);
            SoundHolder& operator= (SoundHolder&& o);
            SoundHolder(ALuint);
            ~SoundHolder();
            
            void addBuffer(ALuint);
            std::vector<ALuint>& buffers();
            ALuint source() const;
            
            SEB_BufferFormat format() const;
            bool setFormat(SEB_BufferFormat);
            std::size_t frequency() const;
            bool setFrequency(std::size_t);
        };
        
        std::shared_ptr<SoundHolder> m_sound;
    protected:
        SoundEmitterBase(Context*);
        bool queueData(SEB_BufferFormat, void const*, std::size_t, std::size_t);
    public:
        inline SoundEmitterBase() {
            throw Lua::lua_exception("You cannot create a new SoundEmitter like this!");
        }
        
        SoundEmitterBase(SoundEmitterBase const&) =default;
        SoundEmitterBase(SoundEmitterBase&&) =default;
        SoundEmitterBase& operator= (SoundEmitterBase const&) =default;
        SoundEmitterBase& operator= (SoundEmitterBase&&) =default;
        
        // RefCnt
        bool good() const;
        void release();
        
        // Source actions
        void play();
        void pause();
        void stop();
        void rewind();
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
        
        virtual ~SoundEmitterBase();
    };
    
    class OggSampleEmitter : public SoundEmitterBase {
        friend class ::LuaApi::Context;
    protected:
        OggSampleEmitter(Context*);
    public:
        bool loadFile(char const*);
    };
    
    class Context {
        class ContextHolder {
            ALCcontext* m_context;
            
            ContextHolder(ContextHolder const&) =delete;
            ContextHolder& operator= (ContextHolder const&) =delete;
            
            bool free();
        public:
            ContextHolder(ALCcontext* c);
            ContextHolder(ContextHolder&&);
            ContextHolder& operator= (ContextHolder&&);
            ~ContextHolder();
            
            ALCcontext* context() const;
        };

        friend class ::LuaApi::Device;
        Device* m_owner;
        std::shared_ptr<ContextHolder> m_context;
        
        void doInit(ALCint*);
        Context(Device*);
    public:
        inline Context() {
            throw Lua::lua_exception("You cannot create a new Context like this!");
        }
        Context& operator= (Context&&) =default;
        Context(Context&&) =default;
        Context& operator= (Context const&) =default;
        Context(Context const&) =default;
        
        Context(Device*,ALCint*);
        Context(Device* d,std::initializer_list<ALCint> const& args)
            : Context(d)
        {
            ALCint data[args.size() + 1];
            int i = 0;
            for(auto it = args.begin(); it != args.end(); ++it, ++i)
            {
                data[i] = *it;
            }
            data[args.size()] = 0;
            doInit(data);
        }
        bool initialized() const;
        void release();
        bool bind() const;
        
        SoundEmitterBase* loadOggFile(char const*);
        
        Lua::ReturnValues LuaLoadFile(std::string const&);
        ~Context();
    };

    class Device {
        class DeviceHolder {
            ALCdevice* m_device;
            
            DeviceHolder(DeviceHolder const&) =delete;
            DeviceHolder& operator= (DeviceHolder const&) =delete;
            
            bool free();
        public:
            DeviceHolder(ALCdevice* d);
            DeviceHolder(DeviceHolder&&);
            DeviceHolder& operator= (DeviceHolder&&);
            ~DeviceHolder();
            
            ALCdevice* device() const;
        };

        friend class ::LuaApi::Context;
        friend class ::LuaApi::DeviceList;
        

        std::string m_name;
        
        class DeviceData {
            DeviceHolder m_device;
            std::vector<Context> m_contexts;
            std::size_t m_lastContext;
        public:
            enum {
                MAX_CONTEXTS = 8
            };
            DeviceData(ALCdevice* dev);
            std::vector<Context>& contexts();
            std::size_t& lastContext();
            ALCdevice* device() const;
        };

        std::shared_ptr<DeviceData> m_device;
    public:
        inline Device() {
            throw Lua::lua_exception("You cannot create a new Device like this!");
        }

        Device& operator= (Device&&) =default;
        Device& operator= (Device const&) =default;
        Device(Device const&) =default;
        Device(Device&&) =default;
        Device(std::string);
        ~Device();
        
        bool initialized() const;
        bool initialize();
        void release();
        std::string name() const;
        Context* createContext();
        
        Lua::ReturnValues LuaOpenContext();
    };

    // Global "Audio" in Lua
    class DeviceList {
        std::vector<Device> m_devices;
        
        DeviceList(DeviceList const&) =delete;
        DeviceList& operator=(DeviceList const&) =delete;
    public:
        DeviceList();
        std::vector<Device>& Devices();
        Device* Default();
        
        Lua::Array<std::string> LuaDevices() const;
        std::string LuaDefaultDevice() const;
        Lua::ReturnValues LuaOpenDevice(std::string const&);
    };
}

// Global "Audio" in Lua
template <> struct MetatableDescriptor<LuaApi::DeviceList> {
    static char const* name() { return "devicelist_mt"; }
    static char const* luaname() { return ""; }
    static char const* constructor() { return ""; }
    static void metatable(Lua::member_function_storage<LuaApi::DeviceList>& mt) {
        mt["List"] = Lua::Transform(&LuaApi::DeviceList::LuaDevices);
        mt["Default"] = Lua::Transform(&LuaApi::DeviceList::LuaDefaultDevice);
        mt["Open"] = Lua::Transform(&LuaApi::DeviceList::LuaOpenDevice);
    }
};

template <> struct MetatableDescriptor<LuaApi::Device> {
    static char const* name() { return "device_mt"; }
    static char const* luaname() { return ""; }
    static char const* constructor() { return ""; }
    static void metatable(Lua::member_function_storage<LuaApi::Device>& mt) {
        mt["Name"] = Lua::Transform(&LuaApi::Device::name);
        mt["CreateContext"] = Lua::Transform(&LuaApi::Device::LuaOpenContext);
    }
};

template <> struct MetatableDescriptor<LuaApi::Context> {
    static char const* name() { return "context_mt"; }
    static char const* luaname() { return ""; }
    static char const* constructor() { return ""; }
    static void metatable(Lua::member_function_storage<LuaApi::Context>& mt) {
        mt["Bind"] = Lua::Transform(&LuaApi::Context::bind);
        mt["LoadFile"] = Lua::Transform(&LuaApi::Context::LuaLoadFile);
    }
};

template <> struct MetatableDescriptor<LuaApi::SoundEmitterBase> {
    static char const* name() { return "sound_mt"; }
    static char const* luaname() { return ""; }
    static char const* constructor() { return ""; }
    static void metatable(Lua::member_function_storage<LuaApi::SoundEmitterBase>& mt) {
#define OAL_PROPERTY(prop) mt[ #prop ] = Lua::Transform(&LuaApi::SoundEmitterBase::query##prop);\
                            mt[ "Set" #prop ] = Lua::Transform(&LuaApi::SoundEmitterBase::set##prop)
#define OAL_PROPERTY_ALT(prop) mt[ #prop ] = Lua::Transform(&LuaApi::SoundEmitterBase::LuaQuery##prop);\
                            mt[ "Set" #prop ] = Lua::Transform(&LuaApi::SoundEmitterBase::set##prop)
#define OAL_PROPERTY_RO(prop) mt[ #prop ] = Lua::Transform(&LuaApi::SoundEmitterBase::query##prop)

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
        
        mt["Play"] = Lua::Transform(&LuaApi::SoundEmitterBase::play);
        mt["Pause"] = Lua::Transform(&LuaApi::SoundEmitterBase::pause);
        mt["Stop"] = Lua::Transform(&LuaApi::SoundEmitterBase::stop);
        mt["Rewind"] = Lua::Transform(&LuaApi::SoundEmitterBase::rewind);
    }
};

#endif // LUA_OAL_LINK_H
