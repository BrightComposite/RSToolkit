//---------------------------------------------------------------------------

#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

//---------------------------------------------------------------------------

#include <meta/Bitmask.h>

//---------------------------------------------------------------------------

namespace asd
{
	struct Key {
		enum Code: int {
			Unknown = 0x0,
			
		/**
		 *	ASCII characters
		 */
			
			Backspace 	= '\b',
			Tab 		= '\t',
			Enter 		= '\n',
			Return 		= Enter,
			Escape 		= 0x1B,
			Space 		= ' ',
			
			Num0	= '0',
			Num1	= '1',
			Num2	= '2',
			Num3	= '3',
			Num4	= '4',
			Num5	= '5',
			Num6	= '6',
			Num7	= '7',
			Num8	= '8',
			Num9	= '9',
			
			A 		= 'A',
			B 		= 'B',
			C 		= 'C',
			D 		= 'D',
			E 		= 'E',
			F 		= 'F',
			G 		= 'G',
			H 		= 'H',
			I 		= 'I',
			J 		= 'J',
			K 		= 'K',
			L 		= 'L',
			M 		= 'M',
			N 		= 'N',
			O 		= 'O',
			P 		= 'P',
			Q 		= 'Q',
			R 		= 'R',
			S 		= 'S',
			T 		= 'T',
			U 		= 'U',
			V 		= 'V',
			W 		= 'W',
			X 		= 'X',
			Y 		= 'Y',
			Z 		= 'Z'
		};
	};
	
	struct KeyMap
	{
	public:
		KeyMap() {}
		
		void press(Key::Code key) {
			data.insert(key);
		}
		
		void unpress(Key::Code key) {
			data.erase(key);
		}
		
		void unpressAll() {
			data.clear();
		}
		
		bool isPressed(Key::Code key) {
			return data.find(key) != data.end();
		}
		
		set<int> data;
	};
	
}

//---------------------------------------------------------------------------
#endif
