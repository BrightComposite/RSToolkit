//---------------------------------------------------------------------------

#pragma once

#ifndef UI_SPACE_H
#define UI_SPACE_H

//---------------------------------------------------------------------------

#include "Widget.h"

#ifdef WIN32
#include "Cursor.h"
#endif

#include <meta/Bitmask.h>
#include <graphics/Surface.h>
#include <graphics/Color.h>

//---------------------------------------------------------------------------

namespace asd
{
	class UISpace;
	class Surface;

	link_class(ui, UISpace, Class<Subject>);

	class UISpace : public Subject
	{
		deny_copy(UISpace);

		friend class Widget;

	public:
#ifdef WIN32
		api(ui) UISpace(Graphics * graphics, const IntSize & size, HWND handle);
#else
		api(ui) UISpace(Graphics * graphics, const IntSize & size);
#endif
		virtual api(ui) ~UISpace();

#ifdef WIN32
		HWND handle() const
		{
			return _handle;
		}
#endif
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
		
		bool enabled() const
		{
			return _enabled;
		}

#ifdef WIN32
		Cursor * cursor() const
		{
			return _cursor;
		}
		
		api(ui) void setCursor(Cursor * cursor);
		api(ui) void showCursor();
		api(ui) void hideCursor();

#endif
		api(ui) void enable() override;
		api(ui) void disable() override;

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
		api(ui) void mouseUpdate();

		bool isCursorClipped()
		{
			return _clippedCursor;
		}

		api(ui) void clipCursor(const IntRect & region);
		api(ui) void unclipCursor();

		api(ui) void setBackgroundColor(const rgb & color);

		virtual api(ui) void registerHotkey(int id, int key, int modifiers = 0) {}
		virtual api(ui) void unregisterHotkey(int id) {}

		virtual api(ui) void read(Handle<KeyDownMessage> & msg);
		virtual api(ui) void read(Handle<CharMessage> & msg);
		virtual api(ui) void read(Handle<KeyUpMessage> & msg);
		virtual api(ui) void read(Handle<MouseDownMessage> & msg);
		virtual api(ui) void read(Handle<MouseMoveMessage> & msg);
		virtual api(ui) void read(Handle<MouseUpdateMessage> & msg);
		virtual api(ui) void read(Handle<MouseUpMessage> & msg);
		virtual api(ui) void read(Handle<UIMoveMessage> & msg);
		virtual api(ui) void read(Handle<UIResizeMessage> & msg);

	protected:
		bind_messages(UISpace, UIMessages)

		Widget * focused() const;

		Widget * focusNext();
		Widget * focusPrevious();

		void acquireCursorPos(IntPoint &) const;
		void unpress(MouseButton buttons, int x, int y, int flags);

		void updateCursorClipRect();
		
	#ifdef WIN32
		HWND _handle;
	#endif
		
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

#ifdef WIN32
		Cursor * _cursor = nullptr;
#endif
		
		IntPoint _cursorPos;

		IntRect _cursorClipRect;
		bool _clippedCursor = false;

		bool _fullscreen = false;
		bool _active = false;
		bool _closed = false;
		bool _enabled = true;
	};

	channels_api(ui, UISpace, UIMessages)

	struct KeyMap
	{
		KeyMap() : data({0, 0, 0, 0, 0, 0, 0, 0}) {}

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
		BackgroundWidget(UISpace * space) : Widget(space, space->root())
		{
			setPlacement(ModelMask::FullSize, {0, 0, 0, 0});
			setName("Background");
		}

		virtual ~BackgroundWidget() {}
	};

#ifdef WIN32
	#define WS_SIMPLE (WS_POPUP | WS_CLIPCHILDREN | WS_MINIMIZEBOX)

	api(ui) HWND createEmptyWindowHandle();
	api(ui) HWND createWindowHandle(const IntRect & rect, const wstring & caption, const wstring & windowClass, WNDPROC wndProc);
#endif
	
}

//---------------------------------------------------------------------------
#endif
