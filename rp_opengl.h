#ifndef LUA_OGL_LINK_H
#define LUA_OGL_LINK_H
#include "shared.h"
#include <chrono>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include "state.h"
#include "library.h"

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
    protected:
        static std::uint32_t FlagToUint(QOpenGLTexture::Filter, bool&);
        static QOpenGLTexture::Filter UintToFlag(std::uint32_t, bool&);
    public:
        Texture() =default;
        Texture(Texture const&) =default;
        Texture(Texture&&) =default;
        Texture& operator= (Texture const&) =default;
        Texture& operator= (Texture&&) =default;

        bool load(std::string const&, Lua::Arg<bool> const&);
        void unload();
        Lua::ReturnValues size() const;
        QOpenGLTexture* texture() const;
        bool good() const;
        std::uint32_t magfilter() const;
        std::uint32_t minfilter() const;
        float anisotropy() const;
        void setmagfilter(std::uint32_t);
        void setminfilter(std::uint32_t);
        void setfilter(std::uint32_t);
        void setanisotropy(float);
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
        bool good() const;
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
        
        bool load(std::string const&);
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
        bool good() const;
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
    
    class Timer {
    public:
        typedef std::chrono::high_resolution_clock clock;
    private:
        bool m_running;
        clock::time_point m_start;
        clock::time_point m_time;
    public:
        Timer();
        void update();
        std::uint64_t timei() const;
        float timef() const;
        
        void start();
        void stop();
        void reset();
        bool running() const;
    };
}

template <> struct MetatableDescriptor<LuaApi::Timer> {
    static char const* name() { return "timer_mt"; }
    static char const* luaname() { return "Timer"; }
    static char const* constructor() { return "New"; }
    static void metatable(Lua::member_function_storage<LuaApi::Timer>& mt) {
        mt["Reset"] = Lua::Transform(&LuaApi::Timer::reset);
        mt["Running"] = Lua::Transform(&LuaApi::Timer::running);
        mt["Start"] = Lua::Transform(&LuaApi::Timer::start);
        mt["Stop"] = Lua::Transform(&LuaApi::Timer::stop);
        mt["TimeI"] = Lua::Transform(&LuaApi::Timer::timei);
        mt["TimeF"] = Lua::Transform(&LuaApi::Timer::timef);
        mt["Update"] = Lua::Transform(&LuaApi::Timer::update);
    }
};
template <> struct MetatableDescriptor<LuaApi::Texture> {
    static char const* name() { return "texture_mt"; }
    static char const* luaname() { return "Texture"; }
    static char const* constructor() { return "New"; }
    static void metatable(Lua::member_function_storage<LuaApi::Texture>& mt) {
        mt["Anisotropy"] = Lua::Transform(&LuaApi::Texture::anisotropy);
        mt["Good"] = Lua::Transform(&LuaApi::Texture::good);
        mt["LoadFile"] = Lua::Transform(&LuaApi::Texture::load);
        mt["MagFilter"] = Lua::Transform(&LuaApi::Texture::magfilter);
        mt["MinFilter"] = Lua::Transform(&LuaApi::Texture::minfilter);
        mt["SetAnisotropy"] = Lua::Transform(&LuaApi::Texture::setanisotropy);
        mt["SetFilter"] = Lua::Transform(&LuaApi::Texture::setfilter);
        mt["SetMagFilter"] = Lua::Transform(&LuaApi::Texture::setmagfilter);
        mt["SetMinFilter"] = Lua::Transform(&LuaApi::Texture::setminfilter);
        mt["Size"] = Lua::Transform(&LuaApi::Texture::size);
        mt["Unload"] = Lua::Transform(&LuaApi::Texture::unload);
    }
};
template <> struct MetatableDescriptor<LuaApi::Shader> {
    static char const* name() { return "shader_mt"; }
    static char const* luaname() { return "Shader"; }
    static char const* constructor() { return "New"; }
    static void metatable(Lua::member_function_storage<LuaApi::Shader>& mt) {
        mt["Load"] = Lua::Transform(&LuaApi::Shader::load);
        mt["LoadFile"] = Lua::Transform(&LuaApi::Shader::loadfromfile);
        mt["Good"] = Lua::Transform(&LuaApi::Shader::good);
        mt["Unload"] = Lua::Transform(&LuaApi::Shader::unload);
    }
};

template <> struct MetatableDescriptor<LuaApi::Object> {
    static char const* name() { return "object_mt"; }
    static char const* luaname() { return "Object"; }
    static char const* constructor() { return "New"; }
    static void metatable(Lua::member_function_storage<LuaApi::Object>& mt) {
        mt["Bind"] = Lua::Transform(&LuaApi::Object::bind);
        mt["Create"] = Lua::Transform(&LuaApi::Object::create);
        mt["CreateIndexed"] = Lua::Transform(&LuaApi::Object::create_indexed);
        mt["Draw"] = Lua::Transform(&LuaApi::Object::draw);
        mt["Good"] = Lua::Transform(&LuaApi::Object::good);
        mt["LoadFile"] = Lua::Transform(&LuaApi::Object::load);
        mt["Lock"] = mt["Link"] = Lua::Transform(&LuaApi::Object::lock);
        mt["Set1D"] = Lua::Transform(&LuaApi::Object::set1d);
        mt["Set2D"] = Lua::Transform(&LuaApi::Object::set2d);
        mt["Set3D"] = Lua::Transform(&LuaApi::Object::set3d);
        mt["Set4D"] = Lua::Transform(&LuaApi::Object::set4d);
        mt["SetIndices"] = Lua::Transform(&LuaApi::Object::setindices);
        mt["SetIndices32"] = Lua::Transform(&LuaApi::Object::setindices_32);
        mt["Unload"] = Lua::Transform(&LuaApi::Object::unload);
    }
};

#endif // LUA_OGL_LINK_H
