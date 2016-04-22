//---------------------------------------------------------------------------

#ifndef MAP_H
#define MAP_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <core/memory/allocator/PoolAllocator.h>

#include <core/container/Hash.h>

#include <map>
#include <unordered_map>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::map;
	using std::unordered_map;

	template<class K, class V>
	class MapObject : public Object, public unordered_map<K, V>
	{
	public:
		using unordered_map<K, V>::unordered_map;
	};

	template<class K, class V, class ... OwnerAttr>
	class HandleMap : public unordered_map<K, Handle<V, OwnerAttr...>>
	{
	public:
		using unordered_map<K, Handle<V, OwnerAttr...>>::unordered_map;
	};

	template<class K, class V, class ... OwnerAttr>
	class Map : public Object, public unordered_map<K, Handle<V, OwnerAttr...>>
	{
	public:
		using unordered_map<K, Handle<V, OwnerAttr...>>::unordered_map;
	};
	
	template<class K, class V>
	class OrderedMapObject : public Object, public map<K, V>
	{
	public:
		using map<K, V>::map;
	};

	template<class K, class V, class ... OwnerAttr>
	class OrderedHandleMap : public map<K, Handle<V, OwnerAttr...>>
	{
	public:
		using map<K, Handle<V, OwnerAttr...>>::map;
	};

	template<class K, class V, class ... OwnerAttr>
	class OrderedMap : public Object, public map<K, Handle<V, OwnerAttr...>>
	{
	public:
		using map<K, Handle<V, OwnerAttr...>>::map;
	};
}

//---------------------------------------------------------------------------
#endif
