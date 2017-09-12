//---------------------------------------------------------------------------

#pragma once

#ifndef STATIC_MAP_H
#define STATIC_MAP_H

//---------------------------------------------------------------------------

#include <meta/meta.h>

//---------------------------------------------------------------------------

namespace asd
{
	template <class K, class V>
	class static_map
	{
	public:
		using value_type = decay_t<V>;
		
		static_map() {}
		
		static_map(const value_type & value) : _value(value) {}
		
		static_map(value_type && value) : _value(forward<value_type>(value)) {}
		
		static_map(const static_map & map) : _value(map._value) {}
		
		static_map(static_map && map) : _value(move(map._value)) {}
		
		void set(const value_type & value) {
			_value = value;
		}
		
		void set(value_type && value) {
			_value = forward<value_type>(value);
		}
		
		const value_type & get() const {
			return _value;
		}
		
		value_type & get() {
			return _value;
		}
	
	private:
		value_type _value;
	};
	
	template <typename V, typename KH, typename ... KT>
	class static_map<tuple<KH, KT...>, V> : public static_map<tuple<KT...>, V>, public static_map<KH, V>
	{
	public:
		using value_type = V;
		using head_type = static_map<KH, decay_t<V>>;
		using base_type = static_map<tuple<KT ...>, V>;
		using values_types = expand_t<sizeof...(KT) + 1, V>;
		using base_values_types = expand_t<sizeof...(KT), V>;
		using keys_types = tuple<KH, KT...>;
		using base_keys_types = tuple<KT...>;
		
		static const int size = sizeof...(KT) + 1;
		
		template <class VH, class ... VT>
		static_map(VH && head, VT && ... tail) : base_type(forward<VT>(tail) ...), head_type(forward<VH>(head)) {}
		
		static_map(const static_map & map) : head_type(map.get<KH>()), base_type(map) {}
		
		static_map(static_map && map) : head_type(move(map.get<KH>())), base_type(forward<base_type>(map)) {}
		
		template <typename ... KL, typename ... KR, useif<same_type<tuple<KL..., KR...>, keys_types>::value>>
		static_map(const static_map<tuple<KH, KL ...>, V> & left, const static_map<tuple<KR ...>, V> & right) : head_type(left.get < KH > ), base_type(left, right) {}
		
		static_map(const static_map<tuple<KH>, V> & left, const base_type & right) : head_type(left), base_type(right) {}
		
		template <typename ... KL, typename ... KR, useif<same_type<tuple<KL..., KR...>, keys_types>::value>>
		static_map(static_map<tuple<KH, KL ...>, V> && left, static_map<tuple<KR ...>, V> && right) : head_type(move(left.get < KH > )), base_type(forward<static_map<tuple<KL ...>, V>>(left), forward<static_map<tuple<KR ...>, V>>(right)) {}
		
		static_map(static_map<tuple<KH>, V> && left, base_type && right) : head_type(forward<V>(left.get < KH > )), base_type(forward<base_type>(right)) {}
		
		template <typename A>
		value_type & get() {
			return static_map<A, V>::get();
		}
		
		template <typename A>
		const value_type & get() const {
			return static_map<A, V>::get();
		}
		
		template <typename A>
		value_type & operator ()() {
			return static_map<A, V>::get();
		}
		
		template <typename A>
		const value_type & operator ()() const {
			return static_map<A, V>::get();
		}
		
		template <int N>
		value_type & get() {
			return static_map<extract_t<N, KH, KT...>, V>::get();
		}
		
		template <int N>
		const value_type & get() const {
			return static_map<extract_t<N, KH, KT...>, V>::get();
		}
		
		values_types values() const {
			return make_tuple(static_map<KH, V>::get(), static_map<KT, V>::get() ...);
		}
		
		template <typename A>
		constexpr int find() const {
			return asd::find_t<A, KH, KT ...>::value;
		}
		
		template <typename A, typename AI>
		void set(AI value) {
			static_map<A, V>::set(value);
		}
		
		void set(const values_types & values) {
			head_type::set(std::get<0>(values));
			base_type::set(values);
		}
		
		void set(values_types && values) {
			head_type::set(move(std::get<0>(values)));
			base_type::set(values);
		}
		
		template <typename A>
		void clear() {
			static_map<A, V>::clear();
		}
		
		void clearAll() {
			clear();
			base_type::clearAll();
		}
	};
	
	
	template <typename I>
	class static_map<tuple<>, I>
	{
	public:
		typedef tuple<> Items;
		typedef tuple<> keys_types;
		
		static_map() {}
		
		static_map(const static_map &) {}
		
		static_map(static_map &&) {}
	};
}

//---------------------------------------------------------------------------
#endif
