//---------------------------------------------------------------------------

#pragma once

#ifndef GUI_SPACE_H
#define GUI_SPACE_H

//---------------------------------------------------------------------------

#include "object.h"

#if BOOST_OS_WINDOWS
#include "Cursor.h"
#endif

#include <meta/bitmask.h>
#include <graphics/surface.h>
#include <graphics/Color.h>
#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace asd
{
	class ui_space;

	class surface;

	class ui_space : public subject
	{
		deny_copy(ui_space);

		friend class widget;

	public:
#if BOOST_OS_WINDOWS
		api(gui)
		ui_space(graphics * graphics, const int_size & size, HWND handle);
#else
		api(gui)
		ui_space(asd::graphics * graphics, const math::int_size & size);
#endif
		virtual api(gui) ~ui_space();

#if BOOST_OS_WINDOWS
		HWND handle() const
		{
			return _handle;
		}
#elif BOOST_OS_LINUX
		::Window handle() const {
			return _handle;
		}

		::Display * display() const {
			return _display;
		}
#endif
		int width() const {
			return _width;
		}

		int height() const {
			return _height;
		}

		int_size size() const {
			return {_width, _height};
		}

		int_rect region() const {
			return {0, 0, _width, _height};
		}

		const unique<widget> & root() const {
			return _root;
		}

		asd::graphics * graphics() const {
			return _graphics;
		}

		const asd::handle<asd::surface> & surface() const {
			return _surface;
		}

		bool enabled() const {
			return _enabled;
		}

#if BOOST_OS_WINDOWS
		Cursor * cursor() const
		{
			return _cursor;
		}

		api(gui) void setCursor(Cursor * cursor);
		api(gui) void showCursor();
		api(gui) void hideCursor();
#endif

		api(gui)
		void enable() override;
		api(gui)
		void disable() override;

		/**
		 *	Marks the whole window for update.
		 */
		api(gui)
		void invalidate();

		/**
		 *	Marks a region of window for update.
		 */
		api(gui)
		void invalidate(const widget * w);

		/**
		 *	Redraws parts of the window which are marked for update.
		 */
		api(gui)
		void validate();

		void update() {
			invalidate();
			validate();
		}

		api(gui)
		const int_point & cursorPos() const;
		api(gui)
		void setCursorPos(const int_point & pt);
		api(gui)
		void mouseUpdate();

		bool isCursorClipped() {
			return _clippedCursor;
		}

		api(gui)
		void clipCursor(const int_rect & region);
		api(gui)
		void unclipCursor();

		api(gui)
		void setBackgroundColor(const rgb & color);

		virtual api(gui) void registerHotkey(int id, int key, int modifiers = 0) {}

		virtual api(gui) void unregisterHotkey(int id) {}

		virtual api(gui) void read(asd::handle<KeyDownMessage> & msg);
		virtual api(gui) void read(asd::handle<CharMessage> & msg);
		virtual api(gui) void read(asd::handle<KeyUpMessage> & msg);
		virtual api(gui) void read(asd::handle<MouseDownMessage> & msg);
		virtual api(gui) void read(asd::handle<MouseMoveMessage> & msg);
		virtual api(gui) void read(asd::handle<MouseUpdateMessage> & msg);
		virtual api(gui) void read(asd::handle<MouseUpMessage> & msg);
		virtual api(gui) void read(asd::handle<UIMoveMessage> & msg);
		virtual api(gui) void read(asd::handle<UIResizeMessage> & msg);

	protected:
		bind_messages(ui_space, UIMessages)

		widget * focused() const;

		widget * focusNext();
		widget * focusPrevious();

		void acquireCursorPos(int_point &) const;
		void unpress(mouse_button buttons, int x, int y, int flags);

		void updateCursorClipRect();

#if BOOST_OS_WINDOWS
		HWND _handle;
#elif BOOST_OS_LINUX
		::Display * _display;
		::Window _handle;

		struct {
			Atom DELETE_WINDOW;
		} _wm;

		auto & pool() {
			static asd::map<::Window, ui_space *> _pool;
			return _pool;
		}
#endif

		int _width, _height;

		asd::widget * _pointed = nullptr;

		std::list<asd::widget *>::iterator _focused;
		std::list<asd::widget *> _focusList; //!!! Create ordered_list
		std::set<pair<mouse_button, widget *>> _pressedList;
		mouse_state _mouseState;

		asd::unique<widget> _root = nullptr;
		asd::handle<asd::surface> _surface = nullptr;
		asd::graphics * _graphics = nullptr;
		asd::array_list<int_rect> _invalids;

#if BOOST_OS_WINDOWS
		Cursor * _cursor = nullptr;
#endif

		int_point _cursorPos;

		int_rect _cursorClipRect;
		bool _clippedCursor = false;

		bool _fullscreen = false;
		bool _active = false;
		bool _closed = false;
		bool _enabled = true;
	};

	channels_api(gui, ui_space, UIMessages)

	struct KeyMap
	{
		KeyMap() : data({0, 0, 0, 0, 0, 0, 0, 0}) {}

		void press(byte key) {
			bits::setState(key % 32, data[key / 32]);
		}

		void unpress(byte key) {
			bits::clearState(key % 32, data[key / 32]);
		}

		void unpressAll() {
			data.fill(0);
		}

		bool isPressed(byte key) {
			return bits::state(key % 32, data[key / 32]);
		}

		array<int, 8> data;
	};

	class BackgroundWidget : public widget
	{
	public:
		BackgroundWidget(ui_space * space) : widget(space, space->root()) {
			setPlacement(ModelMask::FullSize, {0, 0, 0, 0});
			setName("Background");
		}

		virtual ~BackgroundWidget() {}
	};

#ifdef WIN32
#define WS_SIMPLE (WS_POPUP | WS_CLIPCHILDREN | WS_MINIMIZEBOX)

	api(gui) HWND createEmptyWindowHandle();
	api(gui) HWND createWindowHandle(const int_rect & rect, const wstring & caption, const wstring & windowClass, WNDPROC wndProc);
#endif

}

//---------------------------------------------------------------------------
#endif
