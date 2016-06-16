//---------------------------------------------------------------------------

#ifndef WIDGET_H
#define WIDGET_H

//---------------------------------------------------------------------------

#include <core/Subject.h>
#include <core/container/Array.h>
#include <core/container/TypedSet.h>

#include <graphics/Graphics.h>
#include "WidgetMessages.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Widget;
	class BasicWidget;
	class Layer;

	link_class(ui, BasicWidget, Class<Subject>);
	link_class(ui, Widget, Class<BasicWidget>);

	class BasicWidget : public Subject
	{
		friend class Widget;

	public:
		api(ui) BasicWidget(Widget * parent, const IntRect & rect);
		api(ui) BasicWidget(Widget * parent, const IntPoint & pos, const IntSize & size);
		api(ui) BasicWidget(const BasicWidget & w);

		virtual ~BasicWidget() {}

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

		static void api(ui) calculateRegionRect(IntRect & out, const IntRect & offsets, const FloatRect & anchors, const BasicWidget & parent);
		static void api(ui) calculateOffsets(IntRect & out, const BasicWidget & region, const BasicWidget & parent);

		virtual void api(ui) read(Handle<KeyDownMessage> & msg) {}
		virtual void api(ui) read(Handle<CharMessage> & msg) {}
		virtual void api(ui) read(Handle<KeyUpMessage> & msg) {}
		virtual void api(ui) read(Handle<MouseUpdateMessage> & msg) {}
		virtual void api(ui) read(Handle<MouseDownMessage> & msg) {}
		virtual void api(ui) read(Handle<MouseMoveMessage> & msg) {}
		virtual void api(ui) read(Handle<MouseUpMessage> & msg) {}
		virtual void api(ui) read(Handle<MouseClickMessage> & msg) {}
		virtual void api(ui) read(Handle<MouseDblClickMessage> & msg) {}
		virtual void api(ui) read(Handle<MouseWheelMessage> & msg) {}
		virtual void api(ui) read(Handle<MouseEnterMessage> & msg) {}
		virtual void api(ui) read(Handle<MouseLeaveMessage> & msg) {}
		virtual void api(ui) read(Handle<WidgetPressMessage> & msg) {}
		virtual void api(ui) read(Handle<WidgetStopPressMessage> & msg) {}
		virtual void api(ui) read(Handle<WidgetReleaseMessage> & msg) {}
		virtual void api(ui) read(Handle<WidgetChangedStateMessage> & msg) {}
		virtual void api(ui) read(Handle<ChangeFocusOrderMessage> & msg) {}
		virtual void api(ui) read(Handle<AfterChangeFocusOrderMessage> & msg) {}
		virtual void api(ui) read(Handle<ChangeDisplayOrderMessage> & msg) {}
		virtual void api(ui) read(Handle<AfterChangeDisplayOrderMessage> & msg) {}
		virtual void api(ui) read(Handle<WidgetDrawMessage> & msg) {}
		virtual void api(ui) read(Handle<WidgetMoveMessage> & msg) {}
		virtual void api(ui) read(Handle<AfterWidgetMoveMessage> & msg) {}
		virtual void api(ui) read(Handle<WidgetResizeMessage> & msg) {}

	protected:
		void api(ui) changeSize(int width, int height, ModelMask mask);
		void api(ui) changePlacement(int left, int top, int right, int bottom, ModelMask mask);
		void api(ui) updateAnchors();

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

	class Layer : public Shared
	{
		friend class Widget;

	public:
		api(ui) Layer(Widget * widget, int order = 0);
		virtual api(ui) ~Layer();

		virtual void draw(const IntRect & clipRegion) const = 0;

		int order() const
		{
			return _order;
		}

		void api(ui) setOrder(int order);

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

	typebase_api(ui, Layer);

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

	typeid_api(ui, CustomLayer<0>, Layer);
	typeid_api(ui, CustomLayer<1>, Layer);
	typeid_api(ui, CustomLayer<2>, Layer);
	typeid_api(ui, CustomLayer<3>, Layer);
	typeid_api(ui, CustomLayer<4>, Layer);
	typeid_api(ui, CustomLayer<5>, Layer);
	typeid_api(ui, CustomLayer<6>, Layer);
	typeid_api(ui, CustomLayer<7>, Layer);
	typeid_api(ui, CustomLayer<8>, Layer);
	typeid_api(ui, CustomLayer<9>, Layer);

#define is_widget(T) based_on<T, Widget>::value
#define is_layer(T)  based_on<T, Layer>::value
#define is_drawer(T) (std::is_function<T>::value || Rapture::is_callable<T, const Widget *, const IntRect &>::value)

	class Widget : public BasicWidget
	{
		deny_copy(Widget);

		friend class UISpace;
		friend class Layer;

	public:
		api(ui) Widget(Widget * parent);
		api(ui) Widget(Widget * parent, const IntRect & region);
		api(ui) Widget(UISpace * space);
		api(ui) Widget(UISpace * space, const IntRect & region);
		api(ui) Widget(const Widget & widget, Widget * parent);

		virtual api(ui) ~Widget();

		virtual inline Handle<Widget> clone() const;
		virtual inline Handle<Widget> clone(Widget * parent) const;

		Handle<Widget> api(ui) attach(const Handle<Widget> & child);
		Handle<Widget> api(ui) detach();

		Widget * findWidget(const IntPoint & pt);

		template<class WidgetClass, typename ... A, selectif(0) <is_widget(WidgetClass), can_construct<WidgetClass, Widget *, A...>::value> endif>
		inline Handle<WidgetClass> append(A && ... args);

		template<class LayerClass,  typename ... A, selectif(1) <is_layer(LayerClass), can_construct<LayerClass,  Widget *, A...>::value> endif>
		inline Handle<LayerClass> append(A && ... args);

		template<class LayerClass, useif <is_layer(LayerClass)> endif>
		inline Handle<LayerClass> layer();

		template<class Drawer, useif <is_drawer(Drawer)> endif>
		inline Handle<DefaultLayer> attach(const Drawer & drawer, int order = 0);

		template<int Id, class Drawer, useif <is_drawer(Drawer)> endif>
		inline Handle<CustomLayer<Id>> attach(const Drawer & drawer, int order = 0);

		inline Widget & operator += (const Handle<Widget> & child);
		inline Widget & operator -= (Handle<Widget> & child);

		template<class Drawer, useif <is_drawer(Drawer)> endif>
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

		bool api(ui) isPointed() const;
		bool api(ui) isFocused() const;
		bool api(ui) isPressed() const;
		bool api(ui) isPressed(MouseButton button) const;

		MouseButton api(ui) pressedButtons() const;

		int displayOrder() const
		{
			return _displayOrder;
		}

		int focusOrder() const
		{
			return _focusOrder;
		}

		UISpace api(ui) * space() const;
		Graphics api(ui) * graphics() const;

		void show()
		{
			setVisibility(true);
		}

		void hide()
		{
			setVisibility(false);
		}

		void api(ui) focus();

		void api(ui) setVisibility(bool visible);
		void api(ui) setFocusability(bool focusable);
		void api(ui) setDisplayOrder(int order);
		void api(ui) setFocusOrder(int order);

		void api(ui) bringToFront();
		void api(ui) sendToBack();

		using BasicWidget::read;

		virtual void api(ui) read(Handle<KeyDownMessage> & msg) override;
		virtual void api(ui) read(Handle<CharMessage> & msg) override;
		virtual void api(ui) read(Handle<KeyUpMessage> & msg) override;
		virtual void api(ui) read(Handle<WidgetResizeMessage> & msg) override;

	protected:
		api(ui) Widget(UISpace * space, Widget * parent, const IntRect & region);

		void api(ui) draw(Graphics * graphics, const IntRect & clipRegion);

		void api(ui) forgetParent();

		void api(ui) removeFocus();
		void api(ui) receiveFocus();

		UISpace * _space;

		TypedSet<Layer> _layers;
		SubjectSet<BasicWidget> _children;

		array_list<Layer *> _layerList;
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

	inline void BasicWidget::setLeft(int value)
	{
		changePlacement(value, top(), right(), bottom(), ModelMask::Left);
	}

	inline void BasicWidget::setTop(int value)
	{
		changePlacement(left(), value, right(), bottom(), ModelMask::Top);
	}

	inline void BasicWidget::setRight(int value)
	{
		changePlacement(left(), top(), value, bottom(), ModelMask::Right);
	}

	inline void BasicWidget::setBottom(int value)
	{
		changePlacement(left(), top(), right(), value, ModelMask::Bottom);
	}

	inline void BasicWidget::setWidth(int value)
	{
		changeSize(value, height(), ModelMask::Horizontal);
	}

	inline void BasicWidget::setHeight(int value)
	{
		changeSize(width(), value, ModelMask::Vertical);
	}

	inline void BasicWidget::setRegion(const IntRect & r)
	{
		changePlacement(r.left, r.top, r.right, r.bottom, ModelMask::FullSize);
	}

	inline void BasicWidget::setPos(int left, int top)
	{
		changePlacement(left, top, left + width(), top + height(), ModelMask::FullSize);
	}

	inline void BasicWidget::setPos(const IntPoint & pt)
	{
		changePlacement(pt.x, pt.y, pt.x + width(), pt.y + height(), ModelMask::FullSize);
	}

	inline void BasicWidget::setSize(int width, int height)
	{
		changeSize(width, height, ModelMask::FullSize);
	}

	inline void BasicWidget::setSize(const IntSize & size)
	{
		changeSize(size.x, size.y, ModelMask::FullSize);
	}

	inline void BasicWidget::setPlacement(int left, int top, int width, int height)
	{
		changePlacement(left, top, left + width, top + height, ModelMask::FullSize);
	}

	inline void BasicWidget::setPlacement(const IntPoint & pt, const IntSize & size)
	{
		changePlacement(pt.x, pt.y, pt.x + size.x, pt.y + size.y, ModelMask::FullSize);
	}

	inline void BasicWidget::setPlacement(ModelMask alignment, const IntRect & offsets)
	{
		_offsets = offsets;
		setAlignment(alignment);
	}

	inline void BasicWidget::setPlacement(ModelMask alignment, int left, int top, int width, int height)
	{
		_offsets.set(left, top, left, top);
		setAlignment(alignment);
		setSize(width, height);
	}

	inline void BasicWidget::setPlacement(ModelMask alignment, const IntPoint & offset, const IntSize & size)
	{
		_offsets.set(offset.x, offset.y, offset.x, offset.y);
		setAlignment(alignment);
		setSize(size);
	}

	inline void BasicWidget::setPlacement(const FloatRect & anchors, const IntRect & offsets)
	{
		_alignment = ModelMask::Custom;
		_anchors = anchors;
		_offsets = offsets;

		updateAnchors();
	}

	inline void BasicWidget::setAlignment(ModelMask alignment)
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

	inline void BasicWidget::setOffset(int side, int value)
	{
		_offsets[side] = value;
		updateAnchors();
	}

	inline void BasicWidget::setOffsets(const IntRect & offsets)
	{
		_offsets = offsets;
		updateAnchors();
	}

	inline void BasicWidget::setAnchor(int side, float value)
	{
		_alignment = ModelMask::Custom;
		_anchors[side] = value;
		updateAnchors();
	}

	inline void BasicWidget::setAnchors(const FloatRect & anchors)
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

	template<class WidgetClass, typename ... A, selectif_t<0>>
	inline Handle<WidgetClass> Widget::append(A && ... args)
	{
		return Handle<WidgetClass>(this, forward<A>(args)...);
	}

	template<class LayerClass, typename ... A, selectif_t<1>>
	inline Handle<LayerClass> Widget::append(A && ... args)
	{
		return _layers.construct<LayerClass>(this, forward<A>(args)...);
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
		return _layers.require<LayerClass>(this);
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

//---------------------------------------------------------------------------
#endif
