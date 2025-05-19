#pragma once
#include "Engine/UI/UIElement.hpp"
class Button : public UIElementBase
{
public:
	Button(UIElementConfig const& config,UIElementBase* parent = nullptr)
		: UIElementBase(config,parent)
	{}

	Button(std::string const& elementName, Vec2 const& center, Vec2 const& dimension,UIElementBase* parent = nullptr)
		: UIElementBase(elementName, center, dimension, parent)
	{}

	virtual ~Button() {}

	virtual void Update(float deltaTime) override;
	virtual void Draw(Renderer& renderer) override;

	virtual void Click() override;
	virtual void Hover() override;

};