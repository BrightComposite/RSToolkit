//---------------------------------------------------------------------------

#ifndef UNIFORMS_H
#define UNIFORMS_H

//---------------------------------------------------------------------------

#include <meta/Meta.h>
#include <math/Transform.h>

#include <core/container/TypedMap.h>

#include "Shaders.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics3D;

	namespace Uniforms
	{
		struct Base {};
	}

	typebase_api(graphics, Uniforms::Base);

#define uniform_class(name, shader_index, shader_type, components)		\
	namespace Uniforms													\
	{																	\
		struct name : Base												\
		{																\
			static const uint index = shader_index;						\
			static const ShaderType shader = ShaderType::shader_type;	\
		};																\
	}																	\
																		\
	aligned_contents(Uniforms::name, 16, components)					\
	typeid_api(graphics, Uniforms::name, Uniforms::Base)

	uniform_class
	(
		BrushOptions, 0, Pixel,
		(float4, color)
		(float, lineWidth)
	);

	uniform_class
	(
		Model, 1, Vertex,
		(FloatTransform, transform)
	);

	uniform_class
	(
		View, 2, Vertex,
		(FloatTransform, transform)
	);

	uniform_class
	(
		Projection, 3, Vertex,
		(FloatTransform, transform)
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

	template<class T>
	using is_uniform = is_base_of<Uniforms::Base, T>;

	class UniformAdapter : public Shared
	{
		friend class Uniform;

	protected:
		virtual void update(const void * data) = 0;
	};

	class Uniform : public Shared
	{
	public:
		Uniform(const Uniform & uniform) = delete;
		Uniform & operator = (const Uniform & uniform) = delete;

		template<class T, class ... A, useif <can_construct_contents<T, A...>::value> endif>
		void set(A &&... args)
		{
			_adapter->update(Contents<T>(forward<A>(args)...));
		}

		template<class T>
		void set(const Contents<T> & contents)
		{
			_adapter->update(contents);
		}

	protected:
		friend_owned_handle(Uniform, Graphics3D);

		Uniform(UniqueHandle<UniformAdapter> && adapter) : _adapter(forward<UniqueHandle<UniformAdapter>>(adapter)) {}
		Uniform(Uniform && uniform) : _adapter(move(uniform._adapter)) {}

		UniqueHandle<UniformAdapter> _adapter;
	};

	using UniformMap = TypedMap<Uniforms::Base, Uniform, Graphics3D>;
}

//---------------------------------------------------------------------------
#endif
