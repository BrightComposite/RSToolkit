//---------------------------------------------------------------------------

#ifndef CACHED_H
#define CACHED_H

//---------------------------------------------------------------------------

#include <core/container/Map.h>
#include <core/Exception.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	struct ProtectedCache {};

	template<class K, class T, class ... Attr>
	class Cached
	{
	public:
		typedef K Key;
		typedef T Value;

		template<class ... A>
		static auto & get(const K & key, A &&... args)
		{
			static Map<K, T> _cache;

			auto & h = _cache[key];

			if(h == nullptr)
				h.reinit(key, forward<A>(args)...);

			return h;
		}

	protected:
		Cached() {}
		virtual ~Cached() {}
	};

	template<class K, class T>
	class Cached<K, T, ProtectedCache>
	{
		typedef Cached<K, T, ProtectedCache> ThisType;

	public:
		typedef K Key;
		typedef T Value;

		template<class ... A>
		static auto & get(const K & key, A &&... args)
		{
			static Map<K, T, ThisType> _cache;

			auto & h = _cache[key];

			if(h == nullptr)
				h.reinit(key, forward<A>(args)...);

			return h;
		}

	protected:
		Cached() {}
		virtual ~Cached() {}
	};

	template<class K, class T, class ... Attr>
	class Precached
	{
	public:
		typedef K Key;
		typedef T Value;

		static bool has(const K & key)
		{
			auto & _cache = cache();
			return _cache[key] != nullptr;
		}

		static auto & get(const K & key)
		{
			auto & _cache = cache();
#ifdef _DEBUG
			auto & h = _cache[key];

			if(h == nullptr)
				throw Exception("Can't retreive value with the key \"", key, "\" from cache");

			return h;
#else
			return _cache[key];
#endif
		}

	protected:
		Precached(const K & key)
		{
			auto & _cache = cache();
			_cache.insert_or_assign(key, static_cast<T *>(this));
		}

		virtual ~Precached() {}

		static auto & cache()
		{
			static Map<K, T> _cache;
			return _cache;
		}
	};

	template<class K, class T>
	class Precached<K, T, ProtectedCache>
	{
		typedef Precached<K, T, ProtectedCache> ThisType;

	public:
		typedef K Key;
		typedef T Value;

		static bool has(const K & key)
		{
			auto & _cache = cache();
			return _cache[key] != nullptr;
		}

		static auto & get(const K & key)
		{
			auto & _cache = cache();
#ifdef _DEBUG
			auto & h = _cache[key];

			if(h == nullptr)
				throw Exception("Can't retreive value with the key \"", key, "\" from cache");

			return h;
#else
			return _cache[key];
#endif
		}

	protected:
		Precached(const K & key)
		{
			auto & _cache = cache();
			_cache.insert_or_assign(key, static_cast<T *>(this));
		}

		virtual ~Precached() {}

		static auto & cache()
		{
			static Map<K, T, ThisType> _cache;
			return _cache;
		}
	};

	/**
	 *	... is CachedType
	 */
#define public_for_cache(...) public_for_handle(macrowrap(typename __VA_ARGS__::Value), __VA_ARGS__) 
}

//---------------------------------------------------------------------------
#endif
