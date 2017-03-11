//---------------------------------------------------------------------------

#pragma once

#ifndef WIDGET_H
#define WIDGET_H

//---------------------------------------------------------------------------

#include <container/ArrayList.h>
#include <morph/Component.h>
#include <morph/Property.h>

#include <message/Subject.h>

#include <graphics/ScreenCoord.h>

#include "WidgetMessages.h"

//---------------------------------------------------------------------------

namespace asd
{
	class Widget;
	class UISpace;
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
		MouseButton buttons() const {
			return _buttons;
		}

		bool isPressed() const {
			return _buttons != MouseButton::None;
		}

		bool isPressed(MouseButton button) const {
			return check_flag(button, _buttons);
		}

		bool hasPressed(MouseButton buttons) const {
			return has_some_flags(buttons, _buttons);
		}

		void press(MouseButton button) {
			set_flag(button, _buttons);
		}

		void unpress(MouseButton button) {
			clear_flag(button, _buttons);
		}

		void unpress() {
			_buttons = MouseButton::None;
		}

	protected:
		MouseButton _buttons = MouseButton::None;
	};

	class WidgetComponent : public Component, public Connector
	{
		components_origin(WidgetComponent);

	public:
		WidgetComponent(Widget * widget) : _widget(widget) {}
		virtual ~WidgetComponent() {}

		virtual Handle<WidgetComponent> clone(Widget * widget) const = 0;

	protected:
		Widget * _widget;
	};

	create_components_from(ui, WidgetComponent);

	class WidgetProperty : public Property
	{
		properties_origin(WidgetProperty);
	};

	create_properties_from(ui, WidgetProperty);

	class Widget : public Subject
	{
		deny_copy(Widget);

		friend class UISpace;
		friend class WidgetLayer;

		class Tree;

	public:
		api(ui) Widget(Widget * parent);
		api(ui) Widget(Widget * parent, const IntRect & region);
		api(ui) Widget(UISpace * space);
		api(ui) Widget(UISpace * space, const IntRect & region);
		api(ui) Widget(const Widget & widget, Widget * parent);

		virtual api(ui) ~Widget();

		virtual inline Handle<Widget> clone(Widget * parent = nullptr) const;

		inline const IntSize & size() const;
		inline int width() const;
		inline int height() const;

		inline const IntPoint & pos() const;
		inline int left() const;
		inline int top() const;
		inline int right() const;
		inline int bottom() const;
		inline int side(int index) const;

		inline operator IntRect () const;
		inline IntRect region() const;
		inline IntRect localRegion() const;
		inline Viewport viewport() const;

		inline const IntPoint & absPos() const;
		inline int absLeft() const;
		inline int absTop() const;
		inline int absRight() const;
		inline int absBottom() const;

		inline IntRect absRegion() const;

		inline ModelMask alignment() const;
		inline const IntRect & offsets() const;
		inline const FloatRect & anchors() const;

		inline bool isVisible() const;
		inline bool isFocusable() const;
		api(ui) bool isPointed() const;
		api(ui) bool isFocused() const;
		api(ui) bool isPressed() const;
		api(ui) bool isPressed(MouseButton button) const;

		api(ui) MouseButton pressedButtons() const;

		inline int displayOrder() const;
		inline int focusOrder() const;

		api(ui) UISpace * space() const;
		api(ui) Graphics * graphics() const;

		api(ui) Widget * findAt(const IntPoint & pt);

		inline Widget * child(int idx) const;

		inline Tree tree();

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

		template<class WidgetClass, typename ... A, selectif(0) < is_widget<WidgetClass>::value, can_construct<WidgetClass, Widget *, A...>::value > >
		inline Handle<WidgetClass> append(A && ...);
		template<class LayerClass, typename ... A, selectif(1) < is_layer<LayerClass>::value, !is_widget_component<LayerClass>::value, can_construct<LayerClass, A...>::value > >
		inline LayerClass * append(A && ...);

		api(ui) Widget * attach(const Handle<Widget> & child);
		api(ui) void detach(Widget * child);

		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		inline LayerClass * attach(LayerClass * layer);
		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		inline LayerClass * attach(const Handle<LayerClass> & layer);
		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		inline LayerClass * attach(const Unique<LayerClass> & layer);
		template<class Drawer, useif<is_widget_drawer<Drawer>::value>>
		inline CustomLayer * attach(const Drawer & drawer, int order = 0);
		inline void setLayerOrder(WidgetLayer * layer, int order);
		inline void detach(WidgetLayer * layer);

		template<class Drawer, useif<is_widget_drawer<Drawer>::value>>
		inline Widget & operator << (const Drawer & drawer);

		api(ui) void show();
		api(ui) void hide();
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

		bind_components(Widget, WidgetComponent);
		bind_properties(Widget, WidgetProperty);

	protected:
		api(ui) Widget(UISpace * space, Widget * parent, const IntRect & region);

		api(ui) void draw(Graphics * graphics, const IntRect & clipRegion);

		api(ui) void removeFocus();
		api(ui) void receiveFocus();

		api(ui) void changeSize(int width, int height, ModelMask mask);
		api(ui) void changePlacement(int left, int top, int right, int bottom, ModelMask mask);
		api(ui) void updateAnchors();

	private:
		UISpace * _space;
		Widget * _parent;
		Set<Widget> _children;

		IntPoint _relPos;
		IntPoint _absPos;
		IntSize  _size;

		IntRect _offsets = {0, 0, 0, 0};
		FloatRect _anchors = {0.0f, 0.0f, 0.0f, 0.0f};

		ModelMask _alignment = ModelMask::LeftTop;

		array_list<WidgetLayer *> _layers;
		array_list<Widget *> _displayList;

		int _flags = 0;
		MouseState _mouseState;

		int _focusOrder = 0;
		int _displayOrder = 0;

		using DisplaySort = MemberSort<Widget, int, &Widget::_displayOrder>;
		using FocusSort = MemberSort<Widget, int, &Widget::_focusOrder>;

		class Tree : public asd::tree<Tree, Set<Widget>>
		{
		public:
			using asd::tree<Tree, Set<Widget>>::tree;

		private:
			friend asd::tree<Tree, Set<Widget>>::iterator;

			static auto child_begin(typename Set<Widget>::iterator & i) {
				return i->_()->_children.begin();
			}

			static auto child_end(typename Set<Widget>::iterator & i) {
				return i->_()->_children.end();
			}
		};

		using iterator_t = tree_iterator<Tree>;

		bind_messages(Widget, WidgetMessages)
	};

	channels_api(ui, Widget, WidgetMessages)

		template<>
	struct hash<Widget> : hash<Subject> {};

	class WidgetLayer : public Shared
	{
		friend class Widget;

	public:
		WidgetLayer(int order = 0) : _order(order) {}
		virtual ~WidgetLayer() {}

		virtual Handle<WidgetLayer> clone(Widget * widget) const = 0;

		int order() const {
			return _order;
		}

	protected:
		virtual void draw(Widget * w) = 0;

		int _order;

		using Sorter = MemberSort<WidgetLayer, int, &WidgetLayer::_order>;
	};

	template<class T>
	class WidgetLayerComponent : public WidgetComponent
	{
		static_assert(based_on<T, WidgetLayer>::value, "Template argument of the WidgetLayerComponent class must inherit the WidgetLayer");

	public:
		WidgetLayerComponent(Widget * widget) : WidgetComponent(widget) {}

		virtual ~WidgetLayerComponent() {
			if(_layer != nullptr)
				_widget->detach(_layer);
		}

		T * layer() const {
			return _layer;
		}

		virtual Handle<WidgetComponent> clone(Widget * w) const override {
			Handle<WidgetLayerComponent> c(w);

			if(_layer)
				c->set(cast::as<T>(_layer->clone(w)));

			return c;
		}

		template<class ... A, useif<can_construct<T, A...>::value>>
		T * init(A &&... args) {
			if(_layer != nullptr)
				_widget->detach(_layer);

			_layer.init(forward<A>(args)...);
			_widget->attach(_layer);

			return _layer;
		}

		void set(Unique<T> && layer) {
			if(_layer != nullptr)
				_widget->detach(_layer);

			_layer = move(layer);
			_widget->attach(_layer);
		}

	protected:
		Unique<T> _layer;
	};

