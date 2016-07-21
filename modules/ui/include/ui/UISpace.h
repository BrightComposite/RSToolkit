//---------------------------------------------------------------------------

#pragma once

#ifndef UI_SPACE_H
#define UI_SPACE_H

//---------------------------------------------------------------------------

#include "Widget.h"
#include "Cursor.h"

#include <meta/Bitmask.h>
#include <graphics/Surface.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class UISpace;
	class Surface;

	link_class(ui, UISpace, Class<Subject>);

	class UISpace : public Subject
	{
		deny_copy(UISpace);

		friend class Widget;

	public:
		api(ui) UISpace(Graphics * graphics, const IntSize & size, HWND handle);
		virtual api(ui) ~UISpace();

		HWND handle() const
		{
			return _handle;
		}

		int width() const
		{
			return _width;
		}

		int height() const
		{
			return _height;
		}

		IntSize size() const
		{
			return {_width, _height};
		}

		IntRect region() const
		{
			return {0, 0, _width, _height};
		}

		const UniqueHandle<Widget> & root() const
		{
			return _root;
		}

		Graphics * graphics() const
		{
			return _graphics;
		}

		const Handle<Surface> & surface() const
		{
			return _surface;
		}

		Cursor * cursor() const
		{
			return _cursor;
		}

		bool enabled() const
		{
			return _enabled;
		}

		api(ui) void setCursor(Cursor * cursor);

		api(ui) void enable();
		api(ui) void disable();

		/**
		 *	Marks the whole window for update.
		 */
		api(ui) void invalidate();

		/**
		 *	Marks a region of window for update.
		 */
		api(ui) void invalidate(const Widget * w);

		/**
		 *	Redraws parts of the window which are marked for update.
		 */
		api(ui) void validate();

		void update()
		{
			invalidate();
			validate();
		}

		api(ui) const IntPoint & cursorPos() const;
		api(ui) void setCursorPos(const IntPoint & pt);
		api(ui) void clipCursor(const IntRect & region);
		api(ui) void unclipCursor();

		virtual api(ui) void registerHotkey(int id, int key, int modifiers = 0) {}
		virtual api(ui) void unregisterHotkey(int id) {}

		virtual api(ui) void read(Handle<KeyDownMessage> & msg);
		virtual api(ui) void read(Handle<CharMessage> & msg);
		virtual api(ui) void read(Handle<KeyUpMessage> & msg);
		virtual api(ui) void read(Handle<MouseDownMessage> & msg);
		virtual api(ui) void read(Handle<MouseUpdateMessage> & msg);
		virtual api(ui) void read(Handle<MouseUpMessage> & msg);
		virtual api(ui) void read(Handle<UIResizeMessage> & msg);

	protected:
		bind_messages(UISpace, UIMessages)

		Widget * focused() const;

		Widget * focusNext();
		Widget * focusPrevious();

		void acquireCursorPos(IntPoint &) const;
		void unpress(MouseButton buttons, int x, int y, int flags);

		void updateClipRect();

		HWND _handle;
		int _width, _height;

		Widget * _pointed = nullptr;

		list<Widget *>::iterator _focused;
		list<Widget *> _focusList; //!!! Create ordered_list
		set<pair<MouseButton, Widget *>> _pressedList;
		MouseState _mouseState;

		UniqueHandle<Widget> _root = nullptr;
		Handle<Surface> _surface = nullptr;
		Graphics * _graphics = nullptr;
		array_list<IntRect> _invalids;

		Cursor * _cursor = nullptr;
		IntPoint _cursorPos;

		IntRect _clipRect;
		bool _clipped = false;

		bool _fullscreen = false;
		bool _closed = false;
		bool _enabled = true;
	};

	channels_api(ui, UISpace, UIMessages)

	struct KeyMap
	{
		KeyMap() : data {0, 0, 0, 0, 0, 0, 0, 0} {}

		void press(byte key)
		{
			bits::setState(key % 32, data[key / 32]);
		}

		void unpress(byte key)
		{
			bits::clearState(key % 32, data[key / 32]);
		}

		void unpressAll()
		{
			data.fill(0);
		}

		bool isPressed(byte key)
		{
			return bits::state(key % 32, data[key / 32]);
		}

		array<int, 8> data;
	};

	class BackgroundWidget : public Widget
	{
	public:
		BackgroundWidget(UISpace * space) : Widget(space)
		{
			setVisibility(true);
			setPlacement(ModelMask::FullSize, {0, 0, 0, 0});
		}

		virtual ~BackgroundWidget() {}

		virtual bool isDisplayable() const final
		{
			return true;
		}
	};

#define WS_SIMPLE (WS_POPUP | WS_CLIPCHILDREN | WS_MINIMIZEBOX)

	api(ui) HWND createEmptyWindowHandle();
	api(ui) HWND createWindowHandle(const IntRect & rect, const wstring & caption, const wstring & windowClass, WNDPROC wndProc);
}

//---------------------------------------------------------------------------
#endif
