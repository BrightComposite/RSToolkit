//---------------------------------------------------------------------------

#pragma once

#ifndef WIDGET_H
#define WIDGET_H

//---------------------------------------------------------------------------

#include <container/array_list.h>
#include <morph/component.h>
#include <morph/property.h>

#include <message/subject.h>

#include <graphics/ScreenCoord.h>

#include "WidgetMessages.h"

#undef None

//---------------------------------------------------------------------------

namespace asd
{
	class widget;
	class ui_space;
	class widget_component;
	class WidgetLayer;
	class graphics;
	class CustomLayer;

	template<class T>
	using is_widget = based_on<T, widget>;
	template<class T>
	using is_widget_component = based_on<T, widget_component>;
	template<class T>
	using is_layer = based_on<T, WidgetLayer>;
	template<class T>
	using is_widget_drawer = is_callable<T, const widget *, const math::int_rect &>;

	class mouse_state
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

	class widget_component : public component, public Connector
	{
		components_origin(widget_component);

	public:
		widget_component(widget * widget) : _widget(widget) {}
		virtual ~widget_component() {}

		virtual handle<widget_component> clone(widget * widget) const = 0;

	protected:
		widget * _widget;
	};

	create_components_from(ui, widget_component);

	class widget_property : public Property
	{
		properties_origin(widget_property);
	};

	create_properties_from(ui, widget_property);

	class widget : public subject
	{
		deny_copy(widget);

		friend class ui_space;
		friend class WidgetLayer;

		class Tree;

	public:
		api(ui) widget(widget * parent);
		api(ui) widget(widget * parent, const math::int_rect & region);
		api(ui) widget(ui_space * space);
		api(ui) widget(ui_space * space, const math::int_rect & region);
		api(ui) widget(const widget & w, widget * parent);

		virtual api(ui) ~widget();

		virtual inline handle<widget> clone(widget * parent = nullptr) const;

		inline const math::int_size & size() const;
		inline int width() const;
		inline int height() const;

		inline const math::int_point & pos() const;
		inline int left() const;
		inline int top() const;
		inline int right() const;
		inline int bottom() const;
		inline int side(int index) const;

		inline operator math::int_rect () const;
		inline math::int_rect region() const;
		inline math::int_rect localRegion() const;
		inline asd::viewport viewport() const;

		inline const math::int_point & absPos() const;
		inline int absLeft() const;
		inline int absTop() const;
		inline int absRight() const;
		inline int absBottom() const;

		inline math::int_rect absRegion() const;

		inline ModelMask alignment() const;
		inline const math::int_rect & offsets() const;
		inline const math::float_rect & anchors() const;

		inline bool isVisible() const;
		inline bool isFocusable() const;
		api(ui) bool isPointed() const;
		api(ui) bool isFocused() const;
		api(ui) bool isPressed() const;
		api(ui) bool isPressed(MouseButton button) const;

		api(ui) MouseButton pressedButtons() const;

		inline int displayOrder() const;
		inline int focusOrder() const;

		api(ui) ui_space * space() const;
		api(ui) asd::graphics * graphics() const;

		api(ui) widget * findAt(const math::int_point & pt);

		inline widget * child(int idx) const;

		inline Tree tree();

		inline void setLeft(int value);
		inline void setTop(int value);
		inline void setRight(int value);
		inline void setBottom(int value);
		inline void setWidth(int value);
		inline void setHeight(int value);
		inline void setPos(int left, int top);
		inline void setPos(const int_point & pt);
		inline void setRegion(const int_rect & region);
		inline void setSize(int width, int height);
		inline void setSize(const int_size & size);
		inline void setPlacement(int left, int top, int width, int height);
		inline void setPlacement(const int_point & pt, const int_size & size);
		inline void setPlacement(ModelMask alignment, const int_rect & offsets);
		inline void setPlacement(ModelMask alignment, const int_point & offset, const int_size & size);
		inline void setPlacement(ModelMask alignment, int left, int top, int width, int height);
		inline void setPlacement(const float_rect & anchors, const int_rect & offsets);

