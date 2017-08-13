//---------------------------------------------------------------------------

#include <ui/widget.h>
#include <ui/ui_space.h>

#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace asd
{
	void CustomLayer::draw(widget * w)
	{
		drawer(w, w->absRegion());
	}

	widget::widget(widget * parent) : widget(parent, *parent) {}
	widget::widget(ui_space * space) : widget(space, space->region()) {}
	widget::widget(widget * parent, const int_rect & area) : widget(parent->_space, parent, area) {}
	widget::widget(ui_space * space, const int_rect & area) : widget(space, nullptr, area) {}

	widget::widget(ui_space * space, widget * parent, const int_rect & region) :
		_space(space), _parent(parent), _relPos(region.pos()), _absPos(_parent ? _relPos + _parent->_absPos : _relPos), _size(region.size()), _offsets(region)
	{
		components.init(this);
		
		if(_parent != nullptr)
			_parent->_children.insert(this);

		setVisibility(true);
	}

	widget::widget(const widget & w, widget * parent) : widget(parent->_space, parent, w.region())
	{
		_flags = w._flags;

		if(isFocusable())
			setFocusOrder(w._focusOrder);
		else
			_focusOrder = w._focusOrder;

		if(isVisible())
			setDisplayOrder(w._displayOrder);
		else
			_displayOrder = w._displayOrder;

		components->set(*w.components);
	}

	widget::~widget()
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

	void widget::calculateOffsets(int_rect & out, const widget & region, const widget & parent)
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

	void widget::calculateRegionRect(int_rect & out, const int_rect & offsets, const float_rect & anchors, const widget & parent)
	{
		out.left   = (int)(anchors.left   * parent.width())  + offsets.left  ;
		out.top    = (int)(anchors.top    * parent.height()) + offsets.top   ;
		out.right  = (int)(anchors.right  * parent.width())  + offsets.right ;
		out.bottom = (int)(anchors.bottom * parent.height()) + offsets.bottom;
	}

	void widget::draw(asd::graphics * graphics, const int_rect & clipRegion)
	{
		int_rect r = absRegion() & clipRegion;
		
		if(r.isEmpty())
			return;

		graphics->clip(r);

		for(auto & layer : _layers)
			layer->draw(this);

		for(auto & ch : _displayList)
			ch->draw(graphics, r);
	}

	widget * widget::attach(const handle<widget> & child)
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

	void widget::detach(widget * child)
	{
		if(child->_parent != this)
			return;

		if(child->isVisible() && _displayList.size() > 0)
			erase(_displayList, child);
		
		child->_parent = nullptr;
		_children.erase(child);
	}

	void widget::setVisibility(bool visible)
	{
		if(visible == isVisible())
			return;

		select_flag(WidgetFlag::Visible, _flags, visible);

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

	void widget::setFocusability(bool focusable)
	{
		if(focusable == isFocusable())
			return;
		
		select_flag(WidgetFlag::Focusable, _flags, focusable);
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

	void widget::setDisplayOrder(int order)
	{
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

	void widget::bringToFront()
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

	void widget::sendToBack()
	{
		if(_parent == nullptr)
			return;

		auto & dl = _parent->_displayList;

		if(dl.size() == 0)
			return;

		auto w = dl.front();

		if(w == this || w == nullptr)
			return;

		setDisplayOrder(w->_displayOrder - 1);
	}

	void widget::setFocusOrder(int order)
	{
		setFocusability(true);
		auto premsg = send<ChangeFocusOrderMessage>(*this, order, _focusOrder);
		
		if(premsg->newValue == _focusOrder)
			return;

		_focusOrder = premsg->newValue;
		sort<FocusSort>(_space->_focusList);

		send<AfterChangeFocusOrderMessage>(*this, _focusOrder);
	}

	bool widget::isPointed() const
	{
		return this == _space->_pointed;
	}

	bool widget::isPressed() const
	{
		return _mouseState.isPressed();
	}

	bool widget::isPressed(MouseButton button) const
	{
		return _mouseState.isPressed(button);
	}

	MouseButton widget::pressedButtons() const
	{
		return _mouseState.buttons();
	}

	bool widget::isFocused() const
	{
		return this == _space->focused();
	}

	void widget::show()
	{
		setVisibility(true);
	}

	void widget::hide()
	{
		setVisibility(false);
	}

	void widget::focus()
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

	void widget::removeFocus()
	{
		send<WidgetChangedStateMessage>(*this, WidgetState::Focused, false);
	}

	void widget::receiveFocus()
	{
		send<WidgetChangedStateMessage>(*this, WidgetState::Focused, true);
	}

	ui_space * widget::space() const
	{
		return _space;
	}

	asd::graphics * widget::graphics() const
	{
		return _space->graphics();
	}

	void widget::changeSize(int width, int height, ModelMask mask)
	{
		if(_parent != nullptr)
		{
			int_rect off = _offsets;

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
			
			if(mask != ModelMask::Custom) {
				int_rect r;
				calculateRegionRect(r, off, _anchors, *_parent);
				changePlacement(r.left, r.top, r.right, r.bottom, mask);
			}
		}
		else
		{
			int_rect r = region();

			if(r.width() != width)
				r.setWidth(width);
			else
				clear_flag(ModelMask::Horizontal, mask);

			if(r.height() != height)
				r.setHeight(height);
			else
				clear_flag(ModelMask::Vertical, mask);

			if(mask != ModelMask::Custom) {
				changePlacement(r.left, r.top, r.right, r.bottom, mask);
			}
		}
	}

	void widget::changePlacement(int left, int top, int right, int bottom, ModelMask mask)
	{
		auto msg = send<WidgetMoveMessage>(*this, left, top, right, bottom, mask);
		
		ModelMask resizeMask = ModelMask(0);
		int_size size = _size;

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

			if(resizeMask != ModelMask::Custom) {
				send<WidgetResizeMessage>(*this, width(), height(), resizeMask);
			}

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
			
			if(resizeMask != ModelMask::Custom) {
				send<WidgetResizeMessage>(*this, width(), height(), resizeMask);
			}
		}

		send<AfterWidgetMoveMessage>(*this, _relPos.x, _relPos.y, _relPos.x + _size.x, _relPos.y + _size.y);
	}

	void widget::updateAnchors()
	{
		if(_parent == nullptr)
			return;

		int_rect newRegion;
		widget::calculateRegionRect(newRegion, _offsets, _anchors, *_parent);

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

	widget * widget::findAt(const int_point & pt)
	{
		for(auto i = _displayList.rbegin(); i != _displayList.rend(); ++i)
		{
			auto & ch = *i;
			auto p = pt - ch->_relPos;

			if(ch->localRegion().contains(p))
				return ch->findAt(p);
		}

		return this;
	}

	void widget::read(handle<KeyDownMessage> & msg)
	{
		if(_parent != nullptr)
			resend(msg, *_parent);
	}

	void widget::read(handle<CharMessage> & msg)
	{
		if(_parent != nullptr)
			resend(msg, *_parent);
	}

	void widget::read(handle<KeyUpMessage> & msg)
	{
		if(_parent != nullptr)
			resend(msg, *_parent);
	}

	void widget::read(handle<MouseUpdateMessage> & msg) {}
	void widget::read(handle<MouseDownMessage> & msg) {}
	void widget::read(handle<MouseMoveMessage> & msg) {}
	void widget::read(handle<MouseUpMessage> & msg) {}
	void widget::read(handle<MouseClickMessage> & msg) {}
	void widget::read(handle<MouseDblClickMessage> & msg) {}
	void widget::read(handle<MouseWheelMessage> & msg) {}
	void widget::read(handle<MouseEnterMessage> & msg) {}
	void widget::read(handle<MouseLeaveMessage> & msg) {}
	void widget::read(handle<WidgetPressMessage> & msg) {}
	void widget::read(handle<WidgetStopPressMessage> & msg) {}
	void widget::read(handle<WidgetReleaseMessage> & msg) {}
	void widget::read(handle<ChangeFocusOrderMessage> & msg) {}
	void widget::read(handle<AfterChangeFocusOrderMessage> & msg) {}
	void widget::read(handle<ChangeDisplayOrderMessage> & msg) {}
	void widget::read(handle<AfterChangeDisplayOrderMessage> & msg) {}
	void widget::read(handle<WidgetDrawMessage> & msg) {}
	void widget::read(handle<WidgetMoveMessage> & msg) {}
	void widget::read(handle<AfterWidgetMoveMessage> & msg) {}

	void widget::read(handle<WidgetChangedStateMessage> & msg)
	{
		_space->invalidate(this);
	}

	void widget::read(handle<WidgetResizeMessage> & msg)
	{
		for(auto & _child : _children)
			_child->updateAnchors();
	}
}

//---------------------------------------------------------------------------
