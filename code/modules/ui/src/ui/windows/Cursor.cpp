//---------------------------------------------------------------------------

#include <ui/Cursor.h>
#include <ui/ui_space.h>

//---------------------------------------------------------------------------

namespace asd
{
	CursorPool::CursorPool()
	{
		cursors["default"].init(LoadCursor(NULL, IDC_ARROW));
		cursors["wait"].init(LoadCursor(NULL, IDC_WAIT));
		cursors["hand"].init(LoadCursor(NULL, IDC_HAND));
	}

	Cursor::Cursor(HCURSOR handle) : _handle(handle) {}

	void Cursor::reload(HCURSOR handle)
	{
		_handle = handle;

		for(auto * ui : _spaces)
			SetClassLongPtr(ui->handle(), GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(_handle));
	}

	Cursor * Cursor::get(Widget * w)
	{
		auto c = w->components->seek<CursorComponent>();
		return c != nullptr ? c->_cursor : nullptr;
	}

	Cursor * Cursor::getPressed(Widget * w, MouseButton button)
	{
		auto c = w->components->seek<PressedCursorComponent>();
		return c != nullptr ? c->_cursors[button] : nullptr;
	}

	void Cursor::bind(UISpace * ui)
	{
		_spaces.insert(ui);

		SetClassLongPtr(ui->handle(), GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(_handle));
		SetCursor(_handle);
	}

	void Cursor::unbind(UISpace * ui)
	{
		_spaces.erase(ui);
	}

	void Cursor::attach(Widget * w)
	{
		w->components->require<CursorComponent>()->setCursor(this);
	}

	void Cursor::attachPressed(Widget * w, MouseButton button)
	{
		w->components->require<PressedCursorComponent>()->setCursor(this, button);
	}

	void Cursor::clear(Widget * w)
	{
		w->components->remove<CursorComponent>();
	}

	void Cursor::add(const string & name, const handle<Cursor> & cursor)
	{
		CursorPool::instance().cursors[name] = cursor;
	}

	Cursor * Cursor::find(const string & name)
	{
		return CursorPool::instance().cursors[name];
	}

	Cursor * Cursor::default()
	{
		static Cursor * d = find("default");
		return d;
	}

	Cursor * CursorComponent::cursor()
	{
		return _cursor;
	}

	void CursorComponent::setCursor(Cursor * cursor)
	{
		_cursor = cursor;

		if(_bound && _cursor != nullptr)
		{
			auto * ui = _widget->space();
			ui->setCursor(_cursor);
		}
	}

	void CursorComponent::bind()
	{
		_bound = true;

		auto * ui = _widget->space();

		if(_cursor != nullptr)
			ui->setCursor(_cursor);
		else
			ui->setCursor(_external);
	}

	void CursorComponent::unbind()
	{
		_bound = false;

		auto * ui = _widget->space();

		if(_external != nullptr)
			ui->setCursor(_external);
	}

	void CursorComponent::onMouseEnter(handle<MouseEnterMessage> &, Widget & w)
	{
		auto * ui = _widget->space();
		_external = ui->cursor();

		bind();
	}

	void CursorComponent::onMouseLeave(handle<MouseLeaveMessage> &, Widget & w)
	{
		unbind();
	}

	Cursor * PressedCursorComponent::cursor(MouseButton button)
	{
		return _cursors[button];
	}

	void PressedCursorComponent::setCursor(Cursor * cursor, MouseButton button)
	{
		auto & c = _cursors[button];
		c = cursor;

		if(c != nullptr && _widget->isPressed(button))
		{
			auto * ui = _widget->space();
			ui->setCursor(c);
		}
	}

	void PressedCursorComponent::onWidgetPress(handle<WidgetPressMessage> & msg, Widget & w)
	{
		auto & c = _cursors[msg->button];

		if(c != nullptr)
		{
			_cursorComponent->unbind();

			auto * ui = _widget->space();
			ui->setCursor(c);
		}
	}

	void PressedCursorComponent::onWidgetRelease(handle<WidgetReleaseMessage> & msg, Widget & w)
	{
		_cursorComponent->bind();
	}
}

//---------------------------------------------------------------------------
