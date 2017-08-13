//---------------------------------------------------------------------------

#pragma once

#ifndef IMAGE_DATA_H
#define IMAGE_DATA_H

//---------------------------------------------------------------------------

#include <container/Data.h>
#include <core/String.h>

#include <math/rect.h>

#include <io/FileSystem.h>

//---------------------------------------------------------------------------

namespace asd
{
#define enum_image_formats(op)	\
	op(rgb,     3)				\
	op(rgba,    4)				\
	op(bgr,     3)				\
	op(bgra,    4)				\
	op(rgb16f,  6)				\
	op(rgba16f, 8)				\
	op(bgr16f,  6)				\
	op(bgra16f, 8)				\
	op(rgb32f,  12)				\
	op(rgba32f, 16)				\
	op(bgr32f,  12)				\
	op(bgra32f, 16)				\
	op(argb,    4)				\
	op(grayscale, 1)			\
	op(grayscale_alpha, 2)		\

#define enum_image_convertable_formats(op)	\
	op(rgb,     3)				\
	op(rgba,    4)				\
	op(bgr,     3)				\
	op(bgra,    4)				\
	op(argb,    4)				\
	op(grayscale, 1)			\
	op(grayscale_alpha, 2)		\

#define enum_image_bpps(op)		\
	op(grayscale, 1)			\
	op(grayscale_alpha, 2)		\
	op(rgb,  3)					\
	op(rgba, 4)					\
	op(rgb16f, 6)				\
	op(rgba16f, 8)				\
	op(rgb32f,  12)				\
	op(rgba32f, 16)				\

#define enum_alt_image_bpps(op)	\
	op(grayscale, 1)			\
	op(grayscale_alpha, 2)		\
	op(bgr,  3)					\
	op(bgra, 4)					\
	op(bgr16f, 6)				\
	op(bgra16f, 8)				\
	op(bgr32f,  12)				\
	op(bgra32f, 16)				\

#define op(format, _) format,

	enum class image_format
	{
		enum_image_formats(op)
		unknown
	};

#undef op

	template<image_format fmt>
	struct _PreferredBpp
	{
		static const byte value = 0;
	};

#define op(format, bpp) \
	template<>	\
	struct _PreferredBpp<image_format::format>	\
	{	\
		static const byte value = bpp;	\
	};

	enum_image_formats(op)

#undef op

	template<byte bpp>
	struct _PreferredFormat
	{
		static const image_format value = image_format::unknown;
	};

#define op(format, bpp) \
	template<>	\
	struct _PreferredFormat<bpp>	\
	{	\
		static const image_format value = image_format::format; \
	};

	enum_image_bpps(op)

#undef op

	template<image_format format>
	constexpr inline byte preferred_bpp()
	{
		return _PreferredBpp<format>::value;
	}

	inline byte preferred_bpp(const image_format format)
	{
		switch(format)
		{
#define op(format, bpp) \
	case image_format::format: return bpp;
			enum_image_formats(op)
#undef op
		default: return 0;
		}
	}

	template<byte bpp>
	constexpr inline image_format preferred_format()
	{
		return _PreferredFormat<bpp>::value;
	}

	inline image_format preferred_format(byte bpp)
	{
		switch(bpp)
		{
#define op(format, bpp) \
		case bpp:		\
			return image_format::format;

		enum_image_bpps(op)

		default:
			return image_format::unknown;
#undef op
		}
	}

	inline image_format alternativeFormat(byte bpp)
	{
		switch(bpp)
		{
#define op(format, bpp) \
		case bpp:		\
			return image_format::format;

		enum_alt_image_bpps(op)

		default:
			return image_format::unknown;
#undef op
		}
	}

	inline void print(String & s, image_format format)
	{
		switch(format)
		{
#define op(format, bpp)				\
		case image_format::format:	\
			s << #format;			\
			return;

		enum_image_formats(op)

		default:
			s << "unknown";
			return;
#undef op
		}
	}

	class image_data : public OwnedByteData
	{
		typedef OwnedByteData base;
		
	public:
		image_data() {}
		image_data(const image_data & img) : base(img), area(img.area), format(img.format) {}
		image_data(const math::uint_size & area, image_format format) : area(area), format(format) { alloc(); }

		virtual ~image_data() {}

		uint width() const
		{
			return area.x;
		}

		uint height() const
		{
			return area.y;
		}

		byte bpp() const
		{
			return preferred_bpp(format);
		}

		uint pitch() const
		{
			return preferred_bpp(format) * area.x;
		}

		image_data & operator = (const image_data & img)
		{
			base::operator = (img);
			area = img.area;
			format = img.format;

			return *this;
		}

		image_data & operator = (image_data && img)
		{
			base::operator = (forward<base>(img));
			area = img.area;
			format = img.format;

			return *this;
		}

		using base::alloc;

		void alloc()
		{
			base::alloc(area.y * pitch());
		}

		void copy(const math::uint_size & area, image_data & dest) const
		{
			dest.format = format;
			dest.area = area;
			dest.alloc();

			if(this->area.x == dest.area.x)
			{
				auto s = std::min(dest.size, size);
				memory<byte>::move(dest.ptr, ptr, s);

				if(size < dest.size)
					memory<byte>::fill(dest.ptr + s, 0, dest.size - s);

				return;
			}

			memory<byte>::fill(dest.ptr, 0, dest.size);

			uint spitch = pitch();
			uint dpitch = dest.pitch();
			uint pitch = std::min(spitch, dpitch);

			auto * sptr = begin();
			auto * dptr = dest.begin();

			for(; sptr < end() && dptr < dest.end(); sptr += spitch, dptr += dpitch)
				memory<byte>::move(dptr, sptr, pitch);
		}

		api(graphics) void convert(image_format newFormat);
		api(graphics) void convert(image_data * out, image_format newFormat) const;
		
		math::uint_size area = {0, 0};
		image_format format = image_format::unknown;
	};

	template<image_format From, image_format To>
	struct image_format_converter
	{
		static void convert(image_data * img);
	};

	template<image_format Format>
	struct image_format_converter<Format, Format>
	{
		static inline void convert(image_data * img) {}
	};
}

//---------------------------------------------------------------------------
#endif
