#include "lua_ogl_link.h"
#include "gamewindow.h"
#include <QOpenGLFunctions>
#include <QDebug>

namespace LuaApi {

// Texture impl
Texture::TexData::TexData(QString const& qs, bool genMM)
    : m_image(qs),
      m_texture(m_image, genMM ? QOpenGLTexture::GenerateMipMaps
                               : QOpenGLTexture::DontGenerateMipMaps) {}
QOpenGLTexture& Texture::TexData::texture()
{
    return m_texture;
}
bool Texture::TexData::isValid() const
{
    return (!m_image.isNull()) && (m_texture.isCreated());
}
std::size_t Texture::TexData::width() const
{
    return m_image.width();
}
std::size_t Texture::TexData::height() const
{
    return m_image.height();
}

// Texture
bool Texture::load(std::string const& path, Lua::Arg<bool> const& genMipMaps)
{
    unload();
    qDebug() << QString::fromStdString(path);
    m_data = std::make_shared<TexData>(QString::fromStdString(path),genMipMaps.get_safe(false));
    if(!m_data->isValid())
    {
        unload();
        return false;
    }
    return true;
}
void Texture::unload()
{
    m_data.reset();
}
Lua::ReturnValues<std::size_t, std::size_t> Texture::size() const
{
    if(m_data)
        return Lua::Return(m_data->width(), m_data->height());
    return Lua::Return<std::size_t, std::size_t>(0,0);
}
QOpenGLTexture* Texture::texture() const
{
    if(m_data)
        return &(m_data->texture());
    return nullptr;
}
bool Texture::good() const
{
    return m_data.get() != nullptr;
}
void Texture::setfilter(std::uint32_t flag) {
    if(m_data)
    {
        QOpenGLTexture::Filter f;
        switch(flag)
        {
        case GL_NEAREST:
            f = QOpenGLTexture::Nearest;
            break;
        case GL_NEAREST_MIPMAP_LINEAR:
            f = QOpenGLTexture::NearestMipMapLinear;
            break;
        case GL_NEAREST_MIPMAP_NEAREST:
            f = QOpenGLTexture::NearestMipMapNearest;
            break;
        case GL_LINEAR:
            f = QOpenGLTexture::Linear;
            break;
        case GL_LINEAR_MIPMAP_LINEAR:
            f = QOpenGLTexture::LinearMipMapLinear;
            break;
        case GL_LINEAR_MIPMAP_NEAREST:
            f = QOpenGLTexture::LinearMipMapNearest;
            break;
        default:
            return;
        }

        m_data->texture().setMinificationFilter(f);
        m_data->texture().setMagnificationFilter(f);
    }
}
void Texture::setanisotropy(float max) {
    if(m_data)
        m_data->texture().setMaximumAnisotropy(max);
}

// Shader
bool Shader::load(std::string const& shd1, std::string const& shd2, Lua::Arg<std::string> const& geom)
{
    unload();
    m_program = std::make_shared<QOpenGLShaderProgram>();
    if(!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, QString::fromStdString(shd1)) ||
        !m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, QString::fromStdString(shd2)))
    {
        unload();
        return false;
    }
    
    if(geom && !m_program->addShaderFromSourceCode(QOpenGLShader::Geometry, QString::fromStdString(*geom)))
    {
        unload();
        return false;
    }
    if(!m_program->link())
    {
        unload();
        return false;
    }
    return true;
}
bool Shader::loadfromfile(std::string const& shd1, std::string const& shd2, Lua::Arg<std::string> const& geom)
{
    unload();
    m_program = std::make_shared<QOpenGLShaderProgram>();
    if(!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(shd1)) ||
            !m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(shd2)))
    {
        unload();
        return false;
    }
    
    if(geom && !m_program->addShaderFromSourceFile(QOpenGLShader::Geometry, QString::fromStdString(*geom)))
    {
        unload();
        return false;
    }
    if(!m_program->link())
    {
        unload();
        return false;
    }
    return true;
}
void Shader::unload()
{
    m_program.reset();
}
QOpenGLShaderProgram* Shader::shader() const { return m_program.get(); }
bool Shader::good() const { return m_program.get() != nullptr; }

