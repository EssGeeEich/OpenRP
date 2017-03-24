#include "model.h"
#include <QOpenGLContext>
#include <QOpenGLFunctions>

namespace LuaApi {
	
// ModelImpl::impl...
namespace impl {

// ...SharedBuffer
void SharedBuffer::setTupleSize(std::uint32_t s) { m_tupleSize = s; }

SharedBuffer::SharedBuffer() : m_tupleSize(0) {}
QOpenGLBuffer* SharedBuffer::buffer() const { return m_buffer.get(); }
bool SharedBuffer::create(QOpenGLBuffer::Type type) {
    m_buffer = std::make_shared<QOpenGLBuffer>(type);
    return m_buffer->create();
}
void SharedBuffer::unload() {
    m_buffer.reset();
    m_tupleSize = 0;
}
std::uint32_t SharedBuffer::tupleSize() const { return m_tupleSize; }

// ...ModelData_Base
void ModelData_Base::setVertices(uint32_t v) { m_vertices = v; }
ModelData_Base::ModelData_Base() : m_vertices(0) {}
ModelData_Base::~ModelData_Base() {}
bool ModelData_Base::Create() { return m_vao.create(); }
QOpenGLVertexArrayObject& ModelData_Base::VAO() { return m_vao; }
SharedBuffer* ModelData_Base::VBO(std::size_t ix) { if(ix >= 16) return nullptr; return &m_vbo[ix]; }
QOpenGLBuffer* ModelData_Base::IBO() { return nullptr; }
std::uint32_t ModelData_Base::vertices() const { return m_vertices; }

// ...ModelData_NonIndexed
ModelData_NonIndexed::~ModelData_NonIndexed() {}
void ModelData_NonIndexed::Apply() { m_vao.bind(); }

// ...ModelData_Indexed
void ModelData_Indexed::set32bit(bool v) { m_32bit = v; }
void ModelData_Indexed::setIndices(std::uint32_t v) { m_indices = v; }
ModelData_Indexed::ModelData_Indexed() : m_ibo(QOpenGLBuffer::IndexBuffer), m_32bit(false), m_indices(0) {}
ModelData_Indexed::~ModelData_Indexed() {}
bool ModelData_Indexed::Create() {
    return ModelData_Base::Create() &&
            m_ibo.create();
}
void ModelData_Indexed::Apply() { m_vao.bind(); }
QOpenGLBuffer* ModelData_Indexed::IBO() { return &m_ibo; }
bool ModelData_Indexed::is32bit() const { return m_32bit; }
std::uint32_t ModelData_Indexed::indices() const { return m_indices; }
}

// ModelImpl
bool ModelImpl::create(std::uint32_t vxCount)
{
    m_data = std::unique_ptr<impl::ModelData_Base>(new impl::ModelData_NonIndexed);
    if(!m_data->Create())
    {
        m_data.reset();
        return false;
    }
    m_data->setVertices(vxCount);
    return true;
}
bool ModelImpl::create_indexed(std::uint32_t vxCount)
{
    m_data = std::unique_ptr<impl::ModelData_Base>(new impl::ModelData_Indexed);
    if(!m_data->Create())
    {
        m_data.reset();
        return false;
    }
    m_data->setVertices(vxCount);
    return true;
}
bool ModelImpl::setindices(const Lua::Array<std::uint16_t>& indices)
{
    if(!m_data || !m_data->IBO())
        return false;
    for(auto it = indices.m_data.begin(); it != indices.m_data.end(); ++it)
    {
        if(*it >= m_data->vertices())
            return false; // Index out of bounds!
    }
    m_data->IBO()->allocate(indices.m_data.data(), indices.m_data.size() * sizeof(indices.m_data[0]));
    static_cast<impl::ModelData_Indexed*>(m_data.get())->set32bit(false);
    static_cast<impl::ModelData_Indexed*>(m_data.get())->setIndices(indices.m_data.size());
    return true;
}
bool ModelImpl::setindices_32(const Lua::Array<std::uint32_t>& indices)
{
    if(!m_data || !m_data->IBO())
        return false;
    for(auto it = indices.m_data.begin(); it != indices.m_data.end(); ++it)
    {
        if(*it >= m_data->vertices())
            return false; // Index out of bounds!
    }
    m_data->IBO()->allocate(indices.m_data.data(), indices.m_data.size() * sizeof(indices.m_data[0]));
    static_cast<impl::ModelData_Indexed*>(m_data.get())->set32bit(true);
    static_cast<impl::ModelData_Indexed*>(m_data.get())->setIndices(indices.m_data.size());
    return true;
}
bool ModelImpl::set1d(std::size_t attrib, const Lua::Array<float>& data)
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
bool ModelImpl::set2d(std::size_t attrib, const Lua::Array<float>& data)
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
bool ModelImpl::set3d(std::size_t attrib, const Lua::Array<float>& data)
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
bool ModelImpl::set4d(std::size_t attrib, const Lua::Array<float>& data)
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
bool ModelImpl::lock()
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
void ModelImpl::bind()
{
    m_data->Apply();
}
void ModelImpl::draw()
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
        impl::ModelData_Indexed* odi = static_cast<impl::ModelData_Indexed*>(m_data.get());
        f->glDrawElements(GL_TRIANGLES, odi->indices(), odi->is32bit() ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, 0);
    }
    else
    {
        f->glDrawArrays(GL_TRIANGLES, 0, m_data->vertices());
    }
}
void ModelImpl::unload() { m_data.reset(); }
impl::ModelData_Base* ModelImpl::data() const { return m_data.get(); }
bool ModelImpl::good() const { return m_data.get() != nullptr; }

}
