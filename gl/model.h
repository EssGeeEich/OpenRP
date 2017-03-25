#ifndef LUAGL_MODEL_H
#define LUAGL_MODEL_H
#include "shared.h"
namespace LuaApi {
	class ModelStorageImpl;
    namespace impl {
        class SharedBuffer {
            friend class ::LuaApi::ModelStorageImpl;
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

        class ModelData_Base {
            friend class ::LuaApi::ModelStorageImpl;
        protected:
            QOpenGLVertexArrayObject m_vao;
            SharedBuffer m_vbo[16];
            std::uint32_t m_vertices;
            
            void setVertices(std::uint32_t);
        public:
            ModelData_Base();
            virtual ~ModelData_Base();
            virtual bool Create();
            virtual void Apply() =0;
            
            QOpenGLVertexArrayObject& VAO();
            SharedBuffer* VBO(std::size_t);
            virtual QOpenGLBuffer* IBO();
            
            std::uint32_t vertices() const;
        };
        
        class ModelData_NonIndexed : public ModelData_Base {
            friend class ::LuaApi::ModelStorageImpl;
        public:
            virtual ~ModelData_NonIndexed();
            virtual void Apply() override;
        };
        
        class ModelData_Indexed : public ModelData_Base {
            friend class ::LuaApi::ModelStorageImpl;
            QOpenGLBuffer m_ibo;
            bool m_32bit;
            std::uint32_t m_indices;
            
            void set32bit(bool);
            void setIndices(std::uint32_t);
        public:
            ModelData_Indexed();
            virtual ~ModelData_Indexed();
            virtual bool Create() override;
            virtual void Apply() override;
            
            virtual QOpenGLBuffer* IBO() override;
            bool is32bit() const;
            std::uint32_t indices() const;
        };
    }
    
    class ModelStorageImpl {
        std::unique_ptr<impl::ModelData_Base> m_data;
    public:
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
        impl::ModelData_Base* data() const;
        bool good() const;
    };
    
    typedef RefCounted<ModelStorageImpl> ModelStorage;
}

template <> struct MetatableDescriptor<LuaApi::ModelStorageImpl> {
    static char const* name() { return "model_mt"; }
    static char const* luaname() { return "ModelStorage"; }
    static char const* constructor() { return "New"; }
    static bool construct(LuaApi::ModelStorageImpl* v) { return Lua::DefaultConstructor(v); }
    static void metatable(Lua::member_function_storage<LuaApi::ModelStorageImpl>& mt) {
        mt["Bind"] = Lua::Transform(&LuaApi::ModelStorageImpl::bind);
        mt["Create"] = Lua::Transform(&LuaApi::ModelStorageImpl::create);
        mt["CreateIndexed"] = Lua::Transform(&LuaApi::ModelStorageImpl::create_indexed);
        mt["Draw"] = Lua::Transform(&LuaApi::ModelStorageImpl::draw);
        mt["IsValid"] = Lua::Transform(&LuaApi::ModelStorageImpl::good);
        mt["Lock"] = mt["Link"] = Lua::Transform(&LuaApi::ModelStorageImpl::lock);
        mt["Set1D"] = Lua::Transform(&LuaApi::ModelStorageImpl::set1d);
        mt["Set2D"] = Lua::Transform(&LuaApi::ModelStorageImpl::set2d);
        mt["Set3D"] = Lua::Transform(&LuaApi::ModelStorageImpl::set3d);
        mt["Set4D"] = Lua::Transform(&LuaApi::ModelStorageImpl::set4d);
        mt["SetIndices"] = Lua::Transform(&LuaApi::ModelStorageImpl::setindices);
        mt["SetIndices32"] = Lua::Transform(&LuaApi::ModelStorageImpl::setindices_32);
        mt["Unload"] = Lua::Transform(&LuaApi::ModelStorageImpl::unload);
    }
};
#endif
