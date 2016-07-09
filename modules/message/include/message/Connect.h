//---------------------------------------------------------------------------

#pragma once

#ifndef CONNECT_H
#define CONNECT_H

//---------------------------------------------------------------------------

#include "Channel.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Connector;

//---------------------------------------------------------------------------

	namespace Internals
	{
		template<typename T, class RealDst = Empty>
		struct can_connect_method : false_type {};

		template<class C, class Dst, class Msg>
		struct can_connect_method<void (C::*)(Handle<Msg> &, Dst &)> : is_message<Msg> {};

		template<class C, class Dst, class Msg, class RealDst>
		struct can_connect_method<void (C::*)(Handle<Msg> &, Dst &), RealDst> : is_true<is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value> {};

		template<class C, class Dst, class Msg>
		struct can_connect_method<void (C::*)(Handle<Msg> &, Dst &) const> : is_message<Msg> {};

		template<class C, class Dst, class Msg, class RealDst>
		struct can_connect_method<void (C::*)(Handle<Msg> &, Dst &) const, RealDst> : is_true<is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value> {};

		template<typename T, class RealDst = Empty>
		struct can_connect_function : false_type {};

		template<class Dst, class Msg>
		struct can_connect_function<void (Handle<Msg> &, Dst &)> : is_message<Msg> {};

		template<class Dst, class Msg, class RealDst>
		struct can_connect_function<void (Handle<Msg> &, Dst &), RealDst> : is_true<is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value> {};
	}

	template<typename Method, class RealDst = Empty>
	struct can_connect_method : Internals::can_connect_method<Method, RealDst> {};

	template<typename Function, class RealDst = Empty, bool = is_function<Function>::value>
	struct can_connect_function : Internals::can_connect_function<Function, RealDst> {};

	template<typename Function, class RealDst>
	struct can_connect_function<Function, RealDst, false> : false_type {};

	template<typename Functor, class RealDst = Empty, bool = has_caller<Functor>::value, bool = is_std_function<Functor>::value>
	struct can_connect_functor : Internals::can_connect_method<decltype(&Functor::operator()), RealDst> {};

	template<typename Functor, class RealDst>
	struct can_connect_functor<Functor, RealDst, true, true> : false_type {};

	template<typename Functor, class RealDst>
	struct can_connect_functor<Functor, RealDst, false, false> : can_connect_function<Functor, RealDst> {};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg, useif <is_message<Msg>::value> endif>
	void connect(Connector & c, const function<void(Handle<Msg> &, Dst &)> & receiver);
	template<class Dst, typename Msg, class RealDst, useif <is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value> endif>
	void connect(Connector & c, const function<void(Handle<Msg> &, Dst &)> & receiver, RealDst & dest);

//---------------------------------------------------------------------------

	class BasicConnection : public Shared
	{
	public:
		BasicConnection(size_t id) : id(id) {}
		virtual ~BasicConnection() {}

	protected:
		size_t id;
	};

	template<class Dst, typename Msg, class RealDst = Empty>
	class Connection : public BasicConnection
	{
	public:
		Connection(size_t id, RealDst & dest) : BasicConnection(id), dest(dest) {}
		virtual ~Connection() { Channel<Dst, Msg>::disconnect(dest, id); }

	protected:
		RealDst & dest;
	};

	template<class Dst, typename Msg>
	class Connection<Dst, Msg, Empty> : public BasicConnection
	{
	public:
		Connection(size_t id) : BasicConnection(id) {}
		virtual ~Connection() { Channel<Dst, Msg>::disconnect(id); }
	};

	class Connector
	{
		template<class Dst, typename Msg, useif_t>
		friend void connect(Connector &, const function<void(Handle<Msg> &, Dst &)> &);
		template<class Dst, typename Msg, class RealDst, useif_t>
		friend void connect(Connector &, const function<void(Handle<Msg> &, Dst &)> &, RealDst &);

		template<class Dst, typename Msg>
		void add(size_t id)
		{
			_connections.push_back(Handle<Connection<Dst, Msg>>(id));
		}

		template<class Dst, typename Msg, class RealDst>
		void add(size_t id, RealDst & dest)
		{
			_connections.push_back(Handle<Connection<Dst, Msg, RealDst>>(id, dest));
		}

		ArrayList<BasicConnection> _connections;
	};

//---------------------------------------------------------------------------

	/**
	 *	Permanent connections (lifetime of a connection < lifetime of a receiver >= lifetime of a channel/destination)
	 */

	template<class Dst, typename Msg, useif <is_message<Msg>::value> endif>
	forceinline void connect(const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		Channel<Dst, Msg>::connect(receiver);
	}

	template<class Functor, useif <can_connect_functor<Functor>::value> endif, skipif <based_on<Functor, Connector>::value> endif>
	forceinline void connect(Functor & receiver)
	{
		connect(make_function(receiver));
	}

	template<class Functor, useif <can_connect_functor<Functor>::value> endif, skipif <based_on<Functor, Connector>::value> endif>
	forceinline void connect(Functor && receiver)
	{
		connect(make_function(forward<Functor>(receiver)));
	}

	template<class Rcvr, class Method, useif <is_method<Rcvr, Method>::value, can_connect_method<Method>::value> endif, skipif <based_on<Rcvr, Connector>::value> endif>
	forceinline void connect(Rcvr receiver, Method method)
	{
		connect(make_function(receiver, method));
	}

	template<class Dst, typename Msg, class RealDst, useif <is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value> endif>
	forceinline void connect(const function<void(Handle<Msg> &, Dst &)> & receiver, RealDst & dest)
	{
		Channel<Dst, Msg>::connect(dest, receiver);
	}

	template<class Functor, class RealDst, useif <can_connect_functor<Functor, RealDst>::value> endif, skipif <based_on<Functor, Connector>::value> endif>
	forceinline void connect(Functor & receiver, RealDst & dest)
	{
		connect(make_function(receiver), dest);
	}

	template<class Functor, class RealDst, useif <can_connect_functor<Functor, RealDst>::value> endif, skipif <based_on<Functor, Connector>::value> endif>
	forceinline void connect(Functor && receiver, RealDst & dest)
	{
		connect(make_function(forward<Functor>(receiver)), dest);
	}

	template<class Rcvr, class Method, class RealDst, useif <is_method<decay_t<Rcvr>, Method>::value, can_connect_method<Method, RealDst>::value> endif, skipif <based_on<Rcvr, Connector>::value> endif>
	forceinline void connect(Rcvr receiver, Method method, RealDst & dest)
	{
		connect(make_function(receiver, method), dest);
	}

