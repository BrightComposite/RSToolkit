//---------------------------------------------------------------------------

#pragma once

#ifndef SHADER_CODE_H
#define SHADER_CODE_H

//---------------------------------------------------------------------------

#define RFX_VERTEX_SHADER   0
#define RFX_FRAGMENT_SHADER 1
#define RFX_GEOMETRY_SHADER 2

struct RawShaderCodeUnit
{
	template<size_t N>
	constexpr RawShaderCodeUnit(const char (&data)[N], int type) : data(data), size(N), type(type) {}
	template<size_t N>
	constexpr RawShaderCodeUnit(const unsigned char (&data)[N], int type) : data(data), size(N), type(type) {}

	const void * const data;
	const size_t size;
	const int type;
};

struct RawShaderCode
{
	template<size_t N>
	constexpr RawShaderCode(const char * id, const char * layout, const RawShaderCodeUnit (&units)[N]) : id(id), layout(layout), units(units), count(N) {}

	const char * id;
	const char * layout;
	const RawShaderCodeUnit * const units;
	const int count;
};

//---------------------------------------------------------------------------
#endif
