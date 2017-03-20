//---------------------------------------------------------------------------

#pragma once

#ifndef COLOR_H
#define COLOR_H

//---------------------------------------------------------------------------

#include <meta/Meta.h>
#include <math/Vector.h>

//---------------------------------------------------------------------------

namespace asd
{
	enum class ColorFormat
	{
		rgb,
		hsv
	};

	enum class ColorType
	{
		Byte,
		Float
	};

	template<ColorType datatype>
	struct ColorTraits {};

	template<ColorType From, ColorType To>
	struct ColorDataCast {};

	template<ColorFormat From, ColorFormat To>
	struct ColorFormatCast {};

	template<>
	struct ColorTraits<ColorType::Byte>
	{
		using type = byte;

		static inline const ByteVector & default_color()
		{
			static const ByteVector v {0xFF, 0xFF, 0xFF, 0xFF};
			return v;
		}

		static inline byte alpha()
		{
			return 0xFF;
		}
	};

	template<>
	struct ColorTraits<ColorType::Float>
	{
		using type = float;

		static inline const FloatVector & default_color()
		{
			return FloatVector::one;
		}

		static inline float alpha()
		{
			return 1.0f;
		}
	};

	template<ColorType T>
	using color_data_t = typename ColorTraits<T>::type;

	template<ColorType T>
	using ColorVector = Vector<color_data_t<T>>;

	template<ColorType datatype, ColorFormat format>
	struct ColorBase {};

	template<ColorType datatype>
	struct ColorBase<datatype, ColorFormat::rgb>
	{
		using T = color_data_t<datatype>;

		ColorBase() : data {} {}
		ColorBase(const ColorBase & color) : data {color.data} {}
		ColorBase(ColorBase && color) : data {move(color.data)} {}
		ColorBase(const Vector<T> & vec) : data {vec.v} {}
		ColorBase(const Vector<T> & vec, T alpha) : data {vec.x, vec.y, vec.z, alpha} {}
		ColorBase(const array<T, 4> & data) : data {data} {}
		ColorBase(array<T, 4> && data) : data {forward<array<T, 4>>(data)} {}
		ColorBase(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {}

		union
		{
			array<T, 4> data;
			T m[4];

			struct
			{
				T r, g, b, a;
			};
		};
	};

	template<ColorType datatype>
	struct ColorBase<datatype, ColorFormat::hsv>
	{
		using T = color_data_t<datatype>;

		ColorBase() : data {} {}
		ColorBase(const ColorBase & color) : data {color.data} {}
		ColorBase(ColorBase && color) : data {move(color.data)} {}
		ColorBase(const Vector<T> & vec) : data {vec.v} {}
		ColorBase(const Vector<T> & vec, T alpha) : data {vec.x, vec.y, vec.z, alpha} {}
		ColorBase(const array<T, 4> & data) : data {data} {}
		ColorBase(array<T, 4> && data) : data {forward<array<T, 4>>(data)} {}
		ColorBase(T h, T s, T v, T a) : h(h), s(s), v(v), a(a) {}

		union
		{
			array<T, 4> data;
			T m[4];

			struct
			{
				T h, s, v, a;
			};
		};
	};

	template<ColorType datatype, ColorFormat format>
	struct GenericColor : ColorBase<datatype, format>
	{
		using T = color_data_t<datatype>;
		using Base = ColorBase<datatype, format>;
		using Traits = ColorTraits<datatype>;

		member_cast(this->m, array_t<T, 4>);
		member_cast(this->data, array<T, 4>);

		GenericColor() : Base {Traits::default_color()} {}
		GenericColor(const GenericColor & color) : Base {color.data} {}

		template<class ... A, selectif(0)<sizeof...(A) == 3, std::is_convertible<A, T>::value...>>
		GenericColor(A &&... args) : Base {adapt<T, A>(forward<A>(args))..., Traits::alpha()} {}

		template<class ... A, selectif(1)<sizeof...(A) == 4, std::is_convertible<A, T>::value...>>
		GenericColor(A &&... args) : Base {adapt<T, A>(forward<A>(args))...} {}

