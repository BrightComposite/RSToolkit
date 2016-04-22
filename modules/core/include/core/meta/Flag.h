//---------------------------------------------------------------------------

#ifndef META_FLAG_H
#define META_FLAG_H

//---------------------------------------------------------------------------

#include <type_traits>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class F, class S>
	inline auto check_flag(F flag, S set) -> decltype((flag & set) == flag)
	{
		return (flag & set) == flag;
	}

	template<class F, class S, typename = decltype(declval<S>() | declval<F>())>
	inline auto set_flag(F flag, S & set)
	{
		set = static_cast<S>(set | flag);
	}

	template<class F, class S, typename = decltype(declval<S>() & ~declval<F>())>
	inline auto clear_flag(F flag, S & set)
	{
		set = static_cast<S>(set & ~flag);
	}

	template<class F, class S, typename = decltype(declval<S>() | declval<F>()), typename = decltype(declval<S>() & ~declval<F>())>
	inline auto change_flag(F flag, S & set, bool state)
	{
		set = state ? static_cast<S>(set | flag) : static_cast<S>(set & ~flag);
	}

	template<class F, class S>
	inline auto has_some_flags(F flags, S set) -> decltype((flags & set) != 0)
	{
		return (flags & set) != 0;
	}

#ifndef _MSC_VER
	namespace std
	{
		template<class T>
		using underlying_type_t = typename underlying_type<T>::type;
	}
#endif // _MSC_VER

	using std::underlying_type;
	using std::underlying_type_t;

#define adapt_enum_flags(Enum)																		\
	inline underlying_type_t<Enum> operator & (Enum a, Enum b)										\
	{																								\
		return static_cast<underlying_type_t<Enum>>(a) & static_cast<underlying_type_t<Enum>>(b);	\
	}																								\
																									\
	inline underlying_type_t<Enum> operator & (underlying_type_t<Enum> a, Enum b)					\
	{																								\
		return a & static_cast<underlying_type_t<Enum>>(b);											\
	}																								\
																									\
	inline underlying_type_t<Enum> operator & (Enum a, underlying_type_t<Enum> b)					\
	{																								\
		return static_cast<underlying_type_t<Enum>>(a) & b;											\
	}																								\
																									\
	inline underlying_type_t<Enum> operator | (Enum a, Enum b)										\
	{																								\
		return static_cast<underlying_type_t<Enum>>(a) | static_cast<underlying_type_t<Enum>>(b);	\
	}																								\
																									\
	inline underlying_type_t<Enum> operator | (underlying_type_t<Enum> a, Enum b)					\
	{																								\
		return a | static_cast<underlying_type_t<Enum>>(b);											\
	}																								\
																									\
	inline underlying_type_t<Enum> operator | (Enum a, underlying_type_t<Enum> b)					\
	{																								\
		return static_cast<underlying_type_t<Enum>>(a) | b;											\
	}																								\
																									\
	inline bool operator == (Enum a, underlying_type_t<Enum> b)										\
	{																								\
		return static_cast<underlying_type_t<Enum>>(a) == b;										\
	}																								\
																									\
	inline bool operator == (int a, Enum b)															\
	{																								\
		return a == static_cast<underlying_type_t<Enum>>(b);										\
	}																								\
																									\
	inline bool operator != (Enum a, underlying_type_t<Enum> b)										\
	{																								\
		return static_cast<underlying_type_t<Enum>>(a) != b;										\
	}																								\
																									\
	inline bool operator != (underlying_type_t<Enum> a, Enum b)										\
	{																								\
		return a != static_cast<underlying_type_t<Enum>>(b);										\
	}																								\
																									\
	inline underlying_type_t<Enum> operator ~ (Enum value)											\
	{																								\
		return ~static_cast<underlying_type_t<Enum>>(value);										\
	}														
}

//---------------------------------------------------------------------------
#endif