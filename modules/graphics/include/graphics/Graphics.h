//---------------------------------------------------------------------------

#ifndef GRAPHICS_H
#define GRAPHICS_H

//---------------------------------------------------------------------------

#include <windows.h>

#include <thread>

#include <core/Handle.h>
#include <core/Exception.h>
#include <core/addition/State.h>
#include <core/container/Data.h>
#include <core/container/Stack.h>

#include <math/Rect.h>
#include <math/Transform.h>

#include <graphics/ScreenCoord.h>
#include <graphics/image/Image.h>
#include <graphics/font/Font.h>

#include "Color.h"

#undef min
#undef max

//---------------------------------------------------------------------------

namespace Rapture
{
	class Surface;
	class Surface;
	class UISpace;

	class RigidSurfaceException : public Exception
	{
	public:
		RigidSurfaceException() : Exception("Tried to resize rigid surface!") {}
		virtual ~RigidSurfaceException() {}
	};

	class Surface : public Shared
	{
		friend class Graphics;

	public:
		Surface(const IntSize & size) : _viewport(size) {}
		virtual ~Surface() {}

		const Viewport & viewport() const
		{
			return _viewport;
		}

		const IntSize & size() const
		{
			return _viewport.size();
		}

		int width() const
		{
			return _viewport.width();
		}

		int height() const
		{
			return _viewport.height();
		}

		void setSize(const IntSize & size)
		{
			auto tmp = _viewport;
			_viewport.set(size);

			try {
				resize();
			}
			catch(...) {
				_viewport = tmp;
				throw;
			}
		}

		virtual void apply() const = 0;
		virtual void present() const = 0;
		virtual void clear() const = 0;
		virtual void requestData(ImageData * data) const = 0;

		Handle<ImageData> requestData() const
		{
			auto data = handle<ImageData>();
			requestData(data);

			return data;
		}

	protected:
		virtual void resize()
		{
			throw RigidSurfaceException();
		}

		Viewport _viewport;
	};

	enum class FillMode
	{
		Solid,
		Wireframe
	};

	class FigureData : public Shared
	{
	public:
		FigureData(const array_list<FloatPoint> & points) : points(points)
		{
			if(points.size() < 3)
				throw Exception("A number of points should be greater than 3 to construct a figure");
		}

		array_list<FloatPoint> points;
	};

	class Figure : public Shared
	{
	public:
		Figure(Graphics * graphics, const FigureData & data) {}

		virtual void draw() const = 0;
	};

	link_class(graphics, Graphics, Class<Object>);

	class Graphics : public Object
	{
		deny_copy(Graphics);

		friend class UISpace;

	public:
		Graphics() { setclass(Graphics); }
		virtual ~Graphics() {}

		api(graphics) const Handle<Surface> & surface();
		api(graphics) const Viewport & viewport() const;

		api(graphics) const IntRect & clipRect() const;
		api(graphics) const Color & clearColor() const;
		api(graphics) const Color & color() const;

		api(graphics) int fontSize() const;
		api(graphics) int lineWidth() const;
		api(graphics) FillMode fillMode() const;

		api(graphics) IntSize getTextSize(const string & text);
		api(graphics) IntSize getTextSize(const wstring & text);

		api(graphics) State<Color> * colorState();
		api(graphics) State<int> * fontSizeState();
		api(graphics) State<int> * lineWidthState();
		api(graphics) State<FillMode> * fillModeState();

		api(graphics) Handle<ImageData> requestSurfaceData() const;

		template<class ... A, useif <can_construct<Color, A...>::value> endif>
		void setClearColor(A &&... a)
		{
			_clearColor->set(forward<A>(a)...);
		}

		template<class C, class ... A, useif <is_color<C>::value, can_construct<C, A...>::value, not_same_types<C, Types<A...>>::value> endif>
		void setClearColor(A &&... a)
		{
			_clearColor->set(C {forward<A>(a)...});
		}

		template<class ... A, useif <can_construct<Color, A...>::value> endif>
		void setColor(A &&... a)
		{
			_color->set(forward<A>(a)...);
		}

		template<class C, class ... A, useif <is_color<C>::value, can_construct<C, A...>::value, not_same_types<C, Types<A...>>::value> endif>
		void setColor(A &&... a)
		{
			_color->set(C {forward<A>(a)...});
		}

		api(graphics) void setFontSize(int size);
		api(graphics) void setLineWidth(int size);
		api(graphics) void setFillMode(FillMode mode);

		api(graphics) void bind(const Handle<Surface> & surface);
		api(graphics) void bind(const Handle<Font> & font);

