#pragma once
#include <string>
#include <vector>

struct ID3D11Texture3D;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;

class Texture3D 
{
	friend class Renderer;
public:
	Texture3D(int width, int height, int depth)
		: m_textureWidth(width), m_textureHeight(height), m_textureDepth(depth) 
	{
		m_textureData.resize(width * height * depth, 0.0f);  
	}

	virtual ~Texture3D();

public:
	std::string			m_textureName;
	int m_textureWidth, m_textureHeight, m_textureDepth;
	std::vector<float> m_textureData;

	ID3D11Texture3D*			m_texture3D = nullptr;
	ID3D11ShaderResourceView*	m_textureSRV = nullptr;
	ID3D11UnorderedAccessView*	m_textureUAV = nullptr;
};