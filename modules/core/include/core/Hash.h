//---------------------------------------------------------------------------

#pragma once

#ifndef HASH_H
#define HASH_H

//---------------------------------------------------------------------------

#include <meta/Types.h>
#include <cstddef>
#include <string>

//---------------------------------------------------------------------------

namespace asd
{
	template<class T>
	struct hash : std::hash<T> {};

	template<class T>
	struct less : std::less<T> {};

	template<class T>
	struct equal_to : std::equal_to<T> {};

	template<class T>
	struct no_hash
	{
		size_t operator()(const T &) const
		{
			static_assert(!is_pointer<T *>::value, "There is no hash function provided for this type!");
		}
	};

	template<class T>
	using has_hash = is_callable<hash<T>, const T &>;

	template<class T, useif<has_hash<T>::value>>
	size_t gethash(const T & value);

	template<class T, useif<has_hash<T>::value>>
	size_t ptr_hash(const T * value);

	template<class T, skipif<has_hash<T>::value>>
	size_t ptr_hash(const T * value);

	template<class T, bool isPod = std::is_pod<T>::value>
	class Hashed : public T
	{
		size_t _hashValue;

	public:
		template<typename ... A, useif<can_construct<T, A...>::value>>
		Hashed(A &&... args) : T(forward<A>(args)...), _hashValue(gethash(static_cast<const T &>(*this))) {}

		Hashed(const Hashed & val) : T(val), _hashValue(val._hashValue) {}
		Hashed(Hashed && val) : T(forward<Hashed>(val)), _hashValue(val._hashValue) {}

		Hashed & operator = (const Hashed & val)
		{
			T::operator = (val);
			_hashValue = val._hashValue;

			return *this;
		}

		Hashed & operator = (Hashed && val)
		{
			T::operator = (forward<Hashed>(val));
			_hashValue = val._hashValue;

			return *this;
		}

		template<class A, skipif<based_on<A, Hashed>::value>>
		Hashed & operator = (A && val)
		{
			T::operator = (forward<A>(val));
			_hashValue = gethash(static_cast<T &>(*this));

			return *this;
		}

		bool operator == (const Hashed & val) const
		{
			return _hashValue == val._hashValue && static_cast<const T &>(*this) == static_cast<const T &>(val);
		}

		size_t hash() const
		{
			return _hashValue;
		}
	};

	template<class T>
	class Hashed<T, true>
	{
		size_t _hashValue;
		T _inner;

	public:
		Hashed() : _inner(), _hashValue(gethash(_inner)) {}
		Hashed(T inner) : _inner(inner), _hashValue(gethash(_inner)) {}
		Hashed(const Hashed & val) : _inner(val._inner), _hashValue(val._hashValue) {}

		Hashed & operator = (const Hashed & val)
		{
			_inner = val._inner;
			_hashValue = val._hashValue;

			return *this;
		}

		Hashed & operator = (const T & val)
		{
			_inner = val;
			_hashValue = gethash(_inner);

			return *this;
		}

		bool operator == (const Hashed & val) const
		{
			return _hashValue == val._hashValue && _inner == val._inner;
		}

		operator T()
		{
			return _inner;
		}

		operator const T() const
		{
			return _inner;
		}

		size_t hash() const
		{
			return _hashValue;
		}
	};

	template<class T>
	class Hashed<Hashed<T>, false> : public Hashed<T>
	{
	public:
		using Hashed<T>::Hashed;

		template<class A>
		Hashed & operator = (A && val)
		{
			Hashed<T>::operator = (forward<A>(val));
			return *this;
		}
	};

	template<class T, bool isPod>
	struct hash<Hashed<T, isPod>>;

	typedef Hashed<void *> PointerId;

	template<class X, class Y>
	inline size_t mixhash(const X & first, const Y & second)
	{
		return gethash(first) ^ gethash(first);
	}

	template<class X, class Y>
	inline void mixhash(const X & first, const Y & second, size_t & result)
	{
		result = gethash(first) ^ gethash(first);
	}

	struct AutoIdentifier
	{
		bool operator == (const AutoIdentifier & val) const
		{
			return this == &val;
		}

		bool operator < (const AutoIdentifier & val) const
		{
			return this < &val;
		}

		bool operator > (const AutoIdentifier & val) const
		{
			return this > &val;
		}

		size_t hash() const
		{
			return reinterpret_cast<size_t>(this);
		}
	};

	template<class T, used_t>
	size_t gethash(const T & value)
	{
		return hash<T>()(value);
	}

	template<class T, used_t>
	size_t ptr_hash(const T * value)
	{
		return value != nullptr ? hash<T>()(*value) : 0;
	}

	template<class T, skipped_t>
	size_t ptr_hash(const T * value)
	{
		return reinterpret_cast<size_t>(value);
	}

	template<class T>
	class StaticIdentifier
	{
	public:
		static PointerId & id()
		{
			static PointerId _id(&StaticIdentifier::id);
			return _id;
		}
	};

#define use_class_hash(... /* Class */)									\
	struct hash<__VA_ARGS__> : std::unary_function<__VA_ARGS__, size_t>	\
	{																	\
		size_t operator()(const __VA_ARGS__ & val) const noexcept		\
		{																\
			return val.hash();											\
		}																\
	}																	\

#define use_enum_hash(... /* Enum */)									\
	struct hash<__VA_ARGS__> : std::unary_function<__VA_ARGS__, size_t>	\
	{																	\
		size_t operator()(const __VA_ARGS__ & val) const noexcept		\
		{																\
			return std::underlying_type_t<__VA_ARGS__>(val);			\
		}																\
	}																	\

#define use_enum_key(... /* Enum */)													\
	struct less<__VA_ARGS__> : std::binary_function<__VA_ARGS__, __VA_ARGS__, bool>		\
	{																					\
		bool operator() (const __VA_ARGS__ & a, const __VA_ARGS__ & b) const noexcept	\
		{																				\
			return std::underlying_type_t<__VA_ARGS__>(a) <								\
				std::underlying_type_t<__VA_ARGS__>(b);									\
		}																				\
	}

	template<class T>
	use_class_hash(Hashed<T>);

	template<>
	use_class_hash(AutoIdentifier);
}

//---------------------------------------------------------------------------
#endif
