//---------------------------------------------------------------------------

#pragma once

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
#include "Surface.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class UISpace;

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

	class GraphicsDebug : public Shared
	{
	public:
		GraphicsDebug() {}
		virtual ~GraphicsDebug() {}
	};

	link_class(graphics, Graphics, Class<Object>);

	class Graphics : public Object
	{
		deny_copy(Graphics);

		friend class UISpace;

	public:
		Graphics() { setclass(Graphics); }
		virtual ~Graphics() {}

		virtual Handle<GraphicsDebug> getDebug() const = 0;

		api(graphics) Surface * surface();
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
		api(graphics) State<Color> * clearColorState();
		api(graphics) State<int> * fontSizeState();
		api(graphics) State<int> * lineWidthState();
		api(graphics) State<FillMode> * fillModeState();

		api(graphics) Handle<ImageData> requestSurfaceData() const;

		template<class ... A, useif<can_construct<Color, A...>::value>>
		void setClearColor(A &&... a)
		{
			_clearColor->set(forward<A>(a)...);
		}

		template<class C, class ... A, useif<is_color<C>::value, can_construct<C, A...>::value, not_same_types<C, Types<A...>>::value>>
		void setClearColor(A &&... a)
		{
			_clearColor->set(C {forward<A>(a)...});
		}

		template<class ... A, useif<can_construct<Color, A...>::value>>
		void setColor(A &&... a)
		{
			_color->set(forward<A>(a)...);
		}

		template<class C, class ... A, useif<is_color<C>::value, can_construct<C, A...>::value, not_same_types<C, Types<A...>>::value>>
		void setColor(A &&... a)
		{
			_color->set(C {forward<A>(a)...});
		}

		api(graphics) void setFontSize(int size);
		api(graphics) void setLineWidth(int size);
		api(graphics) void setFillMode(FillMode mode);

		api(graphics) void bind(Surface * surface);
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

		api(graphics) void draw(const string & text, int x, int y, int & newx);
		api(graphics) void draw(const wstring & text, int x, int y, int & newx);
		api(graphics) void draw(const string & text, const IntPoint & pt, int & newx);
		api(graphics) void draw(const wstring & text, const IntPoint & pt, int & newx);

		virtual void present() const = 0;

	protected:
		virtual void initFacilities() {}
		virtual void updateBrushState() {}

		virtual api(graphics) void updateSurface();

		template<class string_t>
		static api(graphics) void draw(Graphics * graphics, const string_t & text, int x, int y, int & outx);

		Surface * _surface;
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
		static inline Handle<G> provide()
		{
			Handle<G, CommonGraphicsProvider<G>> graphics(nothing);
			graphics->initFacilities();
			return graphics;
		}
	};

#define friend_graphics_provider(G)																							\
	friend_owned_handle(G, CommonGraphicsProvider<G>);	                                                                    \
	friend struct CommonGraphicsProvider<G>
//----- friend_graphics_provider

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
