#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <algorithm>
#include <utility>

class UIElementBase;
class InputSystem;

struct UISystemConfig 
{
	Vec2		m_screenSize;
	InputSystem* m_inputSystem = nullptr;
};

class UISystem 
{
public:

	UISystem(UISystemConfig const& config)
		: m_config(config)
	{}
	~UISystem();

	template<typename T, typename... Args>
	T* CreateElement(Args&&... args)
	{
		T* element = new T(std::forward<Args>(args)...);
		AddElement(element);
		return element;
	}

	void ProcessMouseInput();

	const std::vector<UIElementBase*>& GetElements() const { return m_elements; }

private:
	void AddElement(UIElementBase* element);
	void RemoveElement(UIElementBase* element);
	void RemoveElement(std::string const& elementName);
private:
	UISystemConfig m_config;
	std::vector<UIElementBase*> m_elements;
};