		GenericColor(const T(&color)[4]) : Base {color} {}
		GenericColor(const T(&color)[3]) : Base {color[0], color[1], color[2], Traits::alpha()} {}
		GenericColor(const Vector<T> & color) : Base {color} {}
		GenericColor(const Vector<T> & color, T alpha) : Base {color, alpha} {}

		template<ColorType d, ColorFormat f, useif<d != datatype || f != format>>
		GenericColor(const GenericColor<d, f> & color)
		{
			Cast<GenericColor<d, f>, GenericColor>::cast(*this, color);
		}

		GenericColor & operator = (const GenericColor & color)
		{
			this->data = color.data;
			return *this;
		}

		bool operator == (const GenericColor & color) const
		{
			return this->data == color.data;
		}

		bool operator != (const GenericColor & color) const
		{
			return this->data != color.data;
		}

		AlignedVector<T> vector() const
		{
			return {this->m};
		}

		void vector(const Vector<T> & v)
		{
			this->data = v.v;
		}

		T & operator [] (int index)
		{
			return this->data[index];
		}

		const T & operator [] (int index) const
		{
			return this->data[index];
		}

		T & operator [] (size_t index)
		{
			return this->data[index];
		}

		const T & operator [] (size_t index) const
		{
			return this->data[index];
		}

		static api(graphics) const GenericColor transparent;
		static api(graphics) const GenericColor white;
		static api(graphics) const GenericColor black;
	};

	using colorf	= GenericColor<ColorType::Float, ColorFormat::rgb>;
	using colorb	= GenericColor<ColorType::Byte,  ColorFormat::rgb>;
	using floathsv	= GenericColor<ColorType::Float, ColorFormat::hsv>;

	using Color		= colorf;
	using rgb		= colorb;
	using hsv		= floathsv;

	template<class T>
	struct is_color : false_type {};

	template<ColorType d, ColorFormat f>
	struct is_color<GenericColor<d, f>> : true_type {};

	template<ColorType inType, ColorFormat inFormat, ColorType outType, ColorFormat outFormat>
	struct Cast<GenericColor<inType, inFormat>, GenericColor<outType, outFormat>>
	{
		template<ColorType In = inType, ColorType Out = outType, useif<In == Out>, useif<inFormat == outFormat>>
		static inline void cast(GenericColor<outType, outFormat> & out, const GenericColor<inType, inFormat> & in)
		{
			out = in;
		}

		template<ColorType In = inType, ColorType Out = outType, useif<In == Out>, skipif<inFormat == outFormat>>
		static inline void cast(GenericColor<outType, outFormat> & out, const GenericColor<inType, inFormat> & in)
		{
			ColorFormatCast<inFormat, outFormat>::cast(out, in);
		}

		template<ColorType In = inType, ColorType Out = outType, skipif<In == Out>, useif<inFormat == outFormat>>
		static inline void cast(GenericColor<outType, outFormat> & out, const GenericColor<inType, inFormat> & in)
		{
			ColorDataCast<inType, outType>::cast(out, in);
		}

		template<ColorType In = inType, ColorType Out = outType, skipif<In == Out>, skipif<inFormat == outFormat>>
		static inline void cast(GenericColor<outType, outFormat> & out, const GenericColor<inType, inFormat> & in)
		{
			GenericColor<outType, inFormat> temp;
			ColorDataCast<inType, outType>::cast(temp, in);
			ColorFormatCast<inFormat, outFormat>::cast(out, temp);
		}
	};

	template<ColorType datatype>
	struct ColorDataCast<datatype, datatype>
	{
		template<ColorFormat format>
		static inline void cast(GenericColor<datatype, format> & out, const GenericColor<datatype, format> & in)
		{
			out = in;
		}
	};

	template<>
	struct ColorDataCast<ColorType::Byte, ColorType::Float>
	{
		template<ColorFormat format>
		static inline void cast(GenericColor<ColorType::Float, format> & out, const GenericColor<ColorType::Byte, format> & in)
		{
			out.vector(FloatVector(in.vector()) / 255.0f);
		}
	};

