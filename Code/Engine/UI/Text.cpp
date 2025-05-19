#include "Engine/UI/Text.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

void Text::Update(float deltaTime)
{
	UIElementBase::Update(deltaTime);
}

void Text::Draw(Renderer& renderer)
{
	UIElementBase::Draw(renderer);

	std::vector<Vertex_PCU> verts;
	
	Vec2 const& boxCenter = m_elementConfig.m_elementBox.GetCenter();
	float totalLength = m_size * m_text.size();
	Vec2 startPos = boxCenter - Vec2(totalLength * 0.5f, 0.0f);
	m_font->AddVertsForText2DTier3(verts, startPos, m_size, m_text, m_color, 0.1f);
	
	renderer.BindTexture(&m_font->GetTexture());
	renderer.SetSamplerMode(SamplerMode::BILINEAR_CLAMP);
	renderer.BindShader(renderer.CreateOrGetShader("Data/Shaders/Tier4_BlurShader"));
	renderer.SetModelConstants();
	renderer.DrawVertexArray((int)verts.size(), verts.data());
}

void Text::SetText(std::string const& text)
{
	m_text = text;
}

std::string Text::GetText() const
{
	return m_text;
}

Text* Text::SetTextPivot(Vec2 const& pivot)
{
	m_pivot = pivot;
	return this;
}

Text* Text::SetTextSize(float size)
{
	m_size = size;
	return this;
}

Text* Text::SetTextColor(Rgba8 color)
{
	m_color = color;
	return this;
}
