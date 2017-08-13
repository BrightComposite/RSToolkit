//---------------------------------------------------------------------------

#pragma once

#ifndef MESSAGE_CONNECTOR_H
#define MESSAGE_CONNECTOR_H

//---------------------------------------------------------------------------

#include <container/ArrayList.h>
#include <forward_list>

#include "Message.h"

//---------------------------------------------------------------------------

namespace asd
{
	template<class Dst, class Msg>
	struct is_dest;

//---------------------------------------------------------------------------

	template<class Dst, class Msg>
	using ReceiversList = array_list<Receiver<Dst, Msg>>;

//---------------------------------------------------------------------------
	
	namespace internals
	{
		template<typename T, class RealDst = empty>
		struct can_connect_method : false_type {};
		
		template<class C, class Dst, class Msg>
		struct can_connect_method<void (C::*)(handle<Msg> &, Dst &)> : is_message<Msg> {};
		
		template<class C, class Dst, class Msg, class RealDst>
		struct can_connect_method<void (C::*)(handle<Msg> &, Dst &), RealDst> : is_true<is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value> {};
		
		template<class C, class Dst, class Msg>
		struct can_connect_method<void (C::*)(handle<Msg> &, Dst &) const> : is_message<Msg> {};
		
		template<class C, class Dst, class Msg, class RealDst>
		struct can_connect_method<void (C::*)(handle<Msg> &, Dst &) const, RealDst> : is_true<is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value> {};
		
		template<typename T, class RealDst = empty>
		struct can_connect_function : false_type {};
		
		template<class Dst, class Msg>
		struct can_connect_function<void (handle<Msg> &, Dst &)> : is_message<Msg> {};
		
		template<class Dst, class Msg, class RealDst>
		struct can_connect_function<void (handle<Msg> &, Dst &), RealDst> : is_true<is_message<Msg>::value, is_dest<RealDst, Msg>::value, based_on<RealDst, Dst>::value> {};
	}
	
	template<typename Method, class RealDst = empty>
	struct can_connect_method : internals::can_connect_method<Method, RealDst> {};
	
	template<typename Function, class RealDst = empty, bool = is_function<Function>::value>
	struct can_connect_function : internals::can_connect_function<Function, RealDst> {};
	
	template<typename Function, class RealDst>
	struct can_connect_function<Function, RealDst, false> : false_type {};
	
	template<typename Functor, class RealDst = empty, bool = has_caller<Functor>::value, bool = is_std_function<Functor>::value>
	struct can_connect_functor : internals::can_connect_method<decltype(&Functor::operator()), RealDst> {};
	
	template<typename Functor, class RealDst>
	struct can_connect_functor<Functor, RealDst, true, true> : false_type {};
	
	template<typename Functor, class RealDst>
	struct can_connect_functor<Functor, RealDst, false, false> : can_connect_function<Functor, RealDst> {};

//---------------------------------------------------------------------------

	template<class Dst, typename Msg>
	struct ReceiversWrapper
	{
		ReceiversList<Dst, Msg> & receivers;

		template<class F, useif<can_connect_functor<F>::value>>
		auto operator += (F & f) {
			return connect(f, &F::operator());
		}

		template<class F, useif<can_connect_functor<F>::value>>
		auto operator += (F && f) {
			return connect(forward<F>(f), &F::operator());
		}

	private:
		template<class F, class C>
		void connect(F & f, void (C::*mf) (handle<Msg> &, Dst &)) {
			receivers.emplace_back(f);
		}

		template<class F, class C>
		void connect(F && f, void (C::*mf) (handle<Msg> &, Dst &)) {
			receivers.emplace_back(forward<F>(f));
		}

		template<class F, class C, class D, skipif<is_same<Dst, D>::value>, useif<is_base_of<Dst, D>::value>>
		void connect(F & f, void (C::*mf) (handle<Msg> &, D &)) {
			receivers.emplace_back([&f](handle<Msg> & message, Dst & dest) {
				f(message, static_cast<D &>(dest));
			});
		}

		template<class F, class C, class D, skipif<is_same<Dst, D>::value>, useif<is_base_of<Dst, D>::value>>
		void connect(F && f, void (C::*mf) (handle<Msg> &, D &)) {
			receivers.emplace_back([f](handle<Msg> & message, Dst & dest) {
				f(message, static_cast<D &>(dest));
			});
		}

		template<class F, class C>
		void connect(F & f, void (C::*mf) (handle<Msg> &, Dst &) const) {
			receivers.emplace_back(f);
		}

		template<class F, class C>
		void connect(F && f, void (C::*mf) (handle<Msg> &, Dst &) const) {
			receivers.emplace_back(forward<F>(f));
		}

		template<class F, class C, class D, skipif<is_same<Dst, D>::value>, useif<is_base_of<Dst, D>::value>>
		void connect(F & f, void (C::*mf) (handle<Msg> &, D &) const) {
			receivers.emplace_back([&f](handle<Msg> & message, Dst & dest) {
				f(message, static_cast<D &>(dest));
			});
		}

		template<class F, class C, class D, skipif<is_same<Dst, D>::value>, useif<is_base_of<Dst, D>::value>>
		void connect(F && f, void (C::*mf) (handle<Msg> &, D &) const) {
			receivers.emplace_back([f](handle<Msg> & message, Dst & dest) {
				f(message, static_cast<D &>(dest));
			});
		}
	};

//---------------------------------------------------------------------------

	template<class MsgDst, typename Msg, bool = is_dest<MsgDst, Msg>::value>
	struct MessageConnector {};

	template<class Dst, typename Msg>
	struct MessageConnector<Dst, Msg, false>
	{
		using Rcvr = Receiver<Dst, Msg>;

		static size_t connect(Rcvr && receiver) {
			size_t id = receiver.id;
			globalReceivers().push_back(forward<Rcvr>(receiver));
			return id;
		}

		static void disconnect(size_t id) {
			auto & r = globalReceivers();

			r.erase(std::remove_if(r.begin(), r.end(), [id](Rcvr & rcvr) {
				return rcvr.id == id;
			}), r.end());
		}

		static forceinline ReceiversWrapper<Dst, Msg> receivers() {
			return {globalReceivers()};
		}

		static size_t count() {
			return globalReceivers().size();
		}

		static forceinline ReceiversList<Dst, Msg> & globalReceivers() {
			static ReceiversList<Dst, Msg> r;
			return r;
		}
	};

	template<class Dst, typename Msg>
	struct MessageConnector<Dst, Msg, true> : MessageConnector<Dst, Msg, false>
	{
		using Base = MessageConnector<Dst, Msg, false>;
		using Rcvr = Receiver<Dst, Msg>;
		using Rcvrs = Receivers<Dst, Msg>;

		using Base::connect;
		using Base::disconnect;
		using Base::receivers;

		static size_t connect(Dst & dest, Rcvr && receiver) {
			return Rcvrs::addTo(dest, forward<Rcvr>(receiver));
		}

		static forceinline ReceiversWrapper<Dst, Msg> receivers(Dst & dest) {
			return {Rcvrs::requireFrom(dest)};
		}

		static void disconnect(Dst & dest, size_t id) {
			auto * local = Receivers<Dst, Msg>::seek(dest);

			if(local == nullptr || local->empty()) {
				return;
			}

			local->erase(std::remove_if(local->begin(), local->end(), [id](Rcvr & rcvr) {
				return rcvr.id == id;
			}), local->end());
		}
	};
}

//---------------------------------------------------------------------------
#endif
