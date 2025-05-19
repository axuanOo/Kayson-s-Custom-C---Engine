#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <array>

enum class TextBoxMode
{
	SHRINK_TO_FIT,
	OVERRUN,
	TEXTBOXMODE_NUM
};

struct FontMetaData
{
	unsigned int m_id;
	unsigned int m_x;
	unsigned int m_y;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_xOffset;
	unsigned int m_yOffset;
};


class BitmapFont
{
	friend class Renderer; // Only the Renderer can create new BitmapFont objects!

private:
	BitmapFont(char const* fontFilePathNameWithNoExtension, Texture& fontTexture);
	BitmapFont(char const* xmlPath, char const* fontPath, Texture& fontTexture);
public:

	Texture const& GetTexture();

	void AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins,
		const float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f);

	void AddVertsForText2DTier2(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins,
		const float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f);

	void AddVertsForText2DTier3(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins,
		float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8(255, 255, 255, 255), float spaceBetweenTwoChar = 0.2f);

	void AddVertsForText2DTier3AlongAxis(std::vector<Vertex_PCU>& vertexArray, Vec2 const& direction,Vec2 const& textMins,
		float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8(255, 255, 255, 255), float spaceBetweenTwoChar = 0.2f);

	void AddVertsForText2DTier5(std::vector<Vertex_Font>& vertexArray, Vec2 const& textMins,
		float cellHeight, std::string const& text, float spaceBetweenTwoChar = 0.2f, Rgba8 const& tint = Rgba8(255, 255, 255, 255), float cellAspect = 1.0f,
		const Vec2& glyphPosition = Vec2(0.f, 0.f), const Vec2& textPosition = Vec2(0.f, 0.f), int characterIndex = 0, float weight = 0.2f);

	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight,
		std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f,
		Vec2 const& alignment = Vec2(.5f, .5f), TextBoxMode mode = TextBoxMode::SHRINK_TO_FIT, int maxGlyphsToDraw = 99999999, float spacingRatio = 1.0f, float verticalLineSpacing = 1.0f, bool autoWrap = false);

	void AddVertsForText3DAtOriginXForward(std::vector<Vertex_PCU>& vertexArray, float cellHeight,
		std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f,
		Vec2 const& alignment = Vec2(.5f, .5f), TextBoxMode mode = TextBoxMode::SHRINK_TO_FIT, int maxGlyphsToDraw = 99999999);

	float GetTextWidth(float cellHeight, std::string const& text, float cellAspect = 1.f);

	std::string		m_fontFilePathNameWithNoExtension;
	SpriteSheet*	m_fontGlyphsSpriteSheet;
protected:

	float GetGlyphAspect(int glyphUnicode) const; // For now this will always return 1.0f!!!

protected:
	std::array<float, 256> m_glyphUOffset;   
	std::array<float, 256> m_glyphUWidth;    
	std::map <unsigned char, FontMetaData> m_customFontMap;
	IntVec2					m_textureDimensions;
};

