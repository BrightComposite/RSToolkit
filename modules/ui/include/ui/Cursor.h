//---------------------------------------------------------------------------

#pragma once

#ifndef CURSOR_H
#define CURSOR_H

//---------------------------------------------------------------------------

#include <ui/Widget.h>
#include <windows.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class UISpace;
	class CursorPool;
	apistruct(ui) Singleton<CursorPool>;

	class Cursor : public Shared
	{
		deny_copy(Cursor);
		friend class UISpace;

	public:
		api(ui) void reload(HCURSOR handle);

		api(ui) void attach(Widget * w);
		api(ui) void attachPressed(Widget * w, MouseButton button = MouseButton::Left);

		static api(ui) void add(const string & name, const Handle<Cursor> & cursor);
		static api(ui) Cursor * find(const string & name);
		static api(ui) Cursor * default();

		static api(ui) Cursor * get(Widget * w);
		static api(ui) Cursor * getPressed(Widget * w, MouseButton button = MouseButton::Left);
		static api(ui) void clear(Widget * w);

	protected:
		friend_handle;
		api(ui) Cursor(HCURSOR handle);

		api(ui) void bind(UISpace * ui);
		api(ui) void unbind(UISpace * ui);

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

		virtual Handle<WidgetComponent> clone(Widget * widget) const override
		{
			return Handle<CursorComponent>(widget, *this);
		}

		api(ui) Cursor * cursor();
		api(ui) void setCursor(Cursor * cursor);

		api(ui) void bind();
		api(ui) void unbind();

	protected:
		api(ui) void onMouseEnter(Handle<MouseEnterMessage> & msg, Widget & w);
		api(ui) void onMouseLeave(Handle<MouseLeaveMessage> & msg, Widget & w);

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

		virtual Handle<WidgetComponent> clone(Widget * widget) const override
		{
			return Handle<PressedCursorComponent>(widget, *this);
		}

		api(ui) Cursor * cursor(MouseButton button);
		api(ui) void setCursor(Cursor * cursor, MouseButton button);

	protected:
		api(ui) void onWidgetPress(Handle<WidgetPressMessage> & msg, Widget & w);
		api(ui) void onWidgetRelease(Handle<WidgetReleaseMessage> & msg, Widget & w);

		Handle<CursorComponent> _cursorComponent;
		MouseButton _button = MouseButton::Left;
		dictionary<MouseButton, Cursor *> _cursors;
		bool _bound = false;
	};

	create_component(ui, CursorComponent);
	create_component(ui, PressedCursorComponent);

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
