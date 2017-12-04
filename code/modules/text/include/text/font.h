//---------------------------------------------------------------------------

#pragma once

#ifndef FONT_H
#define FONT_H

//---------------------------------------------------------------------------

#include <container/data.h>
#include <math/rect.h>
#include <graphics/graphics.h>
#include <io/filesystem.h>
#include <boost/variant.hpp>

//---------------------------------------------------------------------------

namespace asd
{
	using font_handle = int;
	
	struct pixel_unit { int value; };
	struct point_unit { int value; };
	
	enum class resize_mode
	{
		no_resize,
		stretch,
		fit,
		crop
	};
	
	struct no_constraint {};
	struct width_constraint { int value; };
	struct height_constraint { int value; };
	struct ratio_constraint { float value; };
	
	using size_constraint = boost::variant<
		no_constraint,
		width_constraint,
		height_constraint,
		ratio_constraint
	>;
	
	struct text_area
	{
		math::int_rect area;
		resize_mode mode;
	};
	
	class font_interface : public gfx::interface
	{
		morph_type(font_interface)
	
	public:
		virtual point_unit to_points(pixel_unit size) const = 0;
		virtual font_handle create(const byte_data & data, const std::string & type, point_unit size) const = 0;
		virtual void destroy(font_handle font) const = 0;
		
		virtual math::int_size text_size(font_handle font, const std::string & text, const size_constraint & constraint) const = 0;
		virtual void draw(font_handle font, const std::string & text, const text_area & area) const = 0;
	};
	
	class font
	{
	public:
		font(gfx::context & context, const byte_data & data, const std::string & type, point_unit size) :
			_interface(font::interface(context)),
			_handle(_interface.create(data, type, size)) {}
		
		font(gfx::context & context, const byte_data & data, const std::string & type, pixel_unit size) :
			_interface(font::interface(context)),
			_handle(_interface.create(data, type, _interface.to_points(size))) {}
		
		template <class Unit>
		font(gfx::context & context, const boost::filesystem::path & path, Unit size) :
			font(context, fs::read_contents(path), path.extension().string(), size) {}
		
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
		
		math::int_size text_size(const std::string & text, const size_constraint & constraint = no_constraint()) const {
			return _interface.text_size(_handle, text, constraint);
		}
		
		/**
		 * @brief
		 * 	Obtains font interface from context
		 *
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
