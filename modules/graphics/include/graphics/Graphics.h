//---------------------------------------------------------------------------

#ifndef GRAPHICS_H
#define GRAPHICS_H

//---------------------------------------------------------------------------

#include <windows.h>

#include <thread>

#include <core/Handle.h>
#include <core/Subject.h>
#include <core/addition/State.h>
#include <core/container/RawData.h>
#include <core/container/Stack.h>

#include <math/Rect.h>
#include <math/Transform.h>

#include <graphics/ScreenCoord.h>
#include <graphics/image/Image.h>
#include <graphics/text/Font.h>

#include "Color.h"

#undef min
#undef max

//---------------------------------------------------------------------------

namespace Rapture
{
	class Surface : public Shareable<Surface>
	{
		friend class Graphics;

	public:
		Surface(const IntSize & size) : viewport(size) {}
		virtual ~Surface() {}

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
		Viewport viewport;
	};

	enum class FillMode
	{
		Solid,
		Wireframe
	};

	class FigureData : public Shareable<FigureData>
	{
	public:
		FigureData(const vector<FloatPoint> & points) : points(points) {}

		vector<FloatPoint> points;
	};

	class Figure : public Shareable<Figure>
	{
	public:
		Figure(const Graphics * graphics, const FigureData & data) : graphics(graphics) {}

		const Graphics * graphics;
	};

	link_class(Graphics, MetaClass<Subject>);

	class Graphics : public Subject
	{
		friend class WindowAdapter;

	public:
		Graphics() { setclass(Graphics); }
		virtual ~Graphics() {}

		virtual void bind(const Handle<Surface> & surface) = 0;

		void bind(const Handle<Font> & font)
		{
			_font = font;
		}

		const Handle<Surface> & surface()
		{
			return _surface;
		}

		const Viewport & viewport() const
		{
			return _surface->viewport;
		}

		virtual Handle<Image> createImage(const ImageData & data) const = 0;
		virtual Handle<Figure> createFigure(const FigureData & data) const = 0;

		virtual void present() const = 0;

		const IntRect & clipRect() const
		{
			return _clipRect;
		}

		const color4 & clearColor() const
		{
			return *_clearColor;
		}

		const color4 & color() const
		{
			return *_color;
		}

		int fontSize() const
		{
			return *_fontSize;
		}

		int lineSize() const
		{
			return *_lineSize;
		}

		FillMode fillMode() const
		{
			return *_fillMode;
		}

		void setClearColor(const color4 & color)
		{
			*_clearColor = color;
		}

		void setColor(const color4 & color)
		{
			*_color = color;
		}

		void setFontSize(int size)
		{
			*_fontSize = size;
		}

		void setLineSize(int size)
		{
			*_lineSize = size;
		}

		void setFillMode(FillMode mode)
		{
			*_fillMode = mode;
		}

		virtual void clip(const IntRect & rect) = 0;
		virtual void rectangle(const IntRect & rect) = 0;
		virtual void ellipse(const IntRect & rect) = 0;
		virtual void rectangle(const SqRect & r) = 0;
		virtual void ellipse(const SqRect & r) = 0;

		virtual void draw(const Figure * figure, const IntRect & bounds) = 0;
		virtual void draw(const Figure * figure, const FloatTransform & transform) = 0;

		virtual void draw(const Image * image, int x, int y) = 0;
		virtual void draw(const Image * image, int x, int y, int width, int height) = 0;

		void draw(const Image * image, const IntPoint & pt)
		{
			draw(image, pt.x, pt.y);
		}

		void draw(const Image * image, const IntPoint & pt, const IntSize & sz)
		{
			draw(image, pt.x, pt.y, sz.x, sz.y);
		}

		virtual void draw(const Image * image, const IntRect & rect) = 0;
		virtual void draw(const Image * image, const SqRect & r) = 0;

		virtual void draw(const Symbol * symbol, int x, int y) = 0;

		void draw(const Symbol * symbol, const IntPoint & pt)
		{
			draw(symbol, pt.x, pt.y);
		}

		void draw(const string & text, int x, int y)
		{
			draw<string>(this, text, x, y);
		}

		void draw(const wstring & text, int x, int y)
		{
			draw<wstring>(this, text, x, y);
		}

		void draw(const string & text, const IntPoint & pt)
		{
			draw(text, pt.x, pt.y);
		}

		void draw(const wstring & text, const IntPoint & pt)
		{
			draw(text, pt.x, pt.y);
		}

		IntSize getTextSize(const string & text) const
		{
			return textSize<string>(this, text);
		}

		IntSize getTextSize(const wstring & text) const
		{
			return textSize<wstring>(this, text);
		}

		State<color4> * colorState()
		{
			return _color;
		}

		State<int> * fontSizeState()
		{
			return _fontSize;
		}

		State<int> * lineSizeState()
		{
			return _lineSize;
		}

		State<FillMode> * fillModeState()
		{
			return _fillMode;
		}

		Handle<ImageData> surfaceData() const
		{
			auto data = handle<ImageData>();
			_surface->requestData(data);

			return data;
		}

	protected:
		virtual Handle<Surface> createWindowSurface(WindowAdapter * adapter) const = 0;
		virtual void updateBrushState() {}

		template<class string_t>
		static void draw(Graphics * graphics, const string_t & text, int x, int y);

		template<class string_t>
		static IntSize textSize(const Graphics * graphics, const string_t & text);

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

		BrushStateHandle<color4> _color {this, 0.0f, 0.0f, 0.0f, 1.0f};
		BrushStateHandle<int> _lineSize {this, 1};

		StateHandle<color4> _clearColor {1.0f, 1.0f, 1.0f, 1.0f};
		StateHandle<int> _fontSize {14};
		StateHandle<FillMode> _fillMode {FillMode::Solid};
	};

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
