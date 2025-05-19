#pragma once
#include "Engine/Core/EngineCommon.hpp"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
struct ID3D11SamplerState;
struct ID3D11DepthStencilState;
struct ID3D11Buffer;

enum class SkyboxMode 
{
	BOX = 0,
	SPHERE = 1,
	COUNT
};

class Skybox
{
public:
	Skybox(ID3D11Device* device, ID3D11DeviceContext* dc, std::string const& fileString, SkyboxMode mode = SkyboxMode::BOX);
	~Skybox();

	void							Render() const;

private:
	ID3D11Device*					m_device = nullptr;
	ID3D11DeviceContext*			m_deviceContext = nullptr;
	
	SkyboxMode						m_mode;

	ID3D11ShaderResourceView*		m_srv = nullptr;
	ID3D11Texture2D*				m_cubeTexture = nullptr;
	ID3D11SamplerState*				m_samplerState = nullptr;
	ID3D11DepthStencilState*		m_depthStencilState = nullptr;
	ID3D11Buffer*					m_vertexBuffer = nullptr;

};
