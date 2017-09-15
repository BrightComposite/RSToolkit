//---------------------------------------------------------------------------

#pragma once

#ifndef ANY_LIST_H
#define ANY_LIST_H

//---------------------------------------------------------------------------

#include <algorithm>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/assert.hpp>
#include <boost/type_index.hpp>
#include <boost/type_index/ctti_type_index.hpp>

#include <meta/useif.h>

//---------------------------------------------------------------------------

#ifdef _MSC_VER
#define not !
#endif

namespace asd
{
	namespace management
	{
		template <typename T, useif<std::is_copy_constructible<T>::value>>
		auto copy(const void * source, void * destination) {
			new(destination) T(*static_cast<const T *>(source));
		}
		
		template <typename T>
		auto copy(const void *, void *) -> std::enable_if_t<not std::is_copy_constructible<T>::value> {
			auto type_name = boost::typeindex::ctti_type_index::type_id<T>().pretty_name();
			throw std::runtime_error(u8"Объект типа " + type_name + u8" некопируем");
		}
		
		template <typename T>
		void move(void * source, void * destination) {
			new(destination) T(std::move(*static_cast<T *>(source)));
		}
		
		template <typename T>
		void destroy(void * object) {
			static_cast<T *>(object)->~T();
		}
		
		enum struct operation_t
		{
			copy,
			move,
			destroy,
			size,
			type
		};
		
		using manager_t = void (*)(operation_t, const void *, void *);
		
		template <typename T>
		void manage(operation_t todo, const void * source, void * destination) {
			switch(todo) {
				case operation_t::copy: {
					copy<T>(source, destination);
					break;
				}
				case operation_t::move: {
					move<T>(const_cast<void *>(source), destination);
					break;
				}
				case operation_t::destroy: {
					assert(source == nullptr);
					destroy<T>(destination);
					break;
				}
				case operation_t::size: {
					assert(source == nullptr);
					*static_cast<std::size_t *>(destination) = sizeof(T);
					break;
				}
				case operation_t::type: {
					assert(source == nullptr);
					*static_cast<const std::type_info **>(destination) = std::addressof(typeid(T));
					break;
				}
			}
		}
		
		struct object_info_t
		{
			std::size_t offset;
			manager_t manage;
		};
		
		template <typename T>
		object_info_t make_object_info(std::size_t offset) {
			return object_info_t{offset, &manage<T>};
		}
		
		inline void destroy(const object_info_t & object, std::int8_t * data) {
			object.manage(operation_t::destroy, nullptr, data + object.offset);
		}
		
		template <typename InputIterator>
		void destroy(InputIterator first, InputIterator last, std::int8_t * data) {
			std::for_each(first, last,
				[data](const auto & object) {
					destroy(object, data);
				});
		}
		
		template <typename ForwardIterator>
		void move(ForwardIterator first, ForwardIterator last, std::int8_t * source, std::int8_t * destination) {
			for(auto current = first; current != last; ++current) {
				try {
					current->manage(operation_t::move,
						source + current->offset, destination + current->offset);
				}
				catch(...) {
					destroy(first, current, destination);
					throw;
				}
			}
			destroy(first, last, source);
		}
		
		template <typename ForwardIterator>
		void copy(ForwardIterator first, ForwardIterator last, const std::int8_t * source, std::int8_t * destination) {
			for(auto current = first; current != last; ++current) {
				try {
					current->manage(operation_t::copy,
						source + current->offset, destination + current->offset);
				}
				catch(...) {
					destroy(first, current, destination);
					throw;
				}
			}
		}
		
		inline std::size_t size_of(const object_info_t & object) {
			std::size_t size;
			object.manage(operation_t::size, nullptr, std::addressof(size));
			
			return size;
		}
		
		inline const std::type_info & type(const object_info_t & object) {
			const std::type_info * type;
			object.manage(operation_t::type, nullptr, std::addressof(type));
			
			return *type;
		}
	}
	
	namespace detail
	{
		template <typename Integer>
		constexpr Integer ceil(Integer n, Integer f) {
			return ((n - Integer{1}) / f) * f + f;
		}
		
		template <std::size_t Total, typename ... Types>
		struct aligned_volume_impl;
		
		template <std::size_t Total, typename Head, typename ... Tail>
		struct aligned_volume_impl<Total, Head, Tail...>
			: aligned_volume_impl<ceil(Total, alignof(Head)) + sizeof(Head), Tail...>
		{
		};
		
		template <std::size_t Total>
		struct aligned_volume_impl<Total> : std::integral_constant<size_t, Total> {};
	}
	
	template <typename ... Args>
	struct aligned_volume : detail::aligned_volume_impl<0, Args...> {};
	
	class any_list
	{
	private:
		using object_info_container_type = std::vector<management::object_info_t>;
	
	public:
		using size_type = typename object_info_container_type::size_type;
		
		static inline void expand(std::initializer_list<int>) {}
	
	public:
		template <typename Head, typename ... Tail, useif<sizeof...(Tail) != 0 || not std::is_same<std::decay_t<Head>, any_list>::value>>
		explicit any_list(Head && head, Tail && ... tail):
			m_capacity(aligned_volume<Head, Tail...>::value),
			m_data(std::make_unique<std::int8_t[]>(m_capacity)) {
			try {
				push_back_no_realloc(std::forward<Head>(head));
				any_list::expand({push_back_no_realloc(std::forward<Tail>(tail))..., 0});
			}
			catch(...) {
				destroy_all();
				throw;
			}
		}
		
		any_list() :
			m_capacity(0),
			m_data{},
			m_objects{},
			m_volume(0) {
		}
		
		any_list(any_list && that) :
			any_list() {
			this->swap(that);
		}
		
