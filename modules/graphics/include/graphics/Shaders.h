//---------------------------------------------------------------------------

#pragma once

#ifndef GRAPHICS_SHADERS_H
#define GRAPHICS_SHADERS_H

//---------------------------------------------------------------------------

#include "Graphics.h"
#include "VertexLayout.h"
#include "ShaderCode.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics3D;

	enum class ShaderType : int
	{
		Unknown = -1,
		Vertex = 0, // vertex shader
		Fragment = 1, // fragment shader (OpenGL)
		Pixel = 1, // pixel shader (Direct3D)
		Max
	};

	enum class ShaderCodeState
	{
		Raw,
		Compiled
	};

	template<>
	use_enum_hash(ShaderType);

	subclass(ShaderCode, owned_data<void>);

	using ShaderCodeSet = Dictionary<ShaderType, ShaderCode>;

//---------------------------------------------------------------------------

	class ShaderProgram : public Shared
	{
		friend class FxTechnique;

	public:
		virtual ~ShaderProgram() {}

	protected:
		ShaderProgram() {}
		virtual void apply() const {}
	};

	class FxTechnique : public Shared
	{
		friend_owned_handle(FxTechnique, Graphics3D);

	public:
		FxTechnique(const Handle<ShaderProgram> & program) : program(program) {}
		virtual ~FxTechnique() {}

		virtual void apply(uint pass = 0) const
		{
			program->apply();
		}

	protected:
		Handle<ShaderProgram> program;
		uint passes = 0;
	};

//---------------------------------------------------------------------------

	class Shader : public Shared
	{
	public:
		virtual ~Shader() {}

		virtual void apply() const {}
	};

	template<class T>
	using is_shader_program = is_base_of<ShaderProgram, T>;

//---------------------------------------------------------------------------

	inline void print(String & s, ShaderType type)
	{
		switch(type)
		{
			case ShaderType::Unknown:
				s << "Unknown";
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
