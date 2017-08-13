//---------------------------------------------------------------------------

#pragma once

#ifndef GRAPHICS_H
#define GRAPHICS_H

//---------------------------------------------------------------------------

#include <thread>

#include <core/handle.h>
#include <core/Exception.h>
#include <core/addition/state.h>
#include <container/Data.h>
#include <container/Stack.h>

#include <math/rect.h>
#include <math/transform.h>

#include <graphics/ScreenCoord.h>
#include <graphics/image/image.h>
#include <graphics/font/Font.h>

#include "Color.h"
#include "surface.h"

#if BOOST_OS_WINDOWS
#include <windows.h>
#elif BOOST_OS_LINUX

#include <unix/Display.h>

#endif

//---------------------------------------------------------------------------

namespace asd
{
	class ui_space;
	
	enum class FillMode
	{
		Solid, Wireframe
	};
	
	class figure_data : public shareable
	{
	public:
		figure_data(const array_list<float_point> & points) : points(points) {
			if(points.size() < 3) {
				throw Exception("A number of points should be greater than 3 to construct a figure");
			}
		}
		
		array_list<float_point> points;
	};
	
	class Figure : public shareable
	{
	public:
		Figure(graphics * graphics, const figure_data & data) {}
		
		virtual void draw() const = 0;
	};
	
	class GraphicsDebug : public shareable
	{
	public:
		GraphicsDebug() {}
		
		virtual ~GraphicsDebug() {}
	};
	
	class graphics : public object
	{
		deny_copy(graphics);
		
		friend class ui_space;
	
	public:
		graphics() {}
		
		virtual ~graphics() {}
		
		virtual void checkForErrors() {}
		
		virtual handle<GraphicsDebug> getDebug() const = 0;
		
		api(graphics)
		asd::surface * surface();
		api(graphics)
		const asd::viewport & viewport() const;
		
		api(graphics)
		const int_rect & clipRect() const;
		api(graphics)
		const colorf & clearColor() const;
		api(graphics)
		const colorf & color() const;
		
		api(graphics)
		int fontSize() const;
		api(graphics)
		int lineWidth() const;
		api(graphics)
		FillMode fillMode() const;
		
		api(graphics)
		int_size getTextSize(const string & text);
		api(graphics)
		int_size getTextSize(const wstring & text);
		
		api(graphics)
		state<Color> * colorState();
		api(graphics)
		state<Color> * clearColorState();
		api(graphics)
		state<int> * fontSizeState();
		api(graphics)
		state<int> * lineWidthState();
		api(graphics)
		state<FillMode> * fillModeState();
		
		api(graphics)
		handle<image_data> requestSurfaceData() const;
		
		template<class ... A, useif<can_construct<Color, A...>::value>>
		void setClearColor(A && ... a) {
			_clearColor->set(forward<A>(a)...);
		}
		
		template<class C, class ... A, useif<is_color<C>::value, can_construct<C, A...>::value, not_same_types<C, Types<A...>>::value>>
		void setClearColor(A && ... a) {
			_clearColor->set(C {forward<A>(a)...});
		}
		
		template<class ... A, useif<can_construct<Color, A...>::value>>
		void setColor(A && ... a) {
			_color->set(forward<A>(a)...);
		}
		
		template<class C, class ... A, useif<is_color<C>::value, can_construct<C, A...>::value, not_same_types<C, Types<A...>>::value>>
		void setColor(A && ... a) {
			_color->set(C {forward<A>(a)...});
		}
		
		api(graphics)
		void setFontSize(int size);
		api(graphics)
		void setLineWidth(int size);
		api(graphics)
		void setFillMode(FillMode mode);
		
		api(graphics)
		void bind(asd::surface * surface);
		api(graphics)
		void bind(const handle<Font> & font);
		
		virtual handle<asd::image> createImage(const image_data & data) = 0;
		virtual handle<asd::surface> createSurface(ui_space * space) = 0;
		virtual handle<ImageSurface> createSurface(const int_size & size) = 0;
		
		virtual void clip(const int_rect & rect) = 0;
		virtual void rectangle(const int_rect & rect) = 0;
		virtual void ellipse(const int_rect & rect) = 0;
		virtual void rectangle(const sq_rect & r) = 0;
		virtual void ellipse(const sq_rect & r) = 0;
		
		virtual void draw(const Figure * figure, const int_rect & bounds) = 0;
		virtual void draw(const Figure * figure, const math::float_transform & transform) = 0;
		
