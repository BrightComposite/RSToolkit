//---------------------------------------------------------------------------

#include <graphics/image/image_data.h>

#include <core/Exception.h>

//---------------------------------------------------------------------------

namespace asd
{
	using converter_t = void (*)(image_data *);
	
	template<image_format to>
	static converter_t getConverter0(image_format from) {
		switch(from) {
#define op(format, _)                                \
        case image_format::format:                    \
            return image_format_converter<image_format::format, to>::convert;
/*	op	*/
			
			enum_image_convertable_formats(op)
#undef op
			
			default:
				return nullptr;
		}
	}
	
	static converter_t getConverter(image_format from, image_format to) {
		switch(to) {
#define op(format, _)                                \
        case image_format::format:                    \
            return getConverter0<image_format::format>(from);
/*	op	*/
			
			enum_image_convertable_formats(op)
#undef op
			
			default:
				return nullptr;
		}
	}
	
	void image_data::convert(image_format newFormat) {
		if(format == newFormat) {
			return;
		}
		
		auto cvt = getConverter(format, newFormat);
		
		String f(format);
		
		if(cvt == nullptr) {
			throw Exception("Can't transform image data from \"", f, "\" to \"", newFormat, "\" format!");
		}
		
		format = newFormat;
		cvt(this);
	}
	
	void image_data::convert(image_data * out, image_format newFormat) const {
		*out = *this;
		out->convert(newFormat);
	}

#define init_buffer(src_bpp, dst_bpp, src_ptr, dst_ptr)        \
    const byte sbpp = src_bpp;                                \
    const byte dbpp = dst_bpp;                                \
    size_t bsize = data->width() * data->height() * dbpp;    \
    byte * buffer = memory<byte>::allocate(bsize);            \
    byte * src_ptr = data->ptr;                                \
    byte * dst_ptr = buffer;

#define iterate_buffer(src_ptr, dst_ptr)        \
    for(uint y = 0; y < data->height(); ++y)    \
    for(uint x = 0; x < data->width(); ++x, src_ptr += sbpp, dst_ptr += dbpp)

#define set_bpp()        \
    const byte bpp = __min(sbpp, dbpp);

#define copy(dst_ptr, src_ptr)        \
    memcpy(dst_ptr, src_ptr, bpp);

#define move_buffer()                \
    data->set(move(buffer), bsize);

#define init_img_ptr(img_bpp, img_ptr)    \
    const byte bpp = img_bpp;            \
    byte * img_ptr = data->ptr;

#define iterate_image(img_ptr)                 \
    for(uint y = 0; y < data->height(); ++y)   \
    for(uint x = 0; x < data->width(); ++x, img_ptr += bpp)
	
	//-----------------------------------------------------------------------
	
