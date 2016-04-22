//---------------------------------------------------------------------------

#ifndef UNIFORMS_H
#define UNIFORMS_H

//---------------------------------------------------------------------------

#include <core/meta/Meta.h>
#include <math/Matrix.h>

#include "Shaders.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace Uniforms
	{
		template<class T, int index>
		class Component
		{
		public:
			Component(const T & c)
			{
				Initialize::init(component, c);
			}

			Component(T && c)
			{
				Initialize::init(component, forward<T>(c));
			}

			Component(const initializer_list<remove_extent_t<T>> & c)
			{
				Initialize::init(component, c);
			}

		private:
			T component;
		};

		template<class ... T>
		class Base : public Base<make_integer_sequence<size_t, sizeof...(T)>, T...>
		{
			typedef Base<make_integer_sequence<size_t, sizeof...(T)>, T...> Impl;

		public:
			using Impl::Base;
		};

		declare_flag_class(UniformFlag, is_uniform)

		template<int ... I, class ... T>
		class Base<integer_sequence<size_t, I...>, T...> : public UniformFlag, public Component<T, I>...
		{
		public:
			template<class ... A,
				useif(sizeof...(A) == sizeof...(T)),
				useif(is_true<can_construct(Component<T, I>, A)...>::value)
			>
			Base(const A & ... args) : Component<T, I>(static_cast<T>(args))... {}

			template<class ... A,
				useif(sizeof...(A) == sizeof...(T)),
				useif(is_true<can_construct(Component<T, I>, A)...>::value)
			>
			Base(A && ... args) : Component<T, I>(static_cast<adapt_t<T, A>>(forward<A>(args)))... {}
		};

#define declare_uniform(name, shader_index, shader_type, components)	\
		class alignas(16) name : public Base<pp_seq_enum(components)>	\
		{																\
		public:															\
			typedef Base<pp_seq_enum(components)> Base;					\
																		\
			using Base::Base;											\
																		\
			static const uint index = shader_index;						\
			static const ShaderType shader = ShaderType::shader_type;	\
		}

		declare_uniform(
			BrushOptions, 0, Pixel,
			(float4) // color
			(float) // line width
			);

		declare_uniform(
			Model, 1, Vertex,
			(FloatMatrix)
			);

		declare_uniform(
			View, 2, Vertex,
			(FloatMatrix)
			);

		declare_uniform(
			Projection, 3, Vertex,
			(FloatMatrix)
			);

		declare_uniform(
			Area, 4, Vertex,
			(float2) // position
			(float2) // size
			(float)  // depth
			);

		declare_uniform(
			Viewport, 5, Vertex,
			(float2) // width and height
			);

		struct Animation;
		struct PhongLighting;
	}

	using Uniforms::is_uniform;
}

//---------------------------------------------------------------------------
#endif
