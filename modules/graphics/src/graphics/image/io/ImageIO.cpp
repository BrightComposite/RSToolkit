//---------------------------------------------------------------------------

#include <graphics/image/io/ImageIO.h>

#include <iostream>
#include <stack>

//---------------------------------------------------------------------------

namespace Rapture
{
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
		input.read(reinterpret_cast<char *>(raw.ptr), raw.size);

		read(output, get_extension(filepath), &raw);
	}

	void ImageIO::save(const path & filepath, const ImageData * image)
	{
		OwnedByteData raw;
		write(&raw, get_extension(filepath), image);

		FILE * stream;

		if(fopen_s(&stream, filepath.string().c_str(), "w+b") != 0)
			throw ErrnoException();

		fwrite(raw.ptr, 1, raw.size, stream);
		fclose(stream);
	}
}