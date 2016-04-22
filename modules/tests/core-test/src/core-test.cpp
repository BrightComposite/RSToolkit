//---------------------------------------------------------------------------

#include <core/String.h>
#include <core/Subject.h>
#include <core/container/Array.h>
#include <core/container/Map.h>
#include <core/meta/Inheritance.h>
#include <application/Application.h>
#include <iostream>
#include <chrono>
#include <conio.h>

#include <thread>
#include <vld.h>

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

	static void testMaps();
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
	static String str("SomeString");
	static auto shstr = share(str);

	static time_point<hrc> start;
	static nanoseconds elapsed;

	static string keys[count];

//---------------------------------------------------------------------------

	static Entrance open([]()
	{
		testMaps();
		testSubjects();
		testHandles();
		testHandlesSafety();

		absorb() _getch();
		return 0;
	});

//---------------------------------------------------------------------------

	message_class(DummyMessage,
		(int, value)
	)

	class DummySubject : public Subject
	{
	public:
	};

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

	static void testHandlesSafety()
	{
		Handle<Object> h = handle<Object>();

		for(register int i = 0; i < 20; ++i)
		{
			Array<Object> oa;

			auto add = [&h, &oa]()
			{
				oa.push_back(h);
			};

			auto rm = [&oa]()
			{
				if(!oa.empty())
					oa.pop_back();
			};

			vector<std::thread> v;

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

	static void testHandles()
	{
		const int times = 64;

		Handle<Object> h = handle<Object>();

		start = hrc::now();

		for(register int i = 0; i < times; ++i)
		{
			Handle<Object> h1 = h;
		}

		elapsed = hrc::now() - start;
		cout << "Handle assignment time (" << times << " times): " << elapsed.count() << " ns" << endl;

		start = hrc::now();

		for(register int i = 0; i < times; ++i)
		{
			Object * h1 = h;
		}

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
		connect(dummy, lambda([](Handle<DummyMessage> & msg, DummySubject & dst)
		{
		
		}));
		elapsed = hrc::now() - start;
		cout << "Message lambda callback connection: " << elapsed.count() << " ns" << endl;

		start = hrc::now();
		connect(dummy, rcvr, &DummyReceiver::receive);
		elapsed = hrc::now() - start;
		cout << "Message object callback connection: " << elapsed.count() << " ns" << endl;

		start = hrc::now();

		for(register int i = 0; i < times; ++i)
			subj.resend(message, dummy);

		elapsed = hrc::now() - start;
		cout << "Message delivery time with 3 connections (" << times << " times): " << elapsed.count() << " ns" << endl;
		/*
		start = hrc::now();

		for(register int i = 0; i < times; ++i)
			dummy.read(message, &subj);

		elapsed = hrc::now() - start;
		cout << "Immediate message delivery time (" << times << " times): " << elapsed.count() << " ns" << endl;
		*/
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
	static void testMapClass(const string & name, const V & val, A ... args)
	{
		cout << "* " << name << endl;
		MapClass map(args...);

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

	template<class F>
	static void makeMapTest(const String & caption, F & keygen)
	{
		cout << endl;
		cout << "---------------------------------------" << endl;
		cout << endl;
		cout << caption << endl;
		cout << endl;
		cout << "---------------------------------------" << endl;

		for(auto & key : keys)
			key = keygen();

		testMapClass<Map<string, const String>>("Rapture::Map", shstr, map_size);
		testMapClass<map<string, Handle<const String>>>("Standard map", shstr);
		testMapClass<unordered_map<string, Handle<const String>>>("Standard unordered map", shstr, map_size);
	}

	static void testMaps()
	{
		cout << "Maps' performance test" << endl;
		cout << "Each operation is repeated " << count << " times" << endl;
		cout << "Keys are objects of the std::string class. Value is the object of the Rapture::Handle<Rapture::String> class" << endl;

		makeMapTest("The single long key (\"LongLongLongLongLongLongLongKey\")", []()
		{
			return "LongLongLongLongLongLongLongKey";
		});

		makeMapTest("Different random long keys with the length "_s << longstrsize << " (e.g. \"" << randomString(longstrsize) << "\")", []()
		{
			return randomString(longstrsize);
		});

		makeMapTest("The single short key (\"abc\")", []()
		{
			return "abc";
		});

		makeMapTest("Different random short keys with the length "_s << shortstrsize << " (e.g. \"" << randomString(shortstrsize) << "\")", []()
		{
			return randomString(shortstrsize);
		});
	}
}