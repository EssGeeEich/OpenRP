#include "rp_openal.h"
#include "gamewindow.h"
#include <cstring>
#include <vorbis/vorbisfile.h>

enum {
    OGG_MAIN_BUFFER_SIZE = 8 * MEGABYTE,
    OGG_SUB_BUFFER_SIZE = 1536 * KILOBYTE,
    
    OGG_MAIN_BUFFER_THRESHOLD = OGG_MAIN_BUFFER_SIZE * 7 / 8
};

class OggFileReaderCallback {
    QIODevice& m_device;
public:
    OggFileReaderCallback(QIODevice& device)
        : m_device(device) {}
    static size_t read(void* ptr, size_t size, size_t nmemb, void* pSelf)
    {
        if(!size || !nmemb)
            return 0;
        
        OggFileReaderCallback* ofrc = reinterpret_cast<OggFileReaderCallback*>(pSelf);
        qint64 rv = ofrc->m_device.read(reinterpret_cast<char*>(ptr),size * nmemb);
        if(rv <= 0)
            return 0;
        return static_cast<size_t>(rv / static_cast<qint64>(size));
    }
    
    static ov_callbacks Callbacks() {
        ov_callbacks cb;
        cb.read_func = &OggFileReaderCallback::read;
        cb.seek_func = nullptr;
        cb.close_func = nullptr;
        cb.tell_func = nullptr;
        return cb;
    }
};

template <size_t elemSize> struct sizedElement;
template <> struct sizedElement<1> {
    typedef std::uint8_t type;
};
template <> struct sizedElement<2> {
    typedef std::uint16_t type;
};


template <typename T>
void Reorder6(T* start, std::size_t groupCount)
{
    T* end = start + (groupCount * 6);
    for(; start != end; start += 6)
    {
        std::swap(start[1], start[2]);
        std::swap(start[4], start[5]);
        std::swap(start[3], start[4]);
    }
}

template <typename T>
void Reorder7(T* start, std::size_t groupCount)
{
    T* end = start + (groupCount * 7);
    for(; start != end; start += 7)
    {
        std::swap(start[1], start[2]);
        std::swap(start[3], start[6]);
        std::swap(start[4], start[6]);
        std::swap(start[5], start[6]);
    }
}

template <typename T>
void Reorder8(T* start, std::size_t groupCount)
{
    T* end = start + (groupCount * 8);
    for(; start != end; start += 8)
    {
        std::swap(start[1], start[2]);
        std::swap(start[3], start[7]);
        std::swap(start[4], start[5]);
        std::swap(start[5], start[6]);
        std::swap(start[6], start[7]);
    }
}


static thread_local std::vector<unsigned char> g_oggMainBuffer;
static thread_local std::vector<unsigned char> g_oggSubBuffer;

