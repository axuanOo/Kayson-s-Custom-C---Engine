#include "Engine/Renderer/Skybox.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#define DX_SAFE_RELEASE(dxObject)							\
{															\
	if ((dxObject) != nullptr)								\
	{														\
		(dxObject)->Release();								\
		(dxObject) = nullptr;								\
	}														\
}
#include "Engine/Renderer/Image.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVec3.hpp"

Skybox::Skybox(ID3D11Device* device, ID3D11DeviceContext* dc, std::string const& fileString, SkyboxMode mode /*= SkyboxMode::BOX*/)
	:m_device(device), m_deviceContext(dc), m_mode(mode)
{
	std::unique_ptr<Image> wholeImage = std::make_unique<Image>(fileString.c_str());

	int singleImageDimensionX = wholeImage->GetDimensions().x / 4;
	int singleImageDimensionY = wholeImage->GetDimensions().y / 3;
	IntVec2 singleImageDimension = IntVec2(singleImageDimensionX, singleImageDimensionY);

	IntVec2 startCropPosFromBL[6] = 
	{
		IntVec2(2 * singleImageDimensionX,singleImageDimensionY),
		IntVec2(0,singleImageDimensionY),
		IntVec2(singleImageDimensionX,singleImageDimensionY),
		IntVec2(3 * singleImageDimensionX,singleImageDimensionY),
		IntVec2(singleImageDimensionX,2 * singleImageDimensionY),
		IntVec2(singleImageDimensionX,0),
	};

	std::vector<Image> sixFaces = std::vector<Image>(6, Image(singleImageDimension, Rgba8::WHITE));
	// RIGHT
	{
		Image& image = sixFaces[0];
		IntVec2 const& startPosLarge = startCropPosFromBL[0];
		IntVec2 currentPos = startPosLarge;
		Rgba8 color = Rgba8::WHITE;

		for (int x = 0; x < singleImageDimensionX; x++)
		{
			for (int y = 0; y < singleImageDimensionY; y++)
			{
				currentPos = startPosLarge + IntVec2(x, y);
				color = wholeImage->GetTexelColor(currentPos);
				image.SetTexelColor(IntVec2(singleImageDimensionY - 1 - y, x), color);
			}
		}
	}
	// LEFT
	{
		Image& image = sixFaces[1];
		IntVec2 const& startPosLarge = startCropPosFromBL[1];
		IntVec2 currentPos = startPosLarge;
		Rgba8 color = Rgba8::WHITE;

		for (int x = 0; x < singleImageDimensionX; x++)
		{
			for (int y = 0; y < singleImageDimensionY; y++)
			{
				currentPos = startPosLarge + IntVec2(x, y);
				color = wholeImage->GetTexelColor(currentPos);
				image.SetTexelColor(IntVec2(y, singleImageDimensionX - 1 - x), color);
			}
		}
	}
	// TOP
	{
		Image& image = sixFaces[2];
		IntVec2 const& startPosLarge = startCropPosFromBL[2];
		IntVec2 currentPos = startPosLarge;
		Rgba8 color = Rgba8::WHITE;

		for (int x = 0; x < singleImageDimensionX; x++)
		{
			for (int y = 0; y < singleImageDimensionY; y++)
			{
				currentPos = startPosLarge + IntVec2(x, y);
				color = wholeImage->GetTexelColor(currentPos);
				image.SetTexelColor(IntVec2(x, y), color);
			}
		}
	}
	// BOTTOM
	{
		Image& image = sixFaces[3];
		IntVec2 const& startPosLarge = startCropPosFromBL[3];
		IntVec2 currentPos = startPosLarge;
		Rgba8 color = Rgba8::WHITE;

		for (int x = 0; x < singleImageDimensionX; x++)
		{
			for (int y = 0; y < singleImageDimensionY; y++)
			{
				currentPos = startPosLarge + IntVec2(x, y);
				color = wholeImage->GetTexelColor(currentPos);
				image.SetTexelColor(IntVec2(singleImageDimensionX - 1 - x, singleImageDimensionY - 1 - y), color);
			}
		}
	}
	// FRONT
	{
		Image& image = sixFaces[4];
		IntVec2 const& startPosLarge = startCropPosFromBL[4];
		IntVec2 currentPos = startPosLarge;
		Rgba8 color = Rgba8::WHITE;

		for (int x = 0; x < singleImageDimensionX; x++)
		{
			for (int y = 0; y < singleImageDimensionY; y++)
			{
				currentPos = startPosLarge + IntVec2(x, y);
				color = wholeImage->GetTexelColor(currentPos);
				image.SetTexelColor(IntVec2(x, y), color);
			}
		}
	}
	// BACK
	{
		Image& image = sixFaces[5];
		IntVec2 const& startPosLarge = startCropPosFromBL[5];
		IntVec2 currentPos = startPosLarge;
		Rgba8 color = Rgba8::WHITE;

		for (int x = 0; x < singleImageDimensionX; x++)
		{
			for (int y = 0; y < singleImageDimensionY; y++)
			{
				currentPos = startPosLarge + IntVec2(x, y);
				color = wholeImage->GetTexelColor(currentPos);
				image.SetTexelColor(IntVec2(singleImageDimensionX - 1 - x, singleImageDimensionY - 1 - y), color);
			}
		}
	}

	int width = sixFaces[0].GetDimensions().x;
	int height = sixFaces[0].GetDimensions().y;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; 

	D3D11_SUBRESOURCE_DATA initData[6];
	for (int i = 0; i < 6; i++)
	{
		initData[i].pSysMem = sixFaces[i].GetRawData(); 
		initData[i].SysMemPitch = width * 4; 
		initData[i].SysMemSlicePitch = 0;
	}

	HRESULT hr = m_device->CreateTexture2D(&textureDesc, initData, &m_cubeTexture);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create Cube Map texture!\n");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; 
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1; 

	if (m_cubeTexture)
	{
		hr = m_device->CreateShaderResourceView(m_cubeTexture, &srvDesc, &m_srv);
		if (FAILED(hr))
		{
			OutputDebugString(L"Failed to create Shader Resource View for Cube Map!\n");
		}
	}

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  // Bilinear filtering
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;    // Clamp mode on U axis
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;    // Clamp mode on V axis
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;    // Clamp mode on W axis
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerState);


	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE; 
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; 
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; 
	dsDesc.StencilEnable = FALSE; 

	hr = m_device->CreateDepthStencilState(&dsDesc, &m_depthStencilState);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create Skybox DepthStencilState!\n");
	}

	std::vector<Vertex_PCU> m_boxVerts;
	AddVertsForSkyBox3D(m_boxVerts,AABB3(Vec3(-50.0f,-50.0f,-50.0f),Vec3(50.0f,50.0f,50.0f)));

	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex_PCU) * (UINT)m_boxVerts.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_device->CreateBuffer(&vbd, nullptr, &m_vertexBuffer);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create Skybox vertexbuffer!\n");
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, m_boxVerts.data(), m_boxVerts.size() * sizeof(Vertex_PCU));
	m_deviceContext->Unmap(m_vertexBuffer, 0);

}

Skybox::~Skybox()
{
	DX_SAFE_RELEASE(m_srv);
	DX_SAFE_RELEASE(m_cubeTexture);
	DX_SAFE_RELEASE(m_samplerState);
	DX_SAFE_RELEASE(m_depthStencilState);
	DX_SAFE_RELEASE(m_vertexBuffer);
}

void Skybox::Render() const
{

	UINT stride = sizeof(Vertex_PCU);
	UINT offset = 0;
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	m_deviceContext->PSSetShaderResources(0, 1, &m_srv);
	m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);

	m_deviceContext->Draw(36, 0);
	m_deviceContext->OMSetDepthStencilState(nullptr, 0);
}
