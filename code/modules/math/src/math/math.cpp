//---------------------------------------------------------------------------

#include <math/vector.h>
#include <math/matrix.h>
#include <math/quaternion.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace math
	{
		template
		struct constants<float>;
		template
		struct constants<double>;
		
		template<> const float  constants<float>::one = 1.0f;
		template<> const double constants<double>::one = 1.0;
		template<> const float  constants<float>::pi = 3.141592653f;
		template<> const double constants<double>::pi = 3.14159265358979323846;
		template<> const float  constants<float>::pi2 = 6.283185307f;
		template<> const double constants<double>::pi2 = 6.28318530717958647692;
		template<> const float  constants<float>::half_pi = 1.570796326f;
		template<> const double constants<double>::half_pi = 1.57079632679489661923;
		template<> const float  constants<float>::half_pi3 = 4.712388980f;
		template<> const double constants<double>::half_pi3 = 4.71238898038468985769;
		template<> const float  constants<float>::inv_pi = 0.318309886f;
		template<> const double constants<double>::inv_pi = 0.318309886183790671538;
		template<> const float  constants<float>::inv_pi2 = 0.159154943f;
		template<> const double constants<double>::inv_pi2 = 0.159154943091895335769;
		template<> const float  constants<float>::degree180 = 180.0f;
		template<> const double constants<double>::degree180 = 180.0;
		template<> const float  constants<float>::degree360 = 360.0f;
		template<> const double constants<double>::degree360 = 360.0;
		template<> const float  constants<float>::eps = 1.192092896e-07f;
		template<> const double constants<double>::eps = 2.2204460492503131e-016;
		template<> const float  constants<float>::eps2 = constants<float>::eps * constants<float>::eps;
		template<> const double constants<double>::eps2 = constants<double>::eps * constants<double>::eps;
		template<> const float  constants<float>::eps3 = constants<float>::eps * constants<float>::eps * constants<float>::eps;
		template<> const double constants<double>::eps3 = constants<double>::eps * constants<double>::eps * constants<double>::eps;
		template<> const float  constants<float>::infinity = std::numeric_limits<float>::infinity();
		template<> const double constants<double>::infinity = std::numeric_limits<double>::infinity();
		
		template<> const float  constants<float>::degrees2radians = constants<float>::pi / constants<float>::degree180;
		template<> const double constants<double>::degrees2radians = constants<double>::pi / constants<double>::degree180;
		template<> const float  constants<float>::radians2degrees = constants<float>::degree180 / constants<float>::pi;
		template<> const double constants<double>::radians2degrees = constants<double>::degree180 / constants<double>::pi;
		
		template<> const float coefficients<float>::sin[] = {
			-0.16666666666666666666666666666667f,
			+0.00833333333333333333333333333333f,
			-1.984126984126984126984126984127e-4f,
			+2.7557319223985890652557319223986e-6f,
			-2.5052108385441718775052108385442e-8f
		};
		
		template<> const double coefficients<double>::sin[] = {
			-0.16666666666666666666666666666667,
			+0.00833333333333333333333333333333,
			-1.984126984126984126984126984127e-4,
			+2.7557319223985890652557319223986e-6,
			-2.5052108385441718775052108385442e-8
		};
		
		template<> const float coefficients<float>::cos[] = {
			-0.5f,
			+0.04166666666666666666666666666667f,
			-0.00138888888888888888888888888889f,
			+2.4801587301587301587301587301587e-5f,
			-2.7557319223985890652557319223986e-7f
		};
		
		template<> const double coefficients<double>::cos[] = {
			-0.5,
			+0.04166666666666666666666666666667,
			-0.00138888888888888888888888888889,
			+2.4801587301587301587301587301587e-5,
			-2.7557319223985890652557319223986e-7
		};
		
		template
		struct vector<float>;
		template
		struct matrix<float>;
		
		template<> const float_vector  vector_constants<float>::positiveX = {1, 0, 0, 0};
		template<> const float_vector  vector_constants<float>::positiveY = {0, 1, 0, 0};
		template<> const float_vector  vector_constants<float>::positiveZ = {0, 0, 1, 0};
		template<> const float_vector  vector_constants<float>::positiveW = {0, 0, 0, 1};
		template<> const float_vector  vector_constants<float>::negativeX = {-1, 0, 0, 0};
		template<> const float_vector  vector_constants<float>::negativeY = {0, -1, 0, 0};
		template<> const float_vector  vector_constants<float>::negativeZ = {0, 0, -1, 0};
		template<> const float_vector  vector_constants<float>::negativeW = {0, 0, 0, -1};
		
		template<> const float_vector & vector_constants<float>::right = positiveX;
		template<> const float_vector & vector_constants<float>::up = positiveY;
		template<> const float_vector & vector_constants<float>::forward = positiveZ;
		
		template<> const float_vector & vector_constants<float>::left = negativeX;
		template<> const float_vector & vector_constants<float>::down = negativeY;
		template<> const float_vector & vector_constants<float>::back = negativeZ;
		
		template<> const float_vector & vector_constants<float>::identity = zero;
		
		template<> const float_vector  vector_constants<float>::zero = {0, 0, 0, 0};
		template<> const float_vector  vector_constants<float>::one = {1, 1, 1, 1};
		template<> const float_vector  vector_constants<float>::two = {2, 2, 2, 2};
		template<> const float_vector  vector_constants<float>::oneXYZ = {1, 1, 1, 0};
		template<> const float_vector  vector_constants<float>::twoXYZ = {2, 2, 2, 0};
		template<> const float_vector  vector_constants<float>::minusOne = {-1, -1, -1, -1};
		
		template<> const float_vector  vector_constants<float>::half = {.5f, .5f, .5f, .5f};
		
		template
		struct constants<vector<float>>;
		
		template<> const int_vector  vector_constants<int>::positiveX = {1, 0, 0, 0};
		template<> const int_vector  vector_constants<int>::positiveY = {0, 1, 0, 0};
		template<> const int_vector  vector_constants<int>::positiveZ = {0, 0, 1, 0};
		template<> const int_vector  vector_constants<int>::positiveW = {0, 0, 0, 1};
		template<> const int_vector  vector_constants<int>::negativeX = {-1, 0, 0, 0};
		template<> const int_vector  vector_constants<int>::negativeY = {0, -1, 0, 0};
		template<> const int_vector  vector_constants<int>::negativeZ = {0, 0, -1, 0};
		template<> const int_vector  vector_constants<int>::negativeW = {0, 0, 0, -1};
		
		template<> const int_vector & vector_constants<int>::right = positiveX;
		template<> const int_vector & vector_constants<int>::up = positiveY;
		template<> const int_vector & vector_constants<int>::forward = positiveZ;
		
		template<> const int_vector & vector_constants<int>::left = negativeX;
		template<> const int_vector & vector_constants<int>::down = negativeY;
		template<> const int_vector & vector_constants<int>::back = negativeZ;
		
		template<> const int_vector & vector_constants<int>::identity = zero;
		
		template<> const int_vector  vector_constants<int>::zero = {0, 0, 0, 0};
		template<> const int_vector  vector_constants<int>::one = {1, 1, 1, 1};
		template<> const int_vector  vector_constants<int>::two = {2, 2, 2, 2};
		template<> const int_vector  vector_constants<int>::oneXYZ = {1, 1, 1, 0};
		template<> const int_vector  vector_constants<int>::twoXYZ = {2, 2, 2, 0};
		template<> const int_vector  vector_constants<int>::minusOne = {-1, -1, -1, -1};

#ifdef USE_AVX
		template struct vector<double>;
		template struct matrix<double>;
		template struct Matrix2x2<double>;
	
		template<> const DoubleVector vector<double>::positiveX = { 1,  0,  0,  0 };
		template<> const DoubleVector vector<double>::positiveY = { 0,  1,  0,  0 };
		template<> const DoubleVector vector<double>::positiveZ = { 0,  0,  1,  0 };
		template<> const DoubleVector vector<double>::positiveW = { 0,  0,  0,  1 };
		template<> const DoubleVector vector<double>::negativeX = {-1,  0,  0,  0 };
		template<> const DoubleVector vector<double>::negativeY = { 0, -1,  0,  0 };
		template<> const DoubleVector vector<double>::negativeZ = { 0,  0, -1,  0 };
		template<> const DoubleVector vector<double>::negativeW = { 0,  0,  0, -1 };
	
		template<> const DoubleVector & vector<double>::right   = positiveX;
		template<> const DoubleVector & vector<double>::up      = positiveY;
		template<> const DoubleVector & vector<double>::forward = positiveZ;
	
		template<> const DoubleVector & vector<double>::left    = negativeX;
		template<> const DoubleVector & vector<double>::down    = negativeY;
		template<> const DoubleVector & vector<double>::back    = negativeZ;
	
	
		template<> const DoubleVector & vector<double>::identity= zero;
	
		template<> const DoubleVector vector<double>::zero	    = { 0,  0,  0,  0 };
		template<> const DoubleVector vector<double>::one		= { 1,  1,  1,  1 };
		template<> const DoubleVector vector<double>::two		= { 2,  2,  2,  2 };
		template<> const DoubleVector vector<double>::oneXYZ	= { 1,  1,  1,  0 };
		template<> const DoubleVector vector<double>::twoXYZ	= { 2,  2,  2,  0 };
		template<> const DoubleVector vector<double>::minusOne  = {-1, -1, -1, -1 };
	
		template<> const DoubleVector vector<double>::half	    = {.5, .5, .5, .5 };
	
		template struct constants<vector<double>>;
	
#define implement_vector_constants(constant)																													\
	template<> const float_vector  FloatVectorConstants ::constant = { FloatMath::constant,  FloatMath::constant,  FloatMath::constant,  FloatMath::constant  };	\
	template<> const DoubleVector DoubleVectorConstants::constant = { DoubleMath::constant, DoubleMath::constant, DoubleMath::constant, DoubleMath::constant }	\

#else

#define implement_vector_constants(constant)                                                                                                                    \
    template<> const floatv  constants<floatv> ::constant = { constants<float>::constant,  constants<float>::constant,  constants<float>::constant,  constants<float>::constant  };    \

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
		implement_vector_constants(degrees2radians);
		implement_vector_constants(radians2degrees);

#undef implement_vector_constants

#define implement_vector_coefs(coefs, T)                                                \
    template<> const vector<T> coefficients<vector<T>>::coefs[5] = {                                \
        {coefficients<T>::coefs[0], coefficients<T>::coefs[0], coefficients<T>::coefs[0], coefficients<T>::coefs[0]},        \
        {coefficients<T>::coefs[1], coefficients<T>::coefs[1], coefficients<T>::coefs[1], coefficients<T>::coefs[1]},        \
        {coefficients<T>::coefs[2], coefficients<T>::coefs[2], coefficients<T>::coefs[2], coefficients<T>::coefs[2]},        \
        {coefficients<T>::coefs[3], coefficients<T>::coefs[3], coefficients<T>::coefs[3], coefficients<T>::coefs[3]},        \
        {coefficients<T>::coefs[4], coefficients<T>::coefs[4], coefficients<T>::coefs[4], coefficients<T>::coefs[4]},        \
    };
		
		implement_vector_coefs(sin, float);
		implement_vector_coefs(cos, float);

#ifdef USE_AVX
		implement_vector_coefs(sin, double);
		implement_vector_coefs(cos, double);
#endif

#undef implement_vector_coefs
		
		template<> const quaternion<float> quaternion<float>::identity{};

#ifdef USE_AVX
		template<> const quaternion<double> quaternion<double>::identity;
#endif
	}
}
