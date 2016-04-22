//---------------------------------------------------------------------------

#ifndef STRING_H
#define STRING_H

//---------------------------------------------------------------------------

#include <core/algorithm/lookup3.h>
#include <core/container/List.h>

#include <stdarg.h>
#include <array>
#include <string>

//---------------------------------------------------------------------------

using std::array;
using std::string;
using std::wstring;
using std::locale;

//---------------------------------------------------------------------------

namespace Rapture
{
	class String;
	class WideString;

#ifndef _WIN64
	inline errno_t _sttoa_s(size_t _Value, char * _Buffer, size_t _BufferCount, int _Radix)
	{
		return _ultoa_s(_Value, _Buffer, _BufferCount, _Radix);
	}

	inline size_t _strtost(char const * _String, char ** _EndPtr, int _Radix)
	{
		return strtoul(_String, _EndPtr, _Radix);
	}

	inline errno_t _sttow_s(size_t _Value, wchar_t * _Buffer, size_t _BufferCount, int _Radix)
	{
		return _ultow_s(_Value, _Buffer, _BufferCount, _Radix);
	}

	inline size_t _wcstost(wchar_t const * _String, wchar_t ** _EndPtr, int _Radix)
	{
		return wcstoul(_String, _EndPtr, _Radix);
	}
#else
	inline errno_t _sttoa_s(size_t _Value, char * _Buffer, size_t _BufferCount, int _Radix)
	{
		return _ui64toa_s(_Value, _Buffer, _BufferCount, _Radix);
	}

	inline size_t _strtost(char const * _String, char ** _EndPtr, int _Radix)
	{
		return _strtoui64(_String, _EndPtr, _Radix);
	}

	inline errno_t _sttow_s(size_t _Value, wchar_t * _Buffer, size_t _BufferCount, int _Radix)
	{
		return _ui64tow_s(_Value, _Buffer, _BufferCount, _Radix);
	}

	inline size_t _wcstost(wchar_t const * _String, wchar_t ** _EndPtr, int _Radix)
	{
		return _wcstoui64(_String, _EndPtr, _Radix);
	}
#endif

	string  narrow(const wstring & wide);
	string  narrow(const wstring & wide, const locale & loc);

	wstring widen(const string & narrow);
	wstring widen(const string & narrow, const locale & loc);

	function_checker(__is_str_printable, print);

	template<class T>
	struct is_str_printable
	{
		static const bool value = __is_str_printable<String &, T>::value && !is_base_of<string, T>::value && !is_base_of<wstring, T>::value;
	};

	template<class T>
	struct is_wstr_printable
	{
		static const bool value = __is_str_printable<WideString &, T>::value && !is_base_of<string, T>::value && !is_base_of<wstring, T>::value;
	};

	template<class ... T>
	struct is_string_assembleable
	{
		static const bool value = is_true<can_construct(String, T)...>::value;
	};

	template<class ... T>
	struct is_wstring_assembleable
	{
		static const bool value = is_true<can_construct(WideString, T)...>::value;
	};

#define can_string_assemble(...) is_string_assembleable<__VA_ARGS__>::value
#define can_wstring_assemble(...) is_wstring_assembleable<__VA_ARGS__>::value

	link_class(String, Class<Object>);
	link_class(WideString, Class<Object>);

	class String : public string, public Object
	{
	public:
		String() : string()
		{
			setclass(String);
		}

		String(const String & s) : string(s)
		{
			setclass(String);
		}

		String(String && s) : string(forward<String>(s))
		{
			setclass(String);
		}

		String(const string & s) : string(s)
		{
			setclass(String);
		}

		String(string && s) : string(forward<string>(s))
		{
			setclass(String);
		}

		String(const char * s) : string(s)
		{
			setclass(String);
		}

		String(const char * s, size_t length) : string(s, length)
		{
			setclass(String);
		}

		String(const wstring & s) : string(narrow(s))
		{
			setclass(String);
		}

		String(const wchar_t * s) : string(narrow(s))
		{
			setclass(String);
			_Myptr()[_Mysize()] = '\0';
		}

		String(const wchar_t * const s, size_t length) : string(narrow({s, length}))
		{
			setclass(String);
		}

		String(char value) : string(&value, 1)
		{
			setclass(String);
		}

		String(byte value, int radix = 16) : String(static_cast<int>(value), radix) {}

		String(int value, int radix = 10) : String()
		{
			auto * buf = Memory<char>::allocate(16);

			if(radix == 16)
			{
				buf[0] = '0';
				buf[1] = 'x';

				_itoa_s(value, buf + 2, 14, radix);
			}
			else
				_itoa_s(value, buf, 16, radix);

			string::assign(buf);
			Memory<char>::free(buf);
		}

		String(long value, int radix = 16) : String()
		{
			auto * buf = Memory<char>::allocate(16);

			if(radix == 16)
			{
				buf[0] = '0';
				buf[1] = 'x';

				_ltoa_s(value, buf + 2, 14, radix);
			}
			else
				_ltoa_s(value, buf, 16, radix);

			string::assign(buf);
			Memory<char>::free(buf);
		}

		String(ulong value, int radix = 16) : String()
		{
			auto * buf = Memory<char>::allocate(16);

			if(radix == 16)
			{
				buf[0] = '0';
				buf[1] = 'x';

				_ultoa_s(value, buf + 2, 14, radix);
			}
			else
				_ultoa_s(value, buf, 16, radix);

			string::assign(buf);
			Memory<char>::free(buf);
		}

		String(size_t value, int radix = 16) : String()
		{
			auto * buf = Memory<char>::allocate(32);

			if(radix == 16)
			{
				buf[0] = '0';
				buf[1] = 'x';

				_sttoa_s(value, buf + 2, 30, radix);
			}
			else
				_sttoa_s(value, buf, 32, radix);

			string::assign(buf);
			Memory<char>::free(buf);
		}

		String(double value) : String()
		{
			auto * buf = Memory<char>::allocate(16);
			sprintf_s(buf, 16, "%.5f", value);

			size_t length = strlen(buf);

			while(buf[length - 1] == '0')
				length--;

			if(buf[length - 1] == '.')
			{
				length++;
				buf[length - 1] = '0';
			}

			buf[length] = '\0';

			string::assign(buf);
			Memory<char>::free(buf);
		}

		String(float value) : String()
		{
			auto * buf = Memory<char>::allocate(16);
			sprintf_s(buf, 16, "%.5f", value);

			size_t length = strlen(buf);

			while(buf[length - 1] == '0')
				length--;

			if(buf[length - 1] == '.')
			{
				length++;
				buf[length - 1] = '0';
			}

			buf[length] = '\0';

			string::assign(buf);
			Memory<char>::free(buf);
		}

		template<class T, useif(is_str_printable<T>::value)>
		String(const T & obj) : String()
		{
			print(*this, obj);
		}

		String(const char * s1, size_t l1, const char * s2, size_t l2) : string(string(s1, l1) + string(s2, l2))
		{
			setclass(String);
		}

		virtual ~String() {}

		String & operator = (const String & value)
		{
			string::operator = (value);
			return *this;
		}

		String & operator = (String && value)
		{
			string::operator = (value);
			return *this;
		}

		String & operator = (const string & value)
		{
			string::operator = (value);
			return *this;
		}

		String & operator = (string && value)
		{
			string::operator = (forward<string>(value));
			return *this;
		}

		String & operator = (const char * value)
		{
			string::operator = (value);
			return *this;
		}

		String & operator = (const wstring & value)
		{
			string::operator = (narrow(value));
			return *this;
		}

		String & operator = (const wchar_t * value)
		{
			string::operator = (narrow(value));
			return *this;
		}

		String & operator = (char value)
		{
			string::operator = (value);
			return *this;
		}

		String & operator = (int value)
		{
			return operator = (String(value));
		}

		String & operator = (long value)
		{
			return operator = (String(value));
		}

		String & operator = (ulong value)
		{
			return operator = (String(value));
		}

		String & operator = (size_t value)
		{
			return operator = (String(value));
		}

		String & operator = (float value)
		{
			return operator = (String(value));
		}

		String & operator = (const Object & value)
		{
			return operator = (String(value));
		}

		String & operator += (const String & value)
		{
			string::operator += (value);
			return *this;
		}

		String & operator += (const string & value)
		{
			string::operator += (value);
			return *this;
		}

		String & operator += (const char * value)
		{
			string::operator += (value);
			return *this;
		}

		inline String & operator += (const WideString & value);

		String & operator += (const wstring & value)
		{
			string::operator += (narrow(value));
			return *this;
		}

		String & operator += (const wchar_t * value)
		{
			string::operator += (narrow(value));
			return *this;
		}

