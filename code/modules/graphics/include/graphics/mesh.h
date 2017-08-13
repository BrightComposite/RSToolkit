//---------------------------------------------------------------------------

#pragma once

#ifndef MESH_H
#define MESH_H

//---------------------------------------------------------------------------

#include <core/shareable.h>
#include <core/addition/contents.h>
#include <container/Data.h>
#include <core/memory/Pool.h>
#include "vertex_layout.h"

//---------------------------------------------------------------------------

namespace asd
{
	class Graphics3D;
	class mesh;

	class vertex_data : public array_list<float>
	{
	public:
		using array_list<float>::array_list;

		static api(graphics) const vertex_data quad2d;
		static api(graphics) const vertex_data texquad2d;
		static api(graphics) const vertex_data linequad2d;
		static api(graphics) const vertex_data quad3d;
		static api(graphics) const vertex_data texquad3d;
		static api(graphics) const vertex_data linequad3d;
		static api(graphics) const vertex_data cube;
		static api(graphics) const vertex_data texcube;
		static api(graphics) const vertex_data colorcube;
		static api(graphics) const vertex_data normalcube;
		static api(graphics) const vertex_data normaltexcube;
		static api(graphics) const vertex_data normalcolorcube;
	};

	class vertex_indices : public array_list<uint16_t>
	{
	public:
		using array_list<uint16_t>::array_list;

		static api(graphics) vertex_indices cube;
		static api(graphics) vertex_indices linecube;
	};

	enum class vertex_topology
	{
		triangles,
		triangle_strip,
		lines,
		line_strip
	};

	class mesh_buffer : public shareable
	{
	public:
		mesh_buffer() {}
		virtual ~mesh_buffer() {}

		virtual void apply() const = 0;
	};

	class vertex_buffer : public mesh_buffer
	{
	public:
		vertex_buffer(vertex_layout * layout, const vertex_data & vd) : layout(layout), verticesCount(static_cast<uint>(vd.size() / layout->units)) {}

		vertex_layout * layout;
		uint verticesCount;
	};

	struct instanced_mesh_data;

	struct instanced_mesh_data_chunk
	{
		inline instanced_mesh_data_chunk(instanced_mesh_data * pool, uint address);

		void fill(size_t offset) {}
		template<class U>
		void fill(size_t offset, const asd::contents<U> & contents);
		template<class H, class ... T, useif<sizeof...(T) != 0>>
		void fill(size_t offset, const asd::contents<H> & contents, const asd::contents<T> &... other);

		instanced_mesh_data * pool;
		uint address;
	};

	struct instanced_mesh_data
	{
		instanced_mesh_data(uint stride) : stride(stride), contents(stride * 16)
		{
			memory<byte>::fill(contents.ptr, 0, stride * 16);
		}

		~instanced_mesh_data() {}

		instanced_mesh_data_chunk * makeInstance()
		{
			auto * instance = instances.acquire();

			if(instance != nullptr)
				return instance;

			if(currentOffset + stride > contents.size)
				contents.realloc(contents.size * 2);

			instance = instances.create(this, currentOffset);
			currentOffset += stride;

			return instance;
		}

		void releaseInstance(instanced_mesh_data_chunk * instance)
		{
			if(instance->pool != this)
				return;

			memory<byte>::fill(contents.ptr + instance->address, 0, stride);
			instances.free(instance);
		}

		void clear()
		{
			memory<byte>::fill(contents.ptr, 0, instances.count() * stride);
			instances.freeAll();
		}
		
		uint stride;
		owned_data<byte> contents;
		uint currentOffset = 0;
		data_pool<instanced_mesh_data_chunk, 0x100> instances;
	};

	struct position2_attr {};
	struct position3_attr {};
	struct color_attr {};

	declare_contents(position2_attr, (float2, position));
	declare_contents(position3_attr, (float3, position));
	declare_contents(color_attr, (colorf, color));

	template<class ... U>
	struct mesh_instance
	{
		static void fill(instanced_mesh_data_chunk * chunk, const contents<U> &... contents)
		{
			chunk->fill(0, contents...);
		}
	};

	instanced_mesh_data_chunk::instanced_mesh_data_chunk(instanced_mesh_data * pool, uint address) : pool(pool), address(address) {}

	template<class U>
	void instanced_mesh_data_chunk::fill(size_t offset, const asd::contents<U> & contents)
	{
		memory<void>::move(pool->contents.ptr + address + offset, contents.pointer(), sizeof(asd::contents<U>));
	}

	template<class H, class ... T, used_t>
	void instanced_mesh_data_chunk::fill(size_t offset, const asd::contents<H> & contents, const asd::contents<T> &... other)
	{
		fill(offset, contents);
		fill(offset + sizeof(asd::contents<H>), other...);
	}

	enum class mesh_state : int
	{
		ready     = 1,
		indexed   = 2,
		instanced = 4
	};

	adapt_enum_flags(mesh_state);

	class mesh_builder : public shareable
	{
	public:
		virtual ~mesh_builder() {}

		virtual mesh_builder * buffer(const handle<vertex_buffer> & buffer) = 0;
		virtual mesh_builder * indices(const vertex_indices & indices) = 0;
		virtual mesh_builder * offset(uint offset) = 0;
		virtual mesh_builder * topology(vertex_topology topology) = 0;
		virtual mesh_builder * makeInstanced(vertex_layout * layout) = 0;

		virtual handle<mesh> ready() = 0;

	protected:
		uint _state = 0;
	};

	class mesh : public shareable
	{
	public:
		virtual ~mesh() {}

		virtual void draw() const = 0;

		virtual void updateInstanceData() const {}

		virtual instanced_mesh_data_chunk * instance()
		{
			return nullptr;
		}

		virtual void remove(instanced_mesh_data_chunk *) {}
		virtual void removeInstances() {}
	};
}

//---------------------------------------------------------------------------
#endif
