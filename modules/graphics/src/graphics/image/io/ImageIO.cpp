//---------------------------------------------------------------------------

#include <graphics/image/io/ImageIO.h>

#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
	using std::ifstream;
	using std::ofstream;
	using std::ios_base;

	void ImageIO::load(ImageData * output, const path & filepath)
	{
		using namespace boost::filesystem;

		if(!exists(filepath))
			throw Exception("File ", filepath.c_str(), " doesn't exist!");

		OwnedByteData raw(static_cast<size_t>(file_size(filepath)));

		ifstream input(filepath.c_str(), ios_base::binary);
		input.read(reinterpret_cast<char *>(raw.ptr), raw.size);
		input.close();

		read(output, get_extension(filepath), &raw);
	}

	void ImageIO::save(const path & filepath, const ImageData * image)
	{
		OwnedByteData raw;
		write(&raw, get_extension(filepath), image);

		ofstream output(filepath.c_str(), ios_base::binary | ios_base::trunc);
		output.write(reinterpret_cast<char *>(raw.ptr), raw.size);
		output.close();
	}
}