		String & operator += (char value)
		{
			string::operator += (value);
			return *this;
		}

		template<class T, useif(can_construct(String, T))>
		String & operator += (const T & value)
		{
			return operator += (String(value));
		}

		template<class T, useif(can_construct(String, T))>
		String & operator << (const T & value)
		{
			return operator += (value);
		}

		String & operator >> (string & value)
		{
			value.operator += (*this);
			return *this;
		}

		String & operator >> (wstring & value)
		{
			value.operator += (widen(*this));
			return *this;
		}

		char & operator >> (char & value)
		{
			value = asChar();
			return value;
		}

		wchar_t & operator >> (wchar_t & value)
		{
			value = asWideChar();
			return value;
		}

		byte & operator >> (byte & value)
		{
			value = (byte)asInt();
			return value;
		}

		int & operator >> (int & value)
		{
			value = asInt();
			return value;
		}

		long & operator >> (long & value)
		{
			value = asLong();
			return value;
		}

		ulong & operator >> (ulong & value)
		{
			value = asUnsignedLong();
			return value;
		}

		size_t & operator >> (size_t & value)
		{
			value = asSize();
			return value;
		}

		float & operator >> (float & value)
		{
			value = asFloat();
			return value;
		}

		double & operator >> (double & value)
		{
			value = asDouble();
			return value;
		}

		template<class T>
		auto operator + (T value) const
		{
			return String(*this).operator += (value);
		}

		operator const char * () const
		{
			return this->_Myptr();
		}

		char & operator [] (int index)
		{
			return string::operator[](index);
		}

		const char & operator [] (int index) const
		{
			return string::operator[](index);
		}

		char & operator [] (size_t index)
		{
			return string::operator[](index);
		}

		const char & operator [] (size_t index) const
		{
			return string::operator[](index);
		}

		String & trim();

		String & upper();

		String & lower();

		String & replace(const string & pattern, const string & str, size_t offset = 0);

		String & flood(size_t start, size_t count, char sym, char limiter = '\0');

		template<class ... T,
			useif(
				can_string_assemble(T...)
				)>
		static inline String assemble(T &&... args)
		{
			return String().add(forward<T>(args)...);
		}

		template<class H, class ... T,
			useif(
				can_string_assemble(H, T...)
				)>
		String & add(H && head, T &&... tail)
		{
			operator += (forward<H>(head));
			return add(forward<T>(tail)...);
		}

		struct Cleaner
		{
			String & string;

			Cleaner(String & str) : string(str) {}

			Cleaner & q(char ch = '\"'); // quotes
			
			Cleaner & b(char first = '(', char last = ')'); // brackets;

			Cleaner & ls(); // leading spaces
			
			Cleaner & ts(); // trailing spaces

			Cleaner & ds(); // double spaces

			operator String & ()
			{
				return string;
			}
		};

		String & sanitize();

		char asChar()
		{
			return this->empty() ? '\0' : this->_Myptr()[0];
		}

		wchar_t asWideChar()
		{
			return this->empty() ? '\0' : this->_Myptr()[0];
		}

		int asInt(int radix = 10)
		{
			const char * ptr = this->_Myptr();
			char * end = nullptr;

			return static_cast<int>(strtol(ptr, &end, radix));
		}

		long asLong(int radix = 10)
		{
			const char * ptr = this->_Myptr();
			char * end = nullptr;

			return strtol(ptr, &end, radix);
		}

		ulong asUnsignedLong(int radix = 10)
		{
			const char * ptr = this->_Myptr();
			char * end = nullptr;

			return strtoul(ptr, &end, radix);
		}

		size_t asSize(int radix = 10)
		{
			const char * ptr = this->_Myptr();
			char * end = nullptr;

			return _strtost(ptr, &end, radix);
		}

		float asFloat()
		{
			const char * ptr = this->_Myptr();
			char * end = nullptr;

			return strtof(ptr, &end);
		}

		double asDouble()
		{
			const char * ptr = this->_Myptr();
			char * end = nullptr;

			return strtod(ptr, &end);
		}

		bool isAlpha()
		{
			for(auto ptr = this->cbegin(); ptr < this->cend(); ++ptr)
			{
				if(!isalpha((unsigned char)*ptr))
					return false;
			}

			return true;
		}

