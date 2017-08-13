//---------------------------------------------------------------------------

#include "matrix-test.h"

#include <direct3d/Direct3D11.h>
#include <ui/Widget.h>
#include <application/application.h>

#include <iostream>
#include <chrono>

//---------------------------------------------------------------------------

namespace asd
{
	using std::bind;

	using std::cout;
	using std::endl;
	using std::thread;

	using namespace std::chrono;
	using namespace DirectX;

	typedef high_resolution_clock hrc;

	time_point<hrc> last;
	long long t = 0;
	const size_t count = 999;

	int matrixTest()
	{
		try
		{
			Matrix2x2<float> mat1 {
				2.0f, 2.0f,
				1.0f, 5.0f
			};

			Matrix2x2<float> mat2 {mat1};

			FloatMatrix fmat {
				2.0f, 2.0f, 2.0f, 2.0f,
				1.0f, 5.0f, 9.0f, 3.0f,
				0.0f, 8.0f, 6.0f, 4.0f,
				8.0f, 1.0f, 3.0f, 5.0f
			};

			auto fmat2 = fmat;

			XMMATRIX MAT {
				2.0f, 2.0f, 2.0f, 2.0f,
				1.0f, 5.0f, 9.0f, 3.0f,
				0.0f, 8.0f, 6.0f, 4.0f,
				8.0f, 1.0f, 3.0f, 5.0f
			};

			auto MAT2 = MAT;

			auto v1 = vec(1.0f, 1.0f, 1.0f, 1.0f);
			auto v2 = v1;

			auto vec1 = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
			auto vec2 = vec1;

			cout << "Math facilities comparison (asd vs DirectX)." << endl;
			cout << "Each operation is made " << count << " times." << endl;
			cout << "This test also shows the advantages of Matrix2x2." << endl;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			cout << "asd::Vector: " << v1 << endl;
			cout << "DirectX::XMVECTOR: " << vec1 << endl;

			cout << "asd::Matrix2x2: " << mat1 << endl;
			cout << "DirectX::XMMATRIX: " << MAT << endl;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				v2 = mat1 * v2;
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd array_list and matrix2x2 multiplication time: " << t << endl;

			cout << "Vector: " << v2 << endl;

			v2 = v1;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				v2 = mat1.transform(v2);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd array_list transformation time (2x2): " << t << endl;

			cout << "Vector: " << v2 << endl;

			v2 = v1;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				vec2 = DirectX::XMVector2Transform(vec2, MAT);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "DirectX array_list transformation time (2x2): " << t << endl;

			cout << "Vector: " << vec2 << endl;

			vec2 = vec1;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				mat2 = mat2 * mat1;
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix2x2 multiplication time: " << t << endl;

			cout << "Matrix: " << mat2 << endl;

			mat2 = mat1;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				mat2 *= mat1;
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix2x2 self-multiplication time: " << t << endl;

			cout << "Matrix: " << mat2 << endl;

			mat2 = mat1;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				MAT2 = MAT2 * MAT;
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "XMMATRIX multiplication time: " << t << endl;

			cout << "Matrix: " << MAT2 << endl;

			MAT2 = MAT;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				MAT2 *= MAT;
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "XMMATRIX self-multiplication time: " << t << endl;

			cout << "Matrix: " << MAT2 << endl;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			MAT2 = MAT;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				volatile auto & m = mat2.transpose();
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix2x2 transpose time: " << t << endl;

			cout << "Matrix: " << mat2 << endl;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				volatile auto & m = fmat2.transpose();
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix4x4 transpose time: " << t << endl;

			cout << "Matrix: " << fmat2 << endl;

			fmat2 = fmat;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				MAT2 = DirectX::XMMatrixTranspose(MAT2);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "DirectX matrix transpose time: " << t << endl;

			cout << "Matrix: " << MAT2 << endl;

			MAT2 = MAT;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			FloatVector sv, cv;
			v2 = {FloatMath::pi, -FloatMath::pi / 3, FloatMath::pi / 4, -FloatMath::pi / 6};

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				FloatVectorMath::sincos(v2, sv, cv);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd array_list sin cos time: " << t << endl;

			cout << "Sin: " << sv << ". Cos: " << cv << endl;

			v2 = v1;

			XMVECTOR svec, cvec;
			vec2 = {FloatMath::pi, -FloatMath::pi / 3, FloatMath::pi / 4, -FloatMath::pi / 6};

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				XMVectorSinCos(&svec, &cvec, vec2);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "DirectX array_list sin cos time: " << t << endl;

			cout << "Sin: " << svec << ". Cos: " << cvec << endl;

			vec2 = vec1;

			float vsc[4] = {FloatMath::pi, -FloatMath::pi / 3, FloatMath::pi / 4, -FloatMath::pi / 6};

			FloatVector sf = {std::sin(vsc[0]), std::sin(vsc[1]), std::sin(vsc[2]), std::sin(vsc[3])};
			FloatVector cf = {std::cos(vsc[0]), std::cos(vsc[1]), std::cos(vsc[2]), std::cos(vsc[3])};

			cout << "Sin: " << sf << ". Cos: " << cf << endl;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				fmat2 = fmat2.rotation(fmat2[0].positiveX, FloatMath::pi);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix4x4 rotation time: " << t << endl;

			cout << "Matrix: " << fmat2 << endl;

			fmat2 = fmat;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				MAT2 = XMMatrixRotationNormal(FloatVector::positiveX, FloatMath::pi);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "DirectX matrix rotation time: " << t << endl;

			cout << "Matrix: " << MAT2 << endl;

			MAT2 = MAT;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				fmat2 = fmat2.rotation({0.0f, 0.0f, 1.0f});
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix4x4 rotation time from Euler: " << t << endl;

			cout << "Matrix: " << fmat2 << endl;

			fmat2 = fmat;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				MAT2 = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 1.0f);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "DirectX matrix rotation time from Euler: " << t << endl;

			cout << "Matrix: " << MAT2 << endl;

			MAT2 = MAT;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				mat2 = mat2.inverse();
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix2x2 inversion time: " << t << endl;

			cout << "Matrix: " << mat2 << endl;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				fmat2.getInverse(fmat2);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix4x4 inversion time: " << t << endl;

			cout << "Matrix: " << fmat2 << endl;

			fmat2 = fmat;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				mat2.invert();
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix2x2 self-inversion time: " << t << endl;

			cout << "Matrix: " << mat2 << endl;

			mat2 = mat1;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				MAT2 = DirectX::XMMatrixInverse(nullptr, MAT2);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "XMMATRIX inversion time: " << t << endl;

			cout << "Matrix: " << MAT2 << endl;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			MAT2 = MAT;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				v1 = (v1 + v2) * v2;
				v2 = (v1 - v2) / v1;
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd vectors' time: " << t << endl;

			cout << "Vector1: " << v1 << endl;
			cout << "Vector2: " << v2 << endl;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				vec1 = XMVectorMultiply(XMVectorAdd(vec1, vec2), vec2);
				vec2 = XMVectorDivide(XMVectorSubtract(vec1, vec2), vec1);
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "DirectX vectors' time: " << t << endl;

			cout << "Vector1: " << vec1 << endl;
			cout << "Vector2: " << vec2 << endl;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			Matrix<float> m4;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				m4.clear();
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix4x4 clear time: " << t << endl;
			cout << m4 << endl;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			Matrix<float> m1r, m2r, mr;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				mr = m1r * m2r;
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix4x4 multiplication time: " << t << endl;
			cout << "Matrix: " << mr << endl;

			asd::operator << (cout, mr);

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			auto m1d = XMMatrixIdentity(), m2d = m1d, md = m1d;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				md *= m2d;
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "XMMATRIX multiplication time: " << t << endl;

			cout << "Matrix: " << md << endl;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			float det[count];

			mr = {
				1.0f, 3.0f, 4.0f, 1.0f,
				3.0f, 1.0f, 2.0f, 4.0f,
				4.0f, 2.0f, 1.0f, 3.0f,
				0.0f, 4.0f, 3.0f, 1.0f
			};

			md = XMMatrixSet(
				1.0f, 3.0f, 4.0f, 1.0f,
				3.0f, 1.0f, 2.0f, 4.0f,
				4.0f, 2.0f, 1.0f, 3.0f,
				0.0f, 4.0f, 3.0f, 1.0f
			);

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				det[i] = mr.determinant().x;
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "asd matrix4x4 determinant time: " << t << endl;
			cout << det[0] << endl;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;

			last = hrc::now();

			for(register int i = 0; i < count; ++i)
			{
				det[i] = XMMatrixDeterminant(md).m128_f32[0];
			}

			t = duration_cast<nanoseconds>(hrc::now() - last).count();
			cout << "DirectX matrix4x4 determinant time: " << t << endl;
			cout << det[0] << endl;

			cout << endl;
			cout << "---------------------------------------" << endl;
			cout << endl;
		}
		catch(const asd::Exception &)
		{
			return 1;
		}

		return 0;
	}
}