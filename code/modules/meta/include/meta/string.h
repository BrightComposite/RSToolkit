//---------------------------------------------------------------------------

#pragma once

#ifndef ASD_META_STRING_H
#define ASD_META_STRING_H

//---------------------------------------------------------------------------

#include <meta/types.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace meta
	{
		template<size_t N, template<size_t...> class F, size_t ... Indices>
		struct apply_range : apply_range<N - 1, F, N - 1, Indices...> {};

		template<template<size_t...> class F, size_t ... Indices>
		struct apply_range<0, F, Indices...> : F<Indices...> {};

		template<char ... Chars>
		struct string
		{
			static constexpr const char chars[sizeof...(Chars) + 1] = {Chars..., '\0'};
		};

		template<char ... Chars>
		constexpr const char string<Chars...>::chars[sizeof...(Chars) + 1];
		
		template<size_t I, char ... Ch>
		struct string_builder
		{
			template <size_t N>
			constexpr static auto parse(const char (& s)[N]) { 
				return string_builder<I - 1, s[I], Ch...>::parse(s); 
			}
		};

		template<char ... Ch>
		struct string_builder<0, Ch...>
		{
			template <size_t N>
			constexpr static auto parse(const char (& s)[N]) { 
				return string<s[I], Ch...>(); 
			}
		};

		template <size_t N>
		constexpr auto parse(const char (&s)[N]) { 
			return string_builder<N - 1>::parse(s); 
		}
	}
}

//---------------------------------------------------------------------------
#endif
