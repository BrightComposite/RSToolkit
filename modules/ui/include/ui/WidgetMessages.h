//---------------------------------------------------------------------------

#pragma once

#ifndef WIDGET_MESSAGES_H
#define WIDGET_MESSAGES_H

//---------------------------------------------------------------------------

#include <message/Message.h>
#include <core/Handle.h>
#include <math/Rect.h>

#include "WidgetEnum.h"

//---------------------------------------------------------------------------

namespace asd
{
//---------------------------------------------------------------------------

	// Some key was pressed when this widget was in focus
	message_class
	(
		KeyDownMessage,
		(int, key)
		(int, times)
		(bool, isFirst)
		(bool, isAdditional)
	);

//---------------------------------------------------------------------------

	// Some char-key was pressed when this widget was in focus
	message_class
	(
		CharMessage,
		(int, key)
		(int, times)
		(bool, isFirst)
		(bool, isAdditional)
	);

//---------------------------------------------------------------------------

	// Some key was released when this Widget was in focus
	message_class
	(
		KeyUpMessage,
		(int, key)
		(int, times)
		(bool, isFirst)
		(bool, isAdditional)
	);

//---------------------------------------------------------------------------

	// Mouse state is updating over this widget
	message_class
	(
		MouseUpdateMessage,
		(int, x)
		(int, y)
	);

//---------------------------------------------------------------------------

	// Some mouse button was pressed over this widget
	message_class
	(
		MouseDownMessage,
		(MouseButton, button)
		(int, x)
		(int, y)
		(int, flags)
	);

//---------------------------------------------------------------------------

	// Mouse is moving over this widget
	message_class
	(
		MouseMoveMessage,
		(int, x)
		(int, y)
	);

//---------------------------------------------------------------------------

	// Some mouse button was released over this widget
	message_class
	(
		MouseUpMessage,
		(MouseButton, button)
		(int, x)
		(int, y)
		(int, flags)
	);

//---------------------------------------------------------------------------

	// Some mouse button was clicked over this widget
	message_class
	(
		MouseClickMessage,
		(MouseButton, button)
		(int, x)
		(int, y)
		(int, flags)
	);

//---------------------------------------------------------------------------

	// Some mouse button was double-clicked over this widget
	message_class
	(
		MouseDblClickMessage,
		(MouseButton, button)
		(int, x)
		(int, y)
		(int, flags)
	);

//---------------------------------------------------------------------------

	// Mouse wheel was moved when this widget was in focus
	message_class
	(
		MouseWheelMessage,
		(int, data)
		(int, x)
		(int, y)
		(int, flags)
	);

//---------------------------------------------------------------------------

	// Mouse has entered to the region of this widget
	message_class(MouseEnterMessage);

//---------------------------------------------------------------------------

	// Mouse has left to the region of this widget
	message_class(MouseLeaveMessage);

//---------------------------------------------------------------------------

	// After mouse down
	message_class
	(
		WidgetPressMessage,
		(MouseButton, button)
	);

//---------------------------------------------------------------------------

	// After mouse up or leave
	message_class
	(
		WidgetStopPressMessage,
		(MouseButton, button)
	);

//---------------------------------------------------------------------------

	// After mouse up
	message_class
	(
		WidgetReleaseMessage,
		(MouseButton, button)
	);

//---------------------------------------------------------------------------

	// Widget has changed one of its states
	message_class
	(
		WidgetChangedStateMessage,
		(WidgetState, state)
		(bool, value)
	);

//---------------------------------------------------------------------------

	// Widget is preparing to change its focus order
	message_class
	(
		ChangeFocusOrderMessage,
		(int, newValue)
		(int, oldValue)
	);

//---------------------------------------------------------------------------

	// Widget has changed its focus order
	message_class
	(
		AfterChangeFocusOrderMessage,
		(int, newValue)
	);

//---------------------------------------------------------------------------

	// Widget is preparing to change its display order
	message_class
	(
		ChangeDisplayOrderMessage,
		(int, newValue)
		(int, oldValue)
	);

//---------------------------------------------------------------------------

	// Widget has changed its display order
	message_class
	(
		AfterChangeDisplayOrderMessage,
		(int, newValue)
	);

//---------------------------------------------------------------------------

	// Widget should change its placement
	message_class
	(
		WidgetMoveMessage,
		(int, left)
		(int, top)
		(int, right)
		(int, bottom)
		(ModelMask, mask)
	);

//---------------------------------------------------------------------------

	// Widget has changed its placement
	message_class
	(
		AfterWidgetMoveMessage,
		(int, left)
		(int, top)
		(int, right)
		(int, bottom)
	);

//---------------------------------------------------------------------------

	// Widget has changed its sizes
	message_class
	(
		WidgetResizeMessage,
		(int, width)
		(int, height)
		(ModelMask, mask)
	);

//---------------------------------------------------------------------------

	// Widget must be painted
	message_class
	(
		WidgetDrawMessage,
		(Handle<IntRect>, DrawRegion)
		(Handle<IntRect>, ClipRegion)
	);

//---------------------------------------------------------------------------

	// Hot-key combination has been pressed while UI-space was in focus
	message_class
	(
		HotkeyMessage,
		(int, id)
	);

	// UI-space has changed its position
	message_class
	(
		UIMoveMessage,
		(int, x)
		(int, y)
	);

	// UI-space has changed its size
	message_class
	(
		UIResizeMessage,
		(int, width)
		(int, height)
	);

	// UI-space has changed its fullscreen state
	message_class
	(
		UIFullscreenMessage,
		(bool, fullscreen)
	);

	// UI-space is destroying
	message_class(UIDestroyMessage);
	
	// Send this message to window to close it (it is usually sent automatically when window is closed by system command)
	message_class(WindowCloseMessage);

//---------------------------------------------------------------------------

#define WidgetMessages	                                                                                                    \
	KeyDownMessage, CharMessage, KeyUpMessage, MouseUpdateMessage,                                                          \
	MouseDownMessage, MouseMoveMessage, MouseUpMessage, MouseClickMessage,                                                  \
	MouseDblClickMessage, MouseWheelMessage, MouseEnterMessage,                                                             \
	MouseLeaveMessage, WidgetPressMessage, WidgetStopPressMessage,                                                          \
	WidgetReleaseMessage, WidgetChangedStateMessage,                                                                        \
	ChangeFocusOrderMessage, AfterChangeFocusOrderMessage,                                                                  \
	ChangeDisplayOrderMessage, AfterChangeDisplayOrderMessage,                                                              \
	WidgetDrawMessage, WidgetMoveMessage, AfterWidgetMoveMessage,                                                           \
	WidgetResizeMessage

#define UIMessages																											\
	KeyDownMessage, CharMessage, KeyUpMessage, MouseUpdateMessage,                                                          \
	MouseDownMessage, MouseMoveMessage, MouseUpMessage, HotkeyMessage,                                                      \
	UIMoveMessage, UIResizeMessage, UIFullscreenMessage, UIDestroyMessage													\

#define WindowMessages	                                                                                                    \
	WindowCloseMessage
}

//---------------------------------------------------------------------------
#endif
