//---------------------------------------------------------------------------

#ifndef FLAG_H
#define FLAG_H

//---------------------------------------------------------------------------

#include <type_traits>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::enable_if;
	using std::is_same;

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

#define adapt_enum_flags(Enum)								\
	inline int operator & (Enum a, Enum b)					\
	{														\
		return static_cast<int>(a) & static_cast<int>(b);	\
	}														\
															\
	inline int operator & (int a, Enum b)					\
	{														\
		return a & static_cast<int>(b);						\
	}														\
															\
	inline int operator & (Enum a, int b)					\
	{														\
		return static_cast<int>(a) & b;						\
	}														\
															\
	inline int operator | (Enum a, Enum b)					\
	{														\
		return static_cast<int>(a) | static_cast<int>(b);	\
	}														\
															\
	inline int operator | (int a, Enum b)					\
	{														\
		return a | static_cast<int>(b);						\
	}														\
															\
	inline int operator | (Enum a, int b)					\
	{														\
		return static_cast<int>(a) | b;						\
	}														\
															\
	inline bool operator == (Enum a, int b)					\
	{														\
		return static_cast<int>(a) == b;					\
	}														\
															\
	inline bool operator == (int a, Enum b)					\
	{														\
		return a == static_cast<int>(b);					\
	}														\
															\
	inline bool operator != (Enum a, int b)					\
	{														\
		return static_cast<int>(a) != b;					\
	}														\
															\
	inline bool operator != (int a, Enum b)					\
	{														\
		return a != static_cast<int>(b);					\
	}														\
															\
	inline int operator ~ (Enum value)						\
	{														\
		return ~static_cast<int>(value);					\
	}														
}

//---------------------------------------------------------------------------
#endif