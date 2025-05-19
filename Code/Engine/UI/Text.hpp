#pragma once
#include "Engine/UI/UIElement.hpp"

class BitmapFont;
class Text : public UIElementBase
{
public:
	Text(UIElementConfig const& config, std::string const& text, UIElementBase* parent = nullptr, BitmapFont* font = nullptr)
		: UIElementBase(config, parent), m_text(text), m_font(font)
	{
		m_elementConfig.m_canClick = false;
		m_elementConfig.m_canHover = false;
	}

	Text(std::string const& elementName, Vec2 const& center, Vec2 const& dimension, std::string const& text, UIElementBase* parent = nullptr, BitmapFont* font = nullptr)
		: UIElementBase(elementName, center, dimension, parent), m_text(text), m_font(font)
	{
		m_elementConfig.m_canClick = false;
		m_elementConfig.m_canHover = false;
	}

	virtual ~Text() {}

	virtual void					Update(float deltaTime);
	virtual void					Draw(Renderer& renderer);

	void							SetText(std::string const& text);
	std::string						GetText() const;

	Text*							SetTextPivot(Vec2 const& pivot);
	Text*							SetTextSize(float size);
	Text*							SetTextColor(Rgba8 color);

protected:
	std::string						m_text;
	BitmapFont*						m_font = nullptr;
	Rgba8							m_color;
	Vec2							m_pivot = Vec2::ZERO;
	float							m_size = 10.0f;
};