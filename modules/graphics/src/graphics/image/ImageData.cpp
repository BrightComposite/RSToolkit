//---------------------------------------------------------------------------

#include <graphics/image/ImageData.h>

#include <core/Exception.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using converter_t = void(*)(ImageData *);

	template<ImageFormat to>
	static converter_t getConverter0(ImageFormat from)
	{
		switch(from)
		{
#define op(format, _)								\
		case ImageFormat::format:					\
			return ImageFormatConverter<ImageFormat::format, to>::convert;
/*	op	*/

			enum_image_formats(op)
#undef op

		default:
			return nullptr;
		}
	}

	static converter_t getConverter(ImageFormat from, ImageFormat to)
	{
		switch(to)
		{
#define op(format, _)								\
		case ImageFormat::format:					\
			return getConverter0<ImageFormat::format>(from);
/*	op	*/

			enum_image_formats(op)
#undef op

		default:
			return nullptr;
		}
	}

	void ImageData::convert(ImageFormat newFormat)
	{
		if(format == newFormat)
			return;

		auto cvt = getConverter(format, newFormat);

		if(cvt == nullptr)
			throw Exception("Can't transform image data from \"", format, "\" to \"", newFormat, "\" format!");

		format = newFormat;
		cvt(this);
	}

	void ImageData::convert(ImageData * out, ImageFormat newFormat) const
	{
		*out = *this;
		out->convert(newFormat);
	}

#define init_buffer(src_bpp, dst_bpp, src_ptr, dst_ptr)	\
	const byte bpp	= __min(src_bpp, dst_bpp);			\
	const byte sbpp = src_bpp;							\
	const byte dbpp = dst_bpp;							\
	size_t bsize = img->width * img->height * dbpp;		\
	byte * buffer = Memory<byte>::allocate(bsize);		\
	byte * src_ptr = img->data;							\
	byte * dst_ptr = buffer;

#define iterate_buffer(src_ptr, dst_ptr)		\
	for(uint y = 0; y < img->height; ++y)		\
		for(uint x = 0; x < img->width; ++x,	\
			src_ptr += sbpp, dst_ptr += dbpp)

#define copy(dst_ptr, src_ptr)		\
	memcpy(dst_ptr, src_ptr, bpp);

#define move_buffer()				\
	img->set(move(buffer), bsize);

#define init_img_ptr(img_bpp, img_ptr)	\
	const byte bpp = img_bpp;			\
	byte * img_ptr = img->data;

