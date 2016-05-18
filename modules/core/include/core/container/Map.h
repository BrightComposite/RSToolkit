//---------------------------------------------------------------------------

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

	template<class K, class V, class ... OwnerAttr>
	class HashMap : public unordered_map<K, Handle<V, OwnerAttr...>>
	{
	public:
		using unordered_map<K, Handle<V, OwnerAttr...>>::unordered_map;
	};

	template<class K, class V, class Hasher, class ... OwnerAttr>
	class CustomHashMap : public unordered_map<K, Handle<V, OwnerAttr...>, Hasher>
	{
	public:
		using unordered_map<K, Handle<V, OwnerAttr...>, Hasher>::unordered_map;
	};

	template<class K, class V, class ... OwnerAttr>
	class Map : public map<K, Handle<V, OwnerAttr...>>
	{
	public:
		using map<K, Handle<V, OwnerAttr...>>::map;
	};

	template<class K, class V, class Hasher, class ... OwnerAttr>
	class CustomMap : public map<K, Handle<V, OwnerAttr...>, Hasher>
	{
	public:
		using map<K, Handle<V, OwnerAttr...>, Hasher>::map;
	};
}

//---------------------------------------------------------------------------
#endif
