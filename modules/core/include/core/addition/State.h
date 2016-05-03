//---------------------------------------------------------------------------

#ifndef STATE_H
#define STATE_H

//---------------------------------------------------------------------------

#include <core/container/Stack.h>
#include <core/container/Array.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	template<typename T>
	class State : public Shared
	{
	public:
		State() : _state() {}
		State(const State & state) : _state(state._state) {}
		template<class ... A, useif <can_construct<T, A...>::value> endif>
		State(A &&... args) : _state(forward<A>(args)...) {}
		virtual ~State() {}

		State & operator = (const State & state)
		{
			set(state._state);
			return *this;
		}

		State & operator = (State && state)
		{
			set(move(state._state));
			return *this;
		}

		State & operator = (const T & value)
		{
			set(value);
			return *this;
		}

		State & operator = (T && value)
		{
			set(forward<T>(value));
			return *this;
		}

		void set(const T & newState)
		{
			if(_state == newState)
				return;

			_state = newState;
			change();
		}

		void set(T && newState)
		{
			if(_state == newState)
				return;

			_state = forward<T>(newState);
			change();
		}

		operator const T & () const
		{
			return _state;
		}

		T & get()
		{
			return _state;
		}

		const T & get() const
		{
			return _state;
		}

	protected:
		virtual void change() {}

		T _state;
	};

	template<class T>
	using StateHandle = Handle<State<T>>;

	template<class T>
	class StateHolder
	{
	public:
		StateHolder(State<T> * state, const T & newState) : state(state), old(state->get())
		{
			state->set(newState);
		}

		StateHolder(State<T> * state, T && newState) : state(state), old(state->get())
		{
			state->set(forward<T>(newState));
		}

		StateHolder(const StateHolder & h) = delete;
		StateHolder(StateHolder && h) : state(h.state), old(h.old)
		{
			h.state = nullptr;
		}

		StateHolder & operator = (const StateHolder & h) = delete;

		StateHolder & operator = (StateHolder && h)
		{
			state = h.state;
			old = h.old;
			h.state = nullptr;

			return *this;
		}

		StateHolder & operator = (const T & newState)
		{
			state->set(newState);
			return *this;
		}

		StateHolder & operator = (T && newState)
		{
			state->set(forward<T>(newState));
			return *this;
		}

		~StateHolder()
		{
			if(state != nullptr)
				state->set(old);
		}

		State<T> * state;
		T old;
	};

	template<class T>
	StateHolder<T> hold(State<T> * state, const T & newState)
	{
		return {state, newState};
	}

	template<class T>
	StateHolder<T> hold(State<T> * state, T && newState)
	{
		return {state, forward<T>(newState)};
	}

	template<class T>
	StateHolder<T> hold(const Handle<State<T>> & state, const T & newState)
	{
		return {state, newState};
	}

	template<class T>
	StateHolder<T> hold(const Handle<State<T>> & state, T && newState)
	{
		return {state, forward<T>(newState)};
	}
}

//---------------------------------------------------------------------------
#endif
