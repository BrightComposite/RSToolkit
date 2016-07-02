//---------------------------------------------------------------------------

#include <ui/Widget.h>
#include <ui/UISpace.h>

#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	implement_link(Widget);

	void CustomLayer::draw(Widget * w)
	{
		drawer(w, w->region());
	}

	Widget::Widget(Widget * parent) : Widget(parent, *parent) {}
	Widget::Widget(UISpace * space) : Widget(space, space->region()) {}
	Widget::Widget(Widget * parent, const IntRect & area) : Widget(parent->_space, parent, area) {}
	Widget::Widget(UISpace * space, const IntRect & area) : Widget(space, space->_root, area) {}

	Widget::Widget(UISpace * space, Widget * parent, const IntRect & region) :
		_parent(parent), _relPos(region.pos()), _absPos(_parent ? _relPos + _parent->_absPos : _relPos), _size(region.size()), _offsets(region), _space(space)
	{
		setclass(Widget);

		if(_parent != nullptr)
			_parent->_children.insert(this);
	}

	Widget::Widget(const Widget & widget, Widget * parent) : Widget(widget._space, parent, widget)
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
		if(isPointed())
		{
			send<MouseLeaveMessage>(*this);
			_space->_pointed = nullptr;
		}

		if(isPressed())
		{
			auto & pl = _space->_pressedList;

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

	void Widget::calculateOffsets(IntRect & out, const Widget & region, const Widget & parent)
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

	void Widget::calculateRegionRect(IntRect & out, const IntRect & offsets, const FloatRect & anchors, const Widget & parent)
	{
		out.left   = (int)(anchors.left   * parent.width())  + offsets.left  ;
		out.top    = (int)(anchors.top    * parent.height()) + offsets.top   ;
		out.right  = (int)(anchors.right  * parent.width())  + offsets.right ;
		out.bottom = (int)(anchors.bottom * parent.height()) + offsets.bottom;
	}

	void Widget::draw(Graphics * graphics, const IntRect & clipRegion)
	{
		IntRect r = absRegion() & clipRegion;
		
		if(r.isEmpty())
			return;

		graphics->clip(r);

		for(auto & layer : _layers)
			layer->draw(this);

		for(auto & ch : _displayList)
			ch->draw(graphics, r);
	}

	Handle<Widget> Widget::attach(const Handle<Widget> & child)
	{
		if(child->_parent != nullptr || child->_space != _space)
			return child->clone(this);

		child->_parent = this;
		_children.insert(child);

		if(child->isVisible())
		{
			_displayList.push_back(this);
			sort<DisplaySort>(_displayList);
		}

		return child;
	}

	void Widget::detach(Widget * w)
	{
		if(w->_parent != this)
			return;

		if(w->isVisible() && _displayList.size() > 0)
			erase(_displayList, w);

		w->_parent = nullptr;
		_children.erase(w);
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
			dl.push_back(this);
			sort<DisplaySort>(dl);
		}
		else
			erase(dl, this);
	}

	void Widget::setFocusability(bool focusable)
	{
		if(focusable == isFocusable())
			return;
		
		change_flag(WidgetFlag::Focusable, _flags, focusable);
		auto & fl = _space->_focusList;

		if(focusable)
		{
			fl.push_back(this);
			sort<FocusSort>(fl);
		}
		else
		{
			if(isFocused())
				_space->_focused = fl.end();

			erase(fl, this);
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

		sort<DisplaySort>(_parent->_displayList);
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

		if(w == this || w == nullptr)
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

		if(w == this || w == nullptr)
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
		sort<FocusSort>(_space->_focusList);

		send<AfterChangeFocusOrderMessage>(*this, _focusOrder);
	}

	bool Widget::isPointed() const
	{
		return this == _space->_pointed;
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
		return this == _space->focused();
	}

	void Widget::focus()
	{
		if(!isFocusable() || !isVisible() || isFocused())
			return;

		auto & f = _space->_focused;
		auto & fl = _space->_focusList;

		if(f != fl.end())
			(*f)->removeFocus();

		f = std::find(fl.begin(), fl.end(), this);
		receiveFocus();
	}

	void Widget::removeFocus()
	{
		send<WidgetChangedStateMessage>(*this, WidgetState::Focused, false);
	}

	void Widget::receiveFocus()
	{
		send<WidgetChangedStateMessage>(*this, WidgetState::Focused, true);
	}

	UISpace * Widget::space() const
	{
		return _space;
	}

	Graphics * Widget::graphics() const
	{
		return _space->graphics();
	}

	void Widget::changeSize(int width, int height, ModelMask mask)
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

	void Widget::changePlacement(int left, int top, int right, int bottom, ModelMask mask)
	{
		auto msg = send<WidgetMoveMessage>(*this, left, top, right, bottom, mask);
		
		ModelMask resizeMask = ModelMask(0);
		IntSize size = _size;

		if(_parent != nullptr)
		{
			_relPos.x = msg->left;
			_relPos.y = msg->top;
			_absPos.x = _relPos.x + _parent->absLeft();
			_absPos.y = _relPos.y + _parent->absTop();
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

			calculateOffsets(_offsets, *this, *_parent);
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

	void Widget::updateAnchors()
	{
		if(_parent == nullptr)
			return;

		IntRect newRegion;
		Widget::calculateRegionRect(newRegion, _offsets, _anchors, *_parent);

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
	}

	Widget * Widget::findAt(const IntPoint & pt)
	{
		for(auto i = _displayList.rbegin(); i != _displayList.rend(); ++i)
		{
			auto & ch = *i;
			auto p = pt - ch->_relPos;

			if(ch->localRegion().includes(p))
				return ch->findAt(p);
		}

		return this;
	}

	void Widget::read(Handle<KeyDownMessage> & msg)
	{
		if(_parent != nullptr)
			resend(msg, *_parent);
	}

	void Widget::read(Handle<CharMessage> & msg)
	{
		if(_parent != nullptr)
			resend(msg, *_parent);
	}

	void Widget::read(Handle<KeyUpMessage> & msg)
	{
		if(_parent != nullptr)
			resend(msg, *_parent);
	}

	void Widget::read(Handle<MouseUpdateMessage> & msg) {}
	void Widget::read(Handle<MouseDownMessage> & msg) {}
	void Widget::read(Handle<MouseMoveMessage> & msg) {}
	void Widget::read(Handle<MouseUpMessage> & msg) {}
	void Widget::read(Handle<MouseClickMessage> & msg) {}
	void Widget::read(Handle<MouseDblClickMessage> & msg) {}
	void Widget::read(Handle<MouseWheelMessage> & msg) {}
	void Widget::read(Handle<MouseEnterMessage> & msg) {}
	void Widget::read(Handle<MouseLeaveMessage> & msg) {}
	void Widget::read(Handle<WidgetPressMessage> & msg) {}
	void Widget::read(Handle<WidgetStopPressMessage> & msg) {}
	void Widget::read(Handle<WidgetReleaseMessage> & msg) {}
	void Widget::read(Handle<WidgetChangedStateMessage> & msg) {}
	void Widget::read(Handle<ChangeFocusOrderMessage> & msg) {}
	void Widget::read(Handle<AfterChangeFocusOrderMessage> & msg) {}
	void Widget::read(Handle<ChangeDisplayOrderMessage> & msg) {}
	void Widget::read(Handle<AfterChangeDisplayOrderMessage> & msg) {}
	void Widget::read(Handle<WidgetDrawMessage> & msg) {}
	void Widget::read(Handle<WidgetMoveMessage> & msg) {}
	void Widget::read(Handle<AfterWidgetMoveMessage> & msg) {}

	void Widget::read(Handle<WidgetResizeMessage> & msg)
	{
		for(auto & _child : _children)
			_child->updateAnchors();
	}
}

//---------------------------------------------------------------------------