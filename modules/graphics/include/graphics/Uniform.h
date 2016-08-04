//---------------------------------------------------------------------------

#pragma once

#ifndef UNIFORMS_H
#define UNIFORMS_H

//---------------------------------------------------------------------------

#include <meta/Meta.h>
#include <math/Transform.h>

#include <core/addition/Contents.h>
#include <core/Morpher.h>

#include "Shaders.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics3D;
	class UniformAdapter;

	struct UniformData : Shared
	{
		api(graphics) void apply() const;

		uint buffer;
		size_t offset;
		data<void> data;
		UniformAdapter * adapter;
	};

	class UniformAdapter : public Shared
	{
	public:
		UniformAdapter(int index, uint size) : _index(index), _size(size) {}

		template<class U, class ... A, useif<can_construct_contents<U, A...>::value>>
		void update(A &&... args)
		{
			update(Contents<U>(forward<A>(args)...).pointer());
		}

		template<class U>
		void update(const Contents<U> & contents)
		{
			update(contents.pointer());
		}

		virtual void update(const void * data) = 0;
		virtual void append(UniformData & data) = 0;
		virtual void bind(const UniformData & data) = 0;

	protected:
		int _index;
		uint _size;
	};

	struct Uniform
	{
		morph_base(Uniform);
		deny_copy(Uniform);
	};

	create_morph_pool(graphics, Uniform);

	template<class T>
	using is_uniform = is_base_of<Uniform, T>;

	using UniformSet = UnorderedMap<int, UniformAdapter>;

#define uniform_class(U, shader_index, shader_type, components)			\
	namespace Uniforms													\
	{																	\
		struct U;														\
	}																	\
																		\
	aligned_contents(Uniforms::U, alignas(16), components)				\
																		\
	namespace Uniforms													\
	{																	\
		struct U : Uniform												\
		{																\
			static const uint index = shader_index;						\
			static const ShaderType shader = ShaderType::shader_type;	\
																		\
			static const char * name()									\
			{															\
				return #U;												\
			}															\
		};																\
	}																	\
																		\
	create_morph_type(graphics, Uniforms::U)

	uniform_class
	(
		BrushOptions, 0, Pixel,
		(float4, color)
		(float, lineWidth)
	);

	uniform_class
	(
		Model, 1, Vertex,
		(floatm, transform)
	);

	uniform_class
	(
		View, 2, Vertex,
		(floatm, transform)
	);

	uniform_class
	(
		Projection, 3, Vertex,
		(floatm, transform)
	);

	uniform_class
	(
		Area, 4, Vertex,
		(float2, pos)
		(float2, size)
		(float,  depth)
	);

	uniform_class
	(
		Viewport, 5, Vertex,
		(float2, size)
	);
}

//---------------------------------------------------------------------------
#endif
