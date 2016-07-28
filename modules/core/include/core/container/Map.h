//---------------------------------------------------------------------------

#pragma once

#ifndef MAP_H
#define MAP_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <core/container/Container.h>

#include <map>
#include <unordered_map>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::map;
	using std::unordered_map;
	using std::multimap;
	using std::unordered_multimap;

	template<class K, class V, class ... Owner>
	class UnorderedMap : public unordered_map<K, Handle<V, Owner...>>
	{
	public:
		using unordered_map<K, Handle<V, Owner...>>::unordered_map;
	};

	template<class K, class V, class Hasher, class ... Owner>
	class CustomUnorderedMap : public unordered_map<K, Handle<V, Owner...>, Hasher>
	{
	public:
		using unordered_map<K, Handle<V, Owner...>, Hasher>::unordered_map;
	};

	template<class K, class V, class ... Owner>
	class Map : public map<K, Handle<V, Owner...>>
	{
	public:
		using map<K, Handle<V, Owner...>>::map;
	};

	template<class K, class V, class Hasher, class ... Owner>
	class CustomMap : public map<K, Handle<V, Owner...>, Hasher>
	{
	public:
		using map<K, Handle<V, Owner...>, Hasher>::map;
	};

	template<class I>
	auto keyof(I & it) -> decltype(it->first) &
	{
		return it->first;
	}

	template<class I>
	auto valueof(I & it) -> decltype(it->second) &
	{
		return it->second;
	}

	template<class I>
	auto keyof(I & it) -> decltype(it.first) &
	{
		return it.first;
	}

	template<class I>
	auto valueof(I & it) -> decltype(it.second) &
	{
		return it.second;
	}
}

//---------------------------------------------------------------------------
#endif
