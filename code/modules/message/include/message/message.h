//---------------------------------------------------------------------------

#pragma once

#ifndef MESSAGE_H
#define MESSAGE_H

//---------------------------------------------------------------------------

#include "MessageClass.h"
#include <function/method.h>
#include <core/handle.h>

//---------------------------------------------------------------------------

namespace asd
{
	class subject;
	
	template<class, typename>
	struct Channel;
	
	/**
	 *  @brief
	 *  Basic class for all messages
	 */
	struct message : shareable
	{
		const subject * source;
		int result = 0;
		
		message(const subject * source) : source(source) {}
		message(const message &) = delete;
	};
	
	namespace internals
	{
		template<class T>
		struct message : asd::message, asd::contents<T>
		{
			template<class ... A, useif<can_construct_contents<T, A...>::value>>
			message(const subject * source, A &&... args) : asd::message(source), asd::contents<T>(forward<A>(args)...) {}
		};
	}
	
	template<class T>
	using is_message = based_on<T, message>;
	
	/**
	 *  @brief
	 *  Message callback can be binded to any destination which can read
	 *	specified message
	 */
	template<class Dst, typename Msg>
	using msg_callback = function<void(handle<Msg> &, Dst &)>;
	
	/**
	 *  @brief
	 *  Wrapper for message callback
	 */
	template<class Dst, typename Msg>
	class Receiver : public auto_id, public shareable
	{
		deny_copy(Receiver);
		typedef msg_callback<Dst, Msg> Callback;
	
	public:
		Receiver(const Callback & callback) : callback(callback), id(reinterpret_cast<size_t>(reinterpret_cast<const void *>(callback.template target<void(*)(handle<Msg> &, Dst &)>()))) {}
		Receiver(Receiver && receiver) : callback(std::move(receiver.callback)), id(receiver.id) {}
		
		Receiver & operator = (Receiver && receiver)
		{
			callback = std::move(receiver.callback);
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
