//---------------------------------------------------------------------------

#ifndef STATIC_MAP_H
#define STATIC_MAP_H

//---------------------------------------------------------------------------

#include <core/meta/Meta.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<class K, class V>
	class StaticMap
	{
	public:
		typedef decay_t<V> Value;

		StaticMap() {}

		StaticMap(const Value & value) : _value(value) {}
		StaticMap(Value && value) : _value(forward<Value>(value)) {}

		StaticMap(const StaticMap & map) : _value(map._value) {}
		StaticMap(StaticMap && map) : _value(move(map._value)) {}

		void set(const Value & value)
		{
			_value = value;
		}

		void set(Value && value)
		{
			_value = forward<Value>(value);
		}

		const Value & get() const
		{
			return _value;
		}

		Value & get()
		{
			return _value;
		}

	private:
		Value _value;
	};

	template<typename V, typename KH, typename ... KT>
	class StaticMap<Tuple<KH, KT...>, V> : public StaticMap<Tuple<KT...>, V>, public StaticMap<KH, V>
	{
	public:
		typedef V Value;
		typedef StaticMap<KH, decay_t<V>> HeadType;
		typedef StaticMap<Tuple<KT ...>, V> Base;
		typedef expand_t<sizeof...(KT) + 1, V> Values;
		typedef expand_t<sizeof...(KT), V> ValuesBase;
		typedef Tuple<KH, KT...> Types;
		typedef Tuple<KT...> TypesBase;

		static const int size = sizeof...(KT) + 1;

		template<class VH, class ... VT>
		StaticMap(VH && head, VT && ... tail) : Base(forward<VT>(tail) ...), HeadType(forward<VH>(head)) {}

		StaticMap(const StaticMap & map) : HeadType(map.get<KH>()), Base(map) {}
		StaticMap(StaticMap && map) : HeadType(move(map.get<KH>())), Base(forward<Base>(map)) {}

		template <typename ... KL, typename ... KR, require(same_type(Tuple<KL..., KR...>, Types))>
		StaticMap(const StaticMap<Tuple<KH, KL ...>, V> & left, const StaticMap<Tuple<KR ...>, V> & right) : HeadType(left.get<KH>), Base(left, right) {}
		StaticMap(const StaticMap<Tuple<KH>, V> & left, const Base & right) : HeadType(left), Base(right) {}

		template <typename ... KL, typename ... KR, require(same_type(Tuple<KL..., KR...>, Types))>
		StaticMap(StaticMap<Tuple<KH, KL ...>, V> && left, StaticMap<Tuple<KR ...>, V> && right) : HeadType(move(left.get<KH>)), Base(forward<StaticMap<Tuple<KL ...>, V>>(left), forward<StaticMap<Tuple<KR ...>, V>>(right)) {}
		StaticMap(StaticMap<Tuple<KH>, V> && left, Base && right) : HeadType(forward<V>(left.get<KH>)), Base(forward<Base>(right)) {}

		template <typename A>
		Value & get()
		{
			return StaticMap<A, V>::get();
		}

		template <typename A>
		const Value & get() const
		{
			return StaticMap<A, V>::get();
		}

		template <typename A>
		Value & operator () ()
		{
			return StaticMap<A, V>::get();
		}

		template <typename A>
		const Value & operator () () const
		{
			return StaticMap<A, V>::get();
		}

		template <int N>
		Value & get()
		{
			return StaticMap<extract_t<N, KH, KT...>, V>::get();
		}

		template <int N>
		const Value & get() const
		{
			return StaticMap<extract_t<N, KH, KT...>, V>::get();
		}

		Values values() const
		{
			return make_tuple(StaticMap<KH, V>::get(), StaticMap<KT, V>::get() ...);
		}

		template <typename A>
		constexpr int find() const
		{
			return Finder<A, KH, KT ...>::result;
		}

		template <typename A, typename AI>
		void set(AI value)
		{
			StaticMap<A, V>::set(value);
		}

		void set(const Values & values)
		{
			HeadType::set(values.head);
			Base::set(static_cast<const ValuesBase &>(values));
		}

		void set(Values && values)
		{
			HeadType::set(move(values.head));
			Base::set(static_cast<ValuesBase &&>(values));
		}

		template <typename A>
		void clear()
		{
			StaticMap<A, V>::clear();
		}

		void clearAll()
		{
			clear();
			Base::clearAll();
		}
	};


	template <typename I>
	class StaticMap<Tuple<>, I>
	{
	public:
		typedef Tuple<> Items;
		typedef Tuple<> Types;

		StaticMap() {}

		StaticMap(const StaticMap &) {}
		StaticMap(StaticMap &&) {}
	};
}

//---------------------------------------------------------------------------
#endif
