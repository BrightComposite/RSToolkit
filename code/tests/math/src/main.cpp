//---------------------------------------------------------------------------

#include <application/starter.h>

#include <math/vector.h>
#include <math/matrix.h>
#include <math/quaternion.h>

#include <iostream>

//---------------------------------------------------------------------------

namespace asd
{
	struct Obj : shareable
	{
		Obj() : i(0), b(0), matrix() {}
		Obj(int i, char b, const math::fmat & matrix) : i(i), b(b), matrix(matrix) {}

		int i;
		char b;
		math::fmat matrix;
	};

	struct Direction : shareable
	{
		Direction(int i, const math::fquat & rotation) : i(i), rotation(rotation) {}

		int i;
		math::fquat rotation;
	};

	struct Position : shareable
	{
		Position(int i, char b, const math::fvec & position) : i(i), b(b), position(position) {}

		int i;
		char b;
		math::fvec position;
	};

	static entrance open([]() {
		using namespace std;

		handle<Obj>			obj(_);
		handle<Direction>	dir(0, math::fquat(4.0f, 5.0f, 3.0f));
		handle<Position>	pos(0, 0, math::floatv::right);

		for(int i = 0; i < 20; ++i) {
			cout << pos->position << endl;
			pos->position = dir->rotation->applyTo(pos->position);
		}
	
		obj->matrix = dir->rotation->toMatrix();
		cout << obj->matrix << endl;
		cout << obj->matrix->transposition() << endl;
    });
}

//---------------------------------------------------------------------------
