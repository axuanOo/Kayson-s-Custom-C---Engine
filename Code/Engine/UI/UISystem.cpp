#include "Engine/UI/UISystem.hpp"
#include "Engine/UI/UIElement.hpp"
#include "Engine/Input/InputSystem.hpp"

UISystem::~UISystem()
{
	for (UIElementBase* element: m_elements)
	{
		if (element)
		{
			delete element;
			element = nullptr;
		}
	}
	m_elements.clear();
}

void UISystem::ProcessMouseInput()
{
	Vec2 mousePos = m_config.m_inputSystem->m_cursorState.GetCursorNomalizedPosition() * m_config.m_screenSize;
	mousePos.x = GetClamped(mousePos.x, 0.0f, m_config.m_screenSize.x);
	mousePos.y = GetClamped(mousePos.y, 0.0f, m_config.m_screenSize.y);

	UIElementBase* topHoveredElement = nullptr;
	int highestLayer = -100000; 

	for (UIElementBase* element : m_elements)
	{
		// Check if the mouse position is inside the element's bounding box.
		if (element->GetElementBoudingBox().IsPointInside(mousePos) && element->GetElementConfig().m_canHover)
		{
			// Assume each UI element has a method GetElementLayer() returning its layer.
			int elementLayer = element->GetElementLayer();
			// If this element's layer is higher than the current top, update.
			if (elementLayer > highestLayer)
			{
				highestLayer = elementLayer;
				topHoveredElement = element;
			}
		}
	}

	if (topHoveredElement)
	{
		if (topHoveredElement->GetElementConfig().m_canHover)
		{
			topHoveredElement->Hover();
		}

		if (m_config.m_inputSystem->IsKeyDown(KEYCODE_LEFT_MOUSE) && topHoveredElement->GetElementConfig().m_canClick)
		{
			topHoveredElement->Click();
		}
	}
}

void UISystem::AddElement(UIElementBase* element)
{
	for (int i = 0; i < (int)m_elements.size(); i++)
	{
		if (m_elements[i] == nullptr)
		{
			m_elements[i] = element;
			return;
		}
	}

	if (element)
	{
		m_elements.push_back(element);
	}

}

void UISystem::RemoveElement(UIElementBase* element)
{
	for (int i = 0; i < (int)m_elements.size(); i++)
	{
		if (m_elements[i] == element)
		{
			delete m_elements[i];
			m_elements[i] = nullptr;
			return;
		}
	}
}

void UISystem::RemoveElement(std::string const& elementName)
{
	for (int i = 0; i < (int)m_elements.size(); i++)
	{
		if (m_elements[i]->GetElementName() == elementName)
		{
			delete m_elements[i];
			m_elements[i] = nullptr;
			return;
		}
	}

}
