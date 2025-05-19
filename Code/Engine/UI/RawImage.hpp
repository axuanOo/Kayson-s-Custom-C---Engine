#pragma once
#include "Engine/UI/UIElement.hpp"
class RawImage : public UIElementBase
{
public:
	RawImage(UIElementConfig const& config,UIElementBase* parent = nullptr)
		: UIElementBase(config,parent)
	{
		m_elementConfig.m_canClick = false;
		m_elementConfig.m_canHover = false;
	}

	RawImage(std::string const& elementName, Vec2 const& center, Vec2 const& dimension,UIElementBase* parent = nullptr)
		: UIElementBase(elementName, center, dimension, parent)
	{
		m_elementConfig.m_canClick = false;
		m_elementConfig.m_canHover = false;
	}

	virtual ~RawImage() {}

	virtual void					Update(float deltaTime);
	virtual void					Draw(Renderer& renderer);
};