		virtual Handle<Image> createImage(const ImageData & data) = 0;
		virtual Handle<Surface> createSurface(UISpace * space) = 0;
		virtual Handle<Surface> createSurface(const IntSize & size, Handle<Image> & image) = 0;

		virtual void clip(const IntRect & rect) = 0;
		virtual void rectangle(const IntRect & rect) = 0;
		virtual void ellipse(const IntRect & rect) = 0;
		virtual void rectangle(const SqRect & r) = 0;
		virtual void ellipse(const SqRect & r) = 0;

		virtual void draw(const Figure * figure, const IntRect & bounds) = 0;
		virtual void draw(const Figure * figure, const FloatTransform & transform) = 0;

		virtual void draw(const Image * image, const IntRect & rect) = 0;
		virtual void draw(const Image * image, const SqRect & r) = 0;

		virtual void draw(const Symbol * symbol, int x, int y) = 0;

		api(graphics) void draw(const Image * image, int x, int y);
		api(graphics) void draw(const Image * image, int x, int y, int width, int height);

		api(graphics) void draw(const Image * image, const IntPoint & pt);
		api(graphics) void draw(const Image * image, const IntPoint & pt, const IntSize & sz);

		api(graphics) void draw(const Symbol * symbol, const IntPoint & pt);

		api(graphics) void draw(const string & text, int x, int y);
		api(graphics) void draw(const wstring & text, int x, int y);
		api(graphics) void draw(const string & text, const IntPoint & pt);
		api(graphics) void draw(const wstring & text, const IntPoint & pt);

		virtual void present() const = 0;

	protected:
		virtual void initFacilities() {}
		virtual void updateBrushState() {}

		template<class string_t>
		static api(graphics) void draw(Graphics * graphics, const string_t & text, int x, int y);

		template<class string_t>
		static api(graphics) IntSize textSize(Graphics * graphics, const string_t & text);

		Handle<Surface> _surface;
		Handle<Font> _font;

		IntRect _clipRect {0.0f, 0.0f, 10000.0f, 10000.0f};

		template<class T>
		class BrushState : public State<T>
		{
			typedef State<T> Base;

		public:
			template<class ... A>
			BrushState(Graphics * graphics, A &&... args) : Base(forward<A>(args)...), graphics(graphics) {}

			using Base::operator =;

		protected:
			virtual void change() override
			{
				graphics->updateBrushState();
			}

			Graphics * graphics;
		};

		template<class T>
		using BrushStateHandle = Handle<BrushState<T>>;

		BrushStateHandle<Color> _color {this, 0.0f, 0.0f, 0.0f, 1.0f};
		BrushStateHandle<int> _lineWidth {this, 1};

		StateHandle<Color> _clearColor {1.0f, 1.0f, 1.0f, 1.0f};
		StateHandle<int> _fontSize {14};
		StateHandle<FillMode> _fillMode {FillMode::Solid};
	};

	template<class G>
	struct CommonGraphicsProvider
	{
		static inline Handle<G, CommonGraphicsProvider<G>> provide()
		{
			Handle<G, CommonGraphicsProvider<G>> graphics(nothing);
			graphics->initFacilities();
			return graphics;
		}
	};

#define friend_graphics_provider(G)						\
	friend_owned_handle(G, CommonGraphicsProvider<G>);	\
	friend struct CommonGraphicsProvider<G>

	class RectangleData	: public FigureData
	{
	public:
		RectangleData() : FigureData({
			{ -1.0f, -1.0f },
			{ -1.0f,  1.0f },
			{  1.0f,  1.0f },
			{  1.0f, -1.0f },
		}) {}

		RectangleData(const FloatRect & r) : FigureData({
			{ r.left, r.top },
			{ r.left, r.bottom },
			{ r.right, r.bottom },
			{ r.right, r.top },
		}) {}

		RectangleData(const FloatSize & sz) : FigureData({
			{ -sz.x, -sz.y },
			{ -sz.x,  sz.y },
			{  sz.x,  sz.y },
			{  sz.x, -sz.y },
		}) {}

		RectangleData(float w, float h) : FigureData({
			{ -w, -h },
			{ -w,  h },
			{  w,  h },
			{  w, -h },
		}) {}
	};

	inline IntRect scaleToSquare(const IntRect & region)
	{
		int w = region.width();
		int h = region.height();
		IntRect r = region;

		if(w > h)
		{
			r.top -= (w - h) / 2;
			r.setHeight(w);
		}
		else if(h > w)
		{
			r.left -= (h - w) / 2;
			r.setWidth(h);
		}

		return r;
	}
}

//---------------------------------------------------------------------------
#endif
