//---------------------------------------------------------------------------

#pragma once

#ifndef MESSAGE_H
#define MESSAGE_H

//---------------------------------------------------------------------------

#include "MessageClass.h"
#include <core/function/Function.h>

//---------------------------------------------------------------------------

namespace asd
{
	class Subject;

	template<class, typename>
	struct Channel;

	/**
	 *  @brief
	 *  Basic class for all messages
	 */
	struct Message : Shared
	{
		const Subject * source;
		int result = 0;

		Message(const Subject * source) : source(source) {}
		Message(const Message &) = delete;
	};

	namespace Internals
	{
		template<class T>
		struct Message : asd::Message, asd::Contents<T>
		{
			template<class ... A, useif<can_construct_contents<T, A...>::value>>
			Message(const Subject * source, A &&... args) : asd::Message(source), asd::Contents<T>(forward<A>(args)...) {}
		};
	}

	template<class T>
	using is_message = based_on<T, Message>;

	/**
	 *  @brief
	 *  Message callback can be binded to any destination which can read
	 *	specified message
	 */
	template<class Dst, typename Msg>
	using msg_callback = function<void(Handle<Msg> &, Dst &)>;

	/**
	 *  @brief
	 *  Wrapper for message callback
	 */
	template<class Dst, typename Msg>
	class Receiver : public AutoIdentifier, public Shared
	{
		deny_copy(Receiver);
		typedef msg_callback<Dst, Msg> Callback;

	public:
		Receiver(const Callback & callback) : callback(callback), id(reinterpret_cast<size_t>(reinterpret_cast<const void *>(callback.template target<void(*)(Handle<Msg> &, Dst &)>()))) {}
		Receiver(Receiver && receiver) : callback(move(receiver.callback)), id(receiver.id) {}

		Receiver & operator = (Receiver && receiver)
		{
			callback = move(receiver.callback);
			id = receiver.id;

			return *this;
		}

		bool operator == (const Receiver & mc) const
		{
			return id == mc.id;
		}

		Callback callback;
		size_t id;
	};
}

//---------------------------------------------------------------------------
#endif