		bool isAlnum()
		{
			for(auto ptr = this->cbegin(); ptr < this->cend(); ++ptr)
			{
				if(!isalnum((unsigned char)*ptr))
					return false;
			}

			return true;
		}

		bool isDigits()
		{
			for(auto ptr = this->cbegin(); ptr < this->cend(); ++ptr)
			{
				if(!isdigit((unsigned char)*ptr))
					return false;
			}

			return true;
		}

		bool isHexDigits()
		{
			const char * s = this->_Myptr();

			if(s[0] != '0' || s[1] != 'x')
				return false;

			for(const char * ptr = s + 2; *ptr != '\0'; ptr++)
			{
				if(!isxdigit((unsigned char)*ptr))
					return false;
			}

			return true;
		}

		bool isIntegral(int radix = 10)
		{
			const char * ptr = this->_Myptr();
			char * end;
			absorb() strtol(ptr, &end, radix);

			return *end == '\0';
		}

		bool isFloating()
		{
			const char * ptr = this->_Myptr();
			char * end;
			absorb() strtod(ptr, &end);

			return *end == '\0';
		}

	protected:
		String & add()
		{
			return *this;
		}
	};

	class WideString : public wstring, public Object
	{
	public:
		WideString() : wstring()
		{
			setclass(WideString);
		}

		WideString(const WideString & s) : wstring(s)
		{
			setclass(WideString);
		}

		WideString(WideString && s) : wstring(forward<WideString>(s))
		{
			setclass(WideString);
		}

		WideString(const wstring & s) : wstring(s)
		{
			setclass(WideString);
		}

		WideString(wstring && s) : wstring(forward<wstring>(s))
		{
			setclass(WideString);
		}

		WideString(const wchar_t * s) : wstring(s)
		{
			setclass(String);
		}

		WideString(const wchar_t * s, size_t length) : wstring(s, length)
		{
			setclass(String);
		}

		WideString(const string & s) : wstring(widen(s))
		{
			setclass(String);
		}

		WideString(const char * s) : wstring(widen(s))
		{
			setclass(String);
			_Myptr()[_Mysize()] = '\0';
		}

		WideString(const char * const s, size_t length) : wstring(widen({s, length}))
		{
			setclass(String);
		}

		WideString(wchar_t value) : wstring(&value, 1)
		{
			setclass(String);
		}

		WideString(byte value, int radix = 16) : WideString(static_cast<int>(value), radix) {}

		WideString(int value, int radix = 10) : WideString()
		{
			auto * buf = Memory<wchar_t>::allocate(16);

			if(radix == 16)
			{
				buf[0] = '0';
				buf[1] = 'x';

				_itow_s(value, buf + 2, 14, radix);
			}
			else
				_itow_s(value, buf, 16, radix);

			wstring::assign(buf);
			Memory<wchar_t>::free(buf);
		}

		WideString(long value, int radix = 16) : WideString()
		{
			auto * buf = Memory<wchar_t>::allocate(16);

			if(radix == 16)
			{
				buf[0] = '0';
				buf[1] = 'x';

				_ltow_s(value, buf + 2, 14, radix);
			}
			else
				_ltow_s(value, buf, 16, radix);

			wstring::assign(buf);
			Memory<wchar_t>::free(buf);
		}

		WideString(ulong value, int radix = 16) : WideString()
		{
			auto * buf = Memory<wchar_t>::allocate(16);

			if(radix == 16)
			{
				buf[0] = '0';
				buf[1] = 'x';

				_ultow_s(value, buf + 2, 14, radix);
			}
			else
				_ultow_s(value, buf, 16, radix);

			wstring::assign(buf);
			Memory<wchar_t>::free(buf);
		}

		WideString(size_t value, int radix = 16) : WideString()
		{
			auto * buf = Memory<wchar_t>::allocate(32);

			if(radix == 16)
			{
				buf[0] = '0';
				buf[1] = 'x';

				_sttow_s(value, buf + 2, 30, radix);
			}
			else
				_sttow_s(value, buf, 32, radix);

			wstring::assign(buf);
			Memory<wchar_t>::free(buf);
		}

		WideString(double value) : WideString()
		{
			auto * buf = Memory<wchar_t>::allocate(16);
			swprintf_s(buf, 16, L"%.5f", value);

			size_t length = wcslen(buf);

			while(buf[length - 1] == '0')
				length--;

			if(buf[length - 1] == '.')
			{
				length++;
				buf[length - 1] = '0';
			}

			buf[length] = '\0';

			wstring::assign(buf);
			Memory<wchar_t>::free(buf);
		}

