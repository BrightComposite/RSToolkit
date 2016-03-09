//---------------------------------------------------------------------------

#ifndef SHADERS_H
#define SHADERS_H

//---------------------------------------------------------------------------

#include "Graphics.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	enum class ShaderType : int
	{
		Common = -1,
		Vertex = 0, // vertex shader
		Fragment = 1, // fragment shader (OpenGL)
		Pixel = 1, // pixel shader (Direct3D)
		Max
	};

	inline void print(String & s, ShaderType type)
	{
		switch(type)
		{
		case ShaderType::Common:
			s << "Common";
			return;

		case ShaderType::Vertex:
			s << "Vertex";
			return;

		case ShaderType::Fragment:
			s << "Fragment (Pixel)";
			return;
		}
	}
}

//---------------------------------------------------------------------------
#endif
