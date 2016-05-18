//---------------------------------------------------------------------------

#ifndef UI_SPACE_H
#define UI_SPACE_H

//---------------------------------------------------------------------------

#include "Widget.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	declare_and_link(UISpace, Class<Subject>);

	class UISpace : public Subject
	{
		friend class Widget;

	public:
		UISpace(Graphics * graphics, const IntSize & size, HWND handle);
		virtual ~UISpace();

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

		const UniquePtr<Widget> & root() const
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
		void invalidate();

		/**
		 *	Marks a region of window for update.
		 */
		void invalidate(const Widget * w);

		/**
		 *	Redraws parts of the window which are marked for update.
		 */
		void validate();

		void update()
		{
			invalidate();
			validate();
		}

		virtual void registerHotkey(int id, int key, int modifiers = 0) {}
		virtual void unregisterHotkey(int id) {}

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

		create_readers(UISpace,
			KeyDownMessage,
			CharMessage,
			KeyUpMessage,
			MouseDownMessage,
			MouseUpdateMessage,
			MouseUpMessage,
			UIResizeMessage
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

		UniquePtr<Widget> _root;
		Handle<Surface> _surface = nullptr;
		Graphics * _graphics;
		vector<IntRect> _invalids;

		bool _fullscreen = false;
		bool _isClosed = false;
	};

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

		bool isPressed(byte key)
		{
			return bits::state(key % 32, data[key / 32]);
		}

		int data[8];
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
