//---------------------------------------------------------------------------

#ifndef RAW_DATA_H
#define RAW_DATA_H

//---------------------------------------------------------------------------

#include <vector>

#include <core/memory/Memory.h>
#include <core/Handle.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::vector;

	template<class T>
	class RawData : public Shareable<RawData<T>>
	{
	public:
		RawData() : data(nullptr), size(0) {}
		RawData(const RawData & rd) : data(rd.data), size(rd.size) {}
		RawData(T *&& data, size_t size) : data(data), size(size) { data = nullptr; }
		RawData(vector<T> & v) : data(v.data()), size(v.size()) {}
		template<size_t N>
		RawData(T(&data)[N]) : data(data), size(N) {}

		virtual ~RawData() {}

		RawData & operator = (const RawData & rd)
		{
			data = rd.data;
			size = rd.size;

			return *this;
		}

		void set(T *&& data, size_t size)
		{
			this->data = data;
			this->size = size;

			data = nullptr;
		}

		T * data;
		size_t size;
	};

	template<class T>
	class RawData<const T> : public Shareable<RawData<const T>>
	{
	public:
		RawData() : data(nullptr), size(0) {}
		RawData(const RawData & rd) : data(rd.data), size(rd.size) {}
		RawData(const RawData<T> & rd) : data(rd.data), size(rd.size) {}
		RawData(const T * data, size_t size) : data(data), size(size) {}
		RawData(const vector<T> & v) : data(v.data()), size(v.size()) {}
		template<size_t N>
		RawData(const T(&data)[N]) : data(data), size(N) {}

		virtual ~RawData() {}

		RawData & operator = (const RawData & rd)
		{
			data = rd.data;
			size = rd.size;

			return *this;
		}

		const T * data;
		size_t size;
	};

	template<class T>
	class OwnedData : public RawData<T>
	{
		typedef RawData<T> Base;

	public:
		OwnedData() : Base() {}
		OwnedData(const RawData & rd) : Base(Memory<T>::copy(rd.data, rd.size), rd.size) {}
		OwnedData(const OwnedData & rd) : Base(Memory<T>::copy(rd.data, rd.size), rd.size) {}
		OwnedData(const T * data, size_t size) : Base(Memory<T>::copy(data, size), size) {}
		OwnedData(const vector<T> & v) : Base(Memory<T>::copy(v.data(), v.size()), v.size()) {}
		template<size_t N>
		OwnedData(const T(&data)[N]) : Base(Memory<T>::copy(data, N), N) {}

		OwnedData(T *&& data, size_t size) : Base(forward<T*>(data), size) {}

		OwnedData(OwnedData && rd) : Base(move(rd.data), rd.size)
		{
			rd.size = 0;
		}

		virtual ~OwnedData() { delete[] data; }

		OwnedData & operator = (const OwnedData & od)
		{
			delete[] data;
			data = Memory<T>::copy(od.data, od.size);
			size = od.size;

			return *this;
		}

		OwnedData & operator = (OwnedData && rd)
		{
			swap(data, rd.data);
			swap(size, rd.size);

			return *this;
		}

		auto alloc(size_t size)
		{
			delete[] this->data;
			this->data = Memory<T>::allocate(size);
			this->size = size;

			return this->data;
		}

		void set(const OwnedData<T> & od)
		{
			delete[] data;
			data = Memory<T>::copy(od.data, od.size);
			size = od.size;
		}

		void set(OwnedData<T> && od)
		{
			swap(data, od.data);
			swap(size, od.size);
		}

		void set(const T * data, size_t size)
		{
			delete[] this->data;
			this->data = Memory<T>::copy(data, size);
			this->size = size;
		}

		template<size_t N>
		void set(const T(& data)[N])
		{
			delete[] this->data;
			this->data = Memory<T>::copy(data, N);
			this->size = N;
		}

		void set(T *&& data, size_t size)
		{
			delete[] this->data;
			Base::set(forward<T*>(data), size);
		}

		void apply(const T * data)
		{
			Memory<T>::move(this->data, data, size);
		}

		T * send()
		{
			T * out = data;
			size = 0;
			data = nullptr;

			return out;
		}
	};

	template<class T>
	class OwnedData<const T> {};

	template<>
	class RawData<void> : public Shareable<RawData<void>>
	{
	public:
		RawData() : data(nullptr), size(0) {}
		RawData(const RawData & rd) : data(rd.data), size(rd.size) {}
		RawData(void * data, size_t size) : data(data), size(size) {}
		template<typename T>
		RawData(T * data, size_t size) : data(data), size(size * sizeof(T)) {}
		template<typename T>
		RawData(vector<T> & v) : data(v.data()), size(v.size() * sizeof(T)) {}
		template<typename T, size_t N>
		RawData(T(&data)[N]) : data(data), size(N * sizeof(T)) {}

		virtual ~RawData() {}

		RawData & operator = (const RawData & rd)
		{
			data = rd.data;
			size = rd.size;

			return *this;
		}

		void set(void *&& data, size_t size)
		{
			this->data = data;
			this->size = size;

			data = nullptr;
		}

		void * data;
		size_t size;
	};

	template<>
	class RawData<const void> : public Shareable<RawData<const void>>
	{
	public:
		RawData() : data(nullptr), size(0) {}
		RawData(const RawData & rd) : data(rd.data), size(rd.size) {}
		RawData(const void * data, size_t size) : data(data), size(size) {}
		template<typename T>
		RawData(const T * data, size_t size) : data(data), size(size * sizeof(T)) {}
		template<typename T>
		RawData(const vector<T> & v) : data(v.data()), size(v.size() * sizeof(T)) {}
		template<typename T, size_t N>
		RawData(const T(&data)[N]) : data(data), size(N * sizeof(T)) {}

		virtual ~RawData() {}

		RawData & operator = (const RawData & rd)
		{
			data = rd.data;
			size = rd.size;

			return *this;
		}

		RawData & operator = (const RawData<void> & rd)
		{
			data = rd.data;
			size = rd.size;

			return *this;
		}

		const void * data;
		size_t size;
	};

	template<>
	class OwnedData<void> : public RawData<void>
	{
		typedef RawData<void> Base;

	public:
		OwnedData() : Base() {}
		template<class T>
		OwnedData(const RawData<T> & rd) : Base(Memory<T>::copy(rd.data, rd.size), rd.size) {}
		template<class T>
		OwnedData(const OwnedData<T> & rd) : Base(Memory<T>::copy(rd.data, rd.size), rd.size) {}
		template<class T>
		OwnedData(const T * data, size_t size) : Base(Memory<T>::copy(data, size), size) {}
		template<class T>
		OwnedData(const vector<T> & v) : Base(Memory<T>::copy(v.data(), v.size()), v.size()) {}
		template<class T, size_t N>
		OwnedData(const T(&data)[N]) : Base(Memory<T>::copy(data, N), N) {}

		OwnedData(OwnedData && rd) : Base(rd.data, rd.size)
		{
			rd.data = nullptr;
			rd.size = 0;
		}

		OwnedData(void *&& data, size_t size) : Base(data, size)
		{
			data = nullptr;
		}

		virtual ~OwnedData() { delete [] data; }

		OwnedData & operator = (const OwnedData & rd)
		{
			data = Memory<void>::copy(rd.data, rd.size);
			size = rd.size;

			return *this;
		}

		OwnedData & operator = (OwnedData && rd)
		{
			swap(data, rd.data);
			swap(size, rd.size);

			return *this;
		}

		auto alloc(size_t size)
		{
			delete[] this->data;
			this->data = Memory<void>::allocate(size);
			this->size = size;

			return this->data;
		}

		void set(const OwnedData<void> & od)
		{
			delete[] data;
			data = Memory<void>::copy(od.data, od.size);
			size = od.size;
		}

		void set(OwnedData<void> && od)
		{
			swap(data, od.data);
			swap(size, od.size);
		}

		void set(const void * data, size_t size)
		{
			delete[] this->data;
			this->data = Memory<void>::copy(data, size);
			this->size = size;
		}

		template<class T, size_t N>
		void set(const T(&data)[N])
		{
			delete[] this->data;
			this->data = Memory<T>::copy(data, N);
			this->size = N;
		}

		void set(void *&& data, size_t size)
		{
			delete[] this->data;
			Base::set(forward<void *>(data), size);
		}

		void apply(const void * data)
		{
			Memory<void>::move(this->data, data, size);
		}

		void * send()
		{
			void * out = data;
			size = 0;
			data = nullptr;

			return out;
		}
	};

	typedef RawData<byte> ByteData;
	typedef OwnedData<byte> OwnedByteData;
}

//---------------------------------------------------------------------------
#endif