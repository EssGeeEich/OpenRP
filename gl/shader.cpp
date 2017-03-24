#include "shader.h"

namespace LuaApi {
// ShaderImpl
bool ShaderImpl::load(std::string const& shd1, std::string const& shd2, Lua::Arg<std::string> const& geom)
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
bool ShaderImpl::loadfromfile(std::string const& shd1, std::string const& shd2, Lua::Arg<std::string> const& geom)
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
void ShaderImpl::unload()
{
    m_program.reset();
}
QOpenGLShaderProgram* ShaderImpl::shader() const { return m_program.get(); }
bool ShaderImpl::good() const { return m_program.get() != nullptr; }
}
