#ifndef LUAGL_TEXTURE_H
#define LUAGL_TEXTURE_H
#include "shared.h"

namespace LuaApi {

	class TextureImpl {
        
		class TexData {
			QImage m_image;
			QOpenGLTexture m_texture;
		public:
			TexData(QString const&, bool);
			QOpenGLTexture& texture();
			bool good() const;
			std::size_t width() const;
			std::size_t height() const;
		};
        
		std::shared_ptr<TexData> m_data;
	protected:
		static std::uint32_t FilterToUint(QOpenGLTexture::Filter, bool&);
		static QOpenGLTexture::Filter UintToFilter(std::uint32_t, bool&);
		static std::uint32_t WrapToUint(QOpenGLTexture::WrapMode, bool&);
		static QOpenGLTexture::WrapMode UintToWrap(std::uint32_t, bool&);
	public:
		
		TextureImpl() =default;
		TextureImpl(TextureImpl const&) =default;
		TextureImpl(TextureImpl&&) =default;
		TextureImpl& operator= (TextureImpl const&) =default;
		TextureImpl& operator= (TextureImpl&&) =default;

		bool load(std::string const&, Lua::Arg<bool> const&);
		void unload();
		Lua::ReturnValues size() const;
		QOpenGLTexture* texture() const;
		bool good() const;
		std::uint32_t magfilter() const;
		std::uint32_t minfilter() const;
		std::uint32_t wraps() const;
		std::uint32_t wrapt() const;
		std::uint32_t wrapr() const;
		float anisotropy() const;
        void setfilter(std::uint32_t);
		void setmagfilter(std::uint32_t);
		void setminfilter(std::uint32_t);
		void setanisotropy(float);
		void setwraps(std::uint32_t);
		void setwrapt(std::uint32_t);
		void setwrapr(std::uint32_t);
	};

    typedef RefCounted<TextureImpl> Texture;
}

template <> struct MetatableDescriptor<LuaApi::TextureImpl> {
    static char const* name() { return "texture_mt"; }
    static char const* luaname() { return "Texture"; }
    static char const* constructor() { return "New"; }
    static bool construct(LuaApi::TextureImpl* v) { return Lua::DefaultConstructor(v); }
    static void metatable(Lua::member_function_storage<LuaApi::TextureImpl>& mt) {
        mt["Anisotropy"] = Lua::Transform(&LuaApi::TextureImpl::anisotropy);
        mt["IsValid"] = Lua::Transform(&LuaApi::TextureImpl::good);
        mt["LoadFile"] = Lua::Transform(&LuaApi::TextureImpl::load);
        mt["MagFilter"] = Lua::Transform(&LuaApi::TextureImpl::magfilter);
        mt["MinFilter"] = Lua::Transform(&LuaApi::TextureImpl::minfilter);
        mt["SetAnisotropy"] = Lua::Transform(&LuaApi::TextureImpl::setanisotropy);
        mt["SetFilter"] = Lua::Transform(&LuaApi::TextureImpl::setfilter);
        mt["SetMagFilter"] = Lua::Transform(&LuaApi::TextureImpl::setmagfilter);
        mt["SetMinFilter"] = Lua::Transform(&LuaApi::TextureImpl::setminfilter);
        mt["Size"] = Lua::Transform(&LuaApi::TextureImpl::size);
        mt["Unload"] = Lua::Transform(&LuaApi::TextureImpl::unload);
    }
};
#endif
