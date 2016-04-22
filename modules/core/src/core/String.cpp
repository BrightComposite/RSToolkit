//---------------------------------------------------------------------------

#include <core/String.h>
#include <core/addition/Named.h>
#include <core/Exception.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	string narrow(const wstring & wide)
	{
		return narrow(wide, locale());
	}

	string narrow(const wstring & wide, const locale & loc)
	{
		using namespace std;

		if(wide.empty())
			return string();

		typedef wstring::traits_type::state_type state_type;
		typedef codecvt<wchar_t, char, state_type> CVT;

		const CVT & cvt = use_facet<CVT>(loc);
		string narrow(cvt.max_length() * wide.size(), '\0');
		state_type state;

		const wchar_t * from_beg = &wide[0];
		const wchar_t * from_end = from_beg + wide.size();
		const wchar_t * from_nxt = nullptr;

		char * to_beg = &narrow[0];
		char * to_end = to_beg + narrow.size();
		char * to_nxt = nullptr;

		string::size_type sz = 0;
		codecvt_base::result r;

		do
		{
			r = cvt.out(state, from_beg, from_end, from_nxt, to_beg, to_end, to_nxt);

			switch(r)
			{
			case codecvt_base::error:
				throw ErrnoException("Error converting wstring to string");

			case codecvt_base::partial:
				sz += to_nxt - to_beg;
				narrow.resize(2 * narrow.size());
				to_beg = &narrow[sz];
				to_end = &narrow[0] + narrow.size();
				break;

			case codecvt_base::noconv:
				narrow.resize(sz + (from_end - from_beg) * sizeof(wchar_t));
				std::memcpy(&narrow[sz], from_beg, (from_end - from_beg) * sizeof(wchar_t));
				r = std::codecvt_base::ok;
				break;

			case codecvt_base::ok:
				sz += to_nxt - to_beg;
				narrow.resize(sz);
				break;
			}
		}
		while(r != codecvt_base::ok);

		return narrow;
	}

	wstring widen(const string & narrow)
	{
		return widen(narrow, locale());
	}

	wstring widen(const string & narrow, const locale & loc)
	{
		using namespace std;

		if(narrow.empty())
			return wstring();

		typedef string::traits_type::state_type state_type;
		typedef codecvt<char, wchar_t, state_type> CVT;

		const CVT & cvt = use_facet<CVT>(loc);
		wstring wide(cvt.max_length() * narrow.size(), '\0');
		state_type state;

		const char * from_beg = &narrow[0];
		const char * from_end = from_beg + narrow.size();
		const char * from_nxt = nullptr;

		wchar_t * to_beg = &wide[0];
		wchar_t * to_end = to_beg + wide.size();
		wchar_t * to_nxt = nullptr;

		wstring::size_type sz = 0;
		codecvt_base::result r;

		do
		{
			r = cvt.out(state, from_beg, from_end, from_nxt, to_beg, to_end, to_nxt);

			switch(r)
			{
			case codecvt_base::error:
				throw ErrnoException("Error converting string to wstring");

			case codecvt_base::partial:
				sz += to_nxt - to_beg;
				wide.resize(2 * wide.size());
				to_beg = &wide[sz];
				to_end = &wide[0] + wide.size();
				break;

			case codecvt_base::noconv:
				wide.resize(sz + (from_end - from_beg));
				std::memcpy(&wide[sz], from_beg, (from_end - from_beg));
				r = std::codecvt_base::ok;
				break;

			case codecvt_base::ok:
				sz += to_nxt - to_beg;
				wide.resize(sz);
				break;
			}
		}
		while(r != codecvt_base::ok);

		return wide;
	}

	//---------------------------------------------------------------------------

	String & String::upper()
	{
		_strupr_s(this->_Myptr(), this->_Mysize());
		return *this;
	}

	String & String::lower()
	{
		_strlwr_s(this->_Myptr(), this->_Mysize());
		return *this;
	}

	String::Cleaner & String::Cleaner::q(char ch)
	{
		if(string.empty())
			return *this;

		if(*string.cbegin() == ch && *(string.cend() - 1) == ch)
			string = move(string.substr(1, string._Mysize() - 2));

		return *this;
	}

	String::Cleaner & String::Cleaner::b(char first, char last)
	{
		if(string.empty())
			return *this;

		if(*string.cbegin() == first && *(string.cend() - 1) == last)
			string = move(string.substr(1, string._Mysize() - 2));

		return *this;
	}

	String::Cleaner & String::Cleaner::ls()
	{
		if(string.empty())
			return *this;

		size_t i;

		for(i = 0; i < string._Mysize() && isspace(string[i]); ++i)
			;

		string.assign(string.data() + i);

		return *this;
	}

	String::Cleaner & String::Cleaner::ts()
	{
		if(string.empty())
			return *this;

		size_t i;

		for(i = string._Mysize() - 1; i >= 0 && isspace(string[i]); --i)
			;

		if(i < string._Mysize() - 1)
			string.resize(i + 1);

		return *this;
	}

	String::Cleaner & String::Cleaner::ds()
	{
		if(string.empty())
			return *this;

		size_t pos = string.find("  ");

		while(pos != string::npos)
		{
			string.erase(pos, 1);
			pos = string.find("  ");
		}

		return *this;
	}

	String & String::sanitize()
	{
		string::shrink_to_fit();
		return *this;
	}

	String & String::trim()
	{
		return Cleaner(*this).ts().ls().string;
	}

	String & String::replace(const string & search, const string & replace, size_t offset)
	{
		size_t pos = this->find(search, offset);

		if(pos == string::npos)
			return *this;

		string::replace(pos, search._Mysize(), replace);

		return *this;
	}

	String & String::flood(size_t start, size_t count, char sym, char limiter)
	{
		if(start + count >= this->_Myres())
			resize(start + count + 1);

		char * ptr = this->_Myptr();
		memset(ptr + start, sym, count);
		ptr[start + count] = limiter;

		if(limiter != '\0')
			ptr[start + count + 1] = '\0';

		return *this;
	}

	//---------------------------------------------------------------------------

	WideString & WideString::upper()
	{
		_wcsupr_s(this->_Myptr(), this->_Mysize());
		return *this;
	}

	WideString & WideString::lower()
	{
		_wcslwr_s(this->_Myptr(), this->_Mysize());
		return *this;
	}

	WideString::Cleaner & WideString::Cleaner::q(wchar_t ch)
	{
		if(wstring.empty())
			return *this;

		if(*wstring.cbegin() == ch && *(wstring.cend() - 1) == ch)
			wstring = move(wstring.substr(1, wstring._Mysize() - 2));

		return *this;
	}

	WideString::Cleaner & WideString::Cleaner::b(wchar_t first, wchar_t last)
	{
		if(wstring.empty())
			return *this;

		if(*wstring.cbegin() == first && *(wstring.cend() - 1) == last)
			wstring = move(wstring.substr(1, wstring._Mysize() - 2));

		return *this;
	}

	WideString::Cleaner & WideString::Cleaner::ls()
	{
		if(wstring.empty())
			return *this;

		size_t i;

		for(i = 0; i < wstring._Mysize() && isspace(wstring[i]); ++i)
			;

		wstring.assign(wstring.data() + i);

		return *this;
	}

	WideString::Cleaner & WideString::Cleaner::ts()
	{
		if(wstring.empty())
			return *this;

		size_t i;

		for(i = wstring._Mysize() - 1; i >= 0 && isspace(wstring[i]); --i)
			;

		if(i < wstring._Mysize() - 1)
			wstring.resize(i + 1);

		return *this;
	}

	WideString::Cleaner & WideString::Cleaner::ds()
	{
		if(wstring.empty())
			return *this;

		size_t pos = wstring.find(L"  ");

		while(pos != wstring::npos)
		{
			wstring.erase(pos, 1);
			pos = wstring.find(L"  ");
		}

		return *this;
	}

	WideString & WideString::sanitize()
	{
		wstring::shrink_to_fit();
		return *this;
	}

	WideString & WideString::trim()
	{
		return Cleaner(*this).ts().ls().wstring;
	}

	WideString & WideString::replace(const wstring & search, const wstring & replace, size_t offset)
	{
		size_t pos = this->find(search, offset);

		if(pos == wstring::npos)
			return *this;

		wstring::replace(pos, search._Mysize(), replace);

		return *this;
	}

	WideString & WideString::flood(size_t start, size_t count, wchar_t sym, wchar_t limiter)
	{
		if(start + count >= this->_Myres())
			resize(start + count + 1);

		wchar_t * ptr = this->_Myptr();

		for(size_t i = 0; i < count; ++i)
			ptr[start + i] = sym;

		ptr[start + count] = limiter;

		if(limiter != '\0')
			ptr[start + count + 1] = '\0';

		return *this;
	}

	void print(String & target, const Object & obj)
	{
		target.add(addr(&obj), ' ');

		obj.getClass()->forEachClass(
			[&target, &obj](const Class<> * cl) {
				cl->addInfo(target, obj);
			}
		);

		target.add('(', obj.className(), ')');
	}

	Handle<StringList> split(const string & text, const char * sep)
	{
		auto list = handle<StringList>();

		if(text.empty())
			return list;

		char * temp = Memory<char>::copy(text.c_str(), text.length() + 1);
		char * next = nullptr;
		char * token = strtok_s(temp, sep, &next);

		while(token != nullptr)
		{
			list->emplace_back(token, strlen(token));
			token = strtok_s(nullptr, sep, &next);
		}

		Memory<char>::free(temp);

		return list;
	}

	Handle<StringList> splitOnLines(const string & text, uint lineLength, bool separateWords)
	{
		auto list = handle<StringList>();

		if(text.empty())
			return list;

		const char * start = text.c_str();
		const char * end = start + text.size();
		const char * pos = start;

		while(true)
		{
			if(pos >= end)
				break;

			uint length = std::min(lineLength, static_cast<uint>(end - pos));

			if(!separateWords)
			{
				const char * ptr = pos + length - 1;

				while((isalnum(*ptr) || *ptr == '_') && ptr > pos)
					ptr--;

				if(ptr == pos)
					ptr = pos + length - 1;

				length = static_cast<uint>(ptr - pos + 1);
			}

			list->emplace_back(pos, length);
			pos += length;
		}

		return list;
	}

	Handle<WideStringList> split(const wstring & text, const wchar_t * sep)
	{
		auto list = handle<WideStringList>();

		if(text.empty())
			return list;

		wchar_t * temp = Memory<wchar_t>::copy(text.c_str(), text.length() + 1);
		wchar_t * next = nullptr;
		wchar_t * token = wcstok_s(temp, sep, &next);

		while(token != nullptr)
		{
			list->emplace_back(token, wcslen(token));
			token = wcstok_s(nullptr, sep, &next);
		}

		Memory<wchar_t>::free(temp);

		return list;
	}

	Handle<WideStringList> splitOnLines(const wstring & text, uint lineLength, bool separateWords)
	{
		auto list = handle<WideStringList>();

		if(text.empty())
			return list;

		const wchar_t * start = text.c_str();
		const wchar_t * end = start + text.size();
		const wchar_t * pos = start;

		while(true)
		{
			if(pos >= end)
				break;

			uint length = std::min(lineLength, static_cast<uint>(end - pos));

			if(!separateWords)
			{
				const wchar_t * ptr = pos + length - 1;

				while((iswalnum(*ptr) || *ptr == '_') && ptr > pos)
					ptr--;

				if(ptr == pos)
					ptr = pos + length - 1;

				length = static_cast<uint>(ptr - pos + 1);
			}

			list->emplace_back(pos, length);
			pos += length;
		}

		return list;
	}

	byte toByte(const char * s)
	{
		return (byte)toInt(s);
	}

	char toChar(const char * s)
	{
		return s[0];
	}

	wchar_t toWideChar(const char * s)
	{
		return s[0];
	}

	double toDouble(const char * s)
	{
		return atof(s);
	}

	float toFloat(const char * s)
	{
		return (float)atof(s);
	}

	int toInt(const char * s)
	{
		return (int)atoi(s);
	}

	long toLong(const char * s)
	{
		String str(s);
		return str.isHexDigits() ? str.asLong(16) : str.asLong(10);
	}

	byte toByte(const wchar_t * s)
	{
		return (byte)toInt(s);
	}

	char toChar(const wchar_t * s)
	{
		return static_cast<char>(s[0]);
	}

	wchar_t	toWideChar(const wchar_t * s)
	{
		return s[0];
	}

	double toDouble(const wchar_t * s)
	{
		return _wtof(s);
	}

	float toFloat(const wchar_t * s)
	{
		return (float)_wtof(s);
	}

	int toInt(const wchar_t * s)
	{
		return (int)_wtoi(s);
	}

	long toLong(const wchar_t * s)
	{
		WideString str(s);
		return str.isHexDigits() ? str.asLong(16) : str.asLong(10);
	}

	String operator "" _s(const char * s, size_t unitsCount)
	{
		return {s, unitsCount};
	}

	WideString operator "" _s(const wchar_t * s, size_t unitsCount)
	{
		return {s, unitsCount};
	}
}