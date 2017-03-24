#include "texture.h"

namespace LuaApi {

// TextureImpl::TexData
TextureImpl::TexData::TexData(QString const& qs, bool genMM)
    : m_image(qs),
      m_texture(m_image, genMM ? QOpenGLTexture::GenerateMipMaps
                               : QOpenGLTexture::DontGenerateMipMaps) {}
QOpenGLTexture& TextureImpl::TexData::texture()
{
    return m_texture;
}
bool TextureImpl::TexData::good() const
{
    return (!m_image.isNull()) && (m_texture.isCreated());
}
std::size_t TextureImpl::TexData::width() const
{
    return m_image.width();
}
std::size_t TextureImpl::TexData::height() const
{
    return m_image.height();
}

// TextureImpl
std::uint32_t TextureImpl::FilterToUint(QOpenGLTexture::Filter f, bool& r)
{
    switch(f)
    {
    case QOpenGLTexture::Nearest:
    case QOpenGLTexture::NearestMipMapLinear:
    case QOpenGLTexture::NearestMipMapNearest:
    case QOpenGLTexture::Linear:
    case QOpenGLTexture::LinearMipMapLinear:
    case QOpenGLTexture::LinearMipMapNearest:
        r = true;
        return static_cast<std::uint32_t>(f);
    default:
        r = false;
        return 0;
    }
}
QOpenGLTexture::Filter TextureImpl::UintToFilter(std::uint32_t f, bool& r)
{
    switch(f)
    {
    case GL_NEAREST:
    case GL_NEAREST_MIPMAP_LINEAR:
    case GL_NEAREST_MIPMAP_NEAREST:
    case GL_LINEAR:
    case GL_LINEAR_MIPMAP_LINEAR:
    case GL_LINEAR_MIPMAP_NEAREST:
        r = true;
        return static_cast<QOpenGLTexture::Filter>(f);
    default:
        r = false;
        return static_cast<QOpenGLTexture::Filter>(0);
    }
}
std::uint32_t TextureImpl::WrapToUint(QOpenGLTexture::WrapMode w, bool& r)
{
    switch(w)
    {
    case QOpenGLTexture::Repeat:
    case QOpenGLTexture::MirroredRepeat:
    case QOpenGLTexture::ClampToEdge:
    case QOpenGLTexture::ClampToBorder:
        r = true;
        return static_cast<std::uint32_t>(w);
    default:
        r = false;
        return 0;
    }
}
QOpenGLTexture::WrapMode TextureImpl::UintToWrap(std::uint32_t w, bool& r)
{
    switch(w)
    {
    case GL_REPEAT:
    case GL_MIRRORED_REPEAT:
    case GL_CLAMP_TO_EDGE:
    case GL_CLAMP_TO_BORDER:
        r = true;
        return static_cast<QOpenGLTexture::WrapMode>(w);
        break;
    default:
        r = false;
        return static_cast<QOpenGLTexture::WrapMode>(0);
    }
}

bool TextureImpl::load(std::string const& path, Lua::Arg<bool> const& genMipMaps)
{
    unload();
    m_data = std::make_shared<TexData>(QString::fromStdString(path),genMipMaps.get_safe(false));
    if(!m_data->good())
    {
        unload();
        return false;
    }
    return true;
}
void TextureImpl::unload() { m_data.reset(); }
Lua::ReturnValues TextureImpl::size() const
{
    if(m_data)
        return Lua::Return(m_data->width(), m_data->height());
    return Lua::Return<std::size_t, std::size_t>(0,0);
}
QOpenGLTexture* TextureImpl::texture() const
{
    if(m_data)
        return &(m_data->texture());
    return nullptr;
}
bool TextureImpl::good() const { return m_data != nullptr; }
std::uint32_t TextureImpl::magfilter() const {
    if(m_data)
    {
        bool r = false;
        std::uint32_t v = TextureImpl::FilterToUint(m_data->texture().magnificationFilter(),r);
        if(r)
            return v;
    }
    return 0;
}
std::uint32_t TextureImpl::minfilter() const {
    if(m_data)
    {
        bool r = false;
        std::uint32_t v = TextureImpl::FilterToUint(m_data->texture().minificationFilter(),r);
        if(r)
            return v;
    }
    return 0;
}
std::uint32_t TextureImpl::wraps() const {
    if(m_data)
    {
        bool r = false;
        std::uint32_t v = TextureImpl::WrapToUint(m_data->texture().wrapMode(QOpenGLTexture::DirectionS),r);
        if(r)
            return v;
    }
    return 0;
}
std::uint32_t TextureImpl::wrapt() const {
    if(m_data)
    {
        bool r = false;
        std::uint32_t v = TextureImpl::WrapToUint(m_data->texture().wrapMode(QOpenGLTexture::DirectionT),r);
        if(r)
            return v;
    }
    return 0;
}
std::uint32_t TextureImpl::wrapr() const {
    if(m_data)
    {
        bool r = false;
        std::uint32_t v = TextureImpl::WrapToUint(m_data->texture().wrapMode(QOpenGLTexture::DirectionR),r);
        if(r)
            return v;
    }
    return 0;
}
float TextureImpl::anisotropy() const {
    if(m_data)
        return m_data->texture().maximumAnisotropy();
    return 0.f;
}

void TextureImpl::setfilter(std::uint32_t flag) {
    if(m_data)
    {
        bool r = false;
        QOpenGLTexture::Filter f = TextureImpl::UintToFilter(flag,r);
        if(!r)
            return;

        m_data->texture().setMinificationFilter(f);
        m_data->texture().setMagnificationFilter(f);
    }
}
void TextureImpl::setmagfilter(std::uint32_t flag) {
    if(m_data)
    {
        bool r = false;
        QOpenGLTexture::Filter f = TextureImpl::UintToFilter(flag,r);
        if(!r)
            return;
        switch(f)
        {
        case QOpenGLTexture::NearestMipMapNearest:
        case QOpenGLTexture::NearestMipMapLinear:
            r = QOpenGLTexture::Nearest;
            break;
        case QOpenGLTexture::LinearMipMapLinear:
        case QOpenGLTexture::LinearMipMapNearest:
            r = QOpenGLTexture::Linear;
            break;
        default:
            break;
        }

        m_data->texture().setMagnificationFilter(f);
    }
}
void TextureImpl::setminfilter(std::uint32_t flag) {
    if(m_data)
    {
        bool r = false;
        QOpenGLTexture::Filter f = TextureImpl::UintToFilter(flag,r);
        if(!r)
            return;

        m_data->texture().setMinificationFilter(f);
    }
}
void TextureImpl::setanisotropy(float max) {
    if(m_data)
        m_data->texture().setMaximumAnisotropy(max);
}
void TextureImpl::setwraps(std::uint32_t flag) {
    if(m_data)
    {
        bool r = false;
        QOpenGLTexture::WrapMode w = TextureImpl::UintToWrap(flag,r);
        if(!r)
            return;
        m_data->texture().setWrapMode(QOpenGLTexture::DirectionS,w);
    }
}
void TextureImpl::setwrapt(std::uint32_t flag) {
    if(m_data)
    {
        bool r = false;
        QOpenGLTexture::WrapMode w = TextureImpl::UintToWrap(flag,r);
        if(!r)
            return;
        m_data->texture().setWrapMode(QOpenGLTexture::DirectionT,w);
    }
}
void TextureImpl::setwrapr(std::uint32_t flag) {
    if(m_data)
    {
        bool r = false;
        QOpenGLTexture::WrapMode w = TextureImpl::UintToWrap(flag,r);
        if(!r)
            return;
        m_data->texture().setWrapMode(QOpenGLTexture::DirectionR,w);
    }
}
}
