//---------------------------------------------------------------------------

#include <opengl/OpenGL3_3.h>
#include <opengl/GLObjects.h>
#include <opengl/shaders/Shaders.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace OpenGL
	{
		void GLGraphics::initShaders()
		{
			addShaderPrograms(shaders);
		}
	}
}

//---------------------------------------------------------------------------
