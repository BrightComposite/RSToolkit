//---------------------------------------------------------------------------

#ifndef COLOR_H
#define COLOR_H

//---------------------------------------------------------------------------

#include <core/meta/Meta.h>
#include <math/Vector.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	enum class ColorFormat
	{
		rgb,
		hsv
	};

	/**
	 *	0 - unknown type
	 *	1 - floating type (0.0..1.0)
	 *	2 - integral type (0..255)
	 */
	template<class T, int type = std::is_integral<T>::value ? 2 : std::is_floating_point<T>::value ? 1 : 0>
	class ColorTraits {};

	template<class T, int N, ColorFormat format>
	class ColorBase {};

	template<class T>
	struct ColorBase<T, 3, ColorFormat::rgb>
	{
		union
		{
			T data[3];

			struct
			{
				T r, g, b;
			};
		};
	};

	template<class T>
	struct ColorBase<T, 4, ColorFormat::rgb>
	{
		union
		{
			T data[4];

			struct
			{
				T r, g, b, a;
			};
		};
	};

	template<class T>
	struct ColorBase<T, 3, ColorFormat::hsv>
	{
		union
		{
			T data[3];

			struct
			{
				T h, s, v;
			};
		};
	};

	template<class T>
	struct ColorBase<T, 4, ColorFormat::hsv>
	{
		union
		{
			T data[4];

			struct
			{
				T h, s, v, a;
			};
		};
	};

	template<class T, class U>
	inline T color_cast(U component)
	{
		return ColorTraits<T>::cast(component);
	}

	template<class T, int N, ColorFormat format>
	class Color : public ColorBase<T, N, format>
	{
	public:
		Color()
		{
			ColorTraits<T>::init(data);
		}

		template<class F, class ... A, useif(std::is_pod<F>::value)>
		Color(F first, A ... others)
		{
			T color[] {color_cast<T>(first), color_cast<T>(others)...};
			ColorTraits<T>::init(data, color);
		}

		Color(const T(&color)[3])
		{
			ColorTraits<T>::init(data, color);
		}

		Color(const T(&color)[4])
		{
			ColorTraits<T>::init(data, color);
		}

		Color(const Vector<T> & color)
		{
			ColorTraits<T>::init(data, color.v);
		}

		template<int N_>
		Color(const Color<T, N_, format> & color) : Color(color.data) {}

		bool operator == (const Color & color) const
		{
			for(register int i = 0; i < N; ++i)
				if(data[i] != color.data[i])
					return false;

			return true;
		}

		bool operator != (const Color & color) const
		{
			return !operator == (color);
		}

		T & operator [] (int index)
		{
			return data[index];
		}

		const T & operator [] (int index) const
		{
			return data[index];
		}

		operator array_t<T, N> & ()
		{
			return data;
		}

		operator const array_t<T, N> & () const
		{
			return data;
		}
	};

	typedef Color<float, 3, ColorFormat::rgb> color3;
	typedef Color<float, 4, ColorFormat::rgb> color4;

	typedef Color<float, 3, ColorFormat::rgb> FloatRgb;
	typedef Color<float, 4, ColorFormat::rgb> FloatRgba;

	typedef Color<int, 3, ColorFormat::rgb> color3i;
	typedef Color<int, 4, ColorFormat::rgb> color4i;

	typedef Color<int, 3, ColorFormat::rgb> IntRgb;
	typedef Color<int, 4, ColorFormat::rgb> IntRgba;

	typedef Color<int, 3, ColorFormat::rgb> Rgb;
	typedef Color<int, 4, ColorFormat::rgb> Rgba;

	template<class ... T>
	inline Color<float, sizeof...(T), ColorFormat::rgb> color(T ... components)
	{
		return {components...};
	}

	template<class ... T>
	inline Color<float, sizeof...(T), ColorFormat::rgb> rgb(T ... components)
	{
		return {components...};
	}

	template<class ... T>
	inline Color<float, sizeof...(T), ColorFormat::hsv> hsv(T ... components)
	{
		return {components...};
	}

	template<class T, int N, class U, int N_, ColorFormat format>
	inline Color<T, N, format> color_cast(const Color<U, N_, format> & color)
	{
		T data[N];
		ColorTraits<T>::init(data, color.data);
		return {data};
	}

	template<class T>
	class ColorTraits<T, 1>
	{
	public:
		template<int N>
		static inline void init(T(&target)[N])
		{
			Initialize::init(target, 1.0f);
		}

		template<int N>
		static inline void init(T(&target)[N], const T(&color)[N])
		{
			Initialize::init(target, color);
		}

		template<int N>
		static inline void init(T(&target)[N], const initializer_list<T> & list)
		{
			register size_t i = 0;
			const auto ex = list.begin();

			for(; i < list.size() && i < N; ++i)
				target[i] = ex[i];

			for(; i < N; ++i)
				target[i] = i >= 3 ? 1.0f : 0.0f;
		}

		template<int N, int N_>
		static inline void init(T(&target)[N], const T(&color)[N_])
		{
			register int i = 0;

			for(; i < N_ && i < N; ++i)
				target[i] = color[i];

			for(; i < N; ++i)
				target[i] = i >= 3 ? 1.0f : 0.0f;
		}

		template<int N, class U, int N_, useif(is_not_same(T, U))>
		static inline void init(T(&target)[N], const U(&color)[N_])
		{
			register int i = 0;

			for(; i < N_ && i < N; ++i)
				target[i] = cast(color[i]);

			for(; i < N; ++i)
				target[i] = i >= 3 ? 1.0f : 0.0f;
		}

		template<
			class U,
			selectif(1, std::is_integral<U>::value)
		>
		static inline T cast(U val)
		{
			return static_cast<T>(val) / 255.0f;
		}

		template<
			class U,
			selectif(2, std::is_floating_point<U>::value)
		>
		static inline T cast(U val)
		{
			return static_cast<T>(val);
		}
	};

	template<class T>
	class ColorTraits<T, 2>
	{
	public:
		template<int N>
		static inline void init(T(&target)[N])
		{
			Initialize::init(target, 0xFF);
		}

		template<int N>
		static inline void init(T(&target)[N], const T(&color)[N])
		{
			Initialize::init(target, color);
		}

		template<int N>
		static inline void init(T(&target)[N], const initializer_list<T> & list)
		{
			register size_t i = 0;
			const auto ex = list.begin();

			for(; i < list.size() && i < N; ++i)
				target[i] = ex[i];

			for(; i < N; ++i)
				target[i] = i >= 3 ? 0xFF : 0x00;
		}

		template<int N, class U, int N_, useif(is_not_same(T, U))>
		static inline void init(T(&target)[N], const U(&color)[N_])
		{
			register int i = 0;

			for(; i < N_ && i < N; ++i)
				target[i] = cast(color[i]);

			for(; i < N; ++i)
				target[i] = i >= 3 ? 0xFF: 0x00;
		}

		template<
			class U,
			selectif(1, std::is_integral<U>::value)
		>
		static inline T cast(U val)
		{
			return val;
		}

		template<
			class U,
			selectif(2, std::is_floating_point<U>::value)
		>
		static inline T cast(U val)
		{
			return static_cast<T>(std::round(val * 0xFF));
		}
	};
}

//---------------------------------------------------------------------------
#endif