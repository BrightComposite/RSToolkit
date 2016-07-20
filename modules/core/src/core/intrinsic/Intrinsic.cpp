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

	template <> const float  IntrinMax<float>		::value = IntrinData<float, 4>::get<0>(IntrinData<float, 4>(_mm_castsi128_ps(_mm_set1_epi32(0xFFFF'FFFF))));
	template <> const byte	 IntrinMax<byte>		::value = 0xFF;
	template <> const int	 IntrinMax<int>			::value = 0xFFFF'FFFF;
	template <> const int64	 IntrinMax<int64>		::value = 0xFFFF'FFFF'FFFF'FFFF;
	
	template <> const byte   IntrinSignmask<byte>	::value = 0x80;
	template <> const int    IntrinSignmask<int>	::value = 0x8000'0000;
	template <> const int64  IntrinSignmask<int64>	::value = 0x8000'0000'0000'0000;
	template <> const float  IntrinSignmask<float>	::value = IntrinData<float, 4>::get<0>(IntrinData<float, 4>(_mm_castsi128_ps(_mm_set1_epi32(0x8000'0000))));

	template <> const byte	 IntrinNofrac<byte>		::value = 0;
	template <> const int	 IntrinNofrac<int>		::value = 0;
	template <> const int64	 IntrinNofrac<int64>	::value = 0;
	template <> const float  IntrinNofrac<float>	::value = 8388608.0f; // float(0x80'0000)

	//const Intrinsic<byte, 4>  ::inner Intrinsic<byte, 4>  ::maximum  = IntrinsicMask<byte,   IntrinMax,      mk_mask4(1, 1, 1, 1)>::get();
	//const Intrinsic<byte, 4>  ::inner Intrinsic<byte, 4>  ::signmask = IntrinsicMask<byte,   IntrinSignmask, mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<int, 4>   ::inner Intrinsic<int, 4>   ::maximum  = IntrinsicMask<int,    IntrinMax,      mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<int, 4>   ::inner Intrinsic<int, 4>   ::signmask = IntrinsicMask<int,    IntrinSignmask, mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<float, 4> ::inner Intrinsic<float, 4> ::signmask = IntrinsicMask<float,  IntrinSignmask, mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<float, 4> ::inner Intrinsic<float, 4> ::nofrac   = IntrinsicMask<float,  IntrinNofrac,   mk_mask4(1, 1, 1, 1)>::get();
	
#ifdef USE_AVX
	template <> const double IntrinZero<double>		::value = 0.0;
	template <> const double IntrinMax<double>		::value = IntrinData<double, 4>::get<0>(IntrinData<double, 4>(_mm256_castsi256_pd(_mm256_set1_epi64x(0xFFFF'FFFF'FFFF'FFFF))));
	template <> const double IntrinSignmask<double>	::value = IntrinData<double, 4>::get<0>(IntrinData<double, 4>(_mm256_castsi256_pd(_mm256_set1_epi64x(0x8000'0000'0000'0000))));
	template <> const double IntrinNofrac<double>	::value = 36028797018963968.0; // double(0x80'0000'0000'0000)
	const Intrinsic<double, 4>::inner Intrinsic<double, 4>::signmask = IntrinsicMask<double, IntrinSignmask, mk_mask4(1, 1, 1, 1)>::get();
	const Intrinsic<double, 4>::inner Intrinsic<double, 4>::nofrac   = IntrinsicMask<double, IntrinNofrac,   mk_mask4(1, 1, 1, 1)>::get();
#endif
}
