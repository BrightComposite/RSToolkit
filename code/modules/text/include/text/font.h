//---------------------------------------------------------------------------

#pragma once

#ifndef FONT_H
#define FONT_H

//---------------------------------------------------------------------------

#include <container/data.h>
#include <math/rect.h>
#include <graphics/graphics.h>
#include <io/filesystem.h>

//---------------------------------------------------------------------------

namespace asd
{
	using font_handle = int;
	
	enum class resize_mode
	{
		no_resize,
		stretch,
		fit,
		crop
	};
	
	struct text_area
	{
		math::int_rect area;
		resize_mode mode;
	};
	
	class font_interface
	{
	public:
		virtual font_handle create(const byte_data & data, const std::string & type) const = 0;
		virtual void destroy(font_handle font) const = 0;
		
		virtual math::int_size text_size(font_handle font, const std::string & text) const = 0;
		virtual void draw(font_handle font, const std::string & text, const text_area & area) const = 0;
	};
	
	class font
	{
	public:
		font(gfx::context & context, const byte_data & data, const std::string & type) : _interface(font::interface(context)) {
			_handle = _interface.create(data, type);
		}
		
		font(gfx::context & context, const boost::filesystem::path & path) : _interface(font::interface(context)) {
			_handle = _interface.create(fs::read_contents(path), path.extension().string());
		}
		
		~font() {
			if (_handle != 0) {
				_interface.destroy(_handle);
			}
		}
		
		void draw(const std::string & text, const text_area & area) const {
			_interface.draw(_handle, text, area);
		}
		
		void draw(const std::string & text, const math::int_rect & area, resize_mode mode = resize_mode::fit) const {
			_interface.draw(_handle, text, {area, mode});
		}
		
		void draw(const std::string & text, const math::int_point & pos) const {
			_interface.draw(_handle, text, {math::int_rect{pos, pos}, resize_mode::no_resize});
		}
		
		void draw(const std::string & text, int x, int y) const {
			_interface.draw(_handle, text, {math::int_rect{x, y, x, y}, resize_mode::no_resize});
		}
		
		math::int_size text_size(const std::string & text) const {
			return _interface.text_size(_handle, text);
		}
		
		/**
		 * @brief
		 * 	Obtains font interface from context
		 * @param context
		 * @return instance of font interface
		 *
		 * @throws interface_not_found_exception
		 */
		static font_interface & interface(gfx::context & context) {
			return asd::get<font_interface>(context);
		}
	
	protected:
		font_interface & _interface;
		font_handle _handle = 0;
	};
}

//---------------------------------------------------------------------------
#endif
