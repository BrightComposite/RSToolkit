//---------------------------------------------------------------------------

#pragma once

#ifndef CACHED_H
#define CACHED_H

//---------------------------------------------------------------------------

#include <container/Map.h>
#include <core/Exception.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<class K, class T>
	class cached
	{
	public:
		typedef K Key;
		typedef T Value;
		
		template<class ... A>
		static auto & get(const K & key) {
			static Map <K, T> _cache;
			auto & h = _cache[key];
			
			if(h == nullptr) {
				h.init(key);
			}
			
			return h;
		}
	
	protected:
		cached() {}
		
		~cached() {}
	};
	
	template<class K, class T>
	class precached
	{
	public:
		typedef K Key;
		typedef T Value;
		
		static bool has(const K & key) {
			auto & _cache = cache();
			return _cache.find(key) != _cache.end();
		}
		
		static auto & get(const K & key) {
			auto & _cache = cache();
			auto & h = _cache[key];
			
			if(h == nullptr) {
				throw Exception("Can't retreive value with the key \"", key, "\" from cache");
			}
			
			return h;
		}
		
		static auto begin() {
			auto & _cache = cache();
			return _cache.cbegin();
		}
		
		static auto end() {
			auto & _cache = cache();
			return _cache.cend();
		}
	
	protected:
		precached(const K & key) {
			auto & _cache = cache();
			_cache.insert({key, handle<T>(static_cast<T *>(this))});
		}
		
		~precached() {}
		
		static auto & cache() {
			static Map <K, T> _cache;
			return _cache;
		}
	};
}

//---------------------------------------------------------------------------
#endif