		WideString(float value) : WideString()
		{
			auto * buf = Memory<wchar_t>::allocate(16);
			swprintf_s(buf, 16, L"%.5f", value);

			size_t length = wcslen(buf);

			while(buf[length - 1] == '0')
				length--;

			if(buf[length - 1] == '.')
			{
				length++;
				buf[length - 1] = '0';
			}

			buf[length] = '\0';

			wstring::assign(buf);
			Memory<wchar_t>::free(buf);
		}

		template<class T, useif(is_str_printable<T>::value)>
		WideString(const T & obj) : WideString()
		{
			print(*this, obj);
		}

		WideString(const wchar_t * s1, size_t l1, const wchar_t * s2, size_t l2) : wstring(wstring(s1, l1) + wstring(s2, l2))
		{
			setclass(String);
		}

		virtual ~WideString() {}

		WideString & operator = (const WideString & value)
		{
			wstring::operator = (value);
			return *this;
		}

		WideString & operator = (WideString && value)
		{
			wstring::operator = (value);
			return *this;
		}

		WideString & operator = (const wstring & value)
		{
			wstring::operator = (value);
			return *this;
		}

		WideString & operator = (wstring && value)
		{
			wstring::operator = (forward<wstring>(value));
			return *this;
		}

		WideString & operator = (const wchar_t * value)
		{
			wstring::operator = (value);
			return *this;
		}

		WideString & operator = (const string & value)
		{
			wstring::operator = (widen(value));
			return *this;
		}

		WideString & operator = (const char * value)
		{
			wstring::operator = (widen(value));
			return *this;
		}

		WideString & operator = (wchar_t value)
		{
			wstring::operator = (value);
			return *this;
		}

		WideString & operator = (int value)
		{
			return operator = (WideString(value));
		}

		WideString & operator = (long value)
		{
			return operator = (WideString(value));
		}

		WideString & operator = (ulong value)
		{
			return operator = (WideString(value));
		}

		WideString & operator = (size_t value)
		{
			return operator = (WideString(value));
		}

		WideString & operator = (float value)
		{
			return operator = (WideString(value));
		}

		WideString & operator = (const Object & value)
		{
			return operator = (WideString(value));
		}

		WideString & operator += (const WideString & value)
		{
			wstring::operator += (value);
			return *this;
		}

		WideString & operator += (const wstring & value)
		{
			wstring::operator += (value);
			return *this;
		}

		WideString & operator += (const wchar_t * value)
		{
			wstring::operator += (value);
			return *this;
		}

		WideString & operator += (const string & value)
		{
			wstring::operator += (widen(value));
			return *this;
		}

		WideString & operator += (const char * value)
		{
			wstring::operator += (widen(value));
			return *this;
		}

		WideString & operator += (wchar_t value)
		{
			wstring::operator += (value);
			return *this;
		}

		template<class T, useif(can_construct(WideString, T))>
		WideString & operator += (const T & value)
		{
			return operator += (WideString(value));
		}

		template<class T, useif(can_construct(WideString, T))>
		WideString & operator << (const T & value)
		{
			return operator += (value);
		}

		WideString & operator >> (wstring & value)
		{
			value.operator += (*this);
			return *this;
		}

		WideString & operator >> (string & value)
		{
			value.operator += (narrow(*this));
			return *this;
		}

		wchar_t & operator >> (wchar_t & value)
		{
			value = asChar();
			return value;
		}

		char & operator >> (char & value)
		{
			value = asWideChar();
			return value;
		}

		byte & operator >> (byte & value)
		{
			value = (byte)asInt();
			return value;
		}

		int & operator >> (int & value)
		{
			value = asInt();
			return value;
		}

		long & operator >> (long & value)
		{
			value = asLong();
			return value;
		}

		ulong & operator >> (ulong & value)
		{
			value = asUnsignedLong();
			return value;
		}

		size_t & operator >> (size_t & value)
		{
			value = asSize();
			return value;
		}

		float & operator >> (float & value)
		{
			value = asFloat();
			return value;
		}

		double & operator >> (double & value)
		{
			value = asDouble();
			return value;
		}

		template<class T>
		auto operator + (T value) const
		{
			return WideString(*this).operator += (value);
		}

