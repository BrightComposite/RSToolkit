//---------------------------------------------------------------------------

#include <application/Starter.h>

#include <math/Vector.h>
#include <math/Matrix.h>
#include <math/Quaternion.h>
#include <math/Transform.h>

#include <iostream>
#include <conio.h>

//---------------------------------------------------------------------------

namespace asd
{
	struct Obj : Shared
	{
		Obj() : i(0), b(0), matrix() {}
		Obj(int i, char b, const fmat & matrix) : i(i), b(b), matrix(matrix) {}

		int i;
		char b;
		fmat matrix;
	};

	struct Direction : Shared
	{
		Direction(int i, const fquat & rotation) : i(i), rotation(rotation) {}

		int i;
		fquat rotation;
	};

	struct Position : Shared
	{
		Position(int i, char b, const fvec & position) : i(i), b(b), position(position) {}

		int i;
		char b;
		fvec position;
	};

	static Entrance open([]()
    {
		using namespace std;

		Handle<Obj>			obj(default_init);
		Handle<Direction>	dir(0, fquat(4.0f, 5.0f, 3.0f));
		Handle<Position>	pos(0, 0, floatv::right);

		for(int i = 0; i < 20; ++i)
		{
			cout << pos->position << endl;
			pos->position = dir->rotation->applyTo(pos->position);
		}

		obj->matrix = obj->matrix->transposition();

		_getch();
        return 0;
    });
}

//---------------------------------------------------------------------------
