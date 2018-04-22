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
            
            GLuint handle;
            GLsizei vertices_count;
        };
        
        struct index_buffer
        {
            index_buffer(const vertex_indices & indices);
            index_buffer(index_buffer && buffer);
            ~index_buffer();
            
            GLuint handle;
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

        adapt_enum_flags(mesh_type);
        
        struct mesh : public shareable<mesh>
        {
            mesh(const vertex_layout & layout, GLenum topology, GLint offset, GLsizei vertices_count) :
                layout(layout), topology(topology), offset(offset), vertices_count(vertices_count)
            {
                glGenVertexArrays(1, &handle);
            }

            virtual ~mesh() {
                if (handle > 0) {
                    glDeleteVertexArrays(1, &handle);
                }
            }
            

            virtual void draw(context &) const = 0;

            GLuint handle = 0;
            const vertex_layout & layout;

            GLuint attr_count = 0;
            GLenum topology;
            GLint offset;
            GLsizei vertices_count;
        };
        
        inline void draw_mesh(context & ctx, const mesh & m) {
            m.draw(ctx);
        }

        struct mesh_data
        {
            mesh_data(const vertex_layout & layout) : layout(layout) {}

            const vertex_layout & layout;
            GLenum topology = 0;
            GLint offset = 0;
            GLsizei vertices_count = 0;
            GLsizei indices_count = 0;
            array_list<vertex_buffer> buffers;
            boost::optional<index_buffer> index_buffer;
            const vertex_layout * instanced_layout = nullptr;
        };

        template <mesh_type type>
        struct generic_mesh {};

        template <>
        struct generic_mesh<mesh_type::plain> : public opengl::mesh
        {
            deny_copy(generic_mesh);

            generic_mesh(const mesh_data & data);

            virtual ~generic_mesh() {}

            virtual void draw(::asd::opengl::context &) const override;
        };

        template <>
        struct generic_mesh<mesh_type::indexed> : public opengl::mesh
        {
            deny_copy(generic_mesh);

            generic_mesh(const mesh_data & data);

            virtual ~generic_mesh() {}

            virtual void draw(opengl::context &) const override;
            uint indices_count;
        };

        class mesh_builder : public mesh_data
        {
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
            mesh_type _type = mesh_type::plain;
        };
    }
}

//---------------------------------------------------------------------------
#endif
