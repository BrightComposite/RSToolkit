//---------------------------------------------------------------------------

#ifndef TEXT_H
#define TEXT_H

//---------------------------------------------------------------------------

#include <core/String.h>

#include <graphics/Graphics.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Text //: public _____
	{
	public:
		Text(String contents) {}
		virtual ~Text() {}

		virtual void setContents(String contents)
		{
			this->contents = contents;
		}

	protected:
		String contents;
	};
}

//---------------------------------------------------------------------------
#endif