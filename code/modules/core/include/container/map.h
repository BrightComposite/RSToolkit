//---------------------------------------------------------------------------

#pragma once

#ifndef MAP_H
#define MAP_H

//---------------------------------------------------------------------------

#include <core/handle.h>
#include <container/container.h>

#include <map>
#include <unordered_map>
#include <hopscotch_map.h>

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
	
	#ifdef USE_STD_UNORDERED_MAP
		template<class Key, class Val, class Hash = std::hash<Key>, class EqualTo = std::equal_to<Key>, class Alloc = std::allocator<std::pair<Key, Val>>>
		using map = std::unordered_map<Key, Val, Hash, EqualTo, Alloc>;
	#else
		template<class Key, class Val, class Hash = std::hash<Key>, class EqualTo = std::equal_to<Key>, class Alloc = std::allocator<std::pair<Key, Val>>>
		using map = tsl::hopscotch_map<Key, Val, Hash, EqualTo, Alloc, 30, true>;
	#endif
	
	template<class K, class T, class Hasher =  asd::hash<K>, class Eq = asd::equal_to<K>, class Alloc = std::allocator<std::pair<const K, T>>>
	using multimap = std::unordered_multimap<K, T, Hasher, Eq, Alloc>;

	template<class K, class T, class Pred = asd::less<K>, class Alloc = std::allocator<std::pair<const K, T>>>
	using dict = dictionary<K, T, Pred, Alloc>;

	template<class K, class T, class Pred = asd::less<K>, class Alloc = std::allocator<std::pair<const K, T>>>
	using multidict = multidictionary<K, T, Pred, Alloc>;

	template<class I, class R = remove_reference_t<decltype(*declval<I>())>, skipif<is_const<R>::value>>
	auto keyof(I & it) -> std::add_lvalue_reference_t<decltype(it->first)>
	{
		return it->first;
	}

	template<class I, class R = remove_reference_t<decltype(*declval<I>())>, skipif<is_const<R>::value>>
	auto valueof(I & it) -> std::add_lvalue_reference_t<decltype(it->second)>
	{
		return it->second;
	}

	template<class I, class R = remove_reference_t<decltype(*declval<I>())>, useif<is_const<R>::value>>
	auto keyof(I & it) -> const std::add_lvalue_reference_t<decltype(it->first)>
	{
		return it->first;
	}

	template<class I, class R = remove_reference_t<decltype(*declval<I>())>, useif<is_const<R>::value>>
	auto valueof(I & it) -> const std::add_lvalue_reference_t<decltype(it->second)>
	{
		return it->second;
	}

	template<class I, class R = remove_reference_t<decltype(declval<I>())>, skipif<is_const<R>::value>>
	auto keyof(I & it) -> std::add_lvalue_reference_t<decltype(it.first)>
	{
		return it.first;
	}

	template<class I, class R = remove_reference_t<decltype(declval<I>())>, skipif<is_const<R>::value>>
	auto valueof(I & it) -> std::add_lvalue_reference_t<decltype(it.second)>
	{
		return it.second;
	}

	template<class I, class R = remove_reference_t<decltype(declval<I>())>, useif<is_const<R>::value>>
	auto keyof(I & it) -> const std::add_lvalue_reference_t<decltype(it.first)>
	{
		return it.first;
	}

	template<class I, class R = remove_reference_t<decltype(declval<I>())>, useif<is_const<R>::value>>
	auto valueof(I & it) -> const std::add_lvalue_reference_t<decltype(it.second)>
	{
		return it.second;
	}
}

//---------------------------------------------------------------------------
#endif
