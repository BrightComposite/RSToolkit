//---------------------------------------------------------------------------

#pragma once

#ifndef MESH_H
#define MESH_H

//---------------------------------------------------------------------------

#include <opengl/opengl.h>
#include <opengl/vertex_layout.h>

//---------------------------------------------------------------------------

namespace asd
{
	using vertex_data = array_list<float>;
	
	using vertex_indices = array_list<uint>;
	
	class instanced_mesh_data;
	
	namespace opengl
	{
		struct vertex_buffer
		{
			vertex_buffer(const vertex_layout & layout, const vertex_data & vd);
			vertex_buffer(vertex_buffer && buffer);
			~vertex_buffer();
			
			const vertex_layout & layout;
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
		
		class mesh_impl : public shareable<mesh_impl>
		{
		public:
			virtual ~mesh_impl() {}
			
			virtual void draw(context &) const = 0;
		};
		
		struct mesh : public gfx::primitive
		{
			morph_type(mesh);
			
			mesh(const handle<mesh_impl> & impl) : impl(impl) {}
			
			friend inline void draw_mesh(context & ctx, const mesh & m);
		
		private:
			handle<mesh_impl> impl;
		};
		
		inline void draw_mesh(context & ctx, const mesh & m) {
			if(m.impl) {
				m.impl->draw(ctx);
			}
		}
		
		template <mesh_type type>
		class generic_mesh {};
		
		class mesh_builder
		{
			template <mesh_type type>
			friend
			class generic_mesh;
		
		public:
			api(opengl)
			mesh_builder(context & graphics, const vertex_layout & layout, const vertex_data & data, vertex_topology topology = vertex_topology::triangles);
			api(opengl)
			~mesh_builder();
			
			api(opengl)
			mesh_builder & buffer(const vertex_layout & layout, const vertex_data & data);
			api(opengl)
			mesh_builder & indices(const vertex_indices &);
			api(opengl)
			mesh_builder & offset(uint offset);
			/*api(opengl)
			mesh_builder * make_instanced(const vertex_layout *);*/
			
			api(opengl)
			mesh build();
		
		protected:
			context & _context;
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
