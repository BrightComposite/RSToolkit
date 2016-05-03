//---------------------------------------------------------------------------

#ifndef MAP_H
#define MAP_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <core/container/Hash.h>

#include <map>
#include <unordered_map>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::map;
	using std::unordered_map;

	template<class K, class V, class ... OwnerAttr>
	class Map : public unordered_map<K, Handle<V, OwnerAttr...>>
	{
	public:
		using unordered_map<K, Handle<V, OwnerAttr...>>::unordered_map;
	};

	template<class K, class V, class ... OwnerAttr>
	class OrderedMap : public map<K, Handle<V, OwnerAttr...>>
	{
	public:
		using map<K, Handle<V, OwnerAttr...>>::map;
	};
}

//---------------------------------------------------------------------------
#endif
