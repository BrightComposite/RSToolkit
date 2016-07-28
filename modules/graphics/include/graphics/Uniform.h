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

	class UniformAdapter : public Shared
	{
	public:
		virtual void update(const void * data) = 0;
	};

	class Uniform : public Shared
	{
		morph_base(Uniform);
		deny_copy(Uniform);

	protected:
		Uniform(UniqueHandle<UniformAdapter> && adapter) : _adapter(forward<UniqueHandle<UniformAdapter>>(adapter)) {}

		UniqueHandle<UniformAdapter> _adapter;
	};

	create_morph_pool(graphics, Uniform);

	template<class T>
	using is_uniform = is_base_of<Uniform, T>;

	using UniformSet = Morpher<Uniform, Graphics3D>;

#define uniform_class(U, shader_index, shader_type, components)			\
	namespace Uniforms													\
	{																	\
		class U;														\
	}																	\
																		\
	aligned_contents(Uniforms::U, alignas(16), components)				\
																		\
	namespace Uniforms													\
	{																	\
		class U : public Uniform										\
		{																\
		public:															\
			static const uint index = shader_index;						\
			static const ShaderType shader = ShaderType::shader_type;	\
																		\
			static const char * name()									\
			{															\
				return #U;												\
			}															\
																		\
			template<class ... A, useif<								\
				can_construct_contents<U, A...>::value					\
				>														\
			>															\
			void set(A &&... args)										\
			{															\
				_adapter->update(Contents<U>(forward<A>(args)...));		\
			}															\
																		\
			void set(const Contents<U> & contents)						\
			{															\
				_adapter->update(contents);								\
			}															\
																		\
		protected:														\
			friend_owned_handle(U, Graphics3D);							\
																		\
			U(UniqueHandle<UniformAdapter> && a) :						\
				Uniform(forward<UniqueHandle<UniformAdapter>>(a)) {}	\
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
		(fmat, transform)
	);

	uniform_class
	(
		View, 2, Vertex,
		(fmat, transform)
	);

	uniform_class
	(
		Projection, 3, Vertex,
		(fmat, transform)
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