// Object impl
namespace impl {
// SharedBuffer
SharedBuffer::SharedBuffer()
    : m_tupleSize(0) {}
bool SharedBuffer::create(QOpenGLBuffer::Type type) {
    m_buffer = std::make_shared<QOpenGLBuffer>(type);
    return m_buffer->create();
}
void SharedBuffer::unload() {
    m_buffer.reset();
    m_tupleSize = 0;
}
QOpenGLBuffer* SharedBuffer::buffer() const { return m_buffer.get(); }
void SharedBuffer::setTupleSize(std::uint32_t s) { m_tupleSize = s; }
std::uint32_t SharedBuffer::tupleSize() const { return m_tupleSize; }

// ObjectData Base
ObjectData_Base::ObjectData_Base() : m_vertices(0) {}
ObjectData_Base::~ObjectData_Base() {}
QOpenGLVertexArrayObject& ObjectData_Base::VAO() { return m_vao; }
SharedBuffer* ObjectData_Base::VBO(std::size_t ix) { if(ix >= 16) return nullptr; return &m_vbo[ix]; }
QOpenGLBuffer* ObjectData_Base::IBO() { return nullptr; }
void ObjectData_Base::setVertices(uint32_t v) { m_vertices = v; }
std::uint32_t ObjectData_Base::vertices() const { return m_vertices; }
bool ObjectData_Base::Create() { return m_vao.create(); }

// ObjectData Non Indexed
ObjectData_NonIndexed::~ObjectData_NonIndexed() {}
void ObjectData_NonIndexed::Apply() { m_vao.bind(); }

// ObjectData Indexed
ObjectData_Indexed::ObjectData_Indexed() :
    m_ibo(QOpenGLBuffer::IndexBuffer), m_32bit(false), m_indices(0) {}
ObjectData_Indexed::~ObjectData_Indexed() {}
void ObjectData_Indexed::Apply() { m_vao.bind(); }
QOpenGLBuffer* ObjectData_Indexed::IBO() { return &m_ibo; }
bool ObjectData_Indexed::Create() {
    return ObjectData_Base::Create() &&
            m_ibo.create();
}
void ObjectData_Indexed::set32bit(bool v) { m_32bit = v; }
bool ObjectData_Indexed::is32bit() const { return m_32bit; }
void ObjectData_Indexed::setIndices(std::uint32_t v) { m_indices = v; }
std::uint32_t ObjectData_Indexed::indices() const { return m_indices; }
}

