//---------------------------------------------------------------------------

#include <application/starter.h>

#include <math/vector.h>
#include <math/matrix.h>
#include <math/quaternion.h>

#include <iostream>

#include <benchmark>

//---------------------------------------------------------------------------

namespace asd
{
	struct Obj : shareable<Obj>
	{
		Obj() : i(0), b(0), matrix() {}
		Obj(int i, char b, const math::fmat & matrix) : i(i), b(b), matrix(matrix) {}

		int i;
		char b;
		math::fmat matrix;
	};

	struct Direction : shareable<Direction>
	{
		Direction(int i, const math::fquat & rotation) : i(i), rotation(rotation) {}

		int i;
		math::fquat rotation;
	};

	struct Position : shareable<Position>
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
		handle<Position>	pos(0, 0, math::vector_constants<float>::right);

		for(int i = 0; i < 20; ++i) {
			cout << pos->position << endl;
			pos->position = dir->rotation.applyTo(pos->position);
		}
	
		obj->matrix = dir->rotation.to_matrix();
		cout << obj->matrix << endl;
		cout << obj->matrix.transposition() << endl;
		
		{
			auto vec1 = math::vec(0, 1, 2, 3), vec2 = math::vec(0, 1, 2, 3);
			const auto vec3 = math::vec(2, 1, 5, 2);
			
			benchmark("simple multiplication") << [&]() {
				repeat(i, 1000) {
					vec2.x += vec1.x * vec3.x;
					vec2.z += vec1.z * vec3.z;
					vec2.y += vec1.y * vec3.y;
				}
			};
			
			cout << vec2.x << " " << vec2.y << " " << vec2.z << " " << vec2.w << endl;
		}
		
		{
			auto vec1 = math::vec(0, 1, 2, 3), vec2 = math::vec(0, 1, 2, 3);
			const auto vec3 = math::vec(2, 1, 5, 2);
			
			benchmark("vector multiplication") << [&]() {
				repeat(i, 1000) {
					vec2 += vec1 * vec3;
				}
			};
			
			cout << vec2.x << " " << vec2.y << " " << vec2.z << " " << vec2.w << endl;
		}
		
		{
			using Intrin = Intrinsic<float, 4>;
			using Data = IntrinData<float, 4>;
			
			auto vec1 = Intrin::load(0, 1, 2, 3), vec2 = Intrin::load(0, 1, 2, 3);
			const auto vec3 = Intrin::load(2, 1, 5, 2);
			
			benchmark("intrinsic multiplication") << [&]() {
				repeat(i, 1000) {
					vec2 = Intrin::add(vec2, Intrin::mul(vec1, vec3));
				}
			};
			
			cout << Data::get<0>(vec2) << " " << Data::get<1>(vec2) << " " << Data::get<2>(vec2) << " " << Data::get<3>(vec2) << endl;
		}
		
		return 0;
    });
}

//---------------------------------------------------------------------------
