//---------------------------------------------------------------------------

#pragma once

#ifndef WIDGET_H
#define WIDGET_H

//---------------------------------------------------------------------------

#include <container/ArrayList.h>
#include <core/Component.h>

#include <message/Subject.h>

#include <graphics/ScreenCoord.h>

#include "WidgetMessages.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Widget;
	class WidgetComponent;
	class WidgetLayer;
	class Graphics;
	class CustomLayer;

	template<class T>
	using is_widget = based_on<T, Widget>;

	template<class T>
	using is_widget_component = based_on<T, WidgetComponent>;

	template<class T>
	using is_layer = based_on<T, WidgetLayer>;

	template<class T>
	using is_widget_drawer = is_callable<T, const Widget *, const IntRect &>;

	link_class(ui, Widget, Class<Subject>);

	class MouseState
	{
	public:
		MouseButton buttons() const
		{
			return _buttons;
		}

		bool isPressed() const
		{
			return _buttons != MouseButton::None;
		}

		bool isPressed(MouseButton button) const
		{
			return check_flag(button, _buttons);
		}

		bool hasPressed(MouseButton buttons) const
		{
			return has_some_flags(buttons, _buttons);
		}

		void press(MouseButton button)
		{
			set_flag(button, _buttons);
		}

		void unpress(MouseButton button)
		{
			clear_flag(button, _buttons);
		}

		void unpress()
		{
			_buttons = MouseButton::None;
		}

	protected:
		MouseButton _buttons = MouseButton::None;
	};

	class Widget : public Subject
	{
		deny_copy(Widget);

		friend class UISpace;
		friend class WidgetLayer;

	public:
		api(ui) Widget(Widget * parent);
		api(ui) Widget(Widget * parent, const IntRect & region);
		api(ui) Widget(UISpace * space);
		api(ui) Widget(UISpace * space, const IntRect & region);
		api(ui) Widget(const Widget & widget, Widget * parent);

		virtual api(ui) ~Widget();

		virtual inline Handle<Widget> clone() const;
		virtual inline Handle<Widget> clone(Widget * parent) const;

		const IntSize & size() const
		{
			return _size;
		}

		int width() const
		{
			return _size.x;
		}

		int height() const
		{
			return _size.y;
		}

		const IntPoint & pos() const
		{
			return _relPos;
		}

		int side(int index) const
		{
			switch(index)
			{
				case 0:
					return _relPos.x;
				case 1:
					return _relPos.y;
				case 2:
					return _relPos.x + _size.x;
				case 3:
					return _relPos.y + _size.y;
				default:
					return 0;
			}
		}

		int left() const
		{
			return _relPos.x;
		}

		int top() const
		{
			return _relPos.y;
		}

		int right() const
		{
			return _relPos.x + _size.x;
		}

		int bottom() const
		{
			return _relPos.y + _size.y;
		}

		operator IntRect () const
		{
			return {_relPos, _size};
		}

		IntRect region() const
		{
			return {_relPos, _size};
		}

		IntRect localRegion() const
		{
			return {_size};
		}

		Viewport viewport() const
		{
			return {_size};
		}

		const IntPoint & absPos() const
		{
			return _absPos;
		}

		int absLeft() const
		{
			return _absPos.x;
		}

		int absTop() const
		{
			return _absPos.y;
		}

		int absRight() const
		{
			return _absPos.x + _size.x;
		}

		int absBottom() const
		{
			return _absPos.y + _size.y;
		}

		IntRect absRegion() const
		{
			return {_absPos, _size};
		}

		ModelMask alignment() const
		{
			return _alignment;
		}

		const IntRect & offsets() const
		{
			return _offsets;
		}

		const FloatRect & anchors() const
		{
			return _anchors;
		}

		inline void setLeft(int value);
		inline void setTop(int value);
		inline void setRight(int value);
		inline void setBottom(int value);
		inline void setWidth(int value);
		inline void setHeight(int value);
		inline void setPos(int left, int top);
		inline void setPos(const IntPoint & pt);
		inline void setRegion(const IntRect & region);
		inline void setSize(int width, int height);
		inline void setSize(const IntSize & size);
		inline void setPlacement(int left, int top, int width, int height);
		inline void setPlacement(const IntPoint & pt, const IntSize & size);
		inline void setPlacement(ModelMask alignment, const IntRect & offsets);
		inline void setPlacement(ModelMask alignment, const IntPoint & offset, const IntSize & size);
		inline void setPlacement(ModelMask alignment, int left, int top, int width, int height);
		inline void setPlacement(const FloatRect & anchors, const IntRect & offsets);

		inline void setAlignment(ModelMask alignment);
		inline void setOffset(int side, int value);
		inline void setAnchor(int side, float value);
		inline void setOffsets(const IntRect & offsets);
		inline void setAnchors(const FloatRect & anchors);

		static api(ui) void calculateRegionRect(IntRect & out, const IntRect & offsets, const FloatRect & anchors, const Widget & parent);
		static api(ui) void calculateOffsets(IntRect & out, const Widget & region, const Widget & parent);

		template<class WidgetClass, typename ... A, selectif(0)<is_widget<WidgetClass>::value, can_construct<WidgetClass, Widget *, A...>::value>>
		inline Handle<WidgetClass> append(A && ...);
		template<class LayerClass, typename ... A, selectif(1)<is_layer<LayerClass>::value, !is_widget_component<LayerClass>::value, can_construct<LayerClass, A...>::value>>
		inline LayerClass * append(A && ...);
		template<class Component, typename ... A, selectif(2)<is_widget_component<Component>::value, can_construct<Component,  Widget *, A...>::value>>
		inline Handle<Component> append(A && ...);

		api(ui) Handle<Widget> attach(const Handle<Widget> & child);
		api(ui) void detach(Widget * child);

		template<class Component, useif<is_widget_component<Component>::value>>
		inline Handle<Component> & init(Handle<Component> & component);
		template<class Component, useif<is_widget_component<Component>::value>>
		inline Handle<Component> require();
		template<class Component, class LinkComponent, useif<is_widget_component<Component>::value, is_widget_component<LinkComponent>::value>>
		inline Handle<Component> link(LinkComponent *);
		template<class Component, useif<is_widget_component<Component>::value>>
		inline Handle<Component> seek();
		template<class Component, useif<is_widget_component<Component>::value>>
		inline void detach();

		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		inline LayerClass * attach(LayerClass * layer);
		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		inline LayerClass * attach(const Handle<LayerClass> & layer);
		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		inline LayerClass * attach(const UniqueHandle<LayerClass> & layer);
		template<class Drawer, useif<is_widget_drawer<Drawer>::value>>
		inline CustomLayer * attach(const Drawer & drawer, int order = 0);
		inline void setLayerOrder(WidgetLayer * layer, int order);
		inline void detach(WidgetLayer * layer);

		template<class Drawer, useif<is_widget_drawer<Drawer>::value>>
		inline Widget & operator << (const Drawer & drawer);

		virtual bool isDisplayable() const
		{
			return false;
		}

		bool isVisible() const
		{
			return check_flag(WidgetFlag::Visible, _flags);
		}

		bool isFocusable() const
		{
			return check_flag(WidgetFlag::Focusable, _flags);
		}

		api(ui) bool isPointed() const;
		api(ui) bool isFocused() const;
		api(ui) bool isPressed() const;
		api(ui) bool isPressed(MouseButton button) const;

		api(ui) MouseButton pressedButtons() const;

		int displayOrder() const
		{
			return _displayOrder;
		}

		int focusOrder() const
		{
			return _focusOrder;
		}

		api(ui) UISpace * space() const;
		api(ui) Graphics * graphics() const;

		api(ui) Widget * findAt(const IntPoint & pt);

		void show()
		{
			setVisibility(true);
		}

		void hide()
		{
			setVisibility(false);
		}

		api(ui) void focus();

		api(ui) void setVisibility(bool visible);
		api(ui) void setFocusability(bool focusable);
		api(ui) void setDisplayOrder(int order);
		api(ui) void setFocusOrder(int order);

		api(ui) void bringToFront();
		api(ui) void sendToBack();

		virtual api(ui) void read(Handle<KeyDownMessage> &);
		virtual api(ui) void read(Handle<CharMessage> &);
		virtual api(ui) void read(Handle<KeyUpMessage> &);
		virtual api(ui) void read(Handle<MouseUpdateMessage> &);
		virtual api(ui) void read(Handle<MouseDownMessage> &);
		virtual api(ui) void read(Handle<MouseMoveMessage> &);
		virtual api(ui) void read(Handle<MouseUpMessage> &);
		virtual api(ui) void read(Handle<MouseClickMessage> &);
		virtual api(ui) void read(Handle<MouseDblClickMessage> &);
		virtual api(ui) void read(Handle<MouseWheelMessage> &);
		virtual api(ui) void read(Handle<MouseEnterMessage> &);
		virtual api(ui) void read(Handle<MouseLeaveMessage> &);
		virtual api(ui) void read(Handle<WidgetPressMessage> &);
		virtual api(ui) void read(Handle<WidgetStopPressMessage> &);
		virtual api(ui) void read(Handle<WidgetReleaseMessage> &);
		virtual api(ui) void read(Handle<WidgetChangedStateMessage> &);
		virtual api(ui) void read(Handle<ChangeFocusOrderMessage> &);
		virtual api(ui) void read(Handle<AfterChangeFocusOrderMessage> &);
		virtual api(ui) void read(Handle<ChangeDisplayOrderMessage> &);
		virtual api(ui) void read(Handle<AfterChangeDisplayOrderMessage> &);
		virtual api(ui) void read(Handle<WidgetDrawMessage> &);
		virtual api(ui) void read(Handle<WidgetMoveMessage> &);
		virtual api(ui) void read(Handle<AfterWidgetMoveMessage> &);
		virtual api(ui) void read(Handle<WidgetResizeMessage> &);

	protected:
		api(ui) Widget(UISpace * space, Widget * parent, const IntRect & region);

		api(ui) void draw(Graphics * graphics, const IntRect & clipRegion);

		api(ui) void removeFocus();
		api(ui) void receiveFocus();

		api(ui) void changeSize(int width, int height, ModelMask mask);
		api(ui) void changePlacement(int left, int top, int right, int bottom, ModelMask mask);
		api(ui) void updateAnchors();

		UISpace * _space;
		Widget * _parent;
		SubjectSet<Widget> _children;

		IntPoint _relPos;
		IntPoint _absPos;
		IntSize  _size;

		IntRect _offsets = {0, 0, 0, 0};
		FloatRect _anchors = {0.0f, 0.0f, 0.0f, 0.0f};

		ModelMask _alignment = ModelMask::LeftTop;

		ComponentSet<WidgetComponent> _components;

		array_list<WidgetLayer *> _layers;
		array_list<Widget *> _displayList;

		int _flags = 0;
		MouseState _mouseState;

		int _focusOrder = 0;
		int _displayOrder = 0;

		using DisplaySort = MemberSort<Widget, int, &Widget::_displayOrder>;
		using FocusSort = MemberSort<Widget, int, &Widget::_focusOrder>;

		bind_messages(Widget, WidgetMessages)
	};

	channels_api(ui, Widget, WidgetMessages)

	class WidgetComponent : public Component, public Connector
	{
		morph_base(WidgetComponent);

	public:
		WidgetComponent(Widget * widget) : _widget(widget) {}
		virtual ~WidgetComponent() {}

	protected:
		Widget * _widget;
	};

	create_morph_pool(ui, WidgetComponent);

	class WidgetLayer : public Shared
	{
		friend class Widget;

	public:
		WidgetLayer(int order = 0) : _order(order) {}
		virtual ~WidgetLayer() {}

		int order() const
		{
			return _order;
		}

	protected:
		virtual void draw(Widget * w) = 0;

		int _order;

		using Sorter = MemberSort<WidgetLayer, int, &WidgetLayer::_order>;
	};

	class WidgetLayerComponent : public WidgetComponent
	{
	public:
		WidgetLayerComponent(Widget * widget, UniqueHandle<WidgetLayer> && layer) : WidgetComponent(widget), _layer(move(layer))
		{
			_widget->attach(_layer);
		}

		virtual ~WidgetLayerComponent()
		{
			_widget->detach(_layer);
		}

	protected:
		UniqueHandle<WidgetLayer> _layer;
	};

	typedef std::function<void(const Widget *, const IntRect &)> WidgetDrawer;

	class CustomLayer : public WidgetLayer
	{
	public:
		CustomLayer(const WidgetDrawer & drawer, int order = 0) : WidgetLayer(order), drawer(drawer) {}
		virtual ~CustomLayer() {}

	protected:
		virtual api(ui) void draw(Widget * w) override;

		WidgetDrawer drawer;
	};

	class CustomLayerComponent : public WidgetComponent
	{
	public:
		CustomLayerComponent(Widget * w) : WidgetComponent(w) {}

		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		LayerClass * add(const Handle<LayerClass> & layer)
		{
			_layers.push_back(layer);
			return _widget->attach(layer);
		}

	protected:
		ArrayList<WidgetLayer> _layers;
	};

	create_component(ui, CustomLayerComponent);

	inline void Widget::setLeft(int value)
	{
		changePlacement(value, top(), right(), bottom(), ModelMask::Left);
	}

	inline void Widget::setTop(int value)
	{
		changePlacement(left(), value, right(), bottom(), ModelMask::Top);
	}

	inline void Widget::setRight(int value)
	{
		changePlacement(left(), top(), value, bottom(), ModelMask::Right);
	}

	inline void Widget::setBottom(int value)
	{
		changePlacement(left(), top(), right(), value, ModelMask::Bottom);
	}

	inline void Widget::setWidth(int value)
	{
		changeSize(value, height(), ModelMask::Horizontal);
	}

	inline void Widget::setHeight(int value)
	{
		changeSize(width(), value, ModelMask::Vertical);
	}

	inline void Widget::setRegion(const IntRect & r)
	{
		changePlacement(r.left, r.top, r.right, r.bottom, ModelMask::FullSize);
	}

	inline void Widget::setPos(int left, int top)
	{
		changePlacement(left, top, left + width(), top + height(), ModelMask::FullSize);
	}

	inline void Widget::setPos(const IntPoint & pt)
	{
		changePlacement(pt.x, pt.y, pt.x + width(), pt.y + height(), ModelMask::FullSize);
	}

	inline void Widget::setSize(int width, int height)
	{
		changeSize(width, height, ModelMask::FullSize);
	}

	inline void Widget::setSize(const IntSize & size)
	{
		changeSize(size.x, size.y, ModelMask::FullSize);
	}

	inline void Widget::setPlacement(int left, int top, int width, int height)
	{
		changePlacement(left, top, left + width, top + height, ModelMask::FullSize);
	}

	inline void Widget::setPlacement(const IntPoint & pt, const IntSize & size)
	{
		changePlacement(pt.x, pt.y, pt.x + size.x, pt.y + size.y, ModelMask::FullSize);
	}

	inline void Widget::setPlacement(ModelMask alignment, const IntRect & offsets)
	{
		_offsets = offsets;
		setAlignment(alignment);
	}

	inline void Widget::setPlacement(ModelMask alignment, int left, int top, int width, int height)
	{
		_offsets.set(left, top, left, top);
		setAlignment(alignment);
		setSize(width, height);
	}

	inline void Widget::setPlacement(ModelMask alignment, const IntPoint & offset, const IntSize & size)
	{
		_offsets.set(offset.x, offset.y, offset.x, offset.y);
		setAlignment(alignment);
		setSize(size);
	}

	inline void Widget::setPlacement(const FloatRect & anchors, const IntRect & offsets)
	{
		_alignment = ModelMask::Custom;
		_anchors = anchors;
		_offsets = offsets;

		updateAnchors();
	}

	inline void Widget::setAlignment(ModelMask alignment)
	{
		if(_alignment == alignment)
			return;

		_alignment = alignment;

		if(_alignment == ModelMask::Custom)
			return;

		_anchors.left   = check_flag(ModelMask::Left,   _alignment) ? 0.0f : 1.0f;
		_anchors.top    = check_flag(ModelMask::Top,    _alignment) ? 0.0f : 1.0f;
		_anchors.right  = check_flag(ModelMask::Right,  _alignment) ? 1.0f : 0.0f;
		_anchors.bottom = check_flag(ModelMask::Bottom, _alignment) ? 1.0f : 0.0f;

		updateAnchors();
	}

	inline void Widget::setOffset(int side, int value)
	{
		_offsets[side] = value;
		updateAnchors();
	}

	inline void Widget::setOffsets(const IntRect & offsets)
	{
		_offsets = offsets;
		updateAnchors();
	}

	inline void Widget::setAnchor(int side, float value)
	{
		_alignment = ModelMask::Custom;
		_anchors[side] = value;
		updateAnchors();
	}

	inline void Widget::setAnchors(const FloatRect & anchors)
	{
		_alignment = ModelMask::Custom;
		_anchors = anchors;
		updateAnchors();
	}

	inline Handle<Widget> Widget::clone() const
	{
		return Handle<Widget>(*this, _parent);
	}

	inline Handle<Widget> Widget::clone(Widget * parent) const
	{
		return Handle<Widget>(*this, parent);
	}

	template<class WidgetClass, typename ... A, selected_t(0)> // append widget
	inline Handle<WidgetClass> Widget::append(A && ... args)
	{
		return Handle<WidgetClass>(this, forward<A>(args)...);
	}

	template<class LayerClass, typename ... A, selected_t(1)>
	inline LayerClass * Widget::append(A && ... args) // append layer
	{
		return require<CustomLayerComponent>()->add(Handle<LayerClass>(forward<A>(args)...));
	}

	template<class Component, typename ... A, selected_t(2)> // append component
	inline Handle<Component> Widget::append(A && ... args)
	{
		return _components.construct<Component>(this, forward<A>(args)...);
	}

	template<class LayerClass, used_t>
	inline LayerClass * Widget::attach(LayerClass * layer)
	{
		_layers.insert(std::upper_bound(_layers.begin(), _layers.end(), layer, WidgetLayer::Sorter()), layer); // sorted insert
		return layer;
	}

	template<class LayerClass, used_t>
	inline LayerClass * Widget::attach(const Handle<LayerClass> & layer)
	{
		_layers.insert(std::upper_bound(_layers.begin(), _layers.end(), layer, WidgetLayer::Sorter()), layer); // sorted insert
		return layer;
	}

	template<class LayerClass, used_t>
	inline LayerClass * Widget::attach(const UniqueHandle<LayerClass> & layer)
	{
		_layers.insert(std::upper_bound(_layers.begin(), _layers.end(), layer, WidgetLayer::Sorter()), layer); // sorted insert
		return layer;
	}

	template<class Drawer, used_t>
	inline CustomLayer * Widget::attach(const Drawer & drawer, int order)
	{
		return append<CustomLayer>(drawer, order);
	}

	inline void Widget::setLayerOrder(WidgetLayer * layer, int order)
	{
		auto i = std::find(_layers.begin(), _layers.end(), layer);

		if(i == _layers.end())
			return;

		int old = layer->_order;
		layer->_order = order;

		// sorted move
		if(order < old)
			std::rotate(std::upper_bound(_layers.begin(), i, *i, WidgetLayer::Sorter()), i, std::next(i));
		else
			std::rotate(i, i + 1, std::lower_bound(std::next(i), _layers.end(), *i, WidgetLayer::Sorter()));
	}

	inline void Widget::detach(WidgetLayer * layer)
	{
		erase(_layers, layer);
	}

	template<class Component, used_t>
	inline Handle<Component> & Widget::init(Handle<Component> & component)
	{
		return component.init(this);
	}

	template<class Component, used_t>
	inline Handle<Component> Widget::require()
	{
		return _components.require<Component>(this);
	}

	template<class Component, class LinkComponent, used_t>
	inline Handle<Component> Widget::link(LinkComponent * link)
	{
		return _components.link<Component>(link, this);
	}

	template<class Component, used_t>
	inline Handle<Component> Widget::seek()
	{
		return _components.seek<Component>();
	}

	template<class Component, used_t>
	inline void Widget::detach()
	{
		_components.remove<Component>();
	}

	template<class Drawer, used_t>
	inline Widget & Widget::operator << (const Drawer & drawer)
	{
		attach(drawer);
		return *this;
	}
}

//---------------------------------------------------------------------------
#endif
