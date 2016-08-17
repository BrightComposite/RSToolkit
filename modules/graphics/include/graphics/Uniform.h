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

	struct Uniform
	{
		morph_base(Uniform);
		deny_copy(Uniform);
	};

	create_morph_pool(graphics, Uniform);

	template<class T>
	using is_uniform = is_base_of<Uniform, T>;

	struct UniformData : Shared
	{
		api(graphics) void apply() const;

		uint buffer;
		size_t offset;
		data<void> data;
		UniformAdapter * adapter;
	};

	template<class U>
	struct UniformChunk : UniformData
	{
		static_assert(is_uniform<U>::value, "U must be an uniform");

		UniformChunk(Graphics3D * graphics)
		{
			graphics->uniformChunk(*this);
		}

		void fill(const Contents<U> & contents)
		{
			Memory<void>::move(data.ptr, contents.pointer(), data.size);
		}
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

		virtual void update() = 0;
		virtual void append(UniformData & data) = 0;
		virtual void bind(const UniformData & data) = 0;

	protected:
		int _index;
		uint _size;
	};

	using UniformSet = Map<int, UniformAdapter>;

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
		Color, 0, Pixel,
		(colorf, color)
	);

	uniform_class
	(
		BrushOptions, 0, Pixel,
		(colorf, color)
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

	uniform_class
	(
		PointLight, 6, Pixel,
		(float4, position)
		(colorf, color)
		(float4, lightparams) // attenuations (const, linear, exponential), ambient ratio
	);
}

//---------------------------------------------------------------------------
#endif
