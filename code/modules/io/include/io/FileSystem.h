//---------------------------------------------------------------------------

#pragma once

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

//---------------------------------------------------------------------------

#define BOOST_NO_AUTO_PTR

#include <boost/filesystem/operations.hpp>
#include <meta/Macro.h>
#include <string>

//---------------------------------------------------------------------------

namespace asd
{
	using boost::filesystem::file_size;
	using boost::filesystem::path;
	using boost::filesystem::current_path;
	using boost::filesystem::initial_path;
	
	using std::string;
	using std::wstring;
	
	inline string get_extension(const path & filepath) {
		return filepath.extension().string().substr(1);
	}
	
	inline wstring get_extension_w(const path & filepath) {
		return filepath.extension().wstring().substr(1);
	}
	
	inline bool find_file(const path & dir_path, const path & file_name, path & path_found) {
		using namespace boost::filesystem;
		
		const recursive_directory_iterator end;
		const auto it = std::find_if(recursive_directory_iterator(dir_path), end, [&file_name](const directory_entry & e) {
			return e.path().filename() == file_name;
		});
		
		if(it == end) {
			return false;
		}
		
		path_found = it->path();
		return true;
	}
}

template<class Str>
inline auto operator +(const asd::path & filepath, const Str & s) {
	return asd::path(filepath).concat(s);
}

//---------------------------------------------------------------------------
#endif