	template<>
	void image_format_converter<image_format::rgb, image_format::rgba>::convert(image_data * data) {
		init_buffer(3, 4, src, dst);
		set_bpp();
		
		iterate_buffer(src, dst) {
				copy(dst, src);
				dst[3] = 255;
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::rgb, image_format::bgr>::convert(image_data * data) {
		init_img_ptr(preferred_bpp(data->format), buffer);
		
		iterate_image(buffer) {
				swap(buffer[0], buffer[2]);
			}
	}
	
	template<>
	void image_format_converter<image_format::rgb, image_format::bgra>::convert(image_data * data) {
		init_buffer(3, 4, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
				dst[3] = 255;
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::rgb, image_format::argb>::convert(image_data * data) {
		init_buffer(3, 4, src, dst);
		set_bpp();
		
		iterate_buffer(src, dst) {
				dst[0] = 255;
				copy(dst + 1, src);
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::rgb, image_format::grayscale>::convert(image_data * data) {
		init_buffer(preferred_bpp(data->format), 1, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = static_cast<byte>((src[0] + src[1] + src[2]) / 3);
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::rgb, image_format::grayscale_alpha>::convert(image_data * data) {
		init_buffer(3, 2, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = static_cast<byte>((src[0] + src[1] + src[2]) / 3);
				dst[1] = 255;
			}
		
		move_buffer();
	}
	
	//-----------------------------------------------------------------------
	
	template<>
	void image_format_converter<image_format::rgba, image_format::rgb>::convert(image_data * data) {
		init_buffer(4, 3, src, dst);
		set_bpp();
		
		iterate_buffer(src, dst) {
				copy(dst, src);
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::rgba, image_format::bgr>::convert(image_data * data) {
		init_buffer(4, 3, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::rgba, image_format::bgra>::convert(image_data * data) {
		init_buffer(4, 4, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
				dst[3] = src[3];
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::rgba, image_format::argb>::convert(image_data * data) {
		init_buffer(4, 4, src, dst);
		
		iterate_buffer(src, dst) {
				memcpy(dst + 1, src, 3);
				dst[0] = src[3];
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::rgba, image_format::grayscale>::convert(image_data * data) {
		image_format_converter<image_format::rgb, image_format::grayscale>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::rgba, image_format::grayscale_alpha>::convert(image_data * data) {
		init_buffer(4, 2, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = static_cast<byte>((src[0] + src[1] + src[2]) / 3);
				dst[1] = src[3];
			}
		
		move_buffer();
	}
	
	//-----------------------------------------------------------------------
	
	template<>
	void image_format_converter<image_format::bgr, image_format::rgb>::convert(image_data * data) {
		image_format_converter<image_format::rgb, image_format::bgr>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::bgr, image_format::rgba>::convert(image_data * data) {
		image_format_converter<image_format::rgb, image_format::bgra>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::bgr, image_format::bgra>::convert(image_data * data) {
		image_format_converter<image_format::rgb, image_format::rgba>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::bgr, image_format::argb>::convert(image_data * data) {
		init_buffer(3, 4, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = 255;
				dst[1] = src[2];
				dst[2] = src[1];
				dst[3] = src[0];
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::bgr, image_format::grayscale>::convert(image_data * data) {
		image_format_converter<image_format::rgb, image_format::grayscale>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::bgr, image_format::grayscale_alpha>::convert(image_data * data) {
		image_format_converter<image_format::rgb, image_format::grayscale_alpha>::convert(data);
	}
	
	//-----------------------------------------------------------------------
	
	template<>
	void image_format_converter<image_format::bgra, image_format::rgb>::convert(image_data * data) {
		image_format_converter<image_format::rgba, image_format::bgr>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::bgra, image_format::rgba>::convert(image_data * data) {
		image_format_converter<image_format::rgba, image_format::bgra>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::bgra, image_format::bgr>::convert(image_data * data) {
		image_format_converter<image_format::rgba, image_format::rgb>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::bgra, image_format::argb>::convert(image_data * data) {
		init_buffer(4, 4, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = src[3];
				dst[1] = src[2];
				dst[2] = src[1];
				dst[3] = src[0];
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::bgra, image_format::grayscale>::convert(image_data * data) {
		image_format_converter<image_format::rgb, image_format::grayscale>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::bgra, image_format::grayscale_alpha>::convert(image_data * data) {
		image_format_converter<image_format::rgba, image_format::grayscale_alpha>::convert(data);
	}
	
	//-----------------------------------------------------------------------
	
	template<>
	void image_format_converter<image_format::argb, image_format::rgb>::convert(image_data * data) {
		init_buffer(4, 3, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = src[1];
				dst[1] = src[2];
				dst[2] = src[3];
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::argb, image_format::rgba>::convert(image_data * data) {
		init_buffer(4, 4, src, dst);
		
		iterate_buffer(src, dst) {
				memcpy(dst, src + 1, 3);
				dst[3] = src[0];
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::argb, image_format::bgr>::convert(image_data * data) {
		init_buffer(4, 3, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = src[3];
				dst[1] = src[2];
				dst[2] = src[1];
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::argb, image_format::bgra>::convert(image_data * data) {
		image_format_converter<image_format::bgra, image_format::argb>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::argb, image_format::grayscale>::convert(image_data * data) {
		init_buffer(4, 1, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = static_cast<byte>((src[1] + src[2] + src[3]) / 3);
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::argb, image_format::grayscale_alpha>::convert(image_data * data) {
		init_buffer(4, 2, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = static_cast<byte>((src[1] + src[2] + src[3]) / 3);
				dst[1] = src[0];
			}
		
		move_buffer();
	}
	
	//-----------------------------------------------------------------------
	
	template<>
	void image_format_converter<image_format::grayscale, image_format::rgb>::convert(image_data * data) {
		init_buffer(preferred_bpp(data->format), 3, src, dst);
		
		iterate_buffer(src, dst) {
				memset(dst, src[0], 3);
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::grayscale, image_format::rgba>::convert(image_data * data) {
		init_buffer(1, 4, src, dst);
		
		iterate_buffer(src, dst) {
				memset(dst, src[0], 3);
				dst[3] = 255;
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::grayscale, image_format::bgr>::convert(image_data * data) {
		image_format_converter<image_format::grayscale, image_format::rgb>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::grayscale, image_format::bgra>::convert(image_data * data) {
		image_format_converter<image_format::grayscale, image_format::rgba>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::grayscale, image_format::argb>::convert(image_data * data) {
		init_buffer(1, 4, src, dst);
		
		iterate_buffer(src, dst) {
				memset(dst + 1, src[0], 3);
				dst[0] = 255;
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::grayscale, image_format::grayscale_alpha>::convert(image_data * data) {
		init_buffer(1, 2, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = src[0];
				dst[1] = 255;
			}
		
		move_buffer();
	}
	
	//-----------------------------------------------------------------------
	
	template<>
	void image_format_converter<image_format::grayscale_alpha, image_format::rgb>::convert(image_data * data) {
		image_format_converter<image_format::grayscale, image_format::rgb>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::grayscale_alpha, image_format::rgba>::convert(image_data * data) {
		init_buffer(2, 4, src, dst);
		
		iterate_buffer(src, dst) {
				memset(dst, src[0], 3);
				dst[3] = src[1];
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::grayscale_alpha, image_format::bgr>::convert(image_data * data) {
		image_format_converter<image_format::grayscale_alpha, image_format::rgb>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::grayscale_alpha, image_format::bgra>::convert(image_data * data) {
		image_format_converter<image_format::grayscale_alpha, image_format::rgba>::convert(data);
	}
	
	template<>
	void image_format_converter<image_format::grayscale_alpha, image_format::argb>::convert(image_data * data) {
		init_buffer(2, 4, src, dst);
		
		iterate_buffer(src, dst) {
				memset(dst + 1, src[0], 3);
				dst[0] = src[1];
			}
		
		move_buffer();
	}
	
	template<>
	void image_format_converter<image_format::grayscale_alpha, image_format::grayscale>::convert(image_data * data) {
		init_buffer(2, 1, src, dst);
		
		iterate_buffer(src, dst) {
				dst[0] = src[0];
			}
		
		move_buffer();
	}
}

//---------------------------------------------------------------------------