		inline void setAlignment(ModelMask alignment);
		inline void setOffset(int side, int value);
		inline void setAnchor(int side, float value);
		inline void setOffsets(const int_rect & offsets);
		inline void setAnchors(const float_rect & anchors);

		static api(ui) void calculateRegionRect(int_rect & out, const int_rect & offsets, const float_rect & anchors, const widget & parent);
		static api(ui) void calculateOffsets(int_rect & out, const widget & region, const widget & parent);

		template<class WidgetClass, typename ... A, selectif(0) < is_widget<WidgetClass>::value, can_construct<WidgetClass, widget *, A...>::value > >
		inline handle<WidgetClass> append(A && ...);
		template<class LayerClass, typename ... A, selectif(1) < is_layer<LayerClass>::value, !is_widget_component<LayerClass>::value, can_construct<LayerClass, A...>::value > >
		inline LayerClass * append(A && ...);

		api(ui) widget * attach(const handle<widget> & child);
		api(ui) void detach(widget * child);

		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		inline LayerClass * attach(LayerClass * layer);
		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		inline LayerClass * attach(const handle<LayerClass> & layer);
		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		inline LayerClass * attach(const unique<LayerClass> & layer);
		template<class Drawer, useif<is_widget_drawer<Drawer>::value>>
		inline CustomLayer * attach(const Drawer & drawer, int order = 0);
		inline void setLayerOrder(WidgetLayer * layer, int order);
		inline void detach(WidgetLayer * layer);

		template<class Drawer, useif<is_widget_drawer<Drawer>::value>>
		inline widget & operator << (const Drawer & drawer);

		api(ui) void show();
		api(ui) void hide();
		api(ui) void focus();

		api(ui) void setVisibility(bool visible);
		api(ui) void setFocusability(bool focusable);
		api(ui) void setDisplayOrder(int order);
		api(ui) void setFocusOrder(int order);

		api(ui) void bringToFront();
		api(ui) void sendToBack();

		virtual api(ui) void read(handle<KeyDownMessage> &);
		virtual api(ui) void read(handle<CharMessage> &);
		virtual api(ui) void read(handle<KeyUpMessage> &);
		virtual api(ui) void read(handle<MouseUpdateMessage> &);
		virtual api(ui) void read(handle<MouseDownMessage> &);
		virtual api(ui) void read(handle<MouseMoveMessage> &);
		virtual api(ui) void read(handle<MouseUpMessage> &);
		virtual api(ui) void read(handle<MouseClickMessage> &);
		virtual api(ui) void read(handle<MouseDblClickMessage> &);
		virtual api(ui) void read(handle<MouseWheelMessage> &);
		virtual api(ui) void read(handle<MouseEnterMessage> &);
		virtual api(ui) void read(handle<MouseLeaveMessage> &);
		virtual api(ui) void read(handle<WidgetPressMessage> &);
		virtual api(ui) void read(handle<WidgetStopPressMessage> &);
		virtual api(ui) void read(handle<WidgetReleaseMessage> &);
		virtual api(ui) void read(handle<WidgetChangedStateMessage> &);
		virtual api(ui) void read(handle<ChangeFocusOrderMessage> &);
		virtual api(ui) void read(handle<AfterChangeFocusOrderMessage> &);
		virtual api(ui) void read(handle<ChangeDisplayOrderMessage> &);
		virtual api(ui) void read(handle<AfterChangeDisplayOrderMessage> &);
		virtual api(ui) void read(handle<WidgetDrawMessage> &);
		virtual api(ui) void read(handle<WidgetMoveMessage> &);
		virtual api(ui) void read(handle<AfterWidgetMoveMessage> &);
		virtual api(ui) void read(handle<WidgetResizeMessage> &);

		bind_components(widget, widget_component);
		bind_properties(widget, widget_property);

