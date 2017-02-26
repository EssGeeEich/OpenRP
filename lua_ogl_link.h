#ifndef LUA_OGL_LINK_H
#define LUA_OGL_LINK_H
#include <exception>
#include <memory>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include "state.h"

typedef QOpenGLFunctions_4_4_Core GL_t;
class GameWindow;

namespace LuaApi {
    typedef Lua::lua_exception gl_error;

    class Texture {
        class TexData {
            QImage m_image;
            QOpenGLTexture m_texture;
        public:
            TexData();
            TexData(QString const&, bool);
            QOpenGLTexture& texture();
            bool isValid() const;
            std::size_t width() const;
            std::size_t height() const;
        };
        std::shared_ptr<TexData> m_data;
    public:
        Texture() =default;
        Texture(Texture const&) =default;
        Texture(Texture&&) =default;
        Texture& operator= (Texture const&) =default;
        Texture& operator= (Texture&&) =default;

        bool load(std::string const&, Lua::Arg<bool> const&);
        void unload();
        Lua::ReturnValues<std::size_t, std::size_t> size() const;
        QOpenGLTexture* texture() const;
    };
    
    class Shader {
        std::shared_ptr<QOpenGLShaderProgram> m_program;
    public:
        Shader() =default;
        Shader(Shader const&) =default;
        Shader(Shader&&) =default;
        Shader& operator= (Shader const&) =default;
        Shader& operator= (Shader&&) =default;
        
        bool load(std::string const&, std::string const&, Lua::Arg<std::string> const&);
        bool loadfromfile(std::string const&, std::string const&, Lua::Arg<std::string> const&);
        void unload();
        QOpenGLShaderProgram* shader() const;
    };
    
    class Object;
    namespace impl {
        class SharedBuffer {
            friend class ::LuaApi::Object;
        protected:
            std::shared_ptr<QOpenGLBuffer> m_buffer;
            std::uint32_t m_tupleSize;
            void setTupleSize(std::uint32_t);
        public:
            SharedBuffer();
            SharedBuffer(SharedBuffer const&) =default;
            SharedBuffer(SharedBuffer&&) =default;
            SharedBuffer& operator= (SharedBuffer const&) =default;
            SharedBuffer& operator= (SharedBuffer&&) =default;
            
            QOpenGLBuffer* buffer() const;
            bool create(QOpenGLBuffer::Type = QOpenGLBuffer::VertexBuffer);
            void unload();
            std::uint32_t tupleSize() const;
        };

        class ObjectData_Base {
            friend class ::LuaApi::Object;
        protected:
            QOpenGLVertexArrayObject m_vao;
            SharedBuffer m_vbo[16];
            std::uint32_t m_vertices;
            
            void setVertices(std::uint32_t);
        public:
            ObjectData_Base();
            virtual ~ObjectData_Base();
            virtual bool Create();
            virtual void Apply() =0;
            
            QOpenGLVertexArrayObject& VAO();
            SharedBuffer* VBO(std::size_t);
            virtual QOpenGLBuffer* IBO();
            
            std::uint32_t vertices() const;
        };
        
        class ObjectData_NonIndexed : public ObjectData_Base {
            friend class ::LuaApi::Object;
        public:
            virtual ~ObjectData_NonIndexed();
            virtual void Apply() override;
        };
        
        class ObjectData_Indexed : public ObjectData_Base {
            friend class ::LuaApi::Object;
            QOpenGLBuffer m_ibo;
            bool m_32bit;
            std::uint32_t m_indices;
            
            void set32bit(bool);
            void setIndices(std::uint32_t);
        public:
            ObjectData_Indexed();
            virtual ~ObjectData_Indexed();
            virtual bool Create() override;
            virtual void Apply() override;
            
            virtual QOpenGLBuffer* IBO() override;
            bool is32bit() const;
            std::uint32_t indices() const;
        };
    }
    
    class Object {
        std::shared_ptr<impl::ObjectData_Base> m_data;
    public:
        Object() =default;
        Object(Object const&) =default;
        Object(Object&&) =default;
        Object& operator= (Object const&) =default;
        Object& operator= (Object&&) =default;
        
        bool create(std::uint32_t);
        bool create_indexed(std::uint32_t);
        bool setindices(Lua::Array<std::uint16_t> const&);
        bool setindices_32(Lua::Array<std::uint32_t> const&);
        bool set1d(std::size_t attrib, Lua::Array<float> const&);
        bool set2d(std::size_t attrib, Lua::Array<float> const&);
        bool set3d(std::size_t attrib, Lua::Array<float> const&);
        bool set4d(std::size_t attrib, Lua::Array<float> const&);
        bool lock();
        void bind();
        void draw();
        void unload();
        impl::ObjectData_Base* data() const;
    };
    
    class DrawableState {
    protected:
        Texture m_fallback;
        Shader m_shader;
        Texture m_textures[8];
    public:
        DrawableState() =default;
        DrawableState(DrawableState const&) =default;
        DrawableState(DrawableState&&) =default;
        DrawableState& operator= (DrawableState const&) =default;
        DrawableState& operator= (DrawableState&&) =default;
        
        void SetShader(Shader shd);
        void SetTexture(Texture tex, std::size_t index);
        void SetFallbackTexture(Texture fb);
        Shader shader() const;
        Texture texture(std::size_t) const;
        Texture FallbackTexture() const;
    };

    class StatelessDrawable {
        GL_t* m_gl;
        GameWindow* m_gw;
    protected:
        Object m_object;

        
        StatelessDrawable();
        StatelessDrawable(GL_t*, GameWindow*);
        virtual ~StatelessDrawable();
        bool Initialize(GL_t*, GameWindow*);
        GL_t* opengl() const;
        GameWindow* window() const;

        virtual bool OnInitialize();
        virtual bool OnPreDraw();
        virtual void OnDraw();
        virtual void OnPostDraw();
        virtual Shader SelectShader();
        virtual Texture SelectTexture(std::size_t);
        
        void SetObject(Object);
        void Draw();
    };
    
    class Drawable : public StatelessDrawable {
    protected:
        DrawableState m_drawState;
        
        Drawable() =default;
        Drawable(GL_t*, GameWindow*);
        virtual ~Drawable();
        bool Initialize(GL_t*, GameWindow*);
        
        virtual Shader SelectShader() override;
        virtual Texture SelectTexture(std::size_t) override;
    public:
        void SetShader(Shader);
        void SetTexture(Texture, std::size_t);
    };
    
    class SquareShape : public StatelessDrawable {
        DrawableState& m_state;
        Shader m_shader;
    protected:
        virtual Shader SelectShader() override;
        virtual Texture SelectTexture(std::size_t) override;
    public:
        SquareShape(DrawableState&);
        bool Initialize(GL_t*, GameWindow*);
        
        virtual bool OnInitialize();
        virtual ~SquareShape();
        
        void DrawRaw(float, float, float, float);
        void DrawRawCentered(float, float, float, float);
        void DrawCorrected(std::int32_t, std::int32_t, std::int32_t, std::int32_t);
    };

    class Storage {
        DrawableState m_ds;
        SquareShape m_square;
    public:
        Storage();
        bool InitAll(GL_t*, GameWindow*, Lua::State&);
    };
}

#endif // LUA_OGL_LINK_H
