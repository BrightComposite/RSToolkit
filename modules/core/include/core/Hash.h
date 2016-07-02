//---------------------------------------------------------------------------

#pragma once

#ifndef HASH_H
#define HASH_H

//---------------------------------------------------------------------------

#include <meta/Meta.h>
#include <xstddef>
#include <string>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class, bool>
	class Hashed;
}

namespace std
{
	template<class T, bool isPod>
	struct hash<Rapture::Hashed<T, isPod>>;
}

namespace Rapture
{
	template<class T, useif <is_callable<std::hash<T>, const T &>::value> endif>
	size_t hash(const T & value);

	template<class T, useif <is_callable<std::hash<T>, const T &>::value> endif>
	size_t ptr_hash(const T * value);

	template<class T, skipif <is_callable<std::hash<T>, const T &>::value> endif>
	size_t ptr_hash(const T * value);

	template<class T, bool isPod = std::is_pod<T>::value>
	class Hashed : public T
	{
		size_t _hashValue;

	public:
		template<typename ... A, useif <can_construct<T, A...>::value> endif>
		Hashed(A &&... args) : T(forward<A>(args)...), _hashValue(std::hash<Hashed>()(static_cast<const T &>(*this))) {}

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

		template<class A, skipif <based_on<A, Hashed>::value> endif>
		Hashed & operator = (A && val)
		{
			T::operator = (forward<A>(val));
			_hashValue = Rapture::hash(static_cast<T &>(*this));

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
		Hashed() : _inner(), _hashValue(Rapture::hash(_inner)) {}
		Hashed(T inner) : _inner(inner), _hashValue(Rapture::hash(_inner)) {}
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
			_hashValue = Rapture::hash(_inner);

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

	typedef Hashed<void *> PointerId;

	template<class X, class Y>
	inline size_t mixhash(const X & first, const Y & second)
	{
		return hash(first) ^ hash(first);
	}

	template<class X, class Y>
	inline void mixhash(const X & first, const Y & second, size_t & result)
	{
		result = hash(first) ^ hash(first);
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

	template<class T, useif_t>
	size_t hash(const T & value)
	{
		return std::hash<T>()(value);
	}

	template<class T, useif_t>
	size_t ptr_hash(const T * value)
	{
		return value != nullptr ? std::hash<T>()(*value) : 0;
	}

	template<class T, skipif_t>
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
			static PointerId _id(StaticIdentifier::id);
			return _id;
		}
	};
}

#define use_class_hash(... /* Class */)								\
	struct hash<__VA_ARGS__> : unary_function<__VA_ARGS__, size_t>	\
	{																\
		size_t operator()(const __VA_ARGS__ & val) const noexcept	\
		{															\
			return val.hash();										\
		}															\
	}																\

#define use_enum_hash(... /* Enum */)								\
	struct hash<__VA_ARGS__> : unary_function<__VA_ARGS__, size_t>	\
	{																\
		size_t operator()(const __VA_ARGS__ & val) const noexcept	\
		{															\
			return std::underlying_type_t<__VA_ARGS__>(val);		\
		}															\
	}																\

#define use_enum_key(... /* Enum */)													\
	struct less<__VA_ARGS__> : binary_function<__VA_ARGS__, __VA_ARGS__, bool>			\
	{																					\
		bool operator() (const __VA_ARGS__ & a, const __VA_ARGS__ & b) const noexcept	\
		{																				\
			return std::underlying_type_t<__VA_ARGS__>(a) <								\
				std::underlying_type_t<__VA_ARGS__>(b);									\
		}																				\
	}																					\

namespace std
{
	template<class T>
	use_class_hash(Rapture::Hashed<T>);

	template<>
	use_class_hash(Rapture::AutoIdentifier);
}

//---------------------------------------------------------------------------
#endif