	protected:
		api(ui) widget(ui_space * space, widget * parent, const int_rect & region);

		api(ui) void draw(asd::graphics * graphics, const int_rect & clipRegion);

		api(ui) void removeFocus();
		api(ui) void receiveFocus();

		api(ui) void changeSize(int width, int height, ModelMask mask);
		api(ui) void changePlacement(int left, int top, int right, int bottom, ModelMask mask);
		api(ui) void updateAnchors();

	private:
		ui_space * _space;
		widget * _parent;
		Set<widget> _children;

		int_point _relPos;
		int_point _absPos;
		int_size  _size;

		int_rect _offsets = {0, 0, 0, 0};
		float_rect _anchors = {0.0f, 0.0f, 0.0f, 0.0f};

		ModelMask _alignment = ModelMask::LeftTop;

		array_list<WidgetLayer *> _layers;
		array_list<widget *> _displayList;

		int _flags = 0;
		mouse_state _mouseState;

		int _focusOrder = 0;
		int _displayOrder = 0;

		using DisplaySort = member_sort<widget, int, &widget::_displayOrder>;
		using FocusSort = member_sort<widget, int, &widget::_focusOrder>;

		class Tree : public asd::tree<Tree, Set<widget>>
		{
		public:
			using asd::tree<Tree, Set<widget>>::tree;

		private:
			friend asd::tree<Tree, Set<widget>>::iterator;

			static auto child_begin(typename Set<widget>::iterator & i) {
				return i->pointer()->_children.begin();
			}

			static auto child_end(typename Set<widget>::iterator & i) {
				return i->pointer()->_children.end();
			}
		};

		using iterator_t = tree_iterator<Tree>;

		bind_messages(widget, WidgetMessages)
	};

	channels_api(ui, widget, WidgetMessages)

		template<>
	struct hash<widget> : hash<subject> {};

	class WidgetLayer : public shareable<WidgetLayer>
	{
		friend class widget;

	public:
		explicit WidgetLayer(int order = 0) : _order(order) {}
		virtual ~WidgetLayer() = default;

		virtual handle<WidgetLayer> clone(widget * widget) const = 0;

		int order() const {
			return _order;
		}
		
		deny_assign(WidgetLayer);
	
	protected:
		virtual void draw(widget * w) = 0;

		int _order;

		using Sorter = member_sort<WidgetLayer, int, &WidgetLayer::_order>;
	};

	template<class T>
	class WidgetLayerComponent : public widget_component
	{
		static_assert(based_on<T, WidgetLayer>::value, "Template argument of the WidgetLayerComponent class must inherit the WidgetLayer");

	public:
		explicit WidgetLayerComponent(widget * widget) : widget_component(widget) {}

		virtual ~WidgetLayerComponent() override {
			if(_layer != nullptr) {
				_widget->detach(_layer);
			}
		}

		T * layer() const {
			return _layer;
		}

		virtual handle<widget_component> clone(widget * w) const override {
			handle<WidgetLayerComponent> c(w);

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

		void set(unique<T> && layer) {
			if(_layer != nullptr)
				_widget->detach(_layer);

			_layer = move(layer);
			_widget->attach(_layer);
		}
		
		deny_assign(WidgetLayerComponent);
	
	protected:
		unique<T> _layer;
	};

#define create_layer_component(module, ...) create_component(module, WidgetLayerComponent<__VA_ARGS__>);

	typedef std::function<void(const widget *, const int_rect &)> WidgetDrawer;

	class CustomLayer : public WidgetLayer
	{
	public:
		CustomLayer(const WidgetDrawer & drawer, int order = 0) : WidgetLayer(order), drawer(drawer) {}
		virtual ~CustomLayer() = default;

		virtual handle<WidgetLayer> clone(widget * widget) const override {
			return handle<CustomLayer>(drawer, _order);
		}
		
		deny_assign(CustomLayer);
	
	protected:
		virtual api(ui) void draw(widget * w) override;

		WidgetDrawer drawer;
	};

