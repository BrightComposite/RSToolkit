//---------------------------------------------------------------------------

#pragma once

#ifndef IMAGE_DATA_H
#define IMAGE_DATA_H

//---------------------------------------------------------------------------

#include <container/Data.h>
#include <core/String.h>

#include <math/Rect.h>

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

	enum class ImageFormat
	{
		enum_image_formats(op)
		unknown
	};

#undef op

	template<ImageFormat fmt>
	struct PreferredBpp
	{
		static const byte value = 0;
	};

#define op(format, bpp) \
	template<>	\
	struct PreferredBpp<ImageFormat::format>	\
	{	\
		static const byte value = bpp;	\
	};

	enum_image_formats(op)

#undef op

	template<byte bpp>
	struct PreferredFormat
	{
		static const ImageFormat value = ImageFormat::unknown;
	};

#define op(format, bpp) \
	template<>	\
	struct PreferredFormat<bpp>	\
	{	\
		static const ImageFormat value = ImageFormat::format; \
	};

	enum_image_bpps(op)

#undef op

	template<ImageFormat format>
	constexpr inline byte preferredBpp()
	{
		return PreferredBpp<format>::value;
	}

	inline byte preferredBpp(const ImageFormat format)
	{
		switch(format)
		{
#define op(format, bpp) \
	case ImageFormat::format: return bpp;
			enum_image_formats(op)
#undef op
		default: return 0;
		}
	}

	template<byte bpp>
	constexpr inline ImageFormat preferredFormat()
	{
		return PreferredFormat<bpp>::value;
	}

	inline ImageFormat preferredFormat(byte bpp)
	{
		switch(bpp)
		{
#define op(format, bpp) \
		case bpp:		\
			return ImageFormat::format;

		enum_image_bpps(op)

		default:
			return ImageFormat::unknown;
#undef op
		}
	}

	inline ImageFormat alternativeFormat(byte bpp)
	{
		switch(bpp)
		{
#define op(format, bpp) \
		case bpp:		\
			return ImageFormat::format;

		enum_alt_image_bpps(op)

		default:
			return ImageFormat::unknown;
#undef op
		}
	}

	inline void print(String & s, ImageFormat format)
	{
		switch(format)
		{
#define op(format, bpp)				\
		case ImageFormat::format:	\
			s << #format;			\
			return;

		enum_image_formats(op)

		default:
			s << "unknown";
			return;
#undef op
		}
	}

	class ImageData : public OwnedByteData
	{
		typedef OwnedByteData Base;
		
	public:
		ImageData() {}
		ImageData(const ImageData & img) : Base(img), area(img.area), format(img.format) {}
		ImageData(const UintSize & area, ImageFormat format) : area(area), format(format) { alloc(); }

		virtual ~ImageData() {}

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
			return preferredBpp(format);
		}

		uint pitch() const
		{
			return preferredBpp(format) * area.x;
		}

		ImageData & operator = (const ImageData & img)
		{
			Base::operator = (img);
			area = img.area;
			format = img.format;

			return *this;
		}

		ImageData & operator = (ImageData && img)
		{
			Base::operator = (forward<Base>(img));
			area = img.area;
			format = img.format;

			return *this;
		}

		using Base::alloc;

		void alloc()
		{
			Base::alloc(area.y * pitch());
		}

		void copy(const UintSize & area, ImageData & dest) const
		{
			dest.format = format;
			dest.area = area;
			dest.alloc();

			if(this->area.x == dest.area.x)
			{
				auto s = std::min(dest.size, size);
				Memory<byte>::move(dest.ptr, ptr, s);

				if(size < dest.size)
					Memory<byte>::fill(dest.ptr + s, 0, dest.size - s);

				return;
			}

			Memory<byte>::fill(dest.ptr, 0, dest.size);

			uint spitch = pitch();
			uint dpitch = dest.pitch();
			uint pitch = std::min(spitch, dpitch);

			auto * sptr = begin();
			auto * dptr = dest.begin();

			for(; sptr < end() && dptr < dest.end(); sptr += spitch, dptr += dpitch)
				Memory<byte>::move(dptr, sptr, pitch);
		}

		api(graphics) void convert(ImageFormat newFormat);
		api(graphics) void convert(ImageData * out, ImageFormat newFormat) const;

		UintSize area = {0, 0};
		ImageFormat format = ImageFormat::unknown;
	};

	template<ImageFormat From, ImageFormat To>
	struct ImageFormatConverter
	{
		static void convert(ImageData * img);
	};

	template<ImageFormat Format>
	struct ImageFormatConverter<Format, Format>
	{
		static inline void convert(ImageData * img) {}
	};
}

//---------------------------------------------------------------------------
#endif