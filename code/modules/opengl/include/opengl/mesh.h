//---------------------------------------------------------------------------

#pragma once

#ifndef MESH_H
#define MESH_H

//---------------------------------------------------------------------------

#include <opengl/opengl.h>

//---------------------------------------------------------------------------

namespace asd
{
	using vertex_data = array_list<float>;
	
	using vertex_indices = array_list<uint>;
	
	class instanced_mesh_data;

	struct vertex_layout_element
	{
		uint units;
	};

	struct vertex_layout
	{
		uint stride;
		uint units;
		array_list<vertex_layout_element> elements;
	};

	namespace vertex_layouts
	{
		template<class T, T ... E>
		struct sum {};

		template<class T, T Head, T ... Tail>
		struct sum<T, Head, Tail...>
		{
			static constexpr T value = Head + sum<T, Tail...>::value;
		};

		template<class T>
		struct sum<T>
		{
			static constexpr T value = 0;
		};

		template<class ... E>
		struct generator
		{
			static constexpr const char key[sizeof...(E) + 1] = {E::key..., '\0'};
			static const uint units = sum<uint, E::units...>::value;
		};

		template<class ... E>
		constexpr const char generator<E...>::key[sizeof...(E) + 1];

		struct position2
		{
			static const char key = 'p';
			static const uint units = 2;
		};

		struct position3
		{
			static const char key = 'p';
			static const uint units = 3;
		};

		struct color3
		{
			static const char key = 'c';
			static const uint units = 3;
		};

		struct color4
		{
			static const char key = 'c';
			static const uint units = 3;
		};
	}

	namespace opengl
	{
		struct vertex_buffer
		{
			vertex_buffer(const vertex_layout & layout, const vertex_data & vd);
			vertex_buffer(vertex_buffer && buffer);
			~vertex_buffer();

			uint handle;
			uint verticesCount;
			const vertex_layout & layout;
		};

		struct index_buffer
		{
			index_buffer(const vertex_indices & indices);
			index_buffer(index_buffer && buffer);
			~index_buffer();

			uint handle;
		};

		enum vertex_topology
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

		class mesh : public ::asd::gfx::primitive, public shareable
		{
		public:
			virtual void draw(::asd::opengl::context &) const = 0;
		};
	}

	create_morph_type(opengl, opengl::mesh);

	namespace opengl 
	{		
		inline void draw_mesh(::asd::opengl::context & ctx, const mesh & m) {
			m.draw(ctx);
		}
		
		template <mesh_type type>
		class generic_mesh {};
		
		class mesh_builder
		{
			template <mesh_type type>
			friend class generic_mesh;
		
		public:
			api(opengl)
			mesh_builder(context & graphics, vertex_topology topology);
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
			handle<mesh> build();
		
		protected:
			context & _context;
			uint _topology = 0;
			uint _offset = 0;
			uint _verticesCount = 0;
			uint _indicesCount = 0;
			array_list<vertex_buffer> _buffers;
			unique<index_buffer> _ibuffer;
			const vertex_layout * _instancedLayout;
			mesh_type _type;
			// unique<instanced_mesh_data> _instancedData = nullptr;
		};
		
	}
}

//---------------------------------------------------------------------------
#endif
