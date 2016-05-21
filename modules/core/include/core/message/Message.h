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
		struct Message : Rapture::Message, Rapture::Contents<T>
		{
			template<class ... A, useif <can_construct_contents<T, A...>::value> endif>
			Message(const Subject * source, A &&... args) : Rapture::Message(source), Rapture::Contents<T>(forward<A>(args)...) {}
		};
	}

	/**
	 *  @brief
	 *  Message callback can be binded to any destination which can read
	 *	specified message
	 */
	template<class Dst, typename Msg>
	using msg_callback = function<void(Handle<Msg> &, Dst &)>;

	template<class T, class Dst, typename Msg>
	using can_receive = is_callable<T, Handle<Msg> &, Dst &>;

	/**
	 *  @brief
	 *  Wrapper for message callback
	 */
	template<class Dst, typename Msg>
	class Receiver : public AutoIdentifier, public Shared
	{
		typedef msg_callback<Dst, Msg> Callback;

	public:
		Receiver(const Callback & callback, size_t id, size_t additional = 0) : callback(callback), id(id), additional(additional) {}
		Receiver(const Callback & callback, const void * data, size_t additional = 0) : Receiver(callback, reinterpret_cast<size_t>(data), additional) {}
		Receiver(void callback(Handle<Msg> &, Dst &)) : Receiver(callback, reinterpret_cast<void *>(callback)) {}
		Receiver(const Callback & callback) : Receiver(callback, reinterpret_cast<const void *>(callback.template target<void(*)(Handle<Msg> &, Dst &)>())) {}
		template<class Rcvr>
		Receiver(Rcvr * receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t additional = 0) : Receiver(std::bind(callback, receiver, std::placeholders::_1, std::placeholders::_2), static_cast<void *>(receiver), additional) {}
		template<class Rcvr>
		Receiver(const Rcvr * receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t additional = 0) : Receiver(std::bind(callback, receiver, std::placeholders::_1, std::placeholders::_2), static_cast<const void *>(receiver), additional) {}
		template<class Rcvr, class ... Owner>
		Receiver(const Handle<Rcvr, Owner...> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &), size_t additional = 0) : Receiver(std::bind(callback, receiver, std::placeholders::_1, std::placeholders::_2), static_cast<void *>(static_cast<Rcvr *>(receiver)), additional) {}
		template<class Rcvr, class ... Owner>
		Receiver(const Handle<Rcvr, Owner...> & receiver, void(__thiscall Rcvr::*callback)(Handle<Msg> &, Dst &) const, size_t additional = 0) : Receiver(std::bind(callback, receiver, std::placeholders::_1, std::placeholders::_2), static_cast<const void *>(static_cast<const Rcvr *>(receiver)), additional) {}
		Receiver(const Receiver & mc) : callback(mc.callback), id(mc.id), additional(mc.additional) {}

		Receiver & operator = (const Receiver & mc)
		{
			callback = mc.callback;
			id = mc.id;
			additional = mc.additional;
			return *this;
		}

		bool operator == (const Receiver & mc) const
		{
			return id == mc.id && additional == mc.additional;
		}

		Callback callback;
		size_t id;
		size_t additional = 0;
	};
}

//---------------------------------------------------------------------------
#endif
