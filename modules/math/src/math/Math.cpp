//---------------------------------------------------------------------------

#include <math\Vector.h>
#include <math\Matrix.h>
#include <math\Quaternion.h>

//---------------------------------------------------------------------------

namespace asd
{
	template struct MathConstants<float>;
	template struct MathConstants<double>;

	template<> const float  MathConstants<float> ::one			= 1.0f;
	template<> const double MathConstants<double>::one			= 1.0;
	template<> const float  MathConstants<float> ::pi			= 3.141592653f;
	template<> const double MathConstants<double>::pi			= 3.14159265358979323846;
	template<> const float  MathConstants<float> ::pi2			= 6.283185307f;
	template<> const double MathConstants<double>::pi2			= 6.28318530717958647692;
	template<> const float  MathConstants<float> ::half_pi		= 1.570796326f;
	template<> const double MathConstants<double>::half_pi		= 1.57079632679489661923;
	template<> const float  MathConstants<float> ::half_pi3		= 4.712388980f;
	template<> const double MathConstants<double>::half_pi3		= 4.71238898038468985769;
	template<> const float  MathConstants<float> ::inv_pi		= 0.318309886f;
	template<> const double MathConstants<double>::inv_pi		= 0.318309886183790671538;
	template<> const float  MathConstants<float> ::inv_pi2		= 0.159154943f;
	template<> const double MathConstants<double>::inv_pi2		= 0.159154943091895335769;
	template<> const float  MathConstants<float> ::degree180	= 180.0f;
	template<> const double MathConstants<double>::degree180	= 180.0;
	template<> const float  MathConstants<float> ::degree360	= 360.0f;
	template<> const double MathConstants<double>::degree360	= 360.0;
	template<> const float  MathConstants<float> ::eps			= 1.192092896e-07f;
	template<> const double MathConstants<double>::eps			= 2.2204460492503131e-016;
	template<> const float  MathConstants<float> ::eps2			= Math<float> ::eps * Math<float> ::eps;
	template<> const double MathConstants<double>::eps2			= Math<double>::eps * Math<double>::eps;
	template<> const float  MathConstants<float> ::eps3			= Math<float> ::eps * Math<float> ::eps * Math<float> ::eps;
	template<> const double MathConstants<double>::eps3			= Math<double>::eps * Math<double>::eps * Math<double>::eps;
	template<> const float  MathConstants<float> ::infinity		= std::numeric_limits<float>::infinity();
	template<> const double MathConstants<double>::infinity		= std::numeric_limits<double>::infinity();

	template<> const float  MathConstants<float> ::degreetorad	= Math<float> ::pi / Math<float> ::degree180;
	template<> const double MathConstants<double>::degreetorad	= Math<double>::pi / Math<double>::degree180;
	template<> const float  MathConstants<float> ::radtodegree	= Math<float> ::degree180 / Math<float> ::pi;
	template<> const double MathConstants<double>::radtodegree	= Math<double>::degree180 / Math<double>::pi;

	template<> const float MathCoefficients<float>::sin[] = {
		-0.16666666666666666666666666666667f,
		+0.00833333333333333333333333333333f,
		-1.984126984126984126984126984127e-4f,
		+2.7557319223985890652557319223986e-6f,
		-2.5052108385441718775052108385442e-8f
	};

	template<> const double MathCoefficients<double>::sin[] = {
		-0.16666666666666666666666666666667,
		+0.00833333333333333333333333333333,
		-1.984126984126984126984126984127e-4,
		+2.7557319223985890652557319223986e-6,
		-2.5052108385441718775052108385442e-8
	};

	template<> const float MathCoefficients<float>::cos[] = {
		-0.5f,
		+0.04166666666666666666666666666667f,
		-0.00138888888888888888888888888889f,
		+2.4801587301587301587301587301587e-5f,
		-2.7557319223985890652557319223986e-7f
	};

	template<> const double MathCoefficients<double>::cos[] = {
		-0.5,
		+0.04166666666666666666666666666667,
		-0.00138888888888888888888888888889,
		+2.4801587301587301587301587301587e-5,
		-2.7557319223985890652557319223986e-7
	};

	template struct Vector<float>;
	template struct Matrix<float>;

