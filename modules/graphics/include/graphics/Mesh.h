//---------------------------------------------------------------------------

#pragma once

#ifndef MESH_H
#define MESH_H

//---------------------------------------------------------------------------

#include <core/Object.h>
#include <core/container/Data.h>
#include "VertexLayout.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	class Graphics3D;
	class Mesh;

	class VertexData : public array_list<float>
	{
	public:
		using array_list<float>::array_list;

		static api(graphics) const VertexData quad2d;
		static api(graphics) const VertexData texquad2d;
		static api(graphics) const VertexData linequad2d;
		static api(graphics) const VertexData quad3d;
		static api(graphics) const VertexData texquad3d;
		static api(graphics) const VertexData linequad3d;
		static api(graphics) const VertexData cube;
		static api(graphics) const VertexData texcube;
		static api(graphics) const VertexData colorcube;
	};

	class VertexIndices : public array_list<uint16_t>
	{
	public:
		using array_list<uint16_t>::array_list;

		static api(graphics) VertexIndices cube;
		static api(graphics) VertexIndices linecube;
	};

	enum class VertexTopology
	{
		Triangles,
		TriangleStrip,
		Lines,
		LineStrip
	};

	class MeshBuffer : public Shared
	{
	public:
		MeshBuffer() {}

		virtual void apply() const = 0;
	};

	class VertexBuffer : public MeshBuffer
	{
	public:
		VertexBuffer(VertexLayout * layout, const VertexData & vd) : layout(layout), verticesCount(static_cast<uint>(vd.size() / layout->units)) {}

		VertexLayout * layout;
		uint verticesCount;
	};

	class MeshInstance : public Shared
	{
	public:
		MeshInstance(Mesh * mesh, uint index) : mesh(mesh), index(index) {}

		virtual void setData(void * data) = 0;

		Mesh * mesh;
		uint index;
	};

	enum class MeshState
	{
		Ready     = 1,
		Indexed   = 2,
		Instanced = 4
	};

	adapt_enum_flags(MeshState);

	class MIDElement : public Shared
	{
		friend class VertexLayout;

	public:
		string key;
		uint id;
		uint index;
		uint units;

	protected:
		MIDElement(const string & key, uint id, uint index, uint units) : key(key), id(id), index(index), units(units) {}
	};

	class MIDLayout : public Shared
	{
	public:
		api(graphics) MIDLayout(Graphics3D * graphics, const string & fingerprint);

		virtual void apply() {}

		string fingerprint;
		array_list<MIDElement *> elements;
		uint units;
		uint stride;
	};

	class Mesh : public Shared
	{
	public:
		virtual Mesh * buffer(const Handle<VertexBuffer> & buffer) = 0;
		virtual Mesh * indices(const VertexIndices & indices) = 0;
		virtual Mesh * topology(VertexTopology topology) = 0;
		virtual Mesh * ready() = 0;

		virtual void draw() const = 0;

		virtual Mesh * apply(MIDLayout *) = 0;
		virtual MeshInstance * instance() const = 0;

		void setLocation(uint location)
		{
			_verticesLocation = location;
		}

	protected:
		uint _verticesLocation = 0;
		uint _verticesCount = 0;
		uint _state = 0;
		VertexLayout * _layout = nullptr;
	};
}

//---------------------------------------------------------------------------
#endif
