//---------------------------------------------------------------------------

#ifndef CURSOR_H
#define CURSOR_H

//---------------------------------------------------------------------------

#include <ui/Widget.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class UISpace;
	class CursorPool;

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
		friend class Cursor;

	public:
		CursorComponent(Widget * widget) : WidgetComponent(widget)
		{
			connect(*_widget, this, &CursorComponent::onMouseEnter);
			connect(*_widget, this, &CursorComponent::onMouseLeave);
		}

		virtual ~CursorComponent()
		{
			disconnect(*_widget, this, &CursorComponent::onMouseEnter);
			disconnect(*_widget, this, &CursorComponent::onMouseLeave);
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
		friend class Cursor;

	public:
		PressedCursorComponent(Widget * widget) : WidgetComponent(widget)
		{
			_cursorComponent = widget->link<CursorComponent>(this);

			connect(*_widget, this, &PressedCursorComponent::onWidgetPress);
			connect(*_widget, this, &PressedCursorComponent::onWidgetRelease);
		}

		virtual ~PressedCursorComponent()
		{
			disconnect(*_widget, this, &PressedCursorComponent::onWidgetPress);
			disconnect(*_widget, this, &PressedCursorComponent::onWidgetRelease);
		}

		api(ui) Cursor * cursor(MouseButton button);
		api(ui) void setCursor(Cursor * cursor, MouseButton button);

	protected:
		api(ui) void onWidgetPress(Handle<WidgetPressMessage> & msg, Widget & w);
		api(ui) void onWidgetRelease(Handle<WidgetReleaseMessage> & msg, Widget & w);

		Handle<CursorComponent> _cursorComponent;
		MouseButton _button = MouseButton::Left;
		map<MouseButton, Cursor *> _cursors;
		bool _bound = false;
	};

	create_component(ui, CursorComponent);
	create_component(ui, PressedCursorComponent);

	class CursorPool : public Singleton<CursorPool>
	{
		friend struct Singleton<CursorPool>;

	public:
		Map<string, Cursor> map;

	protected:
		CursorPool();
	};

	api_struct(ui, Singleton<CursorPool>);
}

//---------------------------------------------------------------------------
#endif
