//---------------------------------------------------------------------------

#pragma once

#ifndef GRAPHICS_SHADERS_H
#define GRAPHICS_SHADERS_H

//---------------------------------------------------------------------------

#include "Graphics.h"
#include "VertexLayout.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics3D;

	enum class ShaderType : int
	{
		Common = -1,
		Vertex = 0, // vertex shader
		Fragment = 1, // fragment shader (OpenGL)
		Pixel = 1, // pixel shader (Direct3D)
		Max
	};

	enum class ShaderCodeState
	{
		Raw,
		Compiled,
		Embedded
	};

}

namespace std
{
	template<>
	use_enum_hash(Rapture::ShaderType);
}

namespace Rapture
{
	subclass(ShaderCode, data<const void>);

	struct ShaderCodeSet : Shared
	{
		Map<ShaderType, ShaderCode> code;
	};

	typedef Map<string, ShaderCodeSet> ShaderMap;

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
		virtual ~FxTechnique() {}

		virtual void apply(uint pass = 0) const
		{
			program->apply();
		}

	protected:
		FxTechnique(const Handle<ShaderProgram> & program) : program(program) {}

		Handle<ShaderProgram> program;
		uint passes = 0;
	};

//---------------------------------------------------------------------------

	template<ShaderType type>
	class Shader {};

	template<>
	class Shader<ShaderType::Common> : public Shared
	{
		friend class Graphics3D;

	public:
		virtual ~Shader() {}

	protected:
		virtual void apply() const = 0;
	};

	template<>
	class Shader<ShaderType::Vertex> : public Shader<ShaderType::Common>
	{
	public:
		virtual ~Shader() {}
	};

	template<>
	class Shader<ShaderType::Pixel> : public Shader<ShaderType::Common>
	{
	public:
		virtual ~Shader() {}
	};

	using VertexShader = Shader<ShaderType::Vertex>;
	using PixelShader  = Shader<ShaderType::Pixel>;

	template<class T>
	using is_shader_program = is_base_of<ShaderProgram, T>;

//---------------------------------------------------------------------------

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
