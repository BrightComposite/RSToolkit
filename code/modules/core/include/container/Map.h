//---------------------------------------------------------------------------

#pragma once

#ifndef MAP_H
#define MAP_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <container/Container.h>

#include <map>
#include <unordered_map>

//---------------------------------------------------------------------------

namespace std
{
	template<class K, class T, class Pred = asd::less<K>, class Alloc = allocator<pair<const K, T>>>
	struct dictionary : map<K, T, Pred, Alloc>
	{
	public:
		using map<K, T, Pred, Alloc>::map;
	};

	template<class K, class T, class Pred = asd::less<K>, class Alloc = allocator<pair<const K, T>>>
	struct multidictionary : multimap<K, T, Pred, Alloc>
	{
	public:
		using multimap<K, T, Pred, Alloc>::multimap;
	};
}

namespace asd
{
	using std::dictionary;
	using std::multidictionary;

	template<class K, class T, class Hasher = asd::hash<K>, class Eq = asd::equal_to<K>, class Alloc = std::allocator<pair<const K, T>>>
	using map = std::unordered_map<K, T, Hasher, Eq, Alloc>;

	template<class K, class T, class Hasher =  asd::hash<K>, class Eq = asd::equal_to<K>, class Alloc = std::allocator<pair<const K, T>>>
	using multimap = std::unordered_multimap<K, T, Hasher, Eq, Alloc>;

	template<class K, class T, class Pred = asd::less<K>, class Alloc = std::allocator<pair<const K, T>>>
	using dict = dictionary<K, T, Pred, Alloc>;

	template<class K, class T, class Pred = asd::less<K>, class Alloc = std::allocator<pair<const K, T>>>
	using multidict = multidictionary<K, T, Pred, Alloc>;

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

	template<class K, class V, class ... Owner>
	class Dictionary : public dictionary<K, Handle<V, Owner...>>
	{
	public:
		using dictionary<K, Handle<V, Owner...>>::dictionary;
	};

	template<class K, class V, class Pred, class ... Owner>
	class CustomDictionary : public dictionary<K, Handle<V, Owner...>, Pred>
	{
	public:
		using dictionary<K, Handle<V, Owner...>, Pred>::dictionary;
	};

	template<class I, class R = remove_reference_t<decltype(*declval<I>())>, skipif<is_const<R>::value>>
	auto keyof(I & it) -> decltype(it->first) &
	{
		return it->first;
	}

	template<class I, class R = remove_reference_t<decltype(*declval<I>())>, skipif<is_const<R>::value>>
	auto valueof(I & it) -> decltype(it->second) &
	{
		return it->second;
	}

	template<class I, class R = remove_reference_t<decltype(*declval<I>())>, useif<is_const<R>::value>>
	auto keyof(I & it) -> const decltype(it->first) &
	{
		return it->first;
	}

	template<class I, class R = remove_reference_t<decltype(*declval<I>())>, useif<is_const<R>::value>>
	auto valueof(I & it) -> const decltype(it->second) &
	{
		return it->second;
	}

	template<class I, class R = remove_reference_t<decltype(declval<I>())>, skipif<is_const<R>::value>>
	auto keyof(I & it) -> decltype(it.first) &
	{
		return it.first;
	}

	template<class I, class R = remove_reference_t<decltype(declval<I>())>, skipif<is_const<R>::value>>
	auto valueof(I & it) -> decltype(it.second) &
	{
		return it.second;
	}

	template<class I, class R = remove_reference_t<decltype(declval<I>())>, useif<is_const<R>::value>>
	auto keyof(I & it) -> const decltype(it.first) &
	{
		return it.first;
	}

	template<class I, class R = remove_reference_t<decltype(declval<I>())>, useif<is_const<R>::value>>
	auto valueof(I & it) -> const decltype(it.second) &
	{
		return it.second;
	}
}

//---------------------------------------------------------------------------
#endif
