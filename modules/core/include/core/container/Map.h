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
	using HandleMap = unordered_map<K, Handle<V, OwnerAttr...>>;

	template<class K, class V, class ... OwnerAttr>
	using Map = MapObject<K, Handle<V, OwnerAttr...>>;
}

//---------------------------------------------------------------------------
#endif
