//---------------------------------------------------------------------------

#pragma once

#ifndef STRING_H
#define STRING_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <core/container/Data.h>
#include <core/container/List.h>

#include <stdarg.h>
#include <array>
#include <cctype>
#include <string>

#include <core/algorithm/lookup3.h>

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

	string  api(core) narrow(const wstring & wide);
	string  api(core) narrow(const wstring & wide, const locale & loc);

	api(core) wstring widen(const string & narrow);
	api(core) wstring widen(const string & narrow, const locale & loc);

	api(core) void print(String & target, const Object & obj);
	api(core) void print(WideString & target, const Object & obj);

	template<class T>
	struct can_str_print;
	template<class T>
	struct can_wstr_print;
	template<class ... T>
	struct can_string_assemble;
	template<class ... T>
	struct can_wstring_assemble;

	link_class(core, String, Class<Object>);
	link_class(core, WideString, Class<Object>);

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
		}

		String(const wchar_t * const s, size_t length) : string(narrow({s, length}))
		{
			setclass(String);
		}

		String(char value) : string(&value, 1)
		{
			setclass(String);
		}

		String(byte value) : String(static_cast<int>(value)) {}

		template<class T, class U = T, typename = decltype(std::to_string(declval<U>()))>
		String(T value) : String(std::to_string(value)) {}

		template<class T, typename = decltype(print(declval<String &>(), declval<T>()))>
		String(const T & obj) : String() { print(*this, obj); }

		String(const char * s1, size_t l1, const char * s2, size_t l2) : string(string(s1, l1) + string(s2, l2))
		{
			setclass(String);
		}

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

		template<class T, useif<can_construct<String, T>::value>>
		String & operator = (T && value)
		{
			return operator = (String(forward<T>(value)));
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

		template<class T, useif<
			can_construct<String, T>::value,
			not_same_type<String, T>::value,
			not_same_type<WideString, T>::value
			>
		>
		String & operator += (T && value)
		{
			return operator += (String(forward<T>(value)));
		}

		template<class T, useif<can_construct<String, T>::value>>
		String & operator << (T && value)
		{
			return operator += (forward<T>(value));
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
		auto operator + (T && value) const
		{
			return String(*this).operator += (forward<T>(value));
		}

		operator const char * () const
		{
			return this->data();
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

		template<class T, class ... A, useif<
			can_construct<String, T>::value,
			can_construct<String, A>::value...
			>
		>
		static inline String assemble(T && value, A &&... others)
		{
			return String(forward<T>(value)).add(forward<A>(others)...);
		}

		template<class T, class ... A, useif<
			can_construct<String, T>::value,
			can_construct<String, A>::value...
			>
		>
		String & add(T && value, A &&... others)
		{
			operator += (forward<T>(value));
			return add(forward<A>(others)...);
		}

		template<typename = Empty>
		String & add()
		{
			return *this;
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
			return this->empty() ? '\0' : this->data()[0];
		}

		wchar_t asWideChar()
		{
			return this->empty() ? '\0' : this->data()[0];
		}

		int asInt(int radix = 10)
		{
			return std::stoi(*this, nullptr, radix);
		}

		long asLong(int radix = 10)
		{
			return std::stol(*this, nullptr, radix);
		}

		ulong asUnsignedLong(int radix = 10)
		{
			return std::stoul(*this, nullptr, radix);
		}

		#ifdef ARCH_X64
		size_t asSize(int radix = 10)
		{
			return std::stoull(*this, nullptr, radix);
		}
		#else
		size_t asSize(int radix = 10)
		{
			return std::stoul(*this, nullptr, radix);
		}
		#endif

		float asFloat()
		{
			return std::stof(*this);
		}

		double asDouble()
		{
			return std::stod(*this);
		}

		bool isAlpha()
		{
			for(auto ptr = cbegin(); ptr < cend(); ++ptr)
			{
				if(!isalpha(*ptr))
					return false;
			}

			return true;
		}

		bool isAlnum()
		{
			for(auto ptr = cbegin(); ptr < cend(); ++ptr)
			{
				if(!std::isalnum(*ptr))
					return false;
			}

			return true;
		}

		bool isDigits()
		{
			for(auto ptr = cbegin(); ptr < cend(); ++ptr)
			{
				if(!std::isdigit(*ptr))
					return false;
			}

			return true;
		}

		bool isHexDigits()
		{
			const char * s = this->data();

			if(s[0] != '0' || s[1] != 'x')
				return false;

			for(const char * ptr = s + 2; *ptr != '\0'; ptr++)
			{
				if(!std::isxdigit(*ptr))
					return false;
			}

			return true;
		}

		bool isIntegral(int radix = 10)
		{
			size_t i;
			absorb() std::stoi(*this, &i, radix);

			return i == size();
		}

		bool isFloating()
		{
			size_t i;
			absorb() std::stof(*this, &i);

			return i == size();
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
		}

		WideString(const char * const s, size_t length) : wstring(widen({s, length}))
		{
			setclass(String);
		}

		WideString(wchar_t value) : wstring(&value, 1)
		{
			setclass(String);
		}

		WideString(byte value, int radix = 16) : WideString(static_cast<int>(value)) {}

		template<class T, typename =
			decltype(std::to_wstring(declval<T>()))
		>
		WideString(T value) : WideString(std::to_wstring(value)) {}

		template<class T, useif<
			can_wstr_print<T>::value
			>
		>
		WideString(const T & obj) : WideString()
		{
			print(*this, obj);
		}

		WideString(const wchar_t * s1, size_t l1, const wchar_t * s2, size_t l2) : wstring(wstring(s1, l1) + wstring(s2, l2))
		{
			setclass(String);
		}

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

		template<class T, useif<can_construct<WideString, T>::value>>
		WideString & operator = (const T & value)
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

		template<class T, useif<can_construct<WideString, T>::value>>
		WideString & operator += (const T & value)
		{
			return operator += (WideString(value));
		}

		template<class T, useif<can_construct<WideString, T>::value>>
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
			return this->data();
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

		template<class T, class ... A, useif<
			can_construct<WideString, T>::value,
			can_construct<WideString, A>::value...
			>
			endif>
		static inline WideString assemble(T && value, A &&... others)
		{
			return WideString(forward<T>(value)).add(forward<A>(others)...);
		}

		template<class T, class ... A, useif<
			can_construct<WideString, T>::value,
			can_construct<WideString, A>::value...
			>
			endif>
		WideString & add(T && value, A &&... others)
		{
			operator += (forward<T>(value));
			return add(forward<A>(others)...);
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
			return this->empty() ? '\0' : this->data()[0];
		}

		char asWideChar()
		{
			return this->empty() ? '\0' : this->data()[0];
		}

		int asInt(int radix = 10)
		{
			return std::stoi(*this, nullptr, radix);
		}

		long asLong(int radix = 10)
		{
			return std::stol(*this, nullptr, radix);
		}

		ulong asUnsignedLong(int radix = 10)
		{
			return std::stoul(*this, nullptr, radix);
		}

		#ifdef ARCH_X64
		size_t asSize(int radix = 10)
		{
			return std::stoull(*this, nullptr, radix);
		}
		#else
		size_t asSize(int radix = 10)
		{
			return std::stoul(*this, nullptr, radix);
		}
		#endif

		float asFloat()
		{
			return std::stof(*this);
		}

		double asDouble()
		{
			return std::stod(*this);
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
			const wchar_t * s = this->data();

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
			const wchar_t * ptr = this->data();
			wchar_t * end;
			absorb() wcstol(ptr, &end, radix);

			return *end == '\0';
		}

		bool isFloating()
		{
			const wchar_t * ptr = this->data();
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

	template<class T>
	struct printer {};

	template<class T, useif<can_str_print<T>::value>>
	inline void print(String & target, const Handle<T> & object)
	{
		target << *object;
	}

	template<class T, selectif(0)<can_wstr_print<T>::value>>
	inline void print(WideString & target, const Handle<T> & object)
	{
		target << *object;
	}

	template<class T, selectif(1)<can_str_print<T>::value, !can_wstr_print<T>::value>>
	inline void print(WideString & target, const Handle<T> & object)
	{
		target << *object;
	}

	template<class T, useif<can_str_print<T>::value>>
	inline String print(const T & object)
	{
		String target;
		print(target, object);

		return target;
	}

	template<class T, selectif(0)<can_wstr_print<T>::value>>
	inline WideString wprint(const T & object)
	{
		WideString target;
		print(target, object);

		return target;
	}

	template<class T, selectif(1)<can_str_print<T>::value, !can_wstr_print<T>::value>>
	inline WideString wprint(const T & object)
	{
		return widen(print(object));
	}

	typedef list<string> StringList;
	typedef list<wstring> WideStringList;

	api(core) StringList split(const string & text, const char * sep = " \t");
	api(core) StringList splitOnLines(const string & text, size_t lineLength, bool separateWords = true);

	api(core) WideStringList split(const wstring & text, const wchar_t * sep = L" \t");
	api(core) WideStringList splitOnLines(const wstring & text, size_t lineLength, bool separateWords = true);

	inline String operator + (const char * s, const String & string)
	{
		return {s, strlen(s), string.c_str(), string.length()};
	}

	inline WideString operator + (const wchar_t * s, const WideString & string)
	{
		return {s, wcslen(s), string.c_str(), string.length()};
	}

	function_checker(is_printable, print);

	template<class T, class StringType, bool = based_on<T, string>::value || based_on<T, wstring>::value>
	struct is_str_printable : is_printable<StringType &, const T &> {};

	template<class T, class StringType>
	struct is_str_printable<T, StringType, true> : false_type {};

	template<class T>
	struct can_str_print : is_str_printable<T, String> {};

	template<class T>
	struct can_wstr_print : is_str_printable<T, WideString> {};

	template<class ... T>
	struct can_string_assemble : is_true<can_construct<String, T>::value...> {};

	template<class ... T>
	struct can_wstring_assemble : is_true<can_construct<WideString, T>::value...> {};

	byte	api(core) toByte(const char * s);
	char	api(core) toChar(const char * s);
	wchar_t	api(core) toWideChar(const char * s);
	double	api(core) toDouble(const char * s);
	float	api(core) toFloat(const char * s);
	int		api(core) toInt(const char * s);
	long	api(core) toLong(const char * s);

	byte	api(core) toByte(const wchar_t * s);
	char	api(core) toChar(const wchar_t * s);
	wchar_t	api(core) toWideChar(const wchar_t * s);
	double	api(core) toDouble(const wchar_t * s);
	float	api(core) toFloat(const wchar_t * s);
	int		api(core) toInt(const wchar_t * s);
	long	api(core) toLong(const wchar_t * s);

	api(core) String operator "" _s(const char * s, size_t unitsCount);
	api(core) WideString operator "" _s(const wchar_t * s, size_t unitsCount);

	template<class Ch>
	struct GetStringType {};

	template<>
	struct GetStringType<char> : identity<String> {};

	template<>
	struct GetStringType<wchar_t> : identity<WideString> {};

	template<class Ch>
	using string_t = typename GetStringType<Ch>::type;

	template<class Ch, class T>
	struct can_cstr_print : is_str_printable<T, string_t<Ch>> {};

	template<class T>
	struct lookup3hash {};

	template<>
	struct lookup3hash<string> : std::unary_function<string, size_t>
	{
		typedef string _Kty;

		size_t operator () (const _Kty& _Keyval) const
		{
			return hashbytes(_Keyval.data(), _Keyval.length());
		}
	};

	template<>
	struct lookup3hash<wstring> : std::unary_function<wstring, size_t>
	{
		typedef wstring _Kty;

		size_t operator () (const _Kty& _Keyval) const
		{
			return hashbytes(_Keyval.data(), _Keyval.length());
		}
	};

	template<>
	struct lookup3hash<Rapture::String> : std::unary_function<Rapture::String, size_t>
	{
		typedef Rapture::String _Kty;

		size_t operator () (const _Kty& _Keyval) const
		{
			return hashbytes(_Keyval.data(), _Keyval.length());
		}
	};

	template<>
	struct lookup3hash<Rapture::WideString> : std::unary_function<Rapture::WideString, size_t>
	{
		typedef Rapture::WideString _Kty;

		size_t operator () (const _Kty& _Keyval) const
		{
			return hashbytes(_Keyval.data(), _Keyval.length());
		}
	};

	inline std::ostream & operator << (std::ostream & ostr, const String & str)
	{
		return ostr << static_cast<const string &>(str);
	}

	inline std::wostream & operator << (std::wostream & ostr, const WideString & str)
	{
		return ostr << static_cast<const wstring &>(str);
	}
}

//---------------------------------------------------------------------------
#endif
