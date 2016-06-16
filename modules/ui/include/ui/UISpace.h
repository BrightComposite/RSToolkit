//---------------------------------------------------------------------------

#ifndef UI_SPACE_H
#define UI_SPACE_H

//---------------------------------------------------------------------------

#include "Widget.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class UISpace; 

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

		/**
		 *	Marks the whole window for update.
		 */
		void api(ui) invalidate();

		/**
		 *	Marks a region of window for update.
		 */
		void api(ui) invalidate(const Widget * w);

		/**
		 *	Redraws parts of the window which are marked for update.
		 */
		void api(ui) validate();

		void update()
		{
			invalidate();
			validate();
		}

		virtual void api(ui) registerHotkey(int id, int key, int modifiers = 0) {}
		virtual void api(ui) unregisterHotkey(int id) {}

		virtual void api(ui) read(Handle<KeyDownMessage> & msg);
		virtual void api(ui) read(Handle<CharMessage> & msg);
		virtual void api(ui) read(Handle<KeyUpMessage> & msg);
		virtual void api(ui) read(Handle<MouseDownMessage> & msg);
		virtual void api(ui) read(Handle<MouseUpdateMessage> & msg);
		virtual void api(ui) read(Handle<MouseUpMessage> & msg);
		virtual void api(ui) read(Handle<UIResizeMessage> & msg);

	protected:
		bind_messages(UISpace,
			KeyDownMessage,
			CharMessage,
			KeyUpMessage,
			MouseDownMessage,
			MouseUpdateMessage,
			MouseUpMessage,
			HotkeyMessage,
			UIMessages
		)

		Widget * focused() const;

		Widget * focusNext();
		Widget * focusPrevious();

		void unpress(MouseButton buttons, int x, int y, int flags);

		HWND _handle;
		int _width, _height;

		Widget * _pointed = nullptr;

		list<Widget *>::iterator _focused;
		list<Widget *> _focusList; //!!! Create ordered_list
		set<pair<MouseButton, Widget *>> _pressedList;
		MouseState _mouseState;

		UniqueHandle<Widget> _root;
		Handle<Surface> _surface = nullptr;
		Graphics * _graphics;
		array_list<IntRect> _invalids;

		bool _fullscreen = false;
		bool _isClosed = false;
	};

	channels_api(ui, UISpace,
		KeyDownMessage,
		CharMessage,
		KeyUpMessage,
		MouseDownMessage,
		MouseUpdateMessage,
		MouseUpMessage,
		HotkeyMessage,
		UIMessages
	)

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

	HWND createEmptyWindowHandle();
	HWND createWindowHandle(const IntRect & rect, const wstring & caption, const wstring & windowClass, WNDPROC wndProc);
}

//---------------------------------------------------------------------------
#endif