#define create_layer_component(module, ...) create_component(module, WidgetLayerComponent<__VA_ARGS__>);

	typedef std::function<void(const Widget *, const IntRect &)> WidgetDrawer;

	class CustomLayer : public WidgetLayer
	{
	public:
		CustomLayer(const WidgetDrawer & drawer, int order = 0) : WidgetLayer(order), drawer(drawer) {}
		virtual ~CustomLayer() {}

		virtual Handle<WidgetLayer> clone(Widget * widget) const override {
			return Handle<CustomLayer>(drawer, _order);
		}

	protected:
		virtual api(ui) void draw(Widget * w) override;

		WidgetDrawer drawer;
	};

	class CustomLayerComponent : public WidgetComponent
	{
	public:
		CustomLayerComponent(Widget * w) : WidgetComponent(w) {}

		virtual Handle<WidgetComponent> clone(Widget * widget) const override {
			Handle<CustomLayerComponent> c(widget);

			for(auto & l : _layers)
				c->add(l->clone(widget));

			return c;
		}

		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		LayerClass * add(const Handle<LayerClass> & layer) {
			_layers.push_back(layer);
			return _widget->attach(layer);
		}

	protected:
		ArrayList<WidgetLayer> _layers;
	};

	create_component(ui, CustomLayerComponent);

	inline const IntSize & Widget::size() const {
		return _size;
	}

	inline int Widget::width() const {
		return _size.x;
	}

	inline int Widget::height() const {
		return _size.y;
	}

	inline const IntPoint & Widget::pos() const {
		return _relPos;
	}

	inline int Widget::side(int index) const {
		switch(index) {
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

	inline int Widget::left() const {
		return _relPos.x;
	}

	inline int Widget::top() const {
		return _relPos.y;
	}

	inline int Widget::right() const {
		return _relPos.x + _size.x;
	}

	inline int Widget::bottom() const {
		return _relPos.y + _size.y;
	}

	inline Widget::operator IntRect () const {
		return {_relPos, _size};
	}

	inline IntRect Widget::region() const {
		return {_relPos, _size};
	}

	inline IntRect Widget::localRegion() const {
		return {_size};
	}

	inline Viewport Widget::viewport() const {
		return {_size};
	}

	inline const IntPoint & Widget::absPos() const {
		return _absPos;
	}

	inline int Widget::absLeft() const {
		return _absPos.x;
	}

	inline int Widget::absTop() const {
		return _absPos.y;
	}

	inline int Widget::absRight() const {
		return _absPos.x + _size.x;
	}

	inline int Widget::absBottom() const {
		return _absPos.y + _size.y;
	}

	inline IntRect Widget::absRegion() const {
		return {_absPos, _size};
	}

	inline ModelMask Widget::alignment() const {
		return _alignment;
	}

	inline const IntRect & Widget::offsets() const {
		return _offsets;
	}

	inline const FloatRect & Widget::anchors() const {
		return _anchors;
	}

	inline bool Widget::isVisible() const {
		return check_flag(WidgetFlag::Visible, _flags);
	}

	inline bool Widget::isFocusable() const {
		return check_flag(WidgetFlag::Focusable, _flags);
	}

	inline int Widget::displayOrder() const {
		return _displayOrder;
	}

	inline int Widget::focusOrder() const {
		return _focusOrder;
	}

	inline Widget * Widget::child(int idx) const {
		return _displayList[idx];
	}

	inline typename Widget::Tree Widget::tree() {
		return {_children};
	}

	inline void Widget::setLeft(int value) {
		changePlacement(value, top(), right(), bottom(), ModelMask::Left);
	}

	inline void Widget::setTop(int value) {
		changePlacement(left(), value, right(), bottom(), ModelMask::Top);
	}

	inline void Widget::setRight(int value) {
		changePlacement(left(), top(), value, bottom(), ModelMask::Right);
	}

	inline void Widget::setBottom(int value) {
		changePlacement(left(), top(), right(), value, ModelMask::Bottom);
	}

	inline void Widget::setWidth(int value) {
		changeSize(value, height(), ModelMask::Horizontal);
	}

	inline void Widget::setHeight(int value) {
		changeSize(width(), value, ModelMask::Vertical);
	}

	inline void Widget::setRegion(const IntRect & r) {
		changePlacement(r.left, r.top, r.right, r.bottom, ModelMask::FullSize);
	}

	inline void Widget::setPos(int left, int top) {
		changePlacement(left, top, left + width(), top + height(), ModelMask::FullSize);
	}

	inline void Widget::setPos(const IntPoint & pt) {
		changePlacement(pt.x, pt.y, pt.x + width(), pt.y + height(), ModelMask::FullSize);
	}

	inline void Widget::setSize(int width, int height) {
		changeSize(width, height, ModelMask::FullSize);
	}

	inline void Widget::setSize(const IntSize & size) {
		changeSize(size.x, size.y, ModelMask::FullSize);
	}

	inline void Widget::setPlacement(int left, int top, int width, int height) {
		changePlacement(left, top, left + width, top + height, ModelMask::FullSize);
	}

	inline void Widget::setPlacement(const IntPoint & pt, const IntSize & size) {
		changePlacement(pt.x, pt.y, pt.x + size.x, pt.y + size.y, ModelMask::FullSize);
	}

	inline void Widget::setPlacement(ModelMask alignment, const IntRect & offsets) {
		_offsets = offsets;
		setAlignment(alignment);
	}

	inline void Widget::setPlacement(ModelMask alignment, int left, int top, int width, int height) {
		_offsets.set(left, top, left, top);
		setAlignment(alignment);
		setSize(width, height);
	}

	inline void Widget::setPlacement(ModelMask alignment, const IntPoint & offset, const IntSize & size) {
		_offsets.set(offset.x, offset.y, offset.x, offset.y);
		setAlignment(alignment);
		setSize(size);
	}

	inline void Widget::setPlacement(const FloatRect & anchors, const IntRect & offsets) {
		_alignment = ModelMask::Custom;
		_anchors = anchors;
		_offsets = offsets;

		updateAnchors();
	}

	inline void Widget::setAlignment(ModelMask alignment) {
		if(_alignment == alignment)
			return;

		_alignment = alignment;

		if(_alignment == ModelMask::Custom)
			return;

		_anchors.left = check_flag(ModelMask::Left, _alignment) ? 0.0f : 1.0f;
		_anchors.top = check_flag(ModelMask::Top, _alignment) ? 0.0f : 1.0f;
		_anchors.right = check_flag(ModelMask::Right, _alignment) ? 1.0f : 0.0f;
		_anchors.bottom = check_flag(ModelMask::Bottom, _alignment) ? 1.0f : 0.0f;

		updateAnchors();
	}

	inline void Widget::setOffset(int side, int value) {
		_offsets[side] = value;
		updateAnchors();
	}

	inline void Widget::setOffsets(const IntRect & offsets) {
		_offsets = offsets;
		updateAnchors();
	}

	inline void Widget::setAnchor(int side, float value) {
		_alignment = ModelMask::Custom;
		_anchors[side] = value;
		updateAnchors();
	}

	inline void Widget::setAnchors(const FloatRect & anchors) {
		_alignment = ModelMask::Custom;
		_anchors = anchors;
		updateAnchors();
	}

	inline Handle<Widget> Widget::clone(Widget * parent) const {
		return Handle<Widget>(*this, parent);
	}

	template<class WidgetClass, typename ... A, selected_t(0)> // append widget
	inline Handle<WidgetClass> Widget::append(A && ... args) {
		return Handle<WidgetClass>(this, forward<A>(args)...);
	}

	template<class LayerClass, typename ... A, selected_t(1)>
	inline LayerClass * Widget::append(A && ... args) // append layer
	{
		return components->template require<CustomLayerComponent>()->add(Handle<LayerClass>(forward<A>(args)...));
	}

	template<class LayerClass, used_t>
	inline LayerClass * Widget::attach(LayerClass * layer) {
		_layers.insert(std::upper_bound(_layers.begin(), _layers.end(), layer, WidgetLayer::Sorter()), layer); // sorted insert
		return layer;
	}

	template<class LayerClass, used_t>
	inline LayerClass * Widget::attach(const Handle<LayerClass> & layer) {
		_layers.insert(std::upper_bound(_layers.begin(), _layers.end(), layer, WidgetLayer::Sorter()), layer); // sorted insert
		return layer;
	}

	template<class LayerClass, used_t>
	inline LayerClass * Widget::attach(const Unique<LayerClass> & layer) {
		_layers.emplace(std::upper_bound(_layers.begin(), _layers.end(), layer, WidgetLayer::Sorter()), layer); // sorted insert
		return layer;
	}

	template<class Drawer, used_t>
	inline CustomLayer * Widget::attach(const Drawer & drawer, int order) {
		return append<CustomLayer>(drawer, order);
	}

	inline void Widget::setLayerOrder(WidgetLayer * layer, int order) {
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

	inline void Widget::detach(WidgetLayer * layer) {
		erase(_layers, layer);
	}

	template<class Drawer, used_t>
	inline Widget & Widget::operator << (const Drawer & drawer) {
		attach(drawer);
		return *this;
	}
}

//---------------------------------------------------------------------------
#endif
