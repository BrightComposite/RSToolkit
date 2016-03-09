//---------------------------------------------------------------------------

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

//---------------------------------------------------------------------------

#include <boost/filesystem/operations.hpp>
#include <string>

//---------------------------------------------------------------------------

namespace Rapture
{
	using boost::filesystem::file_size;
	using boost::filesystem::path;
	using boost::filesystem::current_path;
	using boost::filesystem::initial_path;

	using std::string;
	using std::wstring;

	inline string get_extension(const path & filepath)
	{
		return filepath.extension().string().substr(1);
	}

	inline wstring get_extension_w(const path & filepath)
	{
		return filepath.extension().wstring().substr(1);
	}
}

template<class Str>
inline auto operator + (const Rapture::path & filepath, const Str & s)
{
	return Rapture::path(filepath).concat(s);
}

//---------------------------------------------------------------------------
#endif