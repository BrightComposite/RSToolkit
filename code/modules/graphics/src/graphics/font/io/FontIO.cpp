//---------------------------------------------------------------------------

#include <graphics/font/io/FontIO.h>

#include <fstream>

//---------------------------------------------------------------------------

namespace asd
{
	using std::ifstream;
	using std::ios_base;

	void FontIO::load(Handle<Font> & output, const path & filepath)
	{
		using namespace boost::filesystem;

		auto raw = handle<OwnedByteData>();
		path pth = filepath;
		
	#if BOOST_OS_WINDOWS
		if(!exists(pth)) {
			pth = path("C:/Windows/Fonts") / filepath;
		}
	#elif BOOST_OS_LINUX
		if(!exists(pth)) {
			for(auto s : {"/usr/share/fonts", "/usr/local/share/fonts", "~/.fonts"}) {
				path out;
				
				if(find_file(s, filepath, out)) {
					pth = out;
					break;
				}
			}
		}
	#endif
		
		if(!exists(pth))
			throw Exception("File ", filepath.string(), " doesn't exist!");
		
		ifstream input(pth.string(), ios_base::binary);

		raw->alloc(static_cast<size_t>(file_size(pth)));
		input.read(reinterpret_cast<char *>(raw->ptr), raw->size);

		read(output, get_extension(pth), raw);
	}

}

//---------------------------------------------------------------------------
