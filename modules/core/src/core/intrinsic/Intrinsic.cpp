//---------------------------------------------------------------------------

#include <core\intrinsic\Intrinsic.h>
#include <limits>

//---------------------------------------------------------------------------

namespace Rapture
{
	template <> const byte	 IntrinZero<byte>		::value = 0;
	template <> const int	 IntrinZero<int>		::value = 0;
	template <> const int64	 IntrinZero<int64>		::value = 0;
	template <> const float  IntrinZero<float>		::value = 0.0f;
	template <> const double IntrinZero<double>		::value = 0.0;

	template <> const byte	 IntrinMax<byte>		::value = 0xFF;
	template <> const int	 IntrinMax<int>			::value = 0xFFFF'FFFF;
	template <> const int64	 IntrinMax<int64>		::value = 0xFFFF'FFFF'FFFF'FFFF;
	template <> const float  IntrinMax<float>		::value = *reinterpret_cast<const float *>(&IntrinMax<int>::value);
	template <> const double IntrinMax<double>		::value = *reinterpret_cast<const double *>(&IntrinMax<int64>::value);

	template <> const byte   IntrinSignmask<byte>	::value = 0x80;
	template <> const int    IntrinSignmask<int>	::value = 0x8000'0000;
	template <> const int64  IntrinSignmask<int64>	::value = 0x8000'0000'0000'0000;
	template <> const float  IntrinSignmask<float>	::value = *reinterpret_cast<const float *>(&IntrinSignmask<int>::value);
	template <> const double IntrinSignmask<double>	::value = *reinterpret_cast<const double *>(&IntrinSignmask<int64>::value);

	template <> const float  IntrinNofrac<float>	::value = 8388608.0f; // float(0x80'0000)
	template <> const double IntrinNofrac<double>	::value = 36028797018963968.0; // double(0x80'0000'0000'0000)

	const Intrinsic<byte, 4>  ::inner Intrinsic<byte, 4>  ::maximum  = IntrinsicMask<byte,   IntrinMax,      mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<byte, 4>  ::inner Intrinsic<byte, 4>  ::signmask = IntrinsicMask<byte,   IntrinSignmask, mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<int, 4>   ::inner Intrinsic<int, 4>   ::maximum  = IntrinsicMask<int,    IntrinMax,      mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<int, 4>   ::inner Intrinsic<int, 4>   ::signmask = IntrinsicMask<int,    IntrinSignmask, mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<float, 4> ::inner Intrinsic<float, 4> ::signmask = IntrinsicMask<float,  IntrinSignmask, mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<float, 4> ::inner Intrinsic<float, 4> ::nofrac   = IntrinsicMask<float,  IntrinNofrac,   mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<double, 4>::inner Intrinsic<double, 4>::signmask = IntrinsicMask<double, IntrinSignmask, mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<double, 4>::inner Intrinsic<double, 4>::nofrac   = IntrinsicMask<double, IntrinNofrac,   mk_mask4(1, 1, 1, 1)>::get();
}
