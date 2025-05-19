#pragma once
#include <string>

struct ID3D11ComputeShader;
struct ID3D11Query;

class ComputeShader 
{
	friend class Renderer;
public:
	ComputeShader();
	~ComputeShader();

	std::string						m_name;
	ID3D11ComputeShader*			m_computeShader = nullptr;
	ID3D11Query*					m_computeShaderQuery = nullptr;
};