//---------------------------------------------------------------------------

#pragma once

#ifndef BITMASK_H
#define BITMASK_H

//---------------------------------------------------------------------------

#include "Types.h"

//---------------------------------------------------------------------------

namespace asd
{
	template<int offset>
	struct bitmask
	{
		static const size_t value = 1 << offset;

		template<typename I>
		static constexpr bool state(I s)
		{
			return check_flag(value, s);
		}

		template<typename I>
		static constexpr void setState(I & s)
		{
			set_flag(value, s);
		}

		template<typename I>
		static constexpr void clearState(I & s)
		{
			clear_flag(value, s);
		}
	};

	struct bits
	{
		template<typename I>
		static constexpr bool state(int offset, I s)
		{
			return check_flag(I(1 << offset), s);
		}

		template<typename I>
		static constexpr void setState(int offset, I & s)
		{
			set_flag(I(1 << offset), s);
		}

		template<typename I>
		static constexpr void selectState(int offset, I & s, bool state)
		{
			select_flag(I(1 << offset), s, state);
		}

		template<typename I>
		static constexpr void clearState(int offset, I & s)
		{
			clear_flag(I(1 << offset), s);
		}
	};

	using lo_bit_mask = bitmask<0>;
	using hi_bit_mask = bitmask<15>;

	template<size_t Mask, int offset>
	struct has_bit
	{
		static const bool value = static_cast<bool>((Mask >> offset) & 1);
	};

	namespace internals
	{
		template<size_t Mask, class S>
		struct unfold_mask {};

		template<size_t Mask, int ... Values>
		struct unfold_mask<Mask, std::integer_sequence<int, Values...>>
		{
			typedef integer_sequence<bool, has_bit<Mask, Values>::value...> type;
		};
	}

	template<size_t Mask, int size>
	using unfold_mask = typename internals::unfold_mask<Mask, std::make_integer_sequence<int, size>>::type;
}

//---------------------------------------------------------------------------
#endif
