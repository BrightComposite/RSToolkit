//---------------------------------------------------------------------------

#pragma once

#ifndef STATE_H
#define STATE_H

//---------------------------------------------------------------------------

#include <container/Stack.h>
#include <container/ArrayList.h>

//---------------------------------------------------------------------------

namespace asd
{
	template<typename T>
	class state : public shareable
	{
	public:
		state() : _state() {}
		
		state(const state & s) : _state(s._state) {}
		
		template<class ... A, useif<can_construct<T, A...>::value>>
		state(A && ... args) : _state(forward<A>(args)...) {}
		
		virtual ~state() {}
		
		state & operator =(const state & s) {
			set(s._state);
			return *this;
		}
		
		state & operator =(state && s) {
			set(move(s._state));
			return *this;
		}
		
		state & operator =(const T & value) {
			set(value);
			return *this;
		}
		
		state & operator =(T && value) {
			set(forward<T>(value));
			return *this;
		}
		
		void set(const T & newState) {
			if(_state == newState) {
				return;
			}
			
			_state = newState;
			change();
		}
		
		void set(T && newState) {
			if(_state == newState) {
				return;
			}
			
			_state = forward<T>(newState);
			change();
		}
		
		template<class ... A, useif<not_same_types<T, tuple<A...>>::value, can_construct<T, A...>::value>>
		void set(A && ... a) {
			_state = T {forward<A>(a)...};
			change();
		}
		
		operator const T &() const {
			return _state;
		}
		
		T & get() {
			return _state;
		}
		
		const T & get() const {
			return _state;
		}
	
	protected:
		virtual void change() {}
		
		T _state;
	};
	
	template<class T>
	using StateHandle = handle<state<T>>;
	
	template<class T>
	class state_holder
	{
	public:
		state_holder(asd::state<T> * state, const T & newState) : state(state), old(state->get()) {
			state->set(newState);
		}
		
		state_holder(asd::state<T> * state, T && newState) : state(state), old(state->get()) {
			state->set(forward<T>(newState));
		}
		
		state_holder(const state_holder & h) = delete;
		
		state_holder(state_holder && h) : state(h.state), old(h.old) {
			h.state = nullptr;
		}
		
		~state_holder() {
			if(state != nullptr) {
				state->set(old);
			}
		}
		
		state_holder & operator =(const state_holder & h) = delete;
		
		state_holder & operator =(state_holder && h) {
			state = h.state;
			old = h.old;
			h.state = nullptr;
			
			return *this;
		}
		
		state_holder & operator =(const T & newState) {
			state->set(newState);
			return *this;
		}
		
		state_holder & operator =(T && newState) {
			state->set(forward<T>(newState));
			return *this;
		}
		
		asd::state<T> * state;
		T old;
	};
	
	template<class T>
	state_holder<T> hold(asd::state<T> * state, const T & newState) {
		return {state, newState};
	}
	
	template<class T>
	state_holder<T> hold(asd::state<T> * state, T && newState) {
		return {state, forward<T>(newState)};
	}
	
	template<class T>
	state_holder<T> hold(const handle<asd::state<T>> & state, const T & newState) {
		return {state, newState};
	}
	
	template<class T>
	state_holder<T> hold(const handle<asd::state<T>> & state, T && newState) {
		return {state, forward<T>(newState)};
	}
}

//---------------------------------------------------------------------------
#endif
