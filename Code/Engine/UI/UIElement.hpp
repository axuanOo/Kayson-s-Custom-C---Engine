#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include <unordered_map>

class Renderer;
class Texture;

struct UIElementConfig 
{
	HashedCaseInsensitiveString 	m_elementName;
	AABB2							m_elementBox;
	unsigned int					m_layer = 0;  // Higher number means near screen
	bool							m_canHover = true;
	bool							m_canClick = true;
	Texture*						m_texture = nullptr;
	AABB2							m_uv = AABB2::ZERO_TO_ONE;
	Rgba8							m_defaultColor = Rgba8::GREY;
	Rgba8							m_highlightedColor = Rgba8::WHITE;
	Rgba8							m_clickColor = Rgba8::RED;
};

class UIElementBase 
{
public:

	UIElementBase(UIElementConfig const& config, UIElementBase* parent = nullptr);
	UIElementBase(std::string const& elementName, Vec2 const& center, Vec2 const& dimension, UIElementBase* parent = nullptr);

	virtual ~UIElementBase() {}

	virtual void					Update(float deltaTime);
	virtual void					Draw(Renderer& renderer);
	virtual void					Click();
	virtual void					Hover();

	template<typename T>
	void							OnClick(T* object, bool (T::* callback)(const EventArgs&));

	template<typename T>
	void							OnHover(T* object, bool (T::* callback)(const EventArgs&));

	void							AddChild(UIElementBase* child);
	void							RemoveChild(UIElementBase* child);

	template<typename T>
	void							AddClickEventArgs(std::string const& clickedName, T value);

	UIElementConfig					GetElementConfig()		const { return m_elementConfig; }
	HashedCaseInsensitiveString		GetElementName()		const { return m_elementConfig.m_elementName;}
	AABB2							GetElementBoudingBox()	const { return m_elementConfig.m_elementBox; }
	int								GetElementLayer()		const { return m_elementConfig.m_layer;}

	UIElementBase&					SetElementName(std::string const& name);
	UIElementBase&					SetElementBoxCenter(Vec2 const& center);
	UIElementBase&					SetElementBoxDimension(Vec2 const& dimension);
	UIElementBase&					SetElementLayer(unsigned int layer);
	UIElementBase&					SetElementTexture(Texture* texture);
	UIElementBase&					SetElementClickable(bool canbeClick);
	UIElementBase&					SetElementHoverable(bool canbeHover);
	UIElementBase&					SetElementColor(Rgba8 color);

protected:
	static const char* const CLICK_PREFIX;
	static const char* const HOVER_PREFIX;

	UIElementConfig					m_elementConfig;
	Rgba8							m_color;

	UIElementBase*					m_parent;
	std::map<HashedCaseInsensitiveString, UIElementBase*>		m_children;
	EventArgs						m_clickEventArgs;
	EventArgs						m_hoverEventArgs;
};

template<typename T>
void UIElementBase::OnClick(T* object, bool (T::* callback)(const EventArgs&))
{
	HashedCaseInsensitiveString elementClickHash = HashedCaseInsensitiveString(m_elementConfig.m_elementName.GetOriginalString() + (CLICK_PREFIX));
	g_theEventSystem->SubscribeEventCallbackFunction(elementClickHash, object, callback);
}

template<typename T>
void UIElementBase::OnHover(T* object, bool (T::* callback)(const EventArgs&))
{
	HashedCaseInsensitiveString elementHoverHash = HashedCaseInsensitiveString(m_elementConfig.m_elementName.GetOriginalString() + (HOVER_PREFIX));
	g_theEventSystem->SubscribeEventCallbackFunction(elementHoverHash, object, callback);
}

template<typename T>
void UIElementBase::AddClickEventArgs(std::string const& clickedName, T value)
{
	m_clickEventArgs.SetValue(clickedName,value);
}
