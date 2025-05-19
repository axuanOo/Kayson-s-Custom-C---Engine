#include "Engine/Renderer/Texture3D.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

Texture3D::~Texture3D()
{
	DX_SAFE_RELEASE(m_texture3D);
	DX_SAFE_RELEASE(m_textureSRV);
	DX_SAFE_RELEASE(m_textureUAV);
}