// Object
bool Object::create(std::uint32_t vxCount)
{
    m_data = std::shared_ptr<impl::ObjectData_Base>(new impl::ObjectData_NonIndexed);
    if(!m_data->Create())
    {
        m_data.reset();
        return false;
    }
    m_data->setVertices(vxCount);
    
    return true;
}
bool Object::create_indexed(std::uint32_t vxCount)
{
    m_data = std::shared_ptr<impl::ObjectData_Base>(new impl::ObjectData_Indexed);
    if(!m_data->Create())
    {
        m_data.reset();
        return false;
    }
    m_data->setVertices(vxCount);
    return true;
}
bool Object::setindices(const Lua::Array<std::uint16_t>& indices)
{
    if(!m_data || !m_data->IBO())
        return false;
    for(auto it = indices.m_data.begin(); it != indices.m_data.end(); ++it)
    {
        if(*it >= m_data->vertices())
            return false; // Index out of bounds!
    }
    m_data->IBO()->allocate(indices.m_data.data(), indices.m_data.size() * sizeof(indices.m_data[0]));
    static_cast<impl::ObjectData_Indexed*>(m_data.get())->set32bit(false);
    static_cast<impl::ObjectData_Indexed*>(m_data.get())->setIndices(indices.m_data.size());
    return true;
}
bool Object::setindices_32(const Lua::Array<std::uint32_t>& indices)
{
    if(!m_data || !m_data->IBO())
        return false;
    for(auto it = indices.m_data.begin(); it != indices.m_data.end(); ++it)
    {
        if(*it >= m_data->vertices())
            return false; // Index out of bounds!
    }
    m_data->IBO()->allocate(indices.m_data.data(), indices.m_data.size() * sizeof(indices.m_data[0]));
    static_cast<impl::ObjectData_Indexed*>(m_data.get())->set32bit(true);
    static_cast<impl::ObjectData_Indexed*>(m_data.get())->setIndices(indices.m_data.size());
    return true;
}
bool Object::set1d(std::size_t attrib, const Lua::Array<float>& data)
{
    if(!m_data)
        return false;
    impl::SharedBuffer* buf = m_data->VBO(attrib);
    if(!buf)
        return false;
    if(data.m_data.empty())
    {
        buf->unload();
        return true;
    }
    if(data.m_data.size() != m_data->vertices())
        return false;
    if(!buf->create() || !buf->buffer()->bind())
        return false;
    buf->setTupleSize(1);
    buf->buffer()->setUsagePattern(QOpenGLBuffer::StaticDraw);
    buf->buffer()->allocate(data.m_data.data(), data.m_data.size() * sizeof(data.m_data[0]));
    buf->buffer()->release();
    return true;
}
bool Object::set2d(std::size_t attrib, const Lua::Array<float>& data)
{
    if(!m_data)
        return false;
    impl::SharedBuffer* buf = m_data->VBO(attrib);
    if(!buf)
        return false;
    if(data.m_data.empty())
    {
        buf->unload();
        return true;
    }
    if(data.m_data.size() != m_data->vertices() * 2)
        return false;
    if(!buf->create() || !buf->buffer()->bind())
        return false;
    buf->setTupleSize(2);
    buf->buffer()->setUsagePattern(QOpenGLBuffer::StaticDraw);
    buf->buffer()->allocate(data.m_data.data(), data.m_data.size() * sizeof(data.m_data[0]));
    buf->buffer()->release();
    return true;
}
bool Object::set3d(std::size_t attrib, const Lua::Array<float>& data)
{
    if(!m_data)
        return false;
    impl::SharedBuffer* buf = m_data->VBO(attrib);
    if(!buf)
        return false;
    if(data.m_data.empty())
    {
        buf->unload();
        return true;
    }
    if(data.m_data.size() != m_data->vertices() * 3)
        return false;
    if(!buf->create() || !buf->buffer()->bind())
        return false;
    buf->setTupleSize(3);
    buf->buffer()->setUsagePattern(QOpenGLBuffer::StaticDraw);
    buf->buffer()->allocate(data.m_data.data(), data.m_data.size() * sizeof(data.m_data[0]));
    buf->buffer()->release();
    return true;
}
bool Object::set4d(std::size_t attrib, const Lua::Array<float>& data)
{
    if(!m_data)
        return false;
    impl::SharedBuffer* buf = m_data->VBO(attrib);
    if(!buf)
        return false;
    if(data.m_data.empty())
    {
        buf->unload();
        return true;
    }
    if(data.m_data.size() != m_data->vertices() * 4)
        return false;
    if(!buf->create() || !buf->buffer()->bind())
        return false;
    buf->setTupleSize(4);
    buf->buffer()->setUsagePattern(QOpenGLBuffer::StaticDraw);
    buf->buffer()->allocate(data.m_data.data(), data.m_data.size() * sizeof(data.m_data[0]));
    buf->buffer()->release();
    return true;
}
bool Object::lock()
{
    QOpenGLContext* context = QOpenGLContext::currentContext();
    if(!m_data || !context)
        return false;
    QOpenGLFunctions* f = context->functions();
    if(!f)
        return false;
    m_data->VAO().bind();
    for(int i = 0; ; ++i)
    {
        impl::SharedBuffer* sb = m_data->VBO(i);
        if(!sb)
            break;
        if(sb->buffer() && sb->buffer()->bind())
        {
            f->glEnableVertexAttribArray(i);
            f->glVertexAttribPointer(i,sb->tupleSize(),GL_FLOAT,GL_FALSE,0,nullptr);
            continue;
        }
        f->glDisableVertexAttribArray(i);
    }
    
    if(m_data->IBO())
    {
        if(!m_data->IBO()->bind())
        {
            m_data->VAO().release();
            return false;
        }
    }
    m_data->VAO().release();
    return true;
}
void Object::bind()
{
    m_data->Apply();
}
void Object::draw()
{
    QOpenGLContext* context = QOpenGLContext::currentContext();
    if(!m_data || !context)
        return;
    QOpenGLFunctions* f = context->functions();
    if(!f)
        return;
    m_data->VAO().bind();
    if(m_data->IBO())
    {
        impl::ObjectData_Indexed* odi = static_cast<impl::ObjectData_Indexed*>(m_data.get());
        f->glDrawElements(GL_TRIANGLES, odi->indices(), odi->is32bit() ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, 0);
    }
    else
    {
        f->glDrawArrays(GL_TRIANGLES, 0, m_data->vertices());
    }
}
void Object::unload()
{
    m_data.reset();
}
impl::ObjectData_Base* Object::data() const
{
    return m_data.get();
}
bool Object::good() const
{
    return m_data.get() != nullptr;
}

// DrawableState
void DrawableState::SetShader(Shader shd) {
    m_shader = shd;
}
void DrawableState::SetTexture(Texture tex, std::size_t index) {
    if(index >= 8)
        return;
    if(tex.texture())
        m_textures[index] = tex;
    else
        m_textures[index] = m_fallback;
}
void DrawableState::SetFallbackTexture(Texture fb) {
    m_fallback = fb;
    for(int i = 0; i < 8; ++i)
    {
        if(!m_textures[i].texture())
            m_textures[i] = m_fallback;
    }
}
Shader DrawableState::shader() const { return m_shader; }
Texture DrawableState::texture(std::size_t ix) const { if(ix >= 8) return m_fallback; return m_textures[ix]; }
Texture DrawableState::FallbackTexture() const { return m_fallback; }

