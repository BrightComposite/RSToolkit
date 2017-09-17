//---------------------------------------------------------------------------

#include <opengl/OpenGL3_3.h>
#include <opengl/GLObjects.h>
#include <opengl/shaders/Shaders.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace opengl
	{
		void GLGraphics::initShaders()
		{
			addShaderPrograms(shaders);
		}
	}
}

//---------------------------------------------------------------------------
