//---------------------------------------------------------------------------

#ifndef SUBJECT_H
#define SUBJECT_H

//---------------------------------------------------------------------------

#include <core/Morph.h>
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

	link_class(Subject, MetaClass<Object, Named>);

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
		
		template<class Dst, class Msg>
		void send(Handle<Msg> & message, Dst & dest) const
		{
			if(!dest.isEnabled())
				return;

			Channel<Dst, Msg>::transmit(message, dest, this);
		}

		template<class Dst, class Msg>
		void send(Handle<Msg> & message, Dst * dest) const
		{
			if(dest == nullptr)
				throw Exception("Destination of message must not be null!");

			send(message, *dest);
		}

	protected:
		bool _enabled = true;
	};

	template<class Dst, class Msg,
		require(
			based_on(Dst, Subject)
			)>
	void send(Handle<Msg> & message, Dst & dest)
	{
		dest.send(message, dest);
	}

	template<class Dst, class Msg,
		require(
			based_on(Dst, Subject)
			)>
	void send(Handle<Msg> & message, Dst * dest)
	{
		if(dest == nullptr)
			throw Exception("Destination of message must not be null!");

		dest->send(message, *dest);
	}
}

//---------------------------------------------------------------------------
#endif