		virtual void draw(const image * image, const int_rect & rect) = 0;
		virtual void draw(const image * image, const sq_rect & r) = 0;
		
		virtual void draw(const Symbol * symbol, int x, int y) = 0;
		
		api(graphics)
		void draw(const image * image, int x, int y);
		api(graphics)
		void draw(const image * image, int x, int y, int width, int height);
		
		api(graphics)
		void draw(const image * image, const int_point & pt);
		api(graphics)
		void draw(const image * image, const int_point & pt, const int_size & sz);
		
		api(graphics)
		void draw(const Symbol * symbol, const int_point & pt);
		
		api(graphics)
		void draw(const string & text, int x, int y);
		api(graphics)
		void draw(const wstring & text, int x, int y);
		api(graphics)
		void draw(const string & text, const int_point & pt);
		api(graphics)
		void draw(const wstring & text, const int_point & pt);
		
		api(graphics)
		void draw(const string & text, int x, int y, int & newx);
		api(graphics)
		void draw(const wstring & text, int x, int y, int & newx);
		api(graphics)
		void draw(const string & text, const int_point & pt, int & newx);
		api(graphics)
		void draw(const wstring & text, const int_point & pt, int & newx);
		
		virtual void present() const = 0;
		
		template<class T>
		class BrushState : public state<T>
		{
			typedef state<T> Base;
		
		public:
			template<class ... A>
			BrushState(asd::graphics * graphics, A && ... args) : Base(forward<A>(args)...), graphics(graphics) {}
			
			using Base::operator =;
		
		protected:
			virtual void change() override {
				graphics->updateBrushState();
			}
			
			asd::graphics * graphics;
		};
		
		
		#if BOOST_OS_LINUX
		
		::Display * display() const {
			return _display;
		}
		
		XVisualInfo * visualInfo() const {
			return _visualInfo;
		}
		
		#endif
	
	protected:
		virtual void initFacilities() {}
		
		virtual void updateBrushState() {}
		
		virtual api(graphics) void updateSurface();
		
		template<class string_t>
		static api(graphics) void draw(asd::graphics * graphics, const string_t & text, int x, int y, int & outx);
		
		asd::surface * _surface;
		handle<Font> _font;
		
		int_rect _clipRect{0.0f, 0.0f, 10000.0f, 10000.0f};
		
		template<class T> using BrushStateHandle = handle<BrushState<T>>;
		
		BrushStateHandle<Color> _color{this, 0.0f, 0.0f, 0.0f, 1.0f};
		BrushStateHandle<int> _lineWidth{this, 1};
		
		StateHandle<Color> _clearColor{1.0f, 1.0f, 1.0f, 1.0f};
		StateHandle<int> _fontSize{14};
		StateHandle<FillMode> _fillMode{FillMode::Solid};
		
#if BOOST_OS_LINUX
		::Display * _display;
		XVisualInfo * _visualInfo;
#endif
	};
	
	template<class G>
	struct CommonGraphicsProvider
	{
		static inline handle<G> provide() {
			handle<G> graphics(_);
			graphics->initFacilities();
			return graphics;
		}
	};

#define friend_graphics_provider(G)                                                                                   		\
	friend_owned_handle(G)                                                                                   				\
    friend struct CommonGraphicsProvider<G>                                                                                 \
//----- friend_graphics_provider
	
	class rectangle_data : public figure_data
	{
	public:
		rectangle_data() : figure_data({
			{-1.0f, -1.0f},
			{-1.0f, 1.0f},
			{1.0f,  1.0f},
			{1.0f,  -1.0f},
		}) {}
		
		rectangle_data(const float_rect & r) : figure_data({
			{r.left,  r.top},
			{r.left,  r.bottom},
			{r.right, r.bottom},
			{r.right, r.top},
		}) {}
		
		rectangle_data(const float_size & sz) : figure_data({
			{-sz.x, -sz.y},
			{-sz.x, sz.y},
			{sz.x,  sz.y},
			{sz.x,  -sz.y},
		}) {}
		
		rectangle_data(float w, float h) : figure_data({
			{-w, -h},
			{-w, h},
			{w,  h},
			{w,  -h},
		}) {}
	};
	
	inline int_rect scaleToSquare(const int_rect & region) {
		int w = region.width();
		int h = region.height();
		int_rect r = region;
		
		if(w > h) {
			r.top -= (w - h) / 2;
			r.setHeight(w);
		} else if(h > w) {
			r.left -= (h - w) / 2;
			r.setWidth(h);
		}
		
		return r;
	}
}

//---------------------------------------------------------------------------
#endif