		operator const wchar_t * () const
		{
			return this->_Myptr();
		}

		wchar_t & operator [] (int index)
		{
			return wstring::operator[](index);
		}

		const wchar_t & operator [] (int index) const
		{
			return wstring::operator[](index);
		}

		wchar_t & operator [] (size_t index)
		{
			return wstring::operator[](index);
		}

		const wchar_t & operator [] (size_t index) const
		{
			return wstring::operator[](index);
		}

		WideString & trim();

		WideString & upper();

		WideString & lower();

		WideString & replace(const wstring & pattern, const wstring & str, size_t offset = 0);

		WideString & flood(size_t start, size_t count, wchar_t sym, wchar_t limiter = '\0');

		template<class ... T,
			useif(
				can_string_assemble(T...)
				)>
		static inline WideString assemble(T &&... args)
		{
			return WideString().add(forward<T>(args)...);
		}

		template<class H, class ... T,
			useif(
				can_string_assemble(H, T...)
				)>
		WideString & add(H && head, T &&... tail)
		{
			operator += (forward<H>(head));
			return add(forward<T>(tail)...);
		}

		WideString & add()
		{
			return *this;
		}

		struct Cleaner
		{
			WideString & wstring;

			Cleaner(WideString & str) : wstring(str) {}

			Cleaner & q(wchar_t ch = '\"'); // quotes

			Cleaner & b(wchar_t first = '(', wchar_t last = ')'); // brackets;

			Cleaner & ls(); // leading spaces

			Cleaner & ts(); // trailing spaces

			Cleaner & ds(); // double spaces

			operator WideString & ()
			{
				return wstring;
			}
		};

		WideString & sanitize();

		wchar_t asChar()
		{
			return this->empty() ? '\0' : this->_Myptr()[0];
		}

		char asWideChar()
		{
			return this->empty() ? '\0' : this->_Myptr()[0];
		}

		int asInt(int radix = 10)
		{
			const wchar_t * ptr = this->_Myptr();
			wchar_t * end = nullptr;

			return static_cast<int>(wcstol(ptr, &end, radix));
		}

		long asLong(int radix = 10)
		{
			const wchar_t * ptr = this->_Myptr();
			wchar_t * end = nullptr;

			return wcstol(ptr, &end, radix);
		}

		ulong asUnsignedLong(int radix = 10)
		{
			const wchar_t * ptr = this->_Myptr();
			wchar_t * end = nullptr;

			return wcstoul(ptr, &end, radix);
		}

		size_t asSize(int radix = 10)
		{
			const wchar_t * ptr = this->_Myptr();
			wchar_t * end = nullptr;

			return _wcstost(ptr, &end, radix);
		}

		float asFloat()
		{
			const wchar_t * ptr = this->_Myptr();
			wchar_t * end = nullptr;

			return wcstof(ptr, &end);
		}

		double asDouble()
		{
			const wchar_t * ptr = this->_Myptr();
			wchar_t * end = nullptr;

			return wcstod(ptr, &end);
		}

		bool isAlpha()
		{
			for(auto ptr = this->cbegin(); ptr < this->cend(); ++ptr)
			{
				if(!iswalpha(*ptr))
					return false;
			}

			return true;
		}

		bool isAlnum()
		{
			for(auto ptr = this->cbegin(); ptr < this->cend(); ++ptr)
			{
				if(!iswalnum(*ptr))
					return false;
			}

			return true;
		}

		bool isDigits()
		{
			for(auto ptr = this->cbegin(); ptr < this->cend(); ++ptr)
			{
				if(!iswdigit(*ptr))
					return false;
			}

			return true;
		}

		bool isHexDigits()
		{
			const wchar_t * s = this->_Myptr();

			if(s[0] != '0' || s[1] != 'x')
				return false;

			for(const wchar_t * ptr = s + 2; *ptr != '\0'; ptr++)
			{
				if(!iswxdigit(*ptr))
					return false;
			}

			return true;
		}

		bool isIntegral(int radix = 10)
		{
			const wchar_t * ptr = this->_Myptr();
			wchar_t * end;
			absorb() wcstol(ptr, &end, radix);

			return *end == '\0';
		}

		bool isFloating()
		{
			const wchar_t * ptr = this->_Myptr();
			wchar_t * end;
			absorb() wcstod(ptr, &end);

			return *end == '\0';
		}

