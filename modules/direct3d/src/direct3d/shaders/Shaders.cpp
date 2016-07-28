//---------------------------------------------------------------------------

#include <direct3d/Direct3D11.h>
#include <direct3d/shaders/Shaders.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace Direct3D
	{
		void D3DGraphics::initShaders()
		{
			//auto p2  = getVertexLayout("p2");
			//auto p2t = getVertexLayout("p2 t");
			//auto p3  = getVertexLayout("p3");
			//auto p3t = getVertexLayout("p3 t");
			//auto p3c4 = getVertexLayout("p3 c4");

			for(int i = 0; i < array_size<decltype(shaders)>::value; ++i)
			{
				auto & shader = shaders[i];
				Handle<ShaderCodeSet> codeSet;

				for(int j = 0; j < shader.count; ++j)
				{
					auto & unit = shader.units[j];
					codeSet->code.insert({ShaderType(unit.type), handle<ShaderCode>(unit.data, unit.size)});
				}

				setShaderProgram(shader.id, codeSet);
			}
		}

		void D3DGraphics::setShaderProgram(const string & id, const Handle<ShaderCodeSet> & codeSet)
		{
			shaderPrograms[id] = handle<D3DShaderProgram>(this, nullptr, codeSet);
		}
	}
}
