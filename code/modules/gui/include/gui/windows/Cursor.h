//---------------------------------------------------------------------------

#pragma once

#ifndef CURSOR_H
#define CURSOR_H

//---------------------------------------------------------------------------

#include <ui/Widget.h>

#ifdef _MSC_VER
#include <windows.h>
#endif

//---------------------------------------------------------------------------

namespace asd
{
	class UISpace;
	class CursorPool;
	apistruct(gui) Singleton<CursorPool>;

	class Cursor : public Shared
	{
		deny_copy(Cursor);
		friend class UISpace;

	public:
		api(gui) void reload(HCURSOR handle);

		api(gui) void attach(Widget * w);
		api(gui) void attachPressed(Widget * w, mouse_button button = mouse_button::left);

		static api(gui) void add(const string & name, const handle<Cursor> & cursor);
		static api(ui) Cursor * find(const string & name);
		static api(ui) Cursor * default();

		static api(ui) Cursor * get(Widget * w);
		static api(ui) Cursor * getPressed(Widget * w, mouse_button button = mouse_button::left);
		static api(gui) void clear(Widget * w);

	protected:
		friend_handle;
		api(gui) Cursor(HCURSOR handle);

		api(gui) void bind(UISpace * ui);
		api(gui) void unbind(UISpace * ui);

		HCURSOR _handle;
		set<UISpace *> _spaces;
	};

	class CursorComponent : public WidgetComponent
	{
		deny_copy(CursorComponent);
		friend class Cursor;

	public:
		CursorComponent(Widget * widget) : WidgetComponent(widget)
		{
			connect(this, &CursorComponent::onMouseEnter, *_widget);
			connect(this, &CursorComponent::onMouseLeave, *_widget);
		}

		CursorComponent(Widget * widget, const CursorComponent & component) : CursorComponent(widget)
		{
			_cursor = component._cursor;
			_external = component._external;
		}

		virtual ~CursorComponent() {}

		virtual handle<WidgetComponent> clone(Widget * widget) const override
		{
			return handle<CursorComponent>(widget, *this);
		}

		api(gui) Cursor * cursor();
		api(gui) void setCursor(Cursor * cursor);

		api(gui) void bind();
		api(gui) void unbind();

	protected:
		api(gui) void onMouseEnter(handle<MouseEnterMessage> & msg, Widget & w);
		api(gui) void onMouseLeave(handle<MouseLeaveMessage> & msg, Widget & w);

		Cursor * _cursor = nullptr;
		Cursor * _external = nullptr;
		bool _bound = false;
	};

	class PressedCursorComponent : public WidgetComponent
	{
		deny_copy(PressedCursorComponent);
		friend class Cursor;

	public:
		PressedCursorComponent(Widget * widget) : WidgetComponent(widget)
		{
			_cursorComponent = widget->components->link<CursorComponent>(this);

			connect(*this, &PressedCursorComponent::onWidgetPress, *_widget);
			connect(*this, &PressedCursorComponent::onWidgetRelease, *_widget);
		}

		PressedCursorComponent(Widget * widget, const PressedCursorComponent & component) : PressedCursorComponent(widget)
		{
			_button = component._button;
			_cursors = component._cursors;
		}

		virtual ~PressedCursorComponent() {}

		virtual handle<WidgetComponent> clone(Widget * widget) const override
		{
			return handle<PressedCursorComponent>(widget, *this);
		}

		api(gui) Cursor * cursor(mouse_button button);
		api(gui) void setCursor(Cursor * cursor, mouse_button button);

	protected:
		api(gui) void onWidgetPress(handle<WidgetPressMessage> & msg, Widget & w);
		api(gui) void onWidgetRelease(handle<WidgetReleaseMessage> & msg, Widget & w);

		handle<CursorComponent> _cursorComponent;
		mouse_button _button = mouse_button::left;
		dictionary<mouse_button, Cursor *> _cursors;
		bool _bound = false;
	};

	create_component(gui, CursorComponent);
	create_component(gui, PressedCursorComponent);

	class CursorPool : public Singleton<CursorPool>
	{
		friend struct Singleton<CursorPool>;

	public:
		Dictionary<string, Cursor> cursors;

	protected:
		CursorPool();
	};
}

//---------------------------------------------------------------------------
#endif