namespace LuaApi {

OggSampleEmitter::OggSampleEmitter(Context* c) : SoundEmitterBase(c) {}
bool OggSampleEmitter::loadFile(const char* filename) {
    QFile f(filename);
    if(!f.open(QFile::ReadOnly))
        return false;
    
    OggFileReaderCallback ofrc(f);
    OggVorbis_File vorbFile;
    int state = ov_open_callbacks(reinterpret_cast<void*>(&ofrc),&vorbFile, nullptr, 0, ofrc.Callbacks());
    if(state != 0)
        return false;
    g_oggMainBuffer.reserve(OGG_MAIN_BUFFER_SIZE);
    g_oggSubBuffer.resize(OGG_SUB_BUFFER_SIZE);
    
    vorbis_info info;
    {
        vorbis_info* pInfo = ov_info(&vorbFile,-1);
        if(!pInfo)
        {
            ov_clear(&vorbFile);
            return false;
        }
        info = *pInfo;
    }
    
    // Just change this to 1 to use 8-bit audio data.
    int constexpr sampleByteSize = 2;
    
    SEB_BufferFormat fmt = SBF_INVALID;
    if(sampleByteSize == 1)
    {
        switch(info.channels)
        {
        case 1:
            fmt = SBF_MONO_8;
            break;
        case 2:
            fmt = SBF_STEREO_8;
            break;
        case 4:
            fmt = SBF_QUAD_8;
            break;
        case 6:
            fmt = SBF_51CHN_8;
            break;
        case 7:
            fmt = SBF_61CHN_8;
            break;
        case 8:
            fmt = SBF_71CHN_8;
            break;
        default:
            break;
        }
    }
    else if(sampleByteSize == 2)
    {
        switch(info.channels)
        {
        case 1:
            fmt = SBF_MONO_16;
            break;
        case 2:
            fmt = SBF_STEREO_16;
            break;
        case 4:
            fmt = SBF_QUAD_16;
            break;
        case 6:
            fmt = SBF_51CHN_16;
            break;
        case 7:
            fmt = SBF_61CHN_16;
            break;
        case 8:
            fmt = SBF_71CHN_16;
            break;
        default:
            break;
        }
    }
    
    if(fmt == SBF_INVALID)
    {
        ov_clear(&vorbFile);
        return false;
    }
    
    int section = 0;
    std::vector<unsigned char>& buffer = g_oggMainBuffer;
    std::vector<unsigned char>& tempBuffer = g_oggSubBuffer;
    
    for(;;) {
        long bytes = ov_read(&vorbFile, reinterpret_cast<char*>(tempBuffer.data()),
                             tempBuffer.size(), 0, sampleByteSize, 1, &section);
        
        if(!bytes)
        {
            ov_clear(&vorbFile);
            break;
        }
        
        if(bytes < 0)
        {
            ov_clear(&vorbFile);
            return false;
        }
        
        std::size_t oldSize = buffer.size();
        buffer.resize(oldSize + bytes);
        
        unsigned char* source = tempBuffer.data();
        unsigned char* destination = buffer.data() + oldSize;
        std::memcpy(destination, source, bytes);
        
        if(buffer.size() >= OGG_MAIN_BUFFER_THRESHOLD)
        {
            long const samples = (buffer.size() / sampleByteSize) / info.channels;
            
            if(info.channels == 6)
            {
                Reorder6(reinterpret_cast<typename sizedElement<sampleByteSize>::type*>(buffer.data()), samples);
            }
            else if(info.channels == 7)
            {
                Reorder7(reinterpret_cast<typename sizedElement<sampleByteSize>::type*>(buffer.data()), samples);
            }
            else if(info.channels == 8)
            {
                Reorder8(reinterpret_cast<typename sizedElement<sampleByteSize>::type*>(buffer.data()), samples);
            }
            this->queueData(fmt, buffer.data(), buffer.size(), info.rate);
            buffer.clear();
        }
    }
    
    if(buffer.size())
    {
        long const samples = (buffer.size() / sampleByteSize) / info.channels;
        
        if(info.channels == 6)
        {
            Reorder6(reinterpret_cast<typename sizedElement<sampleByteSize>::type*>(buffer.data()), samples);
        }
        else if(info.channels == 7)
        {
            Reorder7(reinterpret_cast<typename sizedElement<sampleByteSize>::type*>(buffer.data()), samples);
        }
        else if(info.channels == 8)
        {
            Reorder8(reinterpret_cast<typename sizedElement<sampleByteSize>::type*>(buffer.data()), samples);
        }
        this->queueData(fmt, buffer.data(), buffer.size(), info.rate);
        buffer.clear();
    }
    
    tempBuffer.clear();
    return true;
}

/*namespace impl {
SoundEmitterBase* b = nullptr;
    void TestOpenAL() {
        DeviceList* dl = new DeviceList();
        Device* defaultDevice = dl->Default();
        if(defaultDevice->initialize())
        {
            Context* ctx = defaultDevice->createContext();
            if(ctx)
            {
                ctx->bind();
                SoundEmitterBase* seb = ctx->loadOggFile("C:\\Users\\grass\\Desktop\\Example.ogg");
                if(seb)
                {
                    seb->play();
                    seb->setLooping(true);
                    seb->setPosition(-1.f);
                    b = seb;
                }
            }
        }
    }
    
    void UpdateOpenAL() {
        if(b)
            b->update();
    }
}*/

}
