//---------------------------------------------------------------------------

#include <application/starter.h>
#include <window/window.h>
#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
	template <typename E, typename TR, typename T>
	std::basic_ostream<E, TR> & operator << (std::basic_ostream<E, TR> & s, const math::point<T> & p) {
		return s << p.x << ";" << p.y;
	}

	static entrance open([]() {
		window w("gfx:test", { 50, 50, 400, 400 });

		w.events.position.subscribe([](const auto & pos) {
			std::cout << "Position changed: " << pos << std::endl;
		});

		w.events.size.subscribe([](const auto & size) {
			std::cout << "Size changed: " << size << std::endl;
		});

		w.show();
		return w.loop([] {});
	});
}

//---------------------------------------------------------------------------