	template<>
	struct ColorDataCast<ColorType::Float, ColorType::Byte>
	{
		template<ColorFormat format>
		static inline void cast(GenericColor<ColorType::Byte, format> & out, const GenericColor<ColorType::Float, format> & in)
		{
			out.vector(ByteVector(in.vector() * 255.0f));
		}
	};

	template<ColorFormat format>
	struct ColorFormatCast<format, format>
	{
		template<ColorType datatype>
		static inline void cast(GenericColor<datatype, format> & out, const GenericColor<datatype, format> & in)
		{
			out = in;
		}
	};

	template<>
	struct ColorFormatCast<ColorFormat::rgb, ColorFormat::hsv>
	{
		static inline void cast(GenericColor<ColorType::Byte, ColorFormat::hsv> & out, const GenericColor<ColorType::Byte, ColorFormat::rgb> & color)
		{
			static const int first[]  = {1, 2, 0};
			static const int second[] = {2, 0, 1};

			static const byte third  = 85;

			auto minmax = minmax_index(byte_list{color.r, color.g, color.b});
			out = {byte(0), byte(0), color[minmax.second], color.a};

			byte delta = byte(color[minmax.second] - color[minmax.first]);

			if(delta == 0)
				return;

			out.h = byte(minmax.second * third + (color[first[minmax.second]] - color[second[minmax.second]]) * 255 / delta);
			out.s = byte(delta * 255 / out.v);
		}

		static inline void cast(GenericColor<ColorType::Float, ColorFormat::hsv> & out, const GenericColor<ColorType::Float, ColorFormat::rgb> & color)
		{
			static const int first[]  = {1, 2, 0};
			static const int second[] = {2, 0, 1};

			static const float third  = 1.0f / 3.0f;

			auto minmax = minmax_index(float_list{color.r, color.g, color.b});
			out = {0.0f, 0.0f, color[minmax.second], color.a};

			float delta = color[minmax.second] - color[minmax.first];

			if(delta < fmath::eps)
				return;

			out.h = std::fmod(minmax.second * third + (color[first[minmax.second]] - color[second[minmax.second]]) / delta + 1.0f, 1.0f);
			out.s = delta / out.v;
		}
	};

	template<>
	struct ColorFormatCast<ColorFormat::hsv, ColorFormat::rgb>
	{
		static inline void cast(GenericColor<ColorType::Byte, ColorFormat::rgb> & out, const GenericColor<ColorType::Byte, ColorFormat::hsv> & color)
		{
			// 0 - red, 1 - green, 2 - blue
			static constexpr int map[3][6] = {
				{0, 1, 1, 2, 2, 0}, // primary
				{1, 0, 2, 1, 0, 2}, // secondary
				{2, 2, 0, 0, 1, 1}	// recessive
			};

			//static const byte third = 85;
			static const float sixx = 6.0f / 255.0f;

			float sector = color.h * sixx;
			byte chroma = static_cast<byte>(color.v * color.s / 255);
			byte x = byte(chroma * (1 - std::abs(std::fmod(sector, 2.0f) - 1)));

			int i = std::min(static_cast<int>(sector), 5);

			out[map[0][i]] = color.v;
			out[map[1][i]] = color.v - chroma;
			out[map[2][i]] = std::min<byte>(x + out[map[1][i]], 255);

			out.a = color.a;
		}

		static inline void cast(GenericColor<ColorType::Float, ColorFormat::rgb> & out, const GenericColor<ColorType::Float, ColorFormat::hsv> & color)
		{
			// 0 - red, 1 - green, 2 - blue
			static constexpr int map[3][6] = {
				{0, 1, 1, 2, 2, 0}, // primary
				{1, 0, 2, 1, 0, 2}, // secondary
				{2, 2, 0, 0, 1, 1}	// recessive
			};

			float sector = color.h * 6.0f;
			float chroma = color.v * color.s;
			float x = chroma * (1.0f - std::abs(std::fmod(sector, 2.0f) - 1.0f));

			int i = std::min(static_cast<int>(sector), 5);

			out[map[0][i]] = color.v;
			out[map[1][i]] = color.v - chroma;
			out[map[2][i]] = std::min(x + out[map[1][i]], 1.0f);

			out.a = color.a;
		}
	};
}

//---------------------------------------------------------------------------
#endif
