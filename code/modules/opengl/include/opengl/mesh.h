//---------------------------------------------------------------------------

#pragma once

#ifndef OPENGL_MESH_H
#define OPENGL_MESH_H

//---------------------------------------------------------------------------

#include <opengl/opengl.h>
#include <opengl/vertex_layout.h>
#include <graphics3d/vertex_data.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		using gfx3d::vertex_data;
		using gfx3d::vertex_indices;

		class mesh_builder;
		class instanced_mesh_data;

		struct vertex_buffer
		{
			vertex_buffer(const vertex_layout & layout, const vertex_data & vd);
			vertex_buffer(vertex_buffer && buffer);
			~vertex_buffer();
			
			uint handle;
			uint verticesCount;
		};
		
		struct index_buffer
		{
			index_buffer(const vertex_indices & indices);
			index_buffer(index_buffer && buffer);
			~index_buffer();
			
			uint handle;
		};
		
		enum class vertex_topology
		{
			triangles,
			triangle_strip,
			lines,
			line_strip
		};
		
		enum class mesh_type
		{
			plain,
			indexed,
			instanced,
			instanced_indexed
		};
		
		struct mesh : public shareable<mesh>
		{
			mesh(const vertex_layout & layout) : _layout(layout) {}

			virtual ~mesh() {
				glDeleteVertexArrays(1, &id);
			}
			
			const vertex_layout & layout() const {
				return _layout;
			}

			virtual void draw(context &) const = 0;

			uint id = 0;
			const vertex_layout & _layout;
		};
		
		inline void draw_mesh(context & ctx, const mesh & m) {
			m.draw(ctx);
		}
		
		template <mesh_type type>
		struct generic_mesh {};

		template <>
		struct generic_mesh<mesh_type::plain> : public opengl::mesh
		{
			deny_copy(generic_mesh);

			generic_mesh(mesh_builder & builder);

			virtual ~generic_mesh() {}

			virtual void draw(::asd::opengl::context &) const override;

			uint attrCount = 0;
			uint topology;
			uint offset;
			uint verticesCount;
		};

		template <>
		struct generic_mesh<mesh_type::indexed> : public opengl::mesh
		{
			deny_copy(generic_mesh);

			generic_mesh(mesh_builder & builder);

			virtual ~generic_mesh() {}

			virtual void draw(opengl::context &) const override;

			uint attrCount = 0;
			uint topology;
			uint offset;
			uint verticesCount;
			uint indicesCount;
		};

		class mesh_builder
		{
			template <mesh_type type>
			friend struct generic_mesh;
		
		public:
			api(opengl)
			mesh_builder(context & graphics, const vertex_layout & layout, const vertex_data & data, vertex_topology topology = vertex_topology::triangles);
			api(opengl)
			~mesh_builder();
			
			api(opengl)
			mesh_builder & buffer(const vertex_data & data);
			api(opengl)
			mesh_builder & indices(const vertex_indices &);
			api(opengl)
			mesh_builder & offset(uint offset);
//			api(opengl)
//			mesh_builder * make_instanced(const vertex_layout *);
			
			api(opengl)
			handle<mesh> build();
		
			static inline handle<mesh> build(context & graphics, const vertex_layout & layout, const vertex_data & data, vertex_topology topology = vertex_topology::triangles) {
				return mesh_builder(graphics, layout, data, topology).build();
			}

		protected:
			context & _context;
			const vertex_layout & _layout;
			uint _topology = 0;
			uint _offset = 0;
			uint _verticesCount = 0;
			uint _indicesCount = 0;
			array_list<vertex_buffer> _buffers;
			unique<index_buffer> _ibuffer;
			const vertex_layout * _instancedLayout = nullptr;
			mesh_type _type = mesh_type::plain;
			// unique<instanced_mesh_data> _instancedData = nullptr;
		};
	}
}

//---------------------------------------------------------------------------
#endif
