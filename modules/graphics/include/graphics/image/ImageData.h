//---------------------------------------------------------------------------

#ifndef IMAGE_DATA_H
#define IMAGE_DATA_H

//---------------------------------------------------------------------------

#include <core/container/Data.h>
#include <core/String.h>

#include <io/FileSystem.h>

//---------------------------------------------------------------------------

namespace Rapture
{
#define enum_image_formats(op)	\
	op(rgb,  3)					\
	op(rgba, 4)					\
	op(bgr,  3)					\
	op(bgra, 4)					\
	op(argb, 4)					\
	op(grayscale, 1)			\
	op(grayscale_alpha, 2)		\

#define enum_image_bpps(op)		\
	op(grayscale, 1)			\
	op(grayscale_alpha, 2)		\
	op(rgb,  3)					\
	op(rgba, 4)					\

#define enum_alt_image_bpps(op)	\
	op(grayscale, 1)			\
	op(grayscale_alpha, 2)		\
	op(bgr,  3)					\
	op(bgra, 4)					\

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
		ImageData() : Base() {}
		ImageData(const ImageData & img) : Base(img), width(img.width), height(img.height), format(img.format) {}
		virtual ~ImageData() {}

		ImageData & operator = (const ImageData & img)
		{
			Base::operator = (img);
			width = img.width;
			height = img.height;
			format = img.format;

			return *this;
		}

		ImageData & operator = (ImageData && img)
		{
			Base::operator = (forward<Base>(img));
			width = img.width;
			height = img.height;
			format = img.format;

			return *this;
		}

		void alloc()
		{
			Base::alloc(width * height * preferredBpp(format));
		}

		void api(graphics) convert(ImageFormat newFormat);
		void api(graphics) convert(ImageData * out, ImageFormat newFormat) const;

		uint width = 0, height = 0;
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
