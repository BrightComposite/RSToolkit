//---------------------------------------------------------------------------

#ifndef COLOR_H
#define COLOR_H

//---------------------------------------------------------------------------

#include <meta/Meta.h>
#include <math/Vector.h>

//---------------------------------------------------------------------------

namespace Rapture
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

		ColorBase() : vector {} {}
		ColorBase(const ColorBase & color) : vector {color.vector} {}
		ColorBase(ColorBase && color) : vector {move(color.vector)} {}
		ColorBase(const Vector<T> & vec) : vector {vec} {}
		ColorBase(Vector<T> && vec) : vector {forward<Vector<T>>(vec)} {}
		ColorBase(const array<T, 4> & data) : data {data} {}
		ColorBase(array<T, 4> && data) : data {forward<array<T, 4>>(data)} {}
		ColorBase(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {}

		union
		{
			array<T, 4> data;
			Vector<T> vector;

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

		ColorBase() : vector {} {}
		ColorBase(const ColorBase & color) : vector {color.vector} {}
		ColorBase(ColorBase && color) : vector {move(color.vector)} {}
		ColorBase(const Vector<T> & vec) : vector {vec} {}
		ColorBase(Vector<T> && vec) : vector {forward<Vector<T>>(vec)} {}
		ColorBase(const array<T, 4> & data) : data {data} {}
		ColorBase(array<T, 4> && data) : data {forward<array<T, 4>>(data)} {}
		ColorBase(T h, T s, T v, T a) : h(h), s(s), v(v), a(a) {}

		union
		{
			array<T, 4> data;
			Vector<T> vector;

			struct
			{
				T h, s, v, a;
			};
		};
	};

	template<ColorType datatype, ColorFormat format>
	struct Color : ColorBase<datatype, format>
	{
		using T = color_data_t<datatype>;
		using Base = ColorBase<datatype, format>;
		using Traits = ColorTraits<datatype>;

		Color() : Base {Traits::default_color()} {}
		Color(const Color & color) : Base {color.vector} {}

		template<class ... A, selectif(0) <sizeof...(A) == 3, std::is_convertible<A, T>::value...> endif>
		Color(A &&... args) : Base {adapt<T, A>(forward<A>(args))..., Traits::alpha()} {}

		template<class ... A, selectif(1) <sizeof...(A) == 4, std::is_convertible<A, T>::value...> endif>
		Color(A &&... args) : Base {adapt<T, A>(forward<A>(args))...} {}

		Color(const T(&color)[4]) : Base {color} {}
		Color(const T(&color)[3]) : Base {color[0], color[1], color[2], Traits::alpha()} {}
		Color(const Vector<T> & color) : Base {color} {}

		template<ColorType d, ColorFormat f, useif <d != datatype || f != format> endif>
		Color(const Color<d, f> & color)
		{
			Cast<Color<d, f>, Color>::cast(*this, color);
		}

		Color & operator = (const Color & color)
		{
			vector = color.vector;
			return *this;
		}

		bool operator == (const Color & color) const
		{
			return vector == color.vector;
		}

		bool operator != (const Color & color) const
		{
			return vector != color.vector;
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

		operator Vector<T> & () &
		{
			return this->vector;
		}

		operator const Vector<T> & () const &
		{
			return this->vector;
		}

		operator Vector<T> && () &&
		{
			return move(this->vector);
		}

		operator array<T, 4> & () &
		{
			return this->data;
		}

		operator const array<T, 4> & () const &
		{
			return this->data;
		}

		operator array<T, 4> && () &&
		{
			return move(this->data);
		}
	};

	using color  = Color<ColorType::Float, ColorFormat::rgb>;
	using colorf = Color<ColorType::Float, ColorFormat::rgb>;
	using colorb = Color<ColorType::Byte,  ColorFormat::rgb>;
	using rgb	 = Color<ColorType::Byte,  ColorFormat::rgb>;
	using hsv	 = Color<ColorType::Float, ColorFormat::hsv>;

	template<class T>
	struct is_color : false_type {};

	template<ColorType d, ColorFormat f>
	struct is_color<Color<d, f>> : true_type {};

	template<ColorType inType, ColorFormat inFormat, ColorType outType, ColorFormat outFormat>
	struct Cast<Color<inType, inFormat>, Color<outType, outFormat>>
	{
		template<ColorType In = inType, ColorType Out = outType, useif <In == Out> endif, useif <inFormat == outFormat> endif>
		static inline void cast(Color<outType, outFormat> & out, const Color<inType, inFormat> & in)
		{
			out = in;
		}

		template<ColorType In = inType, ColorType Out = outType, useif <In == Out> endif, skipif <inFormat == outFormat> endif>
		static inline void cast(Color<outType, outFormat> & out, const Color<inType, inFormat> & in)
		{
			ColorFormatCast<inFormat, outFormat>::cast(out, in);
		}

		template<ColorType In = inType, ColorType Out = outType, skipif <In == Out> endif, useif <inFormat == outFormat> endif>
		static inline void cast(Color<outType, outFormat> & out, const Color<inType, inFormat> & in)
		{
			ColorDataCast<inType, outType>::cast(out, in);
		}

		template<ColorType In = inType, ColorType Out = outType, skipif <In == Out> endif, skipif <inFormat == outFormat> endif>
		static inline void cast(Color<outType, outFormat> & out, const Color<inType, inFormat> & in)
		{
			Color<outType, inFormat> temp;
			ColorDataCast<inType, outType>::cast(temp, in);
			ColorFormatCast<inFormat, outFormat>::cast(out, temp);
		}
	};

	template<ColorType datatype>
	struct ColorDataCast<datatype, datatype>
	{
		template<ColorFormat format>
		static inline void cast(Color<datatype, format> & out, const Color<datatype, format> & in)
		{
			out = in;
		}
	};

	template<>
	struct ColorDataCast<ColorType::Byte, ColorType::Float>
	{
		template<ColorFormat format>
		static inline void cast(Color<ColorType::Float, format> & out, const Color<ColorType::Byte, format> & in)
		{
			out.vector = FloatVector(in.vector) / 255.0f;
		}
	};

	template<>
	struct ColorDataCast<ColorType::Float, ColorType::Byte>
	{
		template<ColorFormat format>
		static inline void cast(Color<ColorType::Byte, format> & out, const Color<ColorType::Float, format> & in)
		{
			out.vector = ByteVector(in.vector * 255.0f);
		}
	};

	template<ColorFormat format>
	struct ColorFormatCast<format, format>
	{
		template<ColorType datatype>
		static inline void cast(Color<datatype, format> & out, const Color<datatype, format> & in)
		{
			out = in;
		}
	};

	template<>
	struct ColorFormatCast<ColorFormat::rgb, ColorFormat::hsv>
	{
		static inline void cast(Color<ColorType::Byte, ColorFormat::hsv> & out, const Color<ColorType::Byte, ColorFormat::rgb> & color)
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

		static inline void cast(Color<ColorType::Float, ColorFormat::hsv> & out, const Color<ColorType::Float, ColorFormat::rgb> & color)
		{
			static const int first[]  = {1, 2, 0};
			static const int second[] = {2, 0, 1};

			static const float third  = 1 / 3.0f;

			auto minmax = minmax_index(float_list{color.r, color.g, color.b});
			out = {0.0f, 0.0f, color[minmax.second], color.a};

			float delta = color[minmax.second] - color[minmax.first];

			if(delta < FloatMath::eps)
				return;

			out.h = std::fmodf(minmax.second * third + (color[first[minmax.second]] - color[second[minmax.second]]) / delta + 1.0f, 1.0f);
			out.s = delta / out.v;
		}
	};

	template<>
	struct ColorFormatCast<ColorFormat::hsv, ColorFormat::rgb>
	{
		static inline void cast(Color<ColorType::Byte, ColorFormat::rgb> & out,const Color<ColorType::Byte, ColorFormat::hsv> & color)
		{
			static const int cc[] = {0, 1, 1, 2, 2, 0};
			static const int xx[] = {1, 0, 2, 1, 0, 2};
			static const int zz[] = {2, 2, 0, 0, 1, 1};

			static const byte third = 85;
			static const float sixx = 6.0f / 255.0f;
			static const float rsix = 1.0f / (255.0f * 6.0f);

			byte c = color.v * color.s / 255;
			byte x = byte(c * (1 - std::abs(std::fmodf(color.h * rsix, third) - 1)));

			int i = std::min(static_cast<int>(color.h * sixx), 5);

			out[cc[i]] = color.v;
			out[zz[i]] = color.v - c;
			out[xx[i]] = std::min<byte>(x + out[zz[i]], 255);

			out.a = color.a;
		}

		static inline void cast(Color<ColorType::Float, ColorFormat::rgb> & out, const Color<ColorType::Float, ColorFormat::hsv> & color)
		{
			static const int cc[] = {0, 1, 1, 2, 2, 0};
			static const int xx[] = {1, 0, 2, 1, 0, 2};
			static const int zz[] = {2, 2, 0, 0, 1, 1};

			static const float third  = 1 / 3.0f;

			float c = color.v * color.s;
			float x = c * (1 - std::abs(std::fmodf(color.h * (1 / 6.0f), third) - 1));

			int i = std::min(static_cast<int>(color.h * 6), 5);

			out[cc[i]] = color.v;
			out[zz[i]] = color.v - c;
			out[xx[i]] = std::min(x + out[zz[i]], 1.0f);

			out.a = color.a;
		}
	};
}

//---------------------------------------------------------------------------
#endif