//---------------------------------------------------------------------------

	/**
	 *	Collectable connections (lifetime of a connection < lifetime of a receiver < lifetime of a channel/destination)
	 */

	template<class Dst, typename Msg, useif_t>
	forceinline void connect(Connector & c, const function<void(Handle<Msg> &, Dst &)> & receiver)
	{
		c.add<Dst, Msg>(Channel<Dst, Msg>::connect(receiver));
	}

	template<class Functor, useif <can_connect_functor<Functor>::value> endif>
	forceinline void connect(Connector & c, Functor & receiver)
	{
		connect(c, make_function(receiver));
	}

	template<class Functor, useif <can_connect_functor<Functor>::value> endif>
	forceinline void connect(Connector & c, Functor && receiver)
	{
		connect(c, make_function(forward<Functor>(receiver)));
	}

	template<class Rcvr, class Method, useif <is_method<Rcvr, Method>::value, can_connect_method<Method>::value> endif>
	forceinline void connect(Connector & c, Rcvr receiver, Method method)
	{
		connect(c, make_function(receiver, method));
	}

	template<class Dst, typename Msg, class RealDst, useif_t>
	forceinline void connect(Connector & c, const function<void(Handle<Msg> &, Dst &)> & receiver, RealDst & dest)
	{
		c.add<Dst, Msg>(Channel<Dst, Msg>::connect(dest, receiver), dest);
	}

	template<class Functor, class RealDst, useif <can_connect_functor<Functor, RealDst>::value> endif>
	forceinline void connect(Connector & c, Functor & receiver, RealDst & dest)
	{
		connect(c, make_function(receiver), dest);
	}

	template<class Functor, class RealDst, useif <can_connect_functor<Functor, RealDst>::value> endif>
	forceinline void connect(Connector & c, Functor && receiver, RealDst & dest)
	{
		connect(c, make_function(forward<Functor>(receiver)), dest);
	}

	template<class Rcvr, class Method, class RealDst, useif <is_method<Rcvr, Method>::value, can_connect_method<Method, RealDst>::value> endif>
	forceinline void connect(Connector & c, Rcvr receiver, Method method, RealDst & dest)
	{
		connect(c, make_function(receiver, method), dest);
	}

//---------------------------------------------------------------------------

	/**
	 *	Automatic connections (lifetime of a connection = lifetime of a receiver < lifetime of a channel/destination)
	 */

	template<class Rcvr, useif <can_connect_functor<Rcvr>::value> endif, useif <based_on<Rcvr, Connector>::value> endif>
	forceinline void connect(Rcvr * receiver)
	{
		connect(*receiver, make_function(receiver));
	}

	template<class Rcvr, class RealDst, useif <can_connect_functor<Rcvr, RealDst>::value> endif, useif <based_on<Rcvr, Connector>::value> endif>
	forceinline void connect(Rcvr * receiver, RealDst & dest)
	{
		connect(*receiver, make_function(receiver), dest);
	}

	template<class Rcvr, class Method, useif <is_method<Rcvr, Method>::value, can_connect_method<Method>::value> endif, useif <based_on<Rcvr, Connector>::value> endif>
	forceinline void connect(Rcvr * receiver, Method method)
	{
		connect(*receiver, make_function(receiver, method));
	}

	template<class Rcvr, class Method, class RealDst, useif <is_method<Rcvr, Method>::value, can_connect_method<Method, RealDst>::value> endif, useif <based_on<Rcvr, Connector>::value> endif>
	forceinline void connect(Rcvr * receiver, Method method, RealDst & dest)
	{
		connect(*receiver, make_function(receiver, method), dest);
	}

	template<class Rcvr, useif <can_connect_functor<Rcvr>::value> endif, useif <based_on<Rcvr, Connector>::value> endif>
	forceinline void connect(Rcvr & receiver)
	{
		connect(receiver, make_function(receiver));
	}

	template<class Rcvr, class RealDst, useif <can_connect_functor<Rcvr, RealDst>::value> endif, useif <based_on<Rcvr, Connector>::value> endif>
	forceinline void connect(Rcvr & receiver, RealDst & dest)
	{
		connect(receiver, make_function(receiver), dest);
	}

	template<class Rcvr, class Method, useif <is_method<Rcvr, Method>::value, can_connect_method<Method>::value> endif, useif <based_on<Rcvr, Connector>::value> endif>
	forceinline void connect(Rcvr & receiver, Method method)
	{
		connect(receiver, make_function(receiver, method));
	}

	template<class Rcvr, class Method, class RealDst, useif <is_method<Rcvr, Method>::value, can_connect_method<Method, RealDst>::value> endif, useif <based_on<Rcvr, Connector>::value> endif>
	forceinline void connect(Rcvr & receiver, Method method, RealDst & dest)
	{
		connect(receiver, make_function(receiver, method), dest);
	}
}

//---------------------------------------------------------------------------
#endif