	template<> const FloatVector  Vector<float> ::positiveX = { 1,  0,  0,  0 };
	template<> const FloatVector  Vector<float> ::positiveY = { 0,  1,  0,  0 };
	template<> const FloatVector  Vector<float> ::positiveZ = { 0,  0,  1,  0 };
	template<> const FloatVector  Vector<float> ::positiveW = { 0,  0,  0,  1 };
	template<> const FloatVector  Vector<float> ::negativeX = {-1,  0,  0,  0 };
	template<> const FloatVector  Vector<float> ::negativeY = { 0, -1,  0,  0 };
	template<> const FloatVector  Vector<float> ::negativeZ = { 0,  0, -1,  0 };
	template<> const FloatVector  Vector<float> ::negativeW = { 0,  0,  0, -1 };

	template<> const FloatVector  & Vector<float>::right    = positiveX;
	template<> const FloatVector  & Vector<float>::up       = positiveY;
	template<> const FloatVector  & Vector<float>::forward  = positiveZ;

	template<> const FloatVector  & Vector<float>::left     = negativeX;
	template<> const FloatVector  & Vector<float>::down     = negativeY;
	template<> const FloatVector  & Vector<float>::back     = negativeZ;

	template<> const FloatVector  & Vector<float>::identity = zero;

	template<> const FloatVector  Vector<float> ::zero		= { 0,  0,  0,  0 };
	template<> const FloatVector  Vector<float> ::one		= { 1,  1,  1,  1 };
	template<> const FloatVector  Vector<float> ::two		= { 2,  2,  2,  2 };
	template<> const FloatVector  Vector<float> ::oneXYZ	= { 1,  1,  1,  0 };
	template<> const FloatVector  Vector<float> ::twoXYZ	= { 2,  2,  2,  0 };
	template<> const FloatVector  Vector<float> ::minusOne  = {-1, -1, -1, -1 };

	template<> const FloatVector  Vector<float> ::half	    = {.5f,.5f,.5f,.5f};

	template struct MathConstants<Vector<float>>;

	template<> const IntVector  Vector<int> ::positiveX = { 1,  0,  0,  0 };
	template<> const IntVector  Vector<int> ::positiveY = { 0,  1,  0,  0 };
	template<> const IntVector  Vector<int> ::positiveZ = { 0,  0,  1,  0 };
	template<> const IntVector  Vector<int> ::positiveW = { 0,  0,  0,  1 };
	template<> const IntVector  Vector<int> ::negativeX = {-1,  0,  0,  0 };
	template<> const IntVector  Vector<int> ::negativeY = { 0, -1,  0,  0 };
	template<> const IntVector  Vector<int> ::negativeZ = { 0,  0, -1,  0 };
	template<> const IntVector  Vector<int> ::negativeW = { 0,  0,  0, -1 };

	template<> const IntVector  & Vector<int>::right    = positiveX;
	template<> const IntVector  & Vector<int>::up       = positiveY;
	template<> const IntVector  & Vector<int>::forward  = positiveZ;

	template<> const IntVector  & Vector<int>::left     = negativeX;
	template<> const IntVector  & Vector<int>::down     = negativeY;
	template<> const IntVector  & Vector<int>::back     = negativeZ;

	template<> const IntVector  & Vector<int>::identity = zero;

	template<> const IntVector  Vector<int> ::zero		= { 0,  0,  0,  0 };
	template<> const IntVector  Vector<int> ::one		= { 1,  1,  1,  1 };
	template<> const IntVector  Vector<int> ::two		= { 2,  2,  2,  2 };
	template<> const IntVector  Vector<int> ::oneXYZ	= { 1,  1,  1,  0 };
	template<> const IntVector  Vector<int> ::twoXYZ	= { 2,  2,  2,  0 };
	template<> const IntVector  Vector<int> ::minusOne  = {-1, -1, -1, -1 };

#ifdef USE_AVX

	template struct Vector<double>;
	template struct Matrix<double>;
	template struct Matrix2x2<double>;