		any_list & operator =(any_list && that) {
			clear();
			
			this->swap(that);
			return *this;
		}
		
		any_list(const any_list & that) :
			m_capacity(that.m_volume),
			m_data(std::make_unique<std::int8_t[]>(m_capacity)),
			m_objects(that.m_objects),
			m_volume(that.m_volume) {
			management::copy(m_objects.begin(), m_objects.end(), that.data(), this->data());
		}
		
		any_list & operator =(const any_list & that) {
			clear();
			
			reserve(that.m_volume);
			management::copy(that.m_objects.begin(), that.m_objects.end(), that.data(), this->data());
			
			static_assert(std::is_nothrow_move_assignable<object_info_container_type>::value, "");
			m_objects = that.m_objects;
			m_volume = that.m_volume;
			
			return *this;
		}
		
		~any_list() {
			destroy_all();
		}
		
		void swap(any_list & that) {
			using std::swap;
			
			swap(this->m_capacity, that.m_capacity);
			swap(this->m_data, that.m_data);
			swap(this->m_objects, that.m_objects);
			swap(this->m_volume, that.m_volume);
		}
		
		void reserve(std::size_t new_capacity) {
			if(new_capacity > m_capacity) {
				reallocate(std::max(new_capacity, m_capacity * CAPACITY_INCREASING_FACTOR));
			}
		}
		
		void shrink_to_fit() {
			if(m_capacity > m_volume) {
				reallocate(m_volume);
			}
		}
		
		template <typename T>
		void push_back(T && object) {
			auto creation_place = set_up_creation_place(object);
			accomodate(std::forward<T>(object), creation_place);
		}
		
		void pop_back() {
			management::destroy(m_objects.back(), data());
			m_objects.pop_back();
			
			if(not m_objects.empty()) {
				auto offset = m_objects.back().offset;
				auto size = management::size_of(m_objects.back());
				
				m_volume = offset + size;
			} else {
				m_volume = 0;
			}
		}
		
		void clear() {
			destroy_all();
			m_objects.clear();
			m_volume = 0;
		}
		
		template <typename T>
		const T & get(size_type index) const {
			assert(&management::manage<T> == m_objects[index].manage);
			return *static_cast<const T *>(static_cast<const void *>(data() + offset(index)));
		}
		
		template <typename T>
		T & get(size_type index) {
			assert(&management::manage<T> == m_objects[index].manage);
			return *static_cast<T *>(static_cast<void *>(data() + offset(index)));
		}
		
		size_type offset(size_type index) const {
			return m_objects[index].offset;
		}
		
		template <typename T>
		const T & get_by_offset(size_type offset) const {
			return *static_cast<const T *>(static_cast<const void *>(data() + offset));
		}
		
		template <typename T>
		T & get_by_offset(size_type offset) {
			return *static_cast<T *>(static_cast<void *>(data() + offset));
		}
		
		size_type size() const {
			return m_objects.size();
		}
		
		std::size_t volume() const {
			return m_volume;
		}
		
		std::size_t capacity() const {
			return m_capacity;
		}
		
		bool empty() const {
			return m_objects.empty();
		}
		
		const std::type_info & type(size_type index) const {
			return management::type(m_objects[index]);
		}
	
	private:
		void reallocate(std::size_t new_capacity) {
			assert(new_capacity >= m_volume);
			auto new_data = std::make_unique<std::int8_t[]>(new_capacity);
			m_capacity = new_capacity;
			
			management::move(m_objects.begin(), m_objects.end(), data(), new_data.get());
			std::swap(m_data, new_data);
		}
		
		std::int8_t * data() {
			return m_data.get();
		}
		
		const std::int8_t * data() const {
			return m_data.get();
		}
		
		template <typename T>
		std::int8_t * set_up_creation_place(const T & object) {
			if(auto proposed_creation_place = try_to_align(object)) {
				return proposed_creation_place;
			} else {
				reserve(volume() + sizeof(T) + alignof(T));
				return force_align(object);
			}
		}
		
		template <typename T>
		void push_back_no_realloc(T && object) {
			auto creation_place = force_align(object);
			accomodate(std::forward<T>(object), creation_place);
		}
		
		template <typename T>
		std::int8_t * try_to_align(const T &) {
			auto space_left = capacity() - volume();
			void * creation_place = data() + volume();
			auto aligned_place = std::align(alignof(T), sizeof(T), creation_place, space_left);
			return static_cast<std::int8_t *>(aligned_place);
		}
		
		template <typename T>
		std::int8_t * force_align(const T & object) {
			auto creation_place = try_to_align(object);
			BOOST_ASSERT(creation_place != nullptr);
			
			return creation_place;
		}
		
		template <typename T>
		void accomodate(T && object, std::int8_t * creation_place) {
			using raw_type = std::decay_t<T>;
			new(creation_place) raw_type(std::forward<T>(object));
			
			const auto new_offset = static_cast<std::size_t>(creation_place - data());
			m_objects.push_back(management::make_object_info<raw_type>(new_offset));
			m_volume = new_offset + sizeof(raw_type);
		}
		
		void destroy_all() {
			management::destroy(m_objects.begin(), m_objects.end(), data());
		}
		
		static const auto CAPACITY_INCREASING_FACTOR = std::size_t{2};
		
		static const auto CAPACITY_DECREASING_THRESHOLD = std::size_t{4};
		
		std::size_t m_capacity = 0;
		std::unique_ptr<std::int8_t[]> m_data;
		
		object_info_container_type m_objects;
		std::size_t m_volume = 0;
	};
	
	inline void swap(any_list & left, any_list & right) {
		left.swap(right);
	}
}

//---------------------------------------------------------------------------
#endif
