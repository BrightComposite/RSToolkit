//---------------------------------------------------------------------------

#ifndef WIDGET_H
#define WIDGET_H

//---------------------------------------------------------------------------

#include <core/Subject.h>
#include <core/container/Array.h>
#include <core/container/Set.h>
#include <core/container/TypedMap.h>

#include <graphics/Graphics.h>
#include "WidgetMessages.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Widget;

	class WidgetRegion : public Subject
	{
	public:
		WidgetRegion(Widget * parent, const IntRect & rect);
		WidgetRegion(Widget * parent, const IntPoint & pos, const IntSize & size);
		WidgetRegion(const WidgetRegion & region);

		virtual ~WidgetRegion() {}

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

		static void calculateRegionRect(IntRect & out, const IntRect & offsets, const FloatRect & anchors, const WidgetRegion & parent);
		static void calculateOffsets(IntRect & out, const WidgetRegion & region, const WidgetRegion & parent);

	protected:
		void changeSize(int width, int height, ModelMask mask);
		void changePlacement(int left, int top, int right, int bottom, ModelMask mask);
		void updateAnchors();

		create_readers(
			WidgetMoveMessage,
			AfterWidgetMoveMessage,
			WidgetResizeMessage
		);

		Widget * _parent;

		IntPoint _relPos;
		IntPoint _absPos;
		IntSize  _size;

		IntRect _offsets = {0, 0, 0, 0};
		FloatRect _anchors = {0.0f, 0.0f, 0.0f, 0.0f};

		ModelMask _alignment = ModelMask::LeftTop;
	};

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

	class Layer : public Object
	{
		friend class Widget;

	public:
		Layer(Widget * widget, int order = 0);
		virtual ~Layer();

		virtual void draw(const IntRect & clipRegion) const = 0;

		int order() const
		{
			return _order;
		}

		void setOrder(int order);

		bool operator > (const Layer & layer) const
		{
			return _order > layer._order;
		}

		bool operator < (const Layer & layer) const
		{
			return _order < layer._order;
		}

	protected:
		int _order = 0;
		Widget * _widget;
	};

	typedef std::function<void(const Widget *, const IntRect &)> WidgetDrawer;

	template<int ID>
	class CustomLayer : public Layer
	{
	public:
		CustomLayer(Widget * widget, int order = 0) : Layer(widget, order), drawer(nullptr) {}
		CustomLayer(Widget * widget, const WidgetDrawer & drawer, int order = 0) : Layer(widget, order), drawer(drawer) {}

		void set(const WidgetDrawer & drawer)
		{
			this->drawer = drawer;
		}

		virtual void draw(const IntRect & clipRegion) const override
		{
			if(drawer)
				drawer(_widget, clipRegion);
		}

	protected:
		WidgetDrawer drawer;
	};

	using DefaultLayer = CustomLayer<0>;

