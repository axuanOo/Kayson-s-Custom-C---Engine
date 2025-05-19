#pragma once
#include "Engine//Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

struct Vertex_PCU
{

public:
	Vertex_PCU();
	Vec3   m_position;
	Rgba8  m_color;
	Vec2   m_uvTexCoords;

public:

	Vertex_PCU(float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);		// default constructor (do nothing)
	explicit Vertex_PCU(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords);					// explicit constructor

	explicit Vertex_PCU(Vec2 const& position, Rgba8 const& color, Vec2 const& uvTexCoords);					// explicit constructor

}; 

struct Vertex_PCUTBN 
{
public:
	Vertex_PCUTBN() = default;
	Vertex_PCUTBN(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords,
		Vec3 const& tangent = Vec3::ZERO, Vec3 const& bitangent= Vec3::ZERO, Vec3 const& normal= Vec3::ZERO)
		:m_position(position),
		m_color(color),
		m_uvTexCoords(uvTexCoords),
		m_tangent(tangent),
		m_bitangent(bitangent),
		m_normal(normal)
	{};

	Vertex_PCUTBN(float px, float py, float pz,
		unsigned char r, unsigned char g, unsigned char b, unsigned char a,
		float u, float v,
		float tx, float ty, float tz,
		float bx, float by, float bz,
		float nx, float ny, float nz)
		: m_position(px, py, pz)
		, m_color(r, g, b, a)
		, m_uvTexCoords(u, v)
		, m_tangent(tx, ty, tz)
		, m_bitangent(bx, by, bz)
		, m_normal(nx, ny, nz)
	{};

	bool operator==(const Vertex_PCUTBN& other) const {
		return m_position == other.m_position &&
			m_color == other.m_color&&
			m_uvTexCoords == other.m_uvTexCoords &&
			m_normal == other.m_normal&&
			m_tangent == other.m_tangent&&
			m_bitangent == other.m_bitangent; 
	}	

	Vec3    m_position;
	Rgba8   m_color;
	Vec2    m_uvTexCoords;

	Vec3	m_tangent;
	Vec3	m_bitangent;
	Vec3	m_normal;

};

struct Vertex_Font
{
public:
	Vec3	m_position;
	Rgba8	m_color;
	Vec2	m_uvTexCoords;

	Vec2	m_glyphPosition; // normalized [0,1] coordinates of where this vert lies within this glyph-quad
	Vec2	m_textPosition; // normalized coordinates of where this vert is within this overall text block
	int 	m_characterIndex; // index of which character in this text-string this vert belongs to (same at all four corners of each glyph-quad)
	
	float	m_weight;

	Vertex_Font() = default;
	Vertex_Font(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords, Vec2 glyphPosition, Vec2 textPosition, int characterIndex, float weight = 1.f) 
		:m_position(position),
		m_color(color),
		m_uvTexCoords(uvTexCoords),
		m_glyphPosition(glyphPosition),
		m_textPosition(textPosition),
		m_characterIndex(characterIndex),
		m_weight(weight)
	{};
};
