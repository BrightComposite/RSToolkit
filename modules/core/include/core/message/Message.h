//---------------------------------------------------------------------------

#ifndef MESSAGE_H
#define MESSAGE_H

//---------------------------------------------------------------------------

#include "MessageClass.h"
#include <core/action/Action.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Subject;
	template<class, typename>
	class Channel;

	/**
	 *  @brief
	 *  Basic class for all messages
	 */
	class Message : public Shareable<Message>
	{
	public:
		int result;

		Message() : result(0) {}
        virtual ~Message() {}

		bool operator == (const Message &) const
		{
			return true;
		}

		template <class Msg>
		bool operator != (const Msg & msg) const
		{
			return !msg.operator == (static_cast<const Msg &>(*this));
		}
	};

	/**
	 *  @brief
	 *  Message callback can be binded to any destination which can read
	 *	specified message
	 */
	template<class Dst, typename Msg>
	using msg_callback = function<void(Handle<Msg> &, Dst &, const Subject *)>;

	/**
	 *  @brief
	 *  Wrapper for message callback
	 */
	template<class Dst, typename Msg>
	class MessageCallback : public AutoIdentifier, public Shareable<MessageCallback<Dst, Msg>>
	{
		typedef msg_callback<Dst, Msg> Callback;

	public:
		MessageCallback(const Callback & callback, size_t id, size_t additional = 0) : callback(callback), id(id), additional(additional) {}
		MessageCallback(const Callback & callback, const void * data, size_t additional = 0) : MessageCallback(callback, reinterpret_cast<size_t>(data), additional) {}
		MessageCallback(void callback(Handle<Msg> &, Dst &, const Subject *)) : MessageCallback(callback, static_cast<void *>(callback)) {}
		MessageCallback(const Callback & callback) : MessageCallback(callback, static_cast<const void *>(callback.target<void(*)(Handle<Msg> &, Dst &, const Subject *)>())) {}
		template<class Rcvr>
		MessageCallback(Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &, const Subject *), size_t additional = 0) : MessageCallback(wrap_method(receiver, callback), static_cast<void *>(static_cast<Rcvr *>(receiver)), additional) {}
		template<class Rcvr>
		MessageCallback(const Handle<Rcvr> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &, const Subject *) const, size_t additional = 0) : MessageCallback(wrap_method(receiver, callback), static_cast<const void *>(static_cast<const Rcvr *>(receiver)), additional) {}
		MessageCallback(const MessageCallback & mc) : callback(mc.callback), id(mc.id), additional(mc.additional) {}

		MessageCallback & operator = (const MessageCallback & mc)
		{
			callback = mc.callback;
			id = mc.id;
			additional = mc.additional;
			return *this;
		}

		bool operator == (const MessageCallback & mc) const
		{
			return id == mc.id && additional == mc.additional;
		}

		Callback callback;
		size_t id;
		size_t additional = 0;
	};

	/**
	 *  @brief
	 *  Helping struct for getting of a set of message receivers.
	 *	All specializations are defined automatically by message_class macro
	 */
	template<class Dst, typename Msg>
	class MessageDispatcher
	{
	public:
		static const bool hasDestination = false;
	};
}

//---------------------------------------------------------------------------
#endif
