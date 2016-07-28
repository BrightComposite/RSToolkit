//---------------------------------------------------------------------------

#include <opengl/OpenGL3_3.h>
#include <opengl/shaders/Shaders.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace OpenGL
	{
		void GLGraphics::initShaders()
		{
			string id;

			for(int i = 0; i < array_size<decltype(shaders)>::value; ++i)
			{
				auto & shader = shaders[i];
				ShaderCodeSet codeSet;

				for(int j = 0; j < shader.count; ++j)
				{
					auto & unit = shader.units[j];
					codeSet.insert({ShaderType(unit.type), handle<ShaderCode>(unit.data, unit.size)});
				}

				id = shader.id;
				_shaderPrograms[id] = handle<GLShaderProgram>(this, id, getVertexLayout(shader.layout), codeSet);
			}
		}
	}
}

//---------------------------------------------------------------------------