	template<> const DoubleVector Vector<double>::positiveX = { 1,  0,  0,  0 };
	template<> const DoubleVector Vector<double>::positiveY = { 0,  1,  0,  0 };
	template<> const DoubleVector Vector<double>::positiveZ = { 0,  0,  1,  0 };
	template<> const DoubleVector Vector<double>::positiveW = { 0,  0,  0,  1 };
	template<> const DoubleVector Vector<double>::negativeX = {-1,  0,  0,  0 };
	template<> const DoubleVector Vector<double>::negativeY = { 0, -1,  0,  0 };
	template<> const DoubleVector Vector<double>::negativeZ = { 0,  0, -1,  0 };
	template<> const DoubleVector Vector<double>::negativeW = { 0,  0,  0, -1 };

	template<> const DoubleVector & Vector<double>::right   = positiveX;
	template<> const DoubleVector & Vector<double>::up      = positiveY;
	template<> const DoubleVector & Vector<double>::forward = positiveZ;

	template<> const DoubleVector & Vector<double>::left    = negativeX;
	template<> const DoubleVector & Vector<double>::down    = negativeY;
	template<> const DoubleVector & Vector<double>::back    = negativeZ;


	template<> const DoubleVector & Vector<double>::identity= zero;

	template<> const DoubleVector Vector<double>::zero	    = { 0,  0,  0,  0 };
	template<> const DoubleVector Vector<double>::one		= { 1,  1,  1,  1 };
	template<> const DoubleVector Vector<double>::two		= { 2,  2,  2,  2 };
	template<> const DoubleVector Vector<double>::oneXYZ	= { 1,  1,  1,  0 };
	template<> const DoubleVector Vector<double>::twoXYZ	= { 2,  2,  2,  0 };
	template<> const DoubleVector Vector<double>::minusOne  = {-1, -1, -1, -1 };

	template<> const DoubleVector Vector<double>::half	    = {.5, .5, .5, .5 };

	template struct MathConstants<Vector<double>>;

#define implement_vector_constants(constant)																													\
	template<> const FloatVector  FloatVectorConstants ::constant = { FloatMath::constant,  FloatMath::constant,  FloatMath::constant,  FloatMath::constant  };	\
	template<> const DoubleVector DoubleVectorConstants::constant = { DoubleMath::constant, DoubleMath::constant, DoubleMath::constant, DoubleMath::constant }	\

#else

#define implement_vector_constants(constant)																													\
	template<> const FloatVector  FloatVectorConstants ::constant = { FloatMath::constant,  FloatMath::constant,  FloatMath::constant,  FloatMath::constant  };	\
	
#endif
	implement_vector_constants(one);
	implement_vector_constants(pi);
	implement_vector_constants(pi2);
	implement_vector_constants(half_pi);
	implement_vector_constants(half_pi3);
	implement_vector_constants(inv_pi);
	implement_vector_constants(inv_pi2);
	implement_vector_constants(degree180);
	implement_vector_constants(degree360);
	implement_vector_constants(eps);
	implement_vector_constants(eps2);
	implement_vector_constants(eps3);
	implement_vector_constants(infinity);
	implement_vector_constants(degreetorad);
	implement_vector_constants(radtodegree);

#undef implement_vector_constants

#define implement_vector_coefs(coefs, T)												\
	template<> const T##Vector T##VectorCfs::coefs[5] = {								\
		{T##Cfs::coefs[0], T##Cfs::coefs[0], T##Cfs::coefs[0], T##Cfs::coefs[0]},		\
		{T##Cfs::coefs[1], T##Cfs::coefs[1], T##Cfs::coefs[1], T##Cfs::coefs[1]},		\
		{T##Cfs::coefs[2], T##Cfs::coefs[2], T##Cfs::coefs[2], T##Cfs::coefs[2]},		\
		{T##Cfs::coefs[3], T##Cfs::coefs[3], T##Cfs::coefs[3], T##Cfs::coefs[3]},		\
		{T##Cfs::coefs[4], T##Cfs::coefs[4], T##Cfs::coefs[4], T##Cfs::coefs[4]},		\
	};																		

	implement_vector_coefs(sin, Float);
	implement_vector_coefs(cos, Float);

#ifdef USE_AVX
	implement_vector_coefs(sin, Double);
	implement_vector_coefs(cos, Double);
#endif

#undef implement_vector_coefs

	template<> const Quaternion<float>  Quaternion<float> ::identity;

#ifdef USE_AVX
	template<> const Quaternion<double> Quaternion<double>::identity;
#endif
}
