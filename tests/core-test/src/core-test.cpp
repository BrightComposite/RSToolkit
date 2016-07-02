//---------------------------------------------------------------------------

#include <meta/Inheritance.h>

#include <core/String.h>
#include <core/container/ArrayList.h>
#include <core/container/Map.h>

#include <message/Subject.h>

#include <application/Application.h>
#include <iostream>
#include <chrono>
#include <conio.h>

#include <thread>
#include <vld.h>

#include <application/Starter.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::cout;
	using std::endl;

	using std::chrono::high_resolution_clock;
	using std::chrono::time_point;
	using std::chrono::duration;
	using std::chrono::duration_cast;
	using std::chrono::nanoseconds;

	typedef high_resolution_clock hrc;

//---------------------------------------------------------------------------

	static void testSubjects();
	static void testHandles();
	static void testHandlesSafety();

//---------------------------------------------------------------------------

	static const int count = 16;
	static const int longstrsize = 16;
	static const int shortstrsize = 3;
	static const int map_size = count;

//---------------------------------------------------------------------------

	static Subject a("SomeSubject");
	static String s("SomeString");
	static auto shstr = share(s);

	static time_point<hrc> start;
	static nanoseconds elapsed;

	static string keys[count];

//---------------------------------------------------------------------------

	static Entrance first([]()
	{
		testSubjects();
		testHandles();
		testHandlesSafety();

		absorb() _getch();
		return 0;
	});

//---------------------------------------------------------------------------

	message_class(DummyMessage,
		(int, value)
	);

	class DummySubject : public Subject
	{
	public:
		bind_message(DummySubject, DummyMessage);
	};

	channels_api(core_test, DummySubject, DummyMessage)

	class DummyReceiver : public MessageReceiver
	{
	public:
		void receive(Handle<DummyMessage> & msg, DummySubject & dest) const
		{

		}
	};

	static void receiver(Handle<DummyMessage> & msg, DummySubject & dst)
	{

	}

	static void testHandles()
	{
		static const int times = 64;

		auto h = handle<Object>();

		start = hrc::now();

		for(register int i = 0; i < times; ++i)
			Handle<Object> hh = h;

		elapsed = hrc::now() - start;
		cout << "Handle assignment time (" << times << " times): " << elapsed.count() << " ns" << endl;

		start = hrc::now();

		for(register int i = 0; i < times; ++i)
			Object * h1 = h;

		elapsed = hrc::now() - start;
		cout << "Pointer assignment time (" << times << " times): " << elapsed.count() << " ns" << endl;
	}

	static void testSubjects()
	{
		Subject subj;
		DummySubject dummy;
		auto rcvr = handle<DummyReceiver>();

		auto message = send<DummyMessage>(dummy, 0);

		const int times = 16;

		start = hrc::now();

		for(register int i = 0; i < times; ++i)
			subj.resend(message, dummy);

		elapsed = hrc::now() - start;
		cout << "Message delivery time (" << times << " times): " << elapsed.count() << " ns" << endl;

		start = hrc::now();
		connect(dummy, receiver);
		elapsed = hrc::now() - start;
		cout << "Message plain callback connection: " << elapsed.count() << " ns" << endl;

		start = hrc::now();

		subscribe_on(DummySubject, DummyMessage)
		{

		};

		elapsed = hrc::now() - start;
		cout << "Global message lambda callback connection: " << elapsed.count() << " ns" << endl;

		start = hrc::now();

		subscribe_on(DummySubject, DummyMessage, dummy)
		{
		
		};

		elapsed = hrc::now() - start;
		cout << "Message lambda callback connection: " << elapsed.count() << " ns" << endl;

		start = hrc::now();

		connect<DummySubject, DummyMessage>(dummy, [](Handle<DummyMessage> & msg, DummySubject & dst)
		{

		});

		elapsed = hrc::now() - start;
		cout << "Message lambda callback connection 2: " << elapsed.count() << " ns" << endl;

		start = hrc::now();
		connect(dummy, rcvr, &DummyReceiver::receive);
		elapsed = hrc::now() - start;
		cout << "Message object callback connection: " << elapsed.count() << " ns" << endl;

		start = hrc::now();
		connect<DummySubject, DummyMessage>(dummy, std::bind(&DummyReceiver::receive, rcvr, std::placeholders::_1, std::placeholders::_2));
		elapsed = hrc::now() - start;
		cout << "Message object callback connection 2: " << elapsed.count() << " ns" << endl;

		start = hrc::now();

		for(register int i = 0; i < times; ++i)
			subj.resend(message, dummy);

		elapsed = hrc::now() - start;
		cout << "Message delivery time with 6 connections (" << times << " times): " << elapsed.count() << " ns" << endl;
		/*
		start = hrc::now();

		for(register int i = 0; i < times; ++i)
			dummy.read(message, &subj);

		elapsed = hrc::now() - start;
		cout << "Immediate message delivery time (" << times << " times): " << elapsed.count() << " ns" << endl;
		*/
	}

	static void testHandlesSafety()
	{
		Handle<Object> h = handle<Object>();

		for(register int i = 0; i < 20; ++i)
		{
			ArrayList<Object> oa;

			auto add = [&h, &oa]()
			{
				oa.push_back(h);
			};

			auto rm = [&oa]()
			{
				if(!oa.empty())
					oa.pop_back();
			};

			array_list<std::thread> v;

			for(register int i = 0; i < 5; ++i)
			{
				v.emplace_back(add);
				v.emplace_back(rm);
			}

			for(auto & th : v)
				th.join();

			cout << "Refs: " << h.refs() << endl;
		}
	}

//---------------------------------------------------------------------------

	static String randomString(int size)
	{
		String buf;
		srand(hrc::now().time_since_epoch().count() % INT_MAX);

		buf.reserve(size);

		for(int i = 0; i < size; ++i)
			buf += static_cast<char>((rand() % ('z' - '0')) + '0');

		return buf;
	}

	template<class MapClass, class V, class ... A>
	static void testMapClass(const string & name, const V & val, A &&... args)
	{
		cout << "* " << name << endl;
		MapClass map(forward<A>(args)...);

		start = hrc::now();

		for(register int i = 0; i < count; ++i)
			map[keys[i]] = val;

		elapsed = hrc::now() - start;
		cout << "Inserting time: " << elapsed.count() << "ns" << endl;

		start = hrc::now();

		for(register int i = 0; i < count; ++i)
			map[keys[i]];

		elapsed = hrc::now() - start;
		cout << "Accessing time: " << elapsed.count() << "ns" << endl;
		cout << endl;
	}
}
