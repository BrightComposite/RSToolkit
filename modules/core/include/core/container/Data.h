//---------------------------------------------------------------------------

#pragma once

#ifndef DATA_H
#define DATA_H

//---------------------------------------------------------------------------

#include <core/Handle.h>
#include <core/container/ArrayList.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::array_list;

	template<class T>
	class data : public Shared
	{
	public:
		using iterator = T *;
		using reverse_iterator = T *;
		using const_iterator = const T *;
		using const_reverse_iterator = const T *;

		data() : ptr(nullptr), size(0) {}
		data(const data & rd) : ptr(rd.ptr), size(rd.size) {}
		data(T *&& ptr, size_t size) : ptr(ptr), size(size) { ptr = nullptr; }
		data(array_list<T> & v) : ptr(v.data()), size(v.size()) {}
		template<size_t N>
		data(T(&ptr)[N]) : ptr(ptr), size(N) {}

		virtual ~data() {}

		data & operator = (const data & rd)
		{
			ptr = rd.ptr;
			size = rd.size;

			return *this;
		}

		void set(T *&& ptr, size_t size)
		{
			this->ptr = ptr;
			this->size = size;

			ptr = nullptr;
		}

		T & operator [] (int index)
		{
			return ptr[index];
		}

		const T & operator [] (int index) const
		{
			return ptr[index];
		}

		iterator begin()
		{
			return ptr;
		}

		iterator end()
		{
			return ptr + size;
		}

		reverse_iterator rbegin()
		{
			return ptr + size;
		}

		reverse_iterator rend()
		{
			return ptr;
		}

		const_iterator cbegin() const
		{
			return ptr;
		}

		const_iterator cend() const
		{
			return ptr + size;
		}

		const_reverse_iterator crbegin() const
		{
			return ptr + size;
		}

		const_reverse_iterator crend() const
		{
			return ptr;
		}

		T * ptr;
		size_t size;
	};

	template<class T>
	class data<const T> : public Shared
	{
	public:
		using const_iterator = const T *;
		using const_reverse_iterator = const T *;

		data() : ptr(nullptr), size(0) {}
		data(const data & rd) : ptr(rd.ptr), size(rd.size) {}
		data(const data<T> & rd) : ptr(rd.ptr), size(rd.size) {}
		data(const T * ptr, size_t size) : ptr(ptr), size(size) {}
		data(const array_list<T> & v) : ptr(v.data()), size(v.size()) {}
		template<size_t N>
		data(const T(&ptr)[N]) : ptr(ptr), size(N) {}

		virtual ~data() {}

		data & operator = (const data & rd)
		{
			ptr = rd.ptr;
			size = rd.size;

			return *this;
		}

		const T & operator [] (int index) const
		{
			return ptr[index];
		}

		const_iterator cbegin() const
		{
			return ptr;
		}

		const_iterator cend() const
		{
			return ptr + size;
		}

		const_reverse_iterator crbegin() const
		{
			return ptr + size;
		}

		const_reverse_iterator crend() const
		{
			return ptr;
		}

		const T * ptr;
		size_t size;
	};

	template<class T>
	class owned_data : public data<T>
	{
		typedef data<T> Base;

	public:
		owned_data() : Base() {}
		owned_data(const Base & rd) : Base(Memory<T>::copy(rd.ptr, rd.size), rd.size) {}
		owned_data(const owned_data & rd) : Base(Memory<T>::copy(rd.ptr, rd.size), rd.size) {}
		owned_data(size_t size) : Base(Memory<T>::allocate(size), size) {}
		owned_data(const T * ptr, size_t size) : Base(Memory<T>::copy(ptr, size), size) {}
		owned_data(const array_list<T> & v) : Base(Memory<T>::copy(v.data(), v.size()), v.size()) {}
		template<size_t N>
		owned_data(const T(&ptr)[N]) : Base(Memory<T>::copy(ptr, N), N) {}

		owned_data(owned_data && rd) : Base(move(rd.ptr), rd.size)
		{
			rd.size = 0;
		}

		virtual ~owned_data() { Memory<void>::free(this->ptr); }

		owned_data & operator = (const owned_data & od)
		{
			Memory<void>::free(this->ptr);
			this->ptr = Memory<T>::copy(od.ptr, od.size);
			this->size = od.size;

			return *this;
		}

		owned_data & operator = (owned_data && rd)
		{
			swap(this->ptr, rd.ptr);
			swap(this->size, rd.size);

			return *this;
		}

		auto alloc(size_t size)
		{
			Memory<void>::free(this->ptr);
			this->ptr = Memory<T>::allocate(size);
			this->size = size;

			return this->ptr;
		}

		auto realloc(size_t size)
		{
			this->ptr = Memory<T>::reallocate(this->ptr, size);
			this->size = size;

			return this->ptr;
		}

		void set(const owned_data<T> & od)
		{
			Memory<void>::free(this->ptr);
			this->ptr = Memory<T>::copy(od.ptr, od.size);
			this->size = od.size;
		}

		void set(owned_data<T> && od)
		{
			swap(this->ptr, od.ptr);
			swap(this->size, od.size);
		}

		void set(const T * ptr, size_t size)
		{
			Memory<void>::free(this->ptr);
			this->ptr = Memory<T>::copy(ptr, size);
			this->size = size;
		}

		template<size_t N>
		void set(const T(& ptr)[N])
		{
			Memory<void>::free(this->ptr);
			this->ptr = Memory<T>::copy(ptr, N);
			this->size = N;
		}

		void apply(const T * ptr)
		{
			Memory<T>::move(this->ptr, ptr, this->size);
		}

		T * send()
		{
			T * out = this->ptr;
			this->size = 0;
			this->ptr = nullptr;

			return out;
		}
	};

	template<class T>
	class owned_data<const T> {};

	template<>
	class data<void> : public Shared
	{
	public:
		data() : ptr(nullptr), size(0) {}
		data(const data & rd) : ptr(rd.ptr), size(rd.size) {}
		data(void * ptr, size_t size) : ptr(ptr), size(size) {}
		template<typename T>
		data(T * ptr, size_t size) : ptr(ptr), size(size * sizeof(T)) {}
		template<typename T>
		data(array_list<T> & v) : ptr(v.data()), size(v.size() * sizeof(T)) {}
		template<typename T, size_t N>
		data(T(&ptr)[N]) : ptr(ptr), size(N * sizeof(T)) {}

		virtual ~data() {}

		data & operator = (const data & rd)
		{
			ptr = rd.ptr;
			size = rd.size;

			return *this;
		}

		void set(void *&& ptr, size_t size)
		{
			this->ptr = ptr;
			this->size = size;

			ptr = nullptr;
		}

		void * ptr;
		size_t size;
	};

	template<>
	class data<const void> : public Shared
	{
	public:
		data() : ptr(nullptr), size(0) {}
		data(const data & rd) : ptr(rd.ptr), size(rd.size) {}
		data(const void * const ptr, size_t size) : ptr(ptr), size(size) {}
		template<typename T>
		data(const T * const ptr, size_t size) : ptr(ptr), size(size * sizeof(T)) {}
		template<typename T>
		data(const array_list<T> & v) : ptr(v.data()), size(v.size() * sizeof(T)) {}
		template<typename T, size_t N>
		data(const T(&ptr)[N]) : ptr(ptr), size(N * sizeof(T)) {}

		virtual ~data() {}

		data & operator = (const data & rd)
		{
			ptr = rd.ptr;
			size = rd.size;

			return *this;
		}

		data & operator = (const data<void> & rd)
		{
			ptr = rd.ptr;
			size = rd.size;

			return *this;
		}

		const void * ptr;
		size_t size;
	};

	template<>
	class owned_data<void> : public data<void>
	{
		typedef data<void> Base;

	public:
		owned_data() : Base() {}
		template<class T>
		owned_data(const data<T> & rd) : Base(Memory<T>::copy(rd.ptr, rd.size), rd.size) {}
		template<class T>
		owned_data(const owned_data<T> & rd) : Base(Memory<T>::copy(rd.ptr, rd.size), rd.size) {}
		owned_data(size_t size) : Base(Memory<void>::allocate(size), size) {}
		template<class T, useif<not_same_type<T, void>::value>>
		owned_data(const T * const ptr, size_t size) : Base(Memory<T>::copy(ptr, size), size) {}
		owned_data(const void * const ptr, size_t size) : Base(Memory<void>::copy(ptr, size), size) {}
		template<class T>
		owned_data(const array_list<T> & v) : Base(Memory<T>::copy(v.data(), v.size()), v.size()) {}
		template<class T, size_t N>
		owned_data(const T(&ptr)[N]) : Base(Memory<T>::copy(ptr, N), N) {}

		owned_data(owned_data && rd) : Base(rd.ptr, rd.size)
		{
			rd.ptr = nullptr;
			rd.size = 0;
		}

		virtual ~owned_data() { Memory<void>::free(ptr); }

		owned_data & operator = (const owned_data & rd)
		{
			ptr = Memory<void>::copy(rd.ptr, rd.size);
			size = rd.size;

			return *this;
		}

		owned_data & operator = (owned_data && rd)
		{
			swap(ptr, rd.ptr);
			swap(size, rd.size);

			return *this;
		}

		auto alloc(size_t size)
		{
			Memory<void>::free(this->ptr);
			this->ptr = Memory<void>::allocate(size);
			this->size = size;

			return this->ptr;
		}

		auto realloc(size_t size)
		{
			this->ptr = Memory<void>::reallocate(this->ptr, size);
			this->size = size;

			return this->ptr;
		}

		void set(const owned_data<void> & od)
		{
			Memory<void>::free(ptr);
			ptr = Memory<void>::copy(od.ptr, od.size);
			size = od.size;
		}

		void set(owned_data<void> && od)
		{
			swap(ptr, od.ptr);
			swap(size, od.size);
		}

		void set(const void * ptr, size_t size)
		{
			Memory<void>::free(this->ptr);
			this->ptr = Memory<void>::copy(ptr, size);
			this->size = size;
		}

		template<class T, size_t N>
		void set(const T(&ptr)[N])
		{
			Memory<void>::free(this->ptr);
			this->ptr = Memory<T>::copy(ptr, N);
			this->size = N * sizeof(T);
		}

		void apply(const void * ptr)
		{
			Memory<void>::move(this->ptr, ptr, size);
		}

		void * send()
		{
			void * out = ptr;
			size = 0;
			ptr = nullptr;

			return out;
		}
	};

	typedef data<byte> ByteData;
	typedef owned_data<byte> OwnedByteData;
}

//---------------------------------------------------------------------------
#endif
