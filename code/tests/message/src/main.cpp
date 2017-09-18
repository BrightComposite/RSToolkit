//---------------------------------------------------------------------------

#include <application/starter.h>
#include <message/subject.h>

#include <iostream>
#include <chrono>

//---------------------------------------------------------------------------

using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

typedef high_resolution_clock hrc;

namespace asd
{
	message_class(
		DummyMessage1,
		(int, value)
	)

	message_class(
		DummyMessage2,
		(int, value)
	)

	message_class(
		DummyMessage3,
		(int, value)
	)

	class DummySubject : public subject
	{
	public:
		bind_messages(DummySubject, DummyMessage1, DummyMessage2, DummyMessage3);
	};

	channels_api(message_test, DummySubject, DummyMessage1, DummyMessage2, DummyMessage3)

	static entrance open([]() {
		DummySubject subject;

		for(int i = 0; i < 4000; ++i) {
			subscription(subject) {
				onmessage(DummyMessage1) {
					++msg->value;
				};

				onmessage(DummyMessage2) {
					++msg->value;
				};

				onmessage(DummyMessage3) {
					++msg->value;
				};
			}
		}

		auto start = hrc::now();
		auto msg = send<DummyMessage1>(subject, 0);
		auto elapsed = hrc::now() - start;

		std::cout << "Elapsed time: " << elapsed.count() << " ns" << std::endl;
		std::cout << "Value: " << msg->value << std::endl;
	});
}

//---------------------------------------------------------------------------
