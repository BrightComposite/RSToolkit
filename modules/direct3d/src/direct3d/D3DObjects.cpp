//---------------------------------------------------------------------------

#include <direct3d/Direct3D11.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	namespace Direct3D
	{
		D3DVertexLayout::D3DVertexLayout(D3DGraphics * graphics, const string & fingerprint) : VertexLayout(fingerprint), graphics(graphics) {}

		void D3DVertexLayout::apply()
		{
			graphics->context->IASetInputLayout(handle);
		}

		void D3DVertexLayout::accept(const ShaderCode * code)
		{
			if(handle != nullptr)
				return;

			static const DXGI_FORMAT formats[] = {
				DXGI_FORMAT_UNKNOWN,
				DXGI_FORMAT_R32_FLOAT,
				DXGI_FORMAT_R32G32_FLOAT,
				DXGI_FORMAT_R32G32B32_FLOAT,
				DXGI_FORMAT_R32G32B32A32_FLOAT
			};

			static const char * semantics[] = {
				"POSITION",
				"COLOR",
				"NORMAL",
				"TEXCOORD"
			};

			array_list<D3D11_INPUT_ELEMENT_DESC> descs;
			uint stride = 0;

			for(auto * vie : elements)
			{
				descs.push_back({semantics[vie->type], vie->index, formats[vie->units], 0, stride, D3D11_INPUT_PER_VERTEX_DATA, 0});
				stride += vie->units * sizeof(float);
			}

			com_assert(
				graphics->device->CreateInputLayout(descs.data(), static_cast<uint>(descs.size()), code->ptr, code->size, &handle),
				"Can't create vertex input layout for shader, layout fingerprint is \"", this->fingerprint, "\""
			);
		}

		D3DMeshTrait::D3DMeshTrait(D3DGraphics * graphics, VertexTopology t) : graphics(graphics)
		{
			switch(t)
			{
				case VertexTopology::Triangles:
					topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					break;

				case VertexTopology::TriangleStrip:
					topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
					break;

				case VertexTopology::Lines:
					topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
					break;

				case VertexTopology::LineStrip:
					topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
					break;
			};
		}

		D3DVertexBuffer::D3DVertexBuffer(D3DGraphics * graphics, VertexLayout * layout, const VertexData & vd) : VertexBuffer(layout, vd), graphics(graphics)
		{
			if(vd.size % layout->stride != 0)
				throw Exception("Size of vertex buffer doesn't matches its vertex input layout");
			
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = static_cast<uint>(vd.size);
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sdata;
			ZeroMemory(&sdata, sizeof(sdata));
			sdata.pSysMem = vd.ptr;

			com_assert(
				graphics->device->CreateBuffer(&bd, &sdata, &handle),
				"Can't create vertex buffer!"
			);
		}

		void D3DVertexBuffer::apply() const
		{
			uint stride = layout->stride;
			uint offset = 0;

			graphics->context->IASetVertexBuffers(0, 1, &handle, &stride, &offset);
		}

		D3DIndexBuffer::D3DIndexBuffer(D3DGraphics * graphics, const VertexIndices & indices) : IndexBuffer(indices), graphics(graphics)
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = static_cast<uint>(size * sizeof(uint16_t));
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sdata;
			ZeroMemory(&sdata, sizeof(sdata));
			sdata.pSysMem = indices.data();

			com_assert(
				graphics->device->CreateBuffer(&bd, &sdata, &handle),
				"Can't create index buffer!"
			);
		}

		void D3DIndexBuffer::apply() const
		{
			graphics->context->IASetIndexBuffer(handle, DXGI_FORMAT_R16_UINT, 0);
		}

		void D3DMesh::draw() const
		{
			graphics->bind(vbuffer);
			graphics->context->IASetPrimitiveTopology(topology);
			graphics->context->Draw(vbuffer->verticesCount - verticesLocation, verticesLocation);
		}

		void D3DIndexedMesh::draw() const
		{
			graphics->bind(vbuffer);
			graphics->bind(ibuffer);
			graphics->context->IASetPrimitiveTopology(topology);
			graphics->context->DrawIndexed(ibuffer->size - indicesLocation, indicesLocation, verticesLocation);
		}

		D3DShader::D3DShader(D3DShaderProgram * program) : program(program) {}

		void D3DShader::read(const String & filename, Handle<ShaderCode> & out)
		{
			ComHandle<ID3DBlob> code;

			com_assert(
				D3DReadFileToBlob(widen(filename).c_str(), &code),
				"Can't read shader from file: ", filename
			);

			out.init(code->GetBufferPointer(), code->GetBufferSize());
		}

		void D3DShader::compile(const String & filename, const String & entrance, const String & shaderModel, Handle<ShaderCode> & out)
		{
			ComHandle<ID3DBlob> code;
			DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;
		#ifdef DEBUG_DX
			flags |= D3DCOMPILE_DEBUG;
		#endif
			ComHandle<ID3DBlob> error;
			HRESULT hr = D3DCompileFromFile(widen(filename).c_str(), nullptr, nullptr, entrance, shaderModel, flags, 0, &code, &error);

			com_assert(hr, (char *)error->GetBufferPointer());

			out.init(code->GetBufferPointer(), code->GetBufferSize());
		}

		//#define COMPILE_SHADERS

		D3DVertexShader::D3DVertexShader(D3DShaderProgram * program, const String & path, ShaderCodeState state) : D3DShader(program)
		{
			Handle<ShaderCode> code;

			switch(state)
			{
				case ShaderCodeState::Raw:
					compile(path + "_vs.fx", "main", "vs_4_0", code);
					break;

				case ShaderCodeState::Compiled:
					read(path + "_vs.cso", code);
					break;
			}

			init(code);
		}

		D3DVertexShader::D3DVertexShader(D3DShaderProgram * program, const Handle<ShaderCode> & code) : D3DShader(program)
		{
			init(code);
		}

		void D3DVertexShader::init(const Handle<ShaderCode> & code)
		{
			HRESULT hr = S_OK;

			com_assert(
				program->graphics->device->CreateVertexShader(code->ptr, code->size, nullptr, &id),
				"Can't create vertex shader!"
			);

			program->layout->accept(code);
		}

		void D3DVertexShader::apply() const
		{
			;
		}

		D3DPixelShader::D3DPixelShader(D3DShaderProgram * program, const String & path, ShaderCodeState state) : D3DShader(program)
		{
			Handle<ShaderCode> code;

			switch(state)
			{
				case ShaderCodeState::Raw:
					compile(path + "_ps.fx", "main", "ps_4_0", code);
					break;

				case ShaderCodeState::Compiled:
					read(path + "_ps.cso", code);
					break;
			}

			init(code);
		}

		D3DPixelShader::D3DPixelShader(D3DShaderProgram * program, const Handle<ShaderCode> & code) : D3DShader(program)
		{
			init(code);
		}

		void D3DPixelShader::init(const Handle<ShaderCode> & code)
		{
			com_assert(
				program->graphics->device->CreatePixelShader(code->ptr, code->size, nullptr, &id),
				"Can't create pixel shader!"
			);
		}

		void D3DPixelShader::apply() const
		{
			program->graphics->context->PSSetShader(id, nullptr, 0);
		}

		D3DImage::D3DImage(D3DGraphics * graphics, uint width, uint height) : Image(graphics, width, height, ImageFormat::bgra), _ctx(graphics)
		{
			D3D11_TEXTURE2D_DESC texDesc;

			ZeroMemory(&texDesc, sizeof(texDesc));

			texDesc.Width = _size.x;
			texDesc.Height = _size.y;
			texDesc.ArraySize = 1;
			texDesc.MipLevels = 1;
			texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			com_assert(
				graphics->device->CreateTexture2D(&texDesc, nullptr, &_handle)
			);

			com_assert(
				graphics->device->CreateShaderResourceView(_handle, nullptr, &_resource)
			);

			D3D11_SAMPLER_DESC sampDesc;
			ZeroMemory(&sampDesc, sizeof(sampDesc));
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			com_assert(
				graphics->device->CreateSamplerState(&sampDesc, &_state)
			);
		}

		D3DImage::D3DImage(D3DGraphics * graphics, const ImageData & data) : Image(graphics, data), _ctx(graphics)
		{
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));

			texDesc.Width = _size.x;
			texDesc.Height = _size.y;
			texDesc.ArraySize = 1;
			texDesc.MipLevels = 1;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;

			switch(_format)
			{
				case ImageFormat::grayscale:
					texDesc.Format = DXGI_FORMAT_R8_UNORM;
					break;

				case ImageFormat::grayscale_alpha:
					texDesc.Format = DXGI_FORMAT_R8G8_UNORM;
					break;

				case ImageFormat::bgra:
					texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					break;

				default:
					texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
					_format = ImageFormat::rgba;
			}

			auto newData = handle<ImageData>();
			data.convert(newData, _format);

			D3D11_SUBRESOURCE_DATA texInitData;
			ZeroMemory(&texInitData, sizeof(D3D11_SUBRESOURCE_DATA));

			texInitData.pSysMem = newData->ptr;
			texInitData.SysMemPitch = _size.x * preferredBpp(_format);
			texInitData.SysMemSlicePitch = texInitData.SysMemPitch;

			com_assert(
				graphics->device->CreateTexture2D(&texDesc, &texInitData, &_handle)
			);

			com_assert(
				graphics->device->CreateShaderResourceView(_handle, nullptr, &_resource)
			);

			D3D11_SAMPLER_DESC sampDesc;
			ZeroMemory(&sampDesc, sizeof(sampDesc));
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			com_assert(
				graphics->device->CreateSamplerState(&sampDesc, &_state)
			);
		}

		void D3DImage::apply() const
		{
			_ctx->context->PSSetShaderResources(0, 1, &_resource);
			_ctx->context->PSSetSamplers(0, 1, &_state);
		}

		void D3DImage::requestData(ImageData * output) const
		{
			if(output == nullptr)
				throw Exception("Output image buffer should be not-null!");

			ComHandle<ID3D11Texture2D> texture;
			D3D11_TEXTURE2D_DESC texDesc;

			ComHandle<ID3D11Resource> res;
			ComHandle<ID3D11Texture2D> ownTexture;

			_handle->GetDesc(&texDesc);

			texDesc.BindFlags = 0;
			texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			texDesc.Usage = D3D11_USAGE_STAGING;

			com_assert(
				_ctx->device->CreateTexture2D(&texDesc, nullptr, &texture)
			);

			_ctx->context->CopyResource(texture, _handle);

			D3D11_MAPPED_SUBRESOURCE resource;
			com_assert(
				_ctx->context->Map(texture, 0, D3D11_MAP_READ, 0, &resource)
			);

			output->area = _size;
			output->format = ImageFormat::rgba;

			const uint pitch = output->width() * 4;

			output->alloc();

			byte * src = static_cast<byte *>(resource.pData);
			byte * dst = output->ptr;

			for(uint y = 0; y < output->height(); ++y, src += resource.RowPitch, dst += pitch)
				memcpy(dst, src, pitch);

			_ctx->context->Unmap(texture, 0);
		}

		D3DUniformAdapter::D3DUniformAdapter(D3DGraphics * graphics, ShaderType shader, int index, size_t size) : graphics(graphics), index(index)
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = static_cast<uint>(size);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;

			com_assert(
				graphics->device->CreateBuffer(&bd, NULL, &buffer), "Can't create uniform buffer"
			);

			switch(shader)
			{
				case ShaderType::Vertex:
					graphics->context->VSSetConstantBuffers(index, 1, &buffer);
					break;

				case ShaderType::Pixel:
					graphics->context->PSSetConstantBuffers(index, 1, &buffer);
					break;
			}
		}

		void D3DUniformAdapter::update(const void * data)
		{
			graphics->context->UpdateSubresource(buffer, 0, NULL, data, 0, 0);
		}

		D3DShaderProgram::D3DShaderProgram(D3DGraphics * graphics, const Handle<ShaderCodeSet> & set) : graphics(graphics), layout(layout)
		{
			auto i = set->code.find(ShaderType::Vertex);

			if(i == set->code.end())
				throw Exception("Shader program must contain vertex shader");

			auto & vscode = i->second;
			_shaders[ShaderType::Vertex] = handle<D3DVertexShader>(this, vscode);

			i = set->code.find(ShaderType::Pixel);
			_shaders[ShaderType::Pixel] = i != set->code.end() ? handle<D3DPixelShader>(this, i->second) : nullptr;

			ComHandle<ID3D11ShaderReflection> reflection;

			com_assert(
				D3DReflect(vscode->ptr, vscode->size, __uuidof(ID3D11ShaderReflection), reflection.pointer())
			);

			D3D11_SHADER_DESC shaderDesc;
			reflection->GetDesc( &shaderDesc );

			array_list<D3D11_INPUT_ELEMENT_DESC> descs;

			for(uint i = 0; i< shaderDesc.InputParameters; i++ )
			{
				D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
				reflection->GetInputParameterDesc(i, &paramDesc );

				// fill out input element desc
				D3D11_INPUT_ELEMENT_DESC elementDesc;
				elementDesc.SemanticName = paramDesc.SemanticName;
				elementDesc.SemanticIndex = paramDesc.SemanticIndex;
				elementDesc.InputSlot = 0;
				elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				elementDesc.InstanceDataStepRate = 0;   

				if(paramDesc.Mask == 1)
					elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
				else if(paramDesc.Mask <= 3)
					elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
				else if(paramDesc.Mask <= 7)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				else if(paramDesc.Mask <= 15)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

				descs.push_back(elementDesc);
			}       

			com_assert(
				graphics->device->CreateInputLayout(descs.data(), static_cast<uint>(descs.size()), vscode->ptr, vscode->size, pInputLayout)
			);
		}

		D3DShaderProgram::~D3DShaderProgram() {}

		void D3DShaderProgram::apply() const
		{
			layout->apply();

			for(auto & s : _shaders)
				graphics->bind(s.second, s.first);
		}
	}
}

//---------------------------------------------------------------------------
