//---------------------------------------------------------------------------

#include <ui/Widget.h>
#include <ui/WindowAdapter.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	WidgetRegion::WidgetRegion(Widget * parent, const IntRect & rect) : _relPos(rect.pos()), _absPos(_parent ? _relPos + _parent->_absPos : _relPos), _size(rect.size()), _parent(parent), _offsets(rect) {}
	WidgetRegion::WidgetRegion(Widget * parent, const IntPoint & pos, const IntSize & size) : _relPos(pos), _absPos(_parent ? _relPos + _parent->_absPos : _relPos), _size(size), _parent(parent), _offsets(pos, size) {}
	WidgetRegion::WidgetRegion(const WidgetRegion & region) : _relPos(region._relPos), _absPos(region._absPos), _size(region._size), _parent(region._parent), _offsets(region._offsets), _anchors(region._anchors) {}

	void WidgetRegion::calculateOffsets(IntRect & out, const WidgetRegion & region, const WidgetRegion & parent)
	{
		if(region._alignment == ModelMask::LeftTop)
		{
			out.setPlacement(region._relPos, region._size);
			return;
		}

		out.left   = region.left()   - (int)(region._anchors.left   * parent.width()) ;
		out.top    = region.top()    - (int)(region._anchors.top    * parent.height());
		out.right  = region.right()  - (int)(region._anchors.right  * parent.width()) ;
		out.bottom = region.bottom() - (int)(region._anchors.bottom * parent.height());
	}

	void WidgetRegion::calculateRegionRect(IntRect & out, const IntRect & offsets, const FloatRect & anchors, const WidgetRegion & parent)
	{
		out.left   = (int)(anchors.left   * parent.width())  + offsets.left  ;
		out.top    = (int)(anchors.top    * parent.height()) + offsets.top   ;
		out.right  = (int)(anchors.right  * parent.width())  + offsets.right ;
		out.bottom = (int)(anchors.bottom * parent.height()) + offsets.bottom;
	}

	implement_reader(WidgetRegion, WidgetMoveMessage) {}
	implement_reader(WidgetRegion, AfterWidgetMoveMessage) {}
	implement_reader(WidgetRegion, WidgetResizeMessage) {}

	Layer::Layer(Widget * widget, int order) : _widget(widget), _order(order)
	{
		_widget->_layerList.push_back(this);
		_widget->_layerList.sort();
	}

	Layer::~Layer()
	{
		_widget->_layerList.remove(this);
	}

	void Layer::setOrder(int order)
	{
		_order = order;
		_widget->_layerList.sort();
	}

	Widget::Widget(Widget * parent) : Widget(parent, *parent) {}
	Widget::Widget(WindowAdapter * adapter) : Widget(adapter, adapter->region()) {}
	Widget::Widget(Widget * parent, const IntRect & area) : Widget(parent->_adapter, parent, area) {}
	Widget::Widget(WindowAdapter * adapter, const IntRect & area) : Widget(adapter, adapter->_root, area) {}
	Widget::Widget(const Widget & widget) : Widget(widget, nullptr) {}

	Widget::Widget(WindowAdapter * adapter, Widget * parent, const IntRect & region) : WidgetRegion(parent, region), _adapter(adapter)
	{
		if(_parent != nullptr)
			_parent->_children.insert(this);
	}

	Widget::Widget(const Widget & widget, Widget * parent) : Widget(widget._adapter, parent, widget)
	{
		_flags = widget._flags;

		if(isFocusable())
			setFocusOrder(widget._focusOrder);
		else
			_focusOrder = widget._focusOrder;

		if(isVisible())
			setDisplayOrder(widget._displayOrder);
		else
			_displayOrder = widget._displayOrder;
	}

	Widget::~Widget()
	{
		_layers.clear();
		_children.clear();

		if(_parent != nullptr)
		{
			forgetParent();

			if(isPointed())
			{
				send<MouseLeaveMessage>(*this);
				_adapter->_pointed = nullptr;
			}

			if(isPressed())
			{
				auto & pl = _adapter->_pressedList;

				for(auto i = pl.begin(); i != pl.end(); ++i)
				{
					if(i->second == this)
					{
						i = pl.erase(i);

						if(i == pl.end())
							break;
					}
				}

				send<WidgetStopPressMessage>(*this, _mouseState.buttons());
				_mouseState.unpress();
			}

			setFocusability(false);
		}
	}

	void Widget::draw(Graphics * graphics, const IntRect & clipRegion)
	{
		IntRect r = absRegion();

		if(!r.isIntersecting(clipRegion))
			return;

		r &= clipRegion;

		graphics->clip(r);

		for(auto & layer : _layerList)
			layer->draw(r);

		for(auto & ch : _displayList)
			ch->draw(graphics, r);
	}

	Handle<Widget> Widget::attach(const Handle<Widget> & child)
	{
		if(child->_parent != nullptr || child->_adapter != _adapter)
			return child->clone(this);

		child->_parent = this;
		_children.insert(child);

		if(child->isVisible())
			insert(child, _displayList, &Widget::_displayOrder);

		return child;
	}

	Handle<Widget> Widget::detach()
	{
		Handle<Widget> w = this;

		if(_parent != nullptr)
		{
			forgetParent();

			_parent->_children.erase(w);
			_parent = nullptr;
		}

		return w;
	}

	void Widget::forgetParent()
	{
		auto & dl = _parent->_displayList;

		if(isVisible() && dl.size() > 0)
			remove(this, dl);
	}

	void Widget::sort(list<Widget *> & list, int(Widget::*attribute))
	{
		list.sort([&attribute](const Handle<Widget> & a, const Handle<Widget> & b)
		{
			return a->*attribute < b->*attribute;
		});
	}

	void Widget::insert(Widget * w, list<Widget *> & list, int(Widget::*attribute))
	{
		list.push_back(w);
		sort(list, attribute);
	}

	void Widget::remove(Widget * w, list<Widget *> & list)
	{
		list.remove(w);
	}

	void Widget::setVisibility(bool visible)
	{
		if(!isDisplayable() || visible == isVisible())
			return;

		change_flag(WidgetFlag::Visible, _flags, visible);

		if(_parent == nullptr)
			return;

		auto & dl = _parent->_displayList;

		if(visible)
		{
			insert(this, dl, &Widget::_displayOrder);
		}
		else
		{
			remove(this, dl);
		}
	}

	void Widget::setFocusability(bool focusable)
	{
		if(focusable == isFocusable())
			return;
		
		change_flag(WidgetFlag::Focusable, _flags, focusable);
		auto & fl = _adapter->_focusList;

		if(focusable)
		{
			insert(this, fl, &Widget::_focusOrder);
		}
		else
		{
			if(isFocused())
				_adapter->_focused = fl.end();

			remove(this, fl);
		}
	}

	void Widget::setDisplayOrder(int order)
	{
		if(!isDisplayable())
			return;

		setVisibility(true);
		auto premsg = send<ChangeDisplayOrderMessage>(*this, order, _displayOrder);

		if(premsg->newValue == _displayOrder)
			return;

		_displayOrder = premsg->newValue;

		if(_parent == nullptr)
			return;

		sort(_parent->_displayList, &Widget::_displayOrder);
		send<AfterChangeDisplayOrderMessage>(*this, _displayOrder);
	}

	void Widget::bringToFront()
	{
		if(_parent == nullptr)
			return;

		auto & dl = _parent->_displayList;

		if(dl.size() == 0)
			return;

		auto w = dl.back();

		if(w == this)
			return;

		setDisplayOrder(w->_displayOrder + 1);
	}

	void Widget::sendToBack()
	{
		if(_parent == nullptr)
			return;

		auto & dl = _parent->_displayList;
		int order = -1;

		if(dl.size() == 0)
			return;

		auto w = dl.front();

		if(w == this)
			return;

		setDisplayOrder(w->_displayOrder - 1);
	}

	void Widget::setFocusOrder(int order)
	{
		setFocusability(true);
		auto premsg = send<ChangeFocusOrderMessage>(*this, order, _focusOrder);
		
		if(premsg->newValue == _focusOrder)
			return;

		_focusOrder = premsg->newValue;
		sort(_adapter->_focusList, &Widget::_focusOrder);

		send<AfterChangeFocusOrderMessage>(*this, _focusOrder);
	}

	bool Widget::isPointed() const
	{
		return this == _adapter->_pointed;
	}

	bool Widget::isPressed() const
	{
		return _mouseState.isPressed();
	}

	bool Widget::isPressed(MouseButton button) const
	{
		return _mouseState.isPressed(button);
	}

	MouseButton Widget::pressedButtons() const
	{
		return _mouseState.buttons();
	}

	bool Widget::isFocused() const
	{
		return this == _adapter->focused();
	}

	void Widget::focus()
	{
		if(!isFocusable() || !isVisible() || isFocused())
			return;

		auto & f = _adapter->_focused;
		auto & fl = _adapter->_focusList;

		if(f != fl.end())
			(*f)->removeFocus();

		f = find(this, fl);
		receiveFocus();
	}

	Widget::iterator Widget::find(Widget * w, list<Widget *> & list)
	{
		for(auto i = list.begin(); i != list.end(); ++i)
		{
			if(*i == w)
				return i;
		}

		return list.end();
	}

	void Widget::removeFocus()
	{
		send<WidgetChangedStateMessage>(*this, WidgetState::Focused, false);
	}

	void Widget::receiveFocus()
	{
		send<WidgetChangedStateMessage>(*this, WidgetState::Focused, true);
	}

	WindowAdapter * Widget::adapter() const
	{
		return _adapter;
	}

	Graphics * Widget::graphics() const
	{
		return _adapter->graphics();
	}

	void WidgetRegion::changeSize(int width, int height, ModelMask mask)
	{
		if(_parent != nullptr)
		{
			IntRect off = _offsets;

			if(check_flag(ModelMask::Horizontal, mask))
			{
				if(_size.x != width)
				{
					off.left -= (int)((_anchors.left + _anchors.right) * (width - _size.x) / 2);
					off.setWidth(width);
				}
				else
					clear_flag(ModelMask::Horizontal, mask);
			}

			if(check_flag(ModelMask::Vertical, mask))
			{
				if(_size.y != height)
				{
					off.top -= (int)((_anchors.top + _anchors.bottom) * (height - _size.y) / 2);
					off.setHeight(height);
				}
				else
					clear_flag(ModelMask::Vertical, mask);
			}

			if(mask != 0)
			{
				IntRect r;
				calculateRegionRect(r, off, _anchors, *_parent);
				changePlacement(r.left, r.top, r.right, r.bottom, mask);
			}
		}
		else
		{
			IntRect r = region();

			if(r.width() != width)
				r.setWidth(width);
			else
				clear_flag(ModelMask::Horizontal, mask);

			if(r.height() != height)
				r.setHeight(height);
			else
				clear_flag(ModelMask::Vertical, mask);

			if(mask != 0)
				changePlacement(r.left, r.top, r.right, r.bottom, mask);
		}
	}

	void WidgetRegion::changePlacement(int left, int top, int right, int bottom, ModelMask mask)
	{
		auto msg = send<WidgetMoveMessage>(*this, left, top, right, bottom, mask);
		
		ModelMask resizeMask = ModelMask(0);
		IntSize size = _size;

		if(_parent != nullptr)
		{
			_relPos.x = msg->left;
			_relPos.y = msg->top;
			_absPos.x = _relPos.x + _parent->left();
			_absPos.y = _relPos.y + _parent->top();
			_size.x = msg->right - _relPos.x;
			_size.y = msg->bottom - _relPos.y;

			if(size.x != _size.x)
				set_flag(ModelMask::Horizontal, resizeMask);

			if(size.y != _size.y)
				set_flag(ModelMask::Vertical, resizeMask);

			_offsets.left   = msg->left   - (int)(_anchors.left   * _parent->width()) ;
			_offsets.top    = msg->top    - (int)(_anchors.top    * _parent->height());
			_offsets.right  = msg->right  - (int)(_anchors.right  * _parent->width()) ;
			_offsets.bottom = msg->bottom - (int)(_anchors.bottom * _parent->height());

			if(resizeMask != 0)
				send<WidgetResizeMessage>(*this, width(), height(), resizeMask);
		}
		else
		{
			_absPos.x = _relPos.x = msg->left;
			_absPos.y = _relPos.y = msg->top;
			_size.x = msg->right - _relPos.x;
			_size.y = msg->bottom - _relPos.y;

			if(size.x != _size.x)
				set_flag(ModelMask::Horizontal, resizeMask);

			if(size.y != _size.y)
				set_flag(ModelMask::Vertical, resizeMask);

			if(resizeMask != 0)
				send<WidgetResizeMessage>(*this, width(), height(), resizeMask);
		}

		send<AfterWidgetMoveMessage>(*this, _relPos.x, _relPos.y, _relPos.x + _size.x, _relPos.y + _size.y);
	}

	void WidgetRegion::updateAnchors()
	{
		if(_parent == nullptr)
			return;

		IntRect newRegion;
		WidgetRegion::calculateRegionRect(newRegion, _offsets, _anchors, *_parent);

		ModelMask mask = ModelMask(0);

		if(_relPos.x != newRegion.left)
			set_flag(ModelMask::Left, mask);

		if(_relPos.y != newRegion.top)
			set_flag(ModelMask::Top, mask);

		if(_relPos.x + _size.x != newRegion.right)
			set_flag(ModelMask::Right, mask);

		if(_relPos.y + _size.y != newRegion.bottom)
			set_flag(ModelMask::Bottom, mask);

		changePlacement(newRegion.left, newRegion.top, newRegion.right, newRegion.bottom, mask);
		WidgetRegion::calculateOffsets(_offsets, *this, *_parent);
	}

	Widget * Widget::findWidget(const IntPoint & pt)
	{
		for(auto i = _displayList.rbegin(); i != _displayList.rend(); ++i)
		{
			auto & ch = *i;
			auto p = pt - ch->_relPos;

			if(ch->localRegion().includes(p))
				return ch->findWidget(p);
		}

		return this;
	}

	implement_reader(Widget, KeyDownMessage)
	{
		if(_parent != nullptr)
			resend(msg, *_parent);
	}

	implement_reader(Widget, CharMessage)
	{
		if(_parent != nullptr)
			resend(msg, *_parent);
	}

	implement_reader(Widget, KeyUpMessage)
	{
		if(_parent != nullptr)
			resend(msg, *_parent);
	}

	implement_reader(Widget, WidgetResizeMessage)
	{
		for(auto _child : _children)
			_child->updateAnchors();
	}
}

//---------------------------------------------------------------------------