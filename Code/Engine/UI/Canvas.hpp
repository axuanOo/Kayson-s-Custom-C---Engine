#pragma once
#include "Engine/UI/UIElement.hpp"
class Canvas : public UIElementBase
{
public:
	Canvas(UIElementConfig const& config,UIElementBase* parent = nullptr)
		: UIElementBase(config,parent)
	{}

	Canvas(std::string const& elementName, Vec2 const& center, Vec2 const& dimension,UIElementBase* parent = nullptr)
		: UIElementBase(elementName, center, dimension, parent)
	{}

	virtual ~Canvas() {}

	virtual void					Update(float deltaTime);
	virtual void					Draw(Renderer& renderer);
};