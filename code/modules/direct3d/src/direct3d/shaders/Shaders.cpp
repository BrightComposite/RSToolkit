//---------------------------------------------------------------------------

#include <direct3d/Direct3D11.h>
#include <direct3d/shaders/Shaders.h>

//---------------------------------------------------------------------------

namespace asd
{
	namespace Direct3D
	{
		void D3DGraphics::initShaders()
		{
			addShaderPrograms(shaders, array_size<decltype(shaders)>::value);
		}

		void D3DGraphics::setShaderProgram(const string & id, VertexLayout * layout, const Handle<ShaderCodeSet> & codeSet)
		{
			shaderPrograms[id] = handle<D3DShaderProgram>(this, layout, codeSet);
		}
	}
}