#define iterate_image(img_ptr)					\
	for(uint y = 0; y < img->height; ++y)		\
		for(uint x = 0; x < img->width; ++x,	\
			img_ptr += bpp)

	//-----------------------------------------------------------------------

	template<>
	void ImageFormatConverter<ImageFormat::rgb, ImageFormat::rgba>::convert(ImageData * img)
	{
		init_buffer(3, 4, src, dst);

		iterate_buffer(src, dst)
		{
			copy(dst, src);
			dst[3] = 255;
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::rgb, ImageFormat::bgr>::convert(ImageData * img)
	{
		init_img_ptr(preferredBpp(img->format), buffer);

		iterate_image(buffer)
		{
			swap(buffer[0], buffer[2]);
		}
	}

	template<>
	void ImageFormatConverter<ImageFormat::rgb, ImageFormat::bgra>::convert(ImageData * img)
	{
		init_buffer(3, 4, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = src[2];
			dst[1] = src[1];
			dst[2] = src[0];
			dst[3] = 255;
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::rgb, ImageFormat::argb>::convert(ImageData * img)
	{
		init_buffer(3, 4, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = 255;
			copy(dst + 1, src);
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::rgb, ImageFormat::grayscale>::convert(ImageData * img)
	{
		init_buffer(preferredBpp(img->format), 1, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = (src[0] + src[1] + src[2]) / 3;
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::rgb, ImageFormat::grayscale_alpha>::convert(ImageData * img)
	{
		init_buffer(3, 2, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = (src[0] + src[1] + src[2]) / 3;
			dst[1] = 255;
		}

		move_buffer();
	}

	//-----------------------------------------------------------------------

	template<>
	void ImageFormatConverter<ImageFormat::rgba, ImageFormat::rgb>::convert(ImageData * img)
	{
		init_buffer(4, 3, src, dst);

		iterate_buffer(src, dst)
		{
			copy(dst, src);
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::rgba, ImageFormat::bgr>::convert(ImageData * img)
	{
		init_buffer(4, 3, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = src[2];
			dst[1] = src[1];
			dst[2] = src[0];
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::rgba, ImageFormat::bgra>::convert(ImageData * img)
	{
		init_buffer(4, 4, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = src[2];
			dst[1] = src[1];
			dst[2] = src[0];
			dst[3] = src[3];
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::rgba, ImageFormat::argb>::convert(ImageData * img)
	{
		init_buffer(4, 4, src, dst);

		iterate_buffer(src, dst)
		{
			memcpy(dst + 1, src, 3);
			dst[0] = src[3];
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::rgba, ImageFormat::grayscale>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgb, ImageFormat::grayscale>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::rgba, ImageFormat::grayscale_alpha>::convert(ImageData * img)
	{
		init_buffer(4, 2, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = (src[0] + src[1] + src[2]) / 3;
			dst[1] = src[3];
		}

		move_buffer();
	}

	//-----------------------------------------------------------------------

	template<>
	void ImageFormatConverter<ImageFormat::bgr, ImageFormat::rgb>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgb, ImageFormat::bgr>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::bgr, ImageFormat::rgba>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgb, ImageFormat::bgra>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::bgr, ImageFormat::bgra>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgb, ImageFormat::rgba>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::bgr, ImageFormat::argb>::convert(ImageData * img)
	{
		init_buffer(3, 4, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = 255;
			dst[1] = src[2];
			dst[2] = src[1];
			dst[3] = src[0];
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::bgr, ImageFormat::grayscale>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgb, ImageFormat::grayscale>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::bgr, ImageFormat::grayscale_alpha>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgb, ImageFormat::grayscale_alpha>::convert(img);
	}

	//-----------------------------------------------------------------------

	template<>
	void ImageFormatConverter<ImageFormat::bgra, ImageFormat::rgb>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgba, ImageFormat::bgr>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::bgra, ImageFormat::rgba>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgba, ImageFormat::bgra>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::bgra, ImageFormat::bgr>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgba, ImageFormat::rgb>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::bgra, ImageFormat::argb>::convert(ImageData * img)
	{
		init_buffer(4, 4, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = src[3];
			dst[1] = src[2];
			dst[2] = src[1];
			dst[3] = src[0];
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::bgra, ImageFormat::grayscale>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgb, ImageFormat::grayscale>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::bgra, ImageFormat::grayscale_alpha>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::rgba, ImageFormat::grayscale_alpha>::convert(img);
	}

	//-----------------------------------------------------------------------

	template<>
	void ImageFormatConverter<ImageFormat::argb, ImageFormat::rgb>::convert(ImageData * img)
	{
		init_buffer(4, 3, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = src[1];
			dst[1] = src[2];
			dst[2] = src[3];
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::argb, ImageFormat::rgba>::convert(ImageData * img)
	{
		init_buffer(4, 4, src, dst);

		iterate_buffer(src, dst)
		{
			memcpy(dst, src + 1, 3);
			dst[3] = src[0];
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::argb, ImageFormat::bgr>::convert(ImageData * img)
	{
		init_buffer(4, 3, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = src[3];
			dst[1] = src[2];
			dst[2] = src[1];
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::argb, ImageFormat::bgra>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::bgra, ImageFormat::argb>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::argb, ImageFormat::grayscale>::convert(ImageData * img)
	{
		init_buffer(4, 1, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = (src[1] + src[2] + src[3]) / 3;
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::argb, ImageFormat::grayscale_alpha>::convert(ImageData * img)
	{
		init_buffer(4, 2, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = (src[1] + src[2] + src[3]) / 3;
			dst[1] = src[0];
		}

		move_buffer();
	}

	//-----------------------------------------------------------------------

	template<>
	void ImageFormatConverter<ImageFormat::grayscale, ImageFormat::rgb>::convert(ImageData * img)
	{
		init_buffer(preferredBpp(img->format), 3, src, dst);

		iterate_buffer(src, dst)
		{
			memset(dst, src[0], 3);
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::grayscale, ImageFormat::rgba>::convert(ImageData * img)
	{
		init_buffer(1, 4, src, dst);

		iterate_buffer(src, dst)
		{
			memset(dst, src[0], 3);
			dst[3] = 255;
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::grayscale, ImageFormat::bgr>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::grayscale, ImageFormat::rgb>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::grayscale, ImageFormat::bgra>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::grayscale, ImageFormat::rgba>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::grayscale, ImageFormat::argb>::convert(ImageData * img)
	{
		init_buffer(1, 4, src, dst);

		iterate_buffer(src, dst)
		{
			memset(dst + 1, src[0], 3);
			dst[0] = 255;
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::grayscale, ImageFormat::grayscale_alpha>::convert(ImageData * img)
	{
		init_buffer(1, 2, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = src[0];
			dst[1] = 255;
		}

		move_buffer();
	}

	//-----------------------------------------------------------------------

	template<>
	void ImageFormatConverter<ImageFormat::grayscale_alpha, ImageFormat::rgb>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::grayscale, ImageFormat::rgb>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::grayscale_alpha, ImageFormat::rgba>::convert(ImageData * img)
	{
		init_buffer(2, 4, src, dst);

		iterate_buffer(src, dst)
		{
			memset(dst, src[0], 3);
			dst[3] = src[1];
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::grayscale_alpha, ImageFormat::bgr>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::grayscale_alpha, ImageFormat::rgb>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::grayscale_alpha, ImageFormat::bgra>::convert(ImageData * img)
	{
		ImageFormatConverter<ImageFormat::grayscale_alpha, ImageFormat::rgba>::convert(img);
	}

	template<>
	void ImageFormatConverter<ImageFormat::grayscale_alpha, ImageFormat::argb>::convert(ImageData * img)
	{
		init_buffer(2, 4, src, dst);

		iterate_buffer(src, dst)
		{
			memset(dst + 1, src[0], 3);
			dst[0] = src[1];
		}

		move_buffer();
	}

	template<>
	void ImageFormatConverter<ImageFormat::grayscale_alpha, ImageFormat::grayscale>::convert(ImageData * img)
	{
		init_buffer(2, 1, src, dst);

		iterate_buffer(src, dst)
		{
			dst[0] = src[0];
		}

		move_buffer();
	}
}

//---------------------------------------------------------------------------