		static Class<Object> meta;
	};

	inline String & String::operator += (const WideString & value)
	{
		string::operator += (narrow(value));
		return *this;
	}

	void print(String & target, const Object & obj);

	template<class T,
		useif(
			is_str_printable<T>::value
			)>
	inline void print(String & target, const Handle<T> & object)
	{
		target << *object;
	}

	template<class T,
		useif(
			is_str_printable<T>::value
			)>
	inline String print(const T & object)
	{
		String target;
		print(target, object);

		return target;
	}

	template<class T,
		useif(
			is_str_printable<T>::value
			)>
	inline std::basic_ostream<char, std::char_traits<char>> &
		operator << (std::basic_ostream<char, std::char_traits<char>> & ostr, const T & object)
	{
		String target;
		print(target, object);

		return ostr << target;
	}

	void print(WideString & target, const Object & obj);

	template<class T,
		selectif(1,
			is_wstr_printable<T>::value
			)>
	inline void print(WideString & target, const Handle<T> & object)
	{
		target << *object;
	}

	template<class T,
		selectif(1,
			is_wstr_printable<T>::value
			)>
	inline WideString print(const T & object)
	{
		WideString target;
		print(target, object);

		return target;
	}

	template<class T,
		selectif(1,
			is_wstr_printable<T>::value
			)>
	inline std::basic_ostream<wchar_t, std::char_traits<wchar_t>> &
		operator << (std::basic_ostream<wchar_t, std::char_traits<wchar_t>> & ostr, const T & object)
	{
		WideString target;
		print(target, object);

		return ostr << target;
	}

	template<class T,
		selectif(2,
			is_str_printable<T>::value
			)>
	inline void print(WideString & target, const Handle<T> & object)
	{
		target << *object;
	}

	template<class T,
		selectif(2,
			is_str_printable<T>::value
			)>
	inline WideString print(const T & object)
	{
		String target;
		print(target, object);

		return widen(target);
	}

	template<class T,
		selectif(2,
			is_str_printable<T>::value
			)>
	inline std::basic_ostream<wchar_t, std::char_traits<wchar_t>> &
		operator << (std::basic_ostream<wchar_t, std::char_traits<wchar_t>> & ostr, const T & object)
	{
		String target;
		print(target, object);

		return ostr << widen(target);
	}

	typedef ListObject<string> StringList;
	typedef ListObject<wstring> WideStringList;

	Handle<StringList> split(const string & text, const char * sep = " \t");
	Handle<StringList> splitOnLines(const string & text, size_t lineLength, bool separateWords = true);

	Handle<WideStringList> split(const wstring & text, const wchar_t * sep = L" \t");
	Handle<WideStringList> splitOnLines(const wstring & text, size_t lineLength, bool separateWords = true);

	inline String operator + (const char * s, const String & string)
	{
		return {s, strlen(s), string.c_str(), string.length()};
	}

	inline WideString operator + (const wchar_t * s, const WideString & string)
	{
		return {s, wcslen(s), string.c_str(), string.length()};
	}

	byte	toByte(const char * s);
	char	toChar(const char * s);
	wchar_t	toWideChar(const char * s);
	double	toDouble(const char * s);
	float	toFloat(const char * s);
	int		toInt(const char * s);
	long	toLong(const char * s);

	byte	toByte(const wchar_t * s);
	char	toChar(const wchar_t * s);
	wchar_t	toWideChar(const wchar_t * s);
	double	toDouble(const wchar_t * s);
	float	toFloat(const wchar_t * s);
	int		toInt(const wchar_t * s);
	long	toLong(const wchar_t * s);

	String operator "" _s(const char * s, size_t unitsCount);
	WideString operator "" _s(const wchar_t * s, size_t unitsCount);
}

namespace std
{
	template<>
	struct hash<Rapture::String> : unary_function<Rapture::String, size_t>
	{
		typedef Rapture::String _Kty;

		size_t operator()(const _Kty& _Keyval) const
		{
			return hashbytes(_Keyval.data(), _Keyval.length());
		}
	};

	template<>
	struct hash<Rapture::WideString> : unary_function<Rapture::WideString, size_t>
	{
		typedef Rapture::WideString _Kty;

		size_t operator()(const _Kty& _Keyval) const
		{
			return hashbytes(_Keyval.data(), _Keyval.length());
		}
	};
}

//---------------------------------------------------------------------------
#endif
