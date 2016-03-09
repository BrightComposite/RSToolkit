//---------------------------------------------------------------------------

#include <graphics/image/io/ImageIO.h>

#include <iostream>
#include <stack>

//#include <zlib.h>
//#include <png.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	map<string, ImageDecoder *> ImageIO::decoders;
	map<string, ImageEncoder *> ImageIO::encoders;
	
	using std::ifstream;
	using std::ofstream;
	using std::ios_base;

	using std::stack;

	void ImageIO::load(ImageData * output, const path & filepath)
	{
		using namespace boost::filesystem;

		OwnedByteData raw;
		ifstream input(filepath.string(), ios_base::binary);

		if(!exists(filepath))
			throw Exception("File ", filepath.string(), " doesn't exist!");

		raw.alloc(static_cast<size_t>(file_size(filepath)));
		input.read(reinterpret_cast<char *>(raw.data), raw.size);

		read(output, get_extension(filepath), &raw);
	}

	void ImageIO::save(const path & filepath, const ImageData * image)
	{
		OwnedByteData raw;
		write(&raw, get_extension(filepath), image);

		FILE * stream;

		if(fopen_s(&stream, filepath.string().c_str(), "w+b") != 0)
			throw ErrnoException();

		fwrite(raw.data, 1, raw.size, stream);
		fclose(stream);
	}
	/*
	class ByteInput : public RawData<const byte>
	{
	public:
		using RawData<const byte>::RawData;

		size_t read(byte * out, size_t count)
		{
			size_t delta = std::min(size - readed, count);

			memcpy(out, data + readed, delta);
			readed += delta;

			return delta;
		}

	protected:
		size_t readed = 0;
	};

	class ByteOutput : public OwnedData<byte>
	{
	public:
		ByteOutput() : OwnedData<byte>() {}

		void write(const byte * in, size_t count)
		{
			if(count > size - written)
			{
				size = align(size + count, delta);
				data = Memory<byte>::reallocate(data, size);
			}

			memcpy(data + written, in, count);
			written += count;
		}

	protected:
		size_t written = 0;
		static const size_t delta = 64;
	};

	static void pngRead(png_structp png, png_bytep out, png_size_t count)
	{
		png_voidp io_ptr = png_get_io_ptr(png);

		if(io_ptr == nullptr)
			return;

		ByteInput * input = static_cast<ByteInput *>(io_ptr);

		if(input->read(out, count) != count)
		{
			png_error(png, "Unexpected end of PNG image!");
		}
	}

	static void pngWrite(png_structp png, png_bytep in, png_size_t count)
	{
		png_voidp io_ptr = png_get_io_ptr(png);

		if(io_ptr == nullptr)
			return;

		ByteOutput * output = static_cast<ByteOutput *>(io_ptr);
		output->write(in, count);
	}

	void user_error_fn(png_structp png_ptr, png_const_charp error_msg)
	{
		try {throw Exception(error_msg);} catch(const Exception &) {}
	}

	void PngImageConverter::decode(ImageData * output, const string & type, const ByteData * raw)
	{
		ByteInput input(*raw);

		if(png_sig_cmp(raw->data, 0, 8) != 0)
			throw ImageConversionException("Can't find PNG signature!");

		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if(png == nullptr)
			throw Exception("Can't create PNG structure!");

		png_infop info = png_create_info_struct(png);

		if(info == nullptr)
		{
			png_destroy_read_struct(&png, nullptr, nullptr);
			throw Exception("Can't create PNG info structure!");
		}

		png_voidp error_ptr = png_get_error_ptr(png);
		png_set_error_fn(png, error_ptr, user_error_fn, nullptr);

		int val = setjmp(png_jmpbuf(png));

		if(val)
		{
			png_destroy_read_struct(&png, &info, nullptr);
			throw ImageConversionException("Png error happened!");
		}

		png_set_read_fn(png, &input, pngRead);
		png_set_sig_bytes(png, 0);
		png_read_info(png, info);

		png_uint_32 width = 0;
		png_uint_32 height = 0;
		int bit_depth;
		int color_type;

		png_uint_32 retval = png_get_IHDR(
			png, info,
			&width,
			&height,
			&bit_depth,
			&color_type,
			NULL, NULL, NULL
			);

		if(retval != 1)
			throw ImageConversionException("Can't read PNG header!");

		output->width = width;
		output->height = height;

		switch(color_type)
		{
		case PNG_COLOR_TYPE_RGB:
			output->format = ImageFormat::rgb;
			break;

		case PNG_COLOR_TYPE_RGB_ALPHA:
			output->format = ImageFormat::rgba;
			break;

		default:
			throw ImageConversionException("Invalid PNG color type!");
		}

		output->alloc();
		const png_size_t pitch = png_get_rowbytes(png, info);

		for(uint y = 0; y < output->height; ++y)
			png_read_row(png, output->data + y * pitch, NULL);

		png_read_end(png, info);
		png_destroy_read_struct(&png, &info, nullptr);
	}

	int getPreferredColorType(const ImageData *  image)
	{
		switch(image->format)
		{
		case ImageFormat::rgb:
			return PNG_COLOR_TYPE_RGB;
		case ImageFormat::rgba:
			return PNG_COLOR_TYPE_RGBA;
		case ImageFormat::grayscale:
			return PNG_COLOR_TYPE_GRAY;
		case ImageFormat::grayscale_alpha:
			return PNG_COLOR_TYPE_GRAY_ALPHA;

		default:
			return -1;
		}
	}

	void PngImageConverter::encode(OwnedByteData * output, const string & type, const ImageData *  image)
	{
		ByteOutput out;
		int color_type = getPreferredColorType(image);

		if(color_type == -1)
			throw ImageConversionException("Can't encode image with data format \"", image->format, "\" as PNG!");

		png_byte bit_depth = 8;

		png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if(png == nullptr)
			throw Exception("Can't create PNG structure!");

		png_infop info = png_create_info_struct(png);

		if(info == nullptr)
		{
			png_destroy_write_struct(&png, nullptr);
			throw Exception("Can't create PNG info structure!");
		}

		png_voidp error_ptr = png_get_error_ptr(png);
		png_set_error_fn(png, error_ptr, user_error_fn, nullptr);

		int val = setjmp(png_jmpbuf(png));

		if(val)
		{
			png_destroy_write_struct(&png, &info);
			throw Exception("Error ", val, " happened");
		}

		png_set_write_fn(png, &out, pngWrite, nullptr);
		png_set_compression_level(png, Z_BEST_SPEED);

		png_set_IHDR(
			png,
			info,
			image->width,
			image->height,
			bit_depth,
			color_type,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT
			);

		uint pitch = image->width * preferredBpp(image->format);
		byte * row = image->data;
		vector<byte *> rows;

		rows.reserve(image->height);

		for(uint y = 0; y < image->height; ++y, row += pitch)
			rows.push_back(row);

		png_set_rows(png, info, rows.data());
		png_write_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);
		png_write_end(png, info);

		output->set(move(out));
		png_destroy_write_struct(&png, &info);
	}
	*/
}