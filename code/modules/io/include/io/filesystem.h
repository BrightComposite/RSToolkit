//---------------------------------------------------------------------------

#pragma once

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

//---------------------------------------------------------------------------

#define BOOST_NO_AUTO_PTR

#include <boost/filesystem/operations.hpp>
#include <meta/macro.h>
#include <container/data.h>
#include <string>
#include <fstream>

//---------------------------------------------------------------------------

namespace asd
{
	namespace fs
	{
		using boost::filesystem::file_size;
		using boost::filesystem::path;
		using boost::filesystem::current_path;
		using boost::filesystem::initial_path;
		
		using std::string;
		using std::wstring;
		
		inline string get_extension(const fs::path & path) {
			return path.extension().string().substr(1);
		}
		
		inline wstring get_extension_w(const fs::path & path) {
			return path.extension().wstring().substr(1);
		}
		
		inline bool find_file(const fs::path & dir_path, const fs::path & file_name, fs::path & path_found) {
			using namespace boost::filesystem;
			
			const recursive_directory_iterator end;
			const auto it = std::find_if(recursive_directory_iterator(dir_path), end, [&file_name](const directory_entry & e) {
				return e.path().filename() == file_name;
			});
			
			if (it == end) {
				return false;
			}
			
			path_found = it->path();
			return true;
		}
		
		inline owned_byte_data read_contents(const fs::path & file_path) {
			owned_byte_data data(boost::filesystem::file_size(file_path));
			std::ifstream stream(file_path.string());
			stream.read(reinterpret_cast<char *>(data.ptr), data.size);
			
			return data;
		}
	}
}

template <class Str>
inline auto operator +(const asd::fs::path & path, const Str & s) {
	return asd::fs::path(path).concat(s);
}

//---------------------------------------------------------------------------
#endif
