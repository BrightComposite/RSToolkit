//---------------------------------------------------------------------------

#ifndef SUBJECT_H
#define SUBJECT_H

//---------------------------------------------------------------------------

#include <core/Exception.h>

#include <core/addition/Named.h>
#include <core/message/Channel.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	/**
	 *	@brief
	 *	Receiver must be initialized after this message
	 */
	message_class(InitializeMessage,
		(int, type)
		);

	link_class(Subject, Class<Object, Named>);

	class Subject : public Object, public Named, public AutoIdentifier
	{
	public:
		Subject() : Object(), Named()
		{
			setclass(Subject);
		}

		Subject(const String & name) : Object(), Named(name)
		{
			setclass(Subject);
		}

		Subject(String && name) : Object(), Named(name)
		{
			setclass(Subject);
		}

		Subject(const Subject & subject) : Object(), Named(subject)
		{
			setclass(Subject);
		}

		Subject(Subject && subject) : Object(), Named(forward<Subject>(subject))
		{
			setclass(Subject);
		}

		virtual ~Subject() {}

		void enable()
		{
			_enabled = true;
		}

		void disable()
		{
			_enabled = false;
		}

		bool isEnabled() const
		{
			return _enabled;
		}

		bool isDisabled() const
		{
			return !_enabled;
		}

		template<class Msg, class Dst, class ... F, useif <can_construct<Msg, Subject *, F...>::value> endif>
		Handle<Msg> send(Dst & dest, F &&... fields) const
		{
			auto msg = handle<Msg>(this, forward<F>(fields)...);
			channel_t<Dst, Msg>::transmit(msg, dest);

			return msg;
		}

		template<class Msg, class Dst, class ... F, useif <can_construct<Msg, Subject *, F...>::value> endif>
		Handle<Msg> send(Dst * dest, F &&... fields) const
		{
			if(dest == nullptr)
				throw Exception("Destination should be not null!");

			auto msg = handle<Msg>(this, forward<F>(fields)...);
			channel_t<Dst, Msg>::transmit(msg, *dest);

			return msg;
		}

		template<class Dst, class Msg>
		void resend(Handle<Msg> & message, Dst & dest) const
		{
			auto * src = message->source;
			message->source = this;
			channel_t<Dst, Msg>::transmit(message, dest);
			message->source = src;
		}

		template<class Dst, class Msg>
		void resend(Handle<Msg> & message, Dst * dest) const
		{
			if(dest == nullptr)
				throw Exception("Destination should be not null!");

			auto * src = message->source;
			message->source = this;
			channel_t<Dst, Msg>::transmit(message, *dest);
			message->source = src;
		}

	protected:
		bool _enabled = true;

	private:
		declare_receivers();
	};

	template<class Msg, class Dst, class ... F, useif <can_construct<Msg, Subject *, F...>::value> endif>
	Handle<Msg> send(Dst & dest, F &&... fields)
	{
		auto msg = handle<Msg>(nullptr, forward<F>(fields)...);
		channel_t<Dst, Msg>::transmit(msg, dest);

		return msg;
	}

	template<class Dst, class Msg>
	void resend(Handle<Msg> & message, Dst & dest)
	{
		auto * src = message->source;
		message->source = nullptr;
		channel_t<Dst, Msg>::transmit(message, dest);
		message->source = src;
	}
}

//---------------------------------------------------------------------------
#endif
