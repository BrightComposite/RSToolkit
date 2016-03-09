//---------------------------------------------------------------------------

#include <core\intrinsic\Intrinsic.h>
#include <limits>

//---------------------------------------------------------------------------

namespace Rapture
{
	const int	 IntrinZero<int>		::value = 0;
	const int64	 IntrinZero<int64>		::value = 0;
	const float  IntrinZero<float>		::value = 0.0f;
	const double IntrinZero<double>		::value = 0.0;

	const int	 IntrinMax<int>			::value = 0xFFFF'FFFF;
	const int64	 IntrinMax<int64>		::value = 0xFFFF'FFFF'FFFF'FFFF;
	const float  IntrinMax<float>		::value = *reinterpret_cast<const float *>(&IntrinMax<int>::value);
	const double IntrinMax<double>		::value = *reinterpret_cast<const double *>(&IntrinMax<int64>::value);

	const int    IntrinSignmask<int>	::value = 0x8000'0000;
	const int64  IntrinSignmask<int64>	::value = 0x8000'0000'0000'0000;
	const float  IntrinSignmask<float>	::value = *reinterpret_cast<const float *>(&IntrinSignmask<int>::value);
	const double IntrinSignmask<double>	::value = *reinterpret_cast<const double *>(&IntrinSignmask<int64>::value);

	const float  IntrinNofrac<float>	::value = 8388608.0f; // float(0x80'0000)
	const double IntrinNofrac<double>	::value = 36028797018963968.0; // double(0x80'0000'0000'0000)

	const Intrinsic<int, 4>   ::inner Intrinsic<int, 4>   ::maximum  = IntrinsicMask<int,    IntrinMax,      0xF>::get();
	const Intrinsic<int, 4>   ::inner Intrinsic<int, 4>   ::signmask = IntrinsicMask<int,    IntrinSignmask, 0xF>::get();
	const Intrinsic<float, 4> ::inner Intrinsic<float, 4> ::signmask = IntrinsicMask<float,  IntrinSignmask, 0xF>::get();
	const Intrinsic<float, 4> ::inner Intrinsic<float, 4> ::nofrac   = IntrinsicMask<float,  IntrinNofrac,   0xF>::get();
	const Intrinsic<double, 4>::inner Intrinsic<double, 4>::signmask = IntrinsicMask<double, IntrinSignmask, 0xF>::get();
	const Intrinsic<double, 4>::inner Intrinsic<double, 4>::nofrac   = IntrinsicMask<double, IntrinNofrac,   0xF>::get();
}