#define is_widget(T) based_on(T, Widget)
#define is_layer(T)  based_on(T, Layer)
#define is_drawer(T) (std::is_function<T>::value || Rapture::is_functor<T, const Widget *, const IntRect &>::value)

	attach_hash(Widget);

	class Widget : public WidgetRegion
	{
		friend class WindowAdapter;
		friend class Layer;

	public:
		Widget(Widget * parent);
		Widget(Widget * parent, const IntRect & region);
		Widget(WindowAdapter * adapter);
		Widget(WindowAdapter * adapter, const IntRect & region);
		Widget(const Widget & widget);
		Widget(const Widget & widget, Widget * parent);

		virtual ~Widget();

		virtual inline Handle<Widget> clone() const;
		virtual inline Handle<Widget> clone(Widget * parent) const;

		Handle<Widget> attach(const Handle<Widget> & child);
		Handle<Widget> detach();

		Widget * findWidget(const IntPoint & pt);

		template<class WidgetClass, typename ... A, selectif(0, is_widget(WidgetClass) && can_construct(WidgetClass, Widget *, A...))>
		inline Handle<WidgetClass> append(A && ... args);

		template<class LayerClass,  typename ... A, selectif(1, is_layer(LayerClass)   && can_construct(LayerClass,  Widget *, A...))>
		inline Handle<LayerClass> append(A && ... args);

		template<class LayerClass, useif(is_layer(LayerClass))>
		inline Handle<LayerClass> layer();

		template<class Drawer, useif(is_drawer(Drawer))>
		inline Handle<DefaultLayer> attach(const Drawer & drawer, int order = 0);

		template<int Id, class Drawer, useif(is_drawer(Drawer))>
		inline Handle<CustomLayer<Id>> attach(const Drawer & drawer, int order = 0);

		inline Widget & operator += (const Handle<Widget> & child);
		inline Widget & operator -= (Handle<Widget> & child);

		template<class Drawer, useif(is_drawer(Drawer))>
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

		bool isPointed() const;
		bool isFocused() const;
		bool isPressed() const;
		bool isPressed(MouseButton button) const;

		MouseButton pressedButtons() const;

		int displayOrder() const
		{
			return _displayOrder;
		}

		int focusOrder() const
		{
			return _focusOrder;
		}

		WindowAdapter * adapter() const;
		Graphics * graphics() const;

		void show()
		{
			setVisibility(true);
		}

		void hide()
		{
			setVisibility(false);
		}

		void focus();

		void setVisibility(bool visible);
		void setFocusability(bool focusable);
		void setDisplayOrder(int order);
		void setFocusOrder(int order);

		void bringToFront();
		void sendToBack();

	protected:
		typedef list<Widget *>::iterator iterator;

		Widget(WindowAdapter * adapter, Widget * parent, const IntRect & region);

		void draw(Graphics * graphics, const IntRect & clipRegion);

		void forgetParent();

		void removeFocus();
		void receiveFocus();

		static void sort(list<Widget *> & list, int(Widget::*sortAttribute));
		static void insert(Widget * w, list<Widget *> & list, int(Widget::*sortAttribute));
		static void remove(Widget * w, list<Widget *> & list);

		static iterator find(Widget * w, list<Widget *> & list);

		WindowAdapter * _adapter;

		list<Layer *> _layers;
		TypedMap<Layer> _layerMap;
		Set<Widget> _children;
		list<Widget *> _displayList;

		int _flags = 0;
		MouseState _mouseState;

		int _focusOrder = 0;
		int _displayOrder = 0;

		bind_messages(Widget,
			WidgetMessages
		);

		create_readers(
			KeyDownMessage, KeyUpMessage, CharMessage
		);

		override_reader(
			WidgetResizeMessage
		);
	};

	inline void WidgetRegion::setLeft(int value)
	{
		changePlacement(value, top(), right(), bottom(), ModelMask::Left);
	}

	inline void WidgetRegion::setTop(int value)
	{
		changePlacement(left(), value, right(), bottom(), ModelMask::Top);
	}

	inline void WidgetRegion::setRight(int value)
	{
		changePlacement(left(), top(), value, bottom(), ModelMask::Right);
	}

	inline void WidgetRegion::setBottom(int value)
	{
		changePlacement(left(), top(), right(), value, ModelMask::Bottom);
	}

	inline void WidgetRegion::setWidth(int value)
	{
		changeSize(value, height(), ModelMask::Horizontal);
	}

	inline void WidgetRegion::setHeight(int value)
	{
		changeSize(width(), value, ModelMask::Vertical);
	}

	inline void WidgetRegion::setRegion(const IntRect & r)
	{
		changePlacement(r.left, r.top, r.right, r.bottom, ModelMask::FullSize);
	}

	inline void WidgetRegion::setPos(int left, int top)
	{
		changePlacement(left, top, left + width(), top + height(), ModelMask::FullSize);
	}

	inline void WidgetRegion::setPos(const IntPoint & pt)
	{
		changePlacement(pt.x, pt.y, pt.x + width(), pt.y + height(), ModelMask::FullSize);
	}

	inline void WidgetRegion::setSize(int width, int height)
	{
		changeSize(width, height, ModelMask::FullSize);
	}

	inline void WidgetRegion::setSize(const IntSize & size)
	{
		changeSize(size.x, size.y, ModelMask::FullSize);
	}

	inline void WidgetRegion::setPlacement(int left, int top, int width, int height)
	{
		changePlacement(left, top, left + width, top + height, ModelMask::FullSize);
	}

	inline void WidgetRegion::setPlacement(const IntPoint & pt, const IntSize & size)
	{
		changePlacement(pt.x, pt.y, pt.x + size.x, pt.y + size.y, ModelMask::FullSize);
	}

	inline void WidgetRegion::setPlacement(ModelMask alignment, const IntRect & offsets)
	{
		_offsets = offsets;
		setAlignment(alignment);
	}

	inline void WidgetRegion::setPlacement(ModelMask alignment, int left, int top, int width, int height)
	{
		_offsets.set(left, top, left, top);
		setAlignment(alignment);
		setSize(width, height);
	}

	inline void WidgetRegion::setPlacement(ModelMask alignment, const IntPoint & offset, const IntSize & size)
	{
		_offsets.set(offset.x, offset.y, offset.x, offset.y);
		setAlignment(alignment);
		setSize(size);
	}

	inline void WidgetRegion::setPlacement(const FloatRect & anchors, const IntRect & offsets)
	{
		_alignment = ModelMask::Custom;
		_anchors = anchors;
		_offsets = offsets;

		updateAnchors();
	}

	inline void WidgetRegion::setAlignment(ModelMask alignment)
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

	inline void WidgetRegion::setOffset(int side, int value)
	{
		_offsets[side] = value;
		updateAnchors();
	}

	inline void WidgetRegion::setOffsets(const IntRect & offsets)
	{
		_offsets = offsets;
		updateAnchors();
	}

	inline void WidgetRegion::setAnchor(int side, float value)
	{
		_alignment = ModelMask::Custom;
		_anchors[side] = value;
		updateAnchors();
	}

	inline void WidgetRegion::setAnchors(const FloatRect & anchors)
	{
		_alignment = ModelMask::Custom;
		_anchors = anchors;
		updateAnchors();
	}

	inline Handle<Widget> Widget::clone() const
	{
		return Handle<Widget>(*this);
	}

	inline Handle<Widget> Widget::clone(Widget * parent) const
	{
		return Handle<Widget>(*this, parent);
	}

	template<class WidgetClass, typename ... A, selectif_t<0>>
	inline Handle<WidgetClass> Widget::append(A && ... args)
	{
		return Handle<WidgetClass>(this, forward<A>(args)...);
	}

	template<class LayerClass, typename ... A, selectif_t<1>>
	inline Handle<LayerClass> Widget::append(A && ... args)
	{
		return _layerMap.construct<LayerClass>(this, forward<A>(args)...);
	}

	template<class Drawer, useif_t>
	inline Handle<DefaultLayer> Widget::attach(const Drawer & drawer, int order)
	{
		return append<DefaultLayer>(drawer, order);
	}

	template<int Id, class Drawer, useif_t>
	inline Handle<CustomLayer<Id>> Widget::attach(const Drawer & drawer, int order)
	{
		return append<CustomLayer<Id>>(drawer, order);
	}

	template<class LayerClass, useif_t>
	inline Handle<LayerClass> Widget::layer()
	{
		return _layerMap.request<LayerClass>(this);
	}

	inline Widget & Widget::operator += (const Handle<Widget> & child)
	{
		attach(child);
		return *this;
	}

	inline Widget & Widget::operator -= (Handle<Widget> & child)
	{
		if(child->_parent == this)
			child->detach();

		return *this;
	}

	template<class Drawer, useif_t>
	inline Widget & Widget::operator << (const Drawer & drawer)
	{
		attach(drawer);
		return *this;
	}
}

namespace std
{
	template<>
	use_class_hash(Rapture::Widget);
}

//---------------------------------------------------------------------------
#endif