	class CustomLayerComponent : public widget_component
	{
	public:
		explicit CustomLayerComponent(widget * w) : widget_component(w) {}

		virtual handle<widget_component> clone(widget * widget) const override {
			handle<CustomLayerComponent> c(widget);

			for(auto & l : _layers)
				c->add(l->clone(widget));

			return c;
		}

		template<class LayerClass, useif<is_layer<LayerClass>::value>>
		LayerClass * add(const handle<LayerClass> & layer) {
			_layers.push_back(layer);
			return _widget->attach(layer);
		}

	protected:
		array_list<WidgetLayer> _layers;
	};

	create_component(ui, CustomLayerComponent);

	inline const int_size & widget::size() const {
		return _size;
	}

	inline int widget::width() const {
		return _size.x;
	}

	inline int widget::height() const {
		return _size.y;
	}

	inline const int_point & widget::pos() const {
		return _relPos;
	}

	inline int widget::side(int index) const {
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

	inline int widget::left() const {
		return _relPos.x;
	}

	inline int widget::top() const {
		return _relPos.y;
	}

	inline int widget::right() const {
		return _relPos.x + _size.x;
	}

	inline int widget::bottom() const {
		return _relPos.y + _size.y;
	}

	inline widget::operator math::int_rect () const {
		return {_relPos, _size};
	}

	inline int_rect widget::region() const {
		return {_relPos, _size};
	}

	inline int_rect widget::localRegion() const {
		return {_size};
	}

	inline asd::viewport widget::viewport() const {
		return {_size};
	}

	inline const int_point & widget::absPos() const {
		return _absPos;
	}

	inline int widget::absLeft() const {
		return _absPos.x;
	}

	inline int widget::absTop() const {
		return _absPos.y;
	}

	inline int widget::absRight() const {
		return _absPos.x + _size.x;
	}

	inline int widget::absBottom() const {
		return _absPos.y + _size.y;
	}

	inline int_rect widget::absRegion() const {
		return {_absPos, _size};
	}

	inline ModelMask widget::alignment() const {
		return _alignment;
	}

	inline const int_rect & widget::offsets() const {
		return _offsets;
	}

	inline const float_rect & widget::anchors() const {
		return _anchors;
	}

	inline bool widget::isVisible() const {
		return check_flag(WidgetFlag::Visible, _flags);
	}

	inline bool widget::isFocusable() const {
		return check_flag(WidgetFlag::Focusable, _flags);
	}

	inline int widget::displayOrder() const {
		return _displayOrder;
	}

	inline int widget::focusOrder() const {
		return _focusOrder;
	}

	inline widget * widget::child(int idx) const {
		return _displayList[idx];
	}

	inline typename widget::Tree widget::tree() {
		return {_children};
	}

	inline void widget::setLeft(int value) {
		changePlacement(value, top(), right(), bottom(), ModelMask::Left);
	}

	inline void widget::setTop(int value) {
		changePlacement(left(), value, right(), bottom(), ModelMask::Top);
	}

	inline void widget::setRight(int value) {
		changePlacement(left(), top(), value, bottom(), ModelMask::Right);
	}

	inline void widget::setBottom(int value) {
		changePlacement(left(), top(), right(), value, ModelMask::Bottom);
	}

	inline void widget::setWidth(int value) {
		changeSize(value, height(), ModelMask::Horizontal);
	}

	inline void widget::setHeight(int value) {
		changeSize(width(), value, ModelMask::Vertical);
	}

	inline void widget::setRegion(const int_rect & r) {
		changePlacement(r.left, r.top, r.right, r.bottom, ModelMask::FullSize);
	}

	inline void widget::setPos(int left, int top) {
		changePlacement(left, top, left + width(), top + height(), ModelMask::FullSize);
	}

	inline void widget::setPos(const int_point & pt) {
		changePlacement(pt.x, pt.y, pt.x + width(), pt.y + height(), ModelMask::FullSize);
	}

	inline void widget::setSize(int width, int height) {
		changeSize(width, height, ModelMask::FullSize);
	}

	inline void widget::setSize(const int_size & size) {
		changeSize(size.x, size.y, ModelMask::FullSize);
	}

	inline void widget::setPlacement(int left, int top, int width, int height) {
		changePlacement(left, top, left + width, top + height, ModelMask::FullSize);
	}

	inline void widget::setPlacement(const int_point & pt, const int_size & size) {
		changePlacement(pt.x, pt.y, pt.x + size.x, pt.y + size.y, ModelMask::FullSize);
	}

	inline void widget::setPlacement(ModelMask alignment, const int_rect & offsets) {
		_offsets = offsets;
		setAlignment(alignment);
	}

	inline void widget::setPlacement(ModelMask alignment, int left, int top, int width, int height) {
		_offsets.set(left, top, left, top);
		setAlignment(alignment);
		setSize(width, height);
	}

	inline void widget::setPlacement(ModelMask alignment, const int_point & offset, const int_size & size) {
		_offsets.set(offset.x, offset.y, offset.x, offset.y);
		setAlignment(alignment);
		setSize(size);
	}

	inline void widget::setPlacement(const float_rect & anchors, const int_rect & offsets) {
		_alignment = ModelMask::Custom;
		_anchors = anchors;
		_offsets = offsets;

		updateAnchors();
	}

	inline void widget::setAlignment(ModelMask alignment) {
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

	inline void widget::setOffset(int side, int value) {
		_offsets[side] = value;
		updateAnchors();
	}

	inline void widget::setOffsets(const int_rect & offsets) {
		_offsets = offsets;
		updateAnchors();
	}

	inline void widget::setAnchor(int side, float value) {
		_alignment = ModelMask::Custom;
		_anchors[side] = value;
		updateAnchors();
	}

	inline void widget::setAnchors(const float_rect & anchors) {
		_alignment = ModelMask::Custom;
		_anchors = anchors;
		updateAnchors();
	}

	inline handle<widget> widget::clone(widget * parent) const {
		return handle<widget>(*this, parent);
	}

	template<class WidgetClass, typename ... A, selected_t(0)> // append widget
	inline handle<WidgetClass> widget::append(A && ... args) {
		return handle<WidgetClass>(this, forward<A>(args)...);
	}

	template<class LayerClass, typename ... A, selected_t(1)>
	inline LayerClass * widget::append(A && ... args) // append layer
	{
		return components->template require<CustomLayerComponent>()->add(handle<LayerClass>(forward<A>(args)...));
	}

	template<class LayerClass, used_t>
	inline LayerClass * widget::attach(LayerClass * layer) {
		_layers.insert(std::upper_bound(_layers.begin(), _layers.end(), layer, WidgetLayer::Sorter()), layer); // sorted insert
		return layer;
	}

	template<class LayerClass, used_t>
	inline LayerClass * widget::attach(const handle<LayerClass> & layer) {
		_layers.insert(std::upper_bound(_layers.begin(), _layers.end(), layer, WidgetLayer::Sorter()), layer); // sorted insert
		return layer;
	}

	template<class LayerClass, used_t>
	inline LayerClass * widget::attach(const unique<LayerClass> & layer) {
		_layers.emplace(std::upper_bound(_layers.begin(), _layers.end(), layer, WidgetLayer::Sorter()), layer); // sorted insert
		return layer;
	}

	template<class Drawer, used_t>
	inline CustomLayer * widget::attach(const Drawer & drawer, int order) {
		return append<CustomLayer>(drawer, order);
	}

	inline void widget::setLayerOrder(WidgetLayer * layer, int order) {
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

	inline void widget::detach(WidgetLayer * layer) {
		erase(_layers, layer);
	}

	template<class Drawer, used_t>
	inline widget & widget::operator << (const Drawer & drawer) {
		attach(drawer);
		return *this;
	}
}

//---------------------------------------------------------------------------
#endif