// StatelessDrawable
StatelessDrawable::StatelessDrawable()
    : m_gl(nullptr), m_gw(nullptr) {}
StatelessDrawable::StatelessDrawable(GL_t* gl, GameWindow* gw)
    : StatelessDrawable() { Initialize(gl,gw); }
StatelessDrawable::~StatelessDrawable() {}
bool StatelessDrawable::Initialize(GL_t* gl, GameWindow* gw)
{
    m_gl = gl;
    m_gw = gw;
    return this->OnInitialize();
}
GL_t* StatelessDrawable::opengl() const { return m_gl; }
GameWindow* StatelessDrawable::window() const { return m_gw; }
bool StatelessDrawable::OnInitialize() { return true; }
bool StatelessDrawable::OnPreDraw() {
    Shader shd = this->SelectShader();
    if(!shd.shader() || !shd.shader()->bind())
        return false;
    for(int i = 0; i < 8; ++i)
    {
        Texture tex = this->SelectTexture(i);
        if(tex.texture())
            tex.texture()->bind(i);
    }
    return true;
}
void StatelessDrawable::OnDraw() { m_object.draw(); }
void StatelessDrawable::OnPostDraw() {}
void StatelessDrawable::SetObject(Object obj) { m_object = obj; }
void StatelessDrawable::Draw() {
    if(this->OnPreDraw())
    {
        this->OnDraw();
        this->OnPostDraw();
    }
}
Shader StatelessDrawable::SelectShader() { return Shader(); }
Texture StatelessDrawable::SelectTexture(std::size_t) { return Texture(); }

// Drawable
Drawable::Drawable(GL_t* gl, GameWindow* gw)
    : StatelessDrawable(gl,gw) {}
Drawable::~Drawable() {}
void Drawable::SetShader(Shader shd) { m_drawState.SetShader(shd); }
void Drawable::SetTexture(Texture tex, std::size_t index) { m_drawState.SetTexture(tex,index); }
bool Drawable::Initialize(GL_t* gl, GameWindow* gw) { return StatelessDrawable::Initialize(gl,gw); }
Shader Drawable::SelectShader() { return m_drawState.shader(); }
Texture Drawable::SelectTexture(std::size_t ix) { return m_drawState.texture(ix); }

// SquareShape
SquareShape::SquareShape(DrawableState& state) : m_state(state) {}
SquareShape::~SquareShape() {}
bool SquareShape::Initialize(GL_t* gl, GameWindow* gw) { return StatelessDrawable::Initialize(gl,gw); }
bool SquareShape::OnInitialize()
{
    if(!m_shader.loadfromfile(":/shaders/quad/vert.vsh",":/shaders/quad/frag.fsh",Lua::nil))
        return false;
    
    Object obj;
    if(!obj.create(6))
        return false;
    Lua::Array<float> data;
    data.m_data.assign({0.f,0.f,
                        0.f,2.f,
                        2.f,0.f,
                        2.f,0.f,
                        0.f,2.f,
                        2.f,2.f});
    obj.set2d(0, data);
    if(!obj.lock())
        return false;
    this->SetObject(std::move(obj));
    return true;
}
Shader SquareShape::SelectShader() {
    return m_shader;
}
Texture SquareShape::SelectTexture(std::size_t ix) {
    return m_state.texture(ix);
}

void SquareShape::DrawRaw(float x, float y, float w, float h)
{
    if(m_shader.shader())
    {
        m_shader.shader()->setUniformValue("g_offset", x * 2.f, y * 2.f);
        m_shader.shader()->setUniformValue("g_size", w, h);
    }
    this->Draw();
}
void SquareShape::DrawRawCentered(float x, float y, float w, float h)
{
    if(m_shader.shader())
    {
        m_shader.shader()->setUniformValue("g_offset", x * 2.f - (w*0.5f), y * 2.f - (h*0.5f));
        m_shader.shader()->setUniformValue("g_size", w, h);
    }
    this->Draw();
}

void SquareShape::DrawCorrected(int32_t x, int32_t y, int32_t w, int32_t h)
{
    if(m_shader.shader())
    {
        QSize winSize = this->window()->size();
        float invSizeX = 1.f / static_cast<float>(winSize.width());
        float invSizeY = 1.f / static_cast<float>(winSize.height());
        
        m_shader.shader()->setUniformValue("g_offset", static_cast<float>(x * 2) * invSizeX,
                                                        static_cast<float>(y * 2) * invSizeY);
        m_shader.shader()->setUniformValue("g_size", static_cast<float>(w) * invSizeX,
                                                        static_cast<float>(h) * invSizeY);
    }
    this->Draw();
}



}
