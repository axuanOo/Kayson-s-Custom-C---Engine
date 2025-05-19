#include "Engine/UI/UIElement.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

const char* const UIElementBase::CLICK_PREFIX = "ON_CLICK";
const char* const UIElementBase::HOVER_PREFIX = "ON_HOVER";

UIElementBase::UIElementBase(UIElementConfig const& config, UIElementBase* parent)
	:m_elementConfig(config), m_parent(parent)
{
	if (parent)
	{
		parent->AddChild(this);
	}
	m_color = config.m_defaultColor;
}

UIElementBase::UIElementBase(std::string const& elementName, Vec2 const& center, Vec2 const& dimension, UIElementBase* parent)
	:m_elementConfig(UIElementConfig()) , m_parent(parent)
{
	m_elementConfig.m_elementName = HashedCaseInsensitiveString(elementName);
	m_elementConfig.m_elementBox = AABB2();
	m_elementConfig.m_elementBox.SetCenter(center);
	m_elementConfig.m_elementBox.SetDimensions(dimension);
	if (parent)
	{
		parent->AddChild(this);
	}

}

void UIElementBase::Update(float deltaTime)
{
	m_color = m_elementConfig.m_defaultColor;

	for (const auto& childPair : m_children)
	{
		if (childPair.second)
		{
			childPair.second->Update(deltaTime);
		}
	}
}

void UIElementBase::Draw(Renderer& renderer)
{
	std::vector<Vertex_PCU> verts;
	AddVertsForAABB2D(verts, m_elementConfig.m_elementBox, m_color, m_elementConfig.m_uv);
	renderer.BindTexture(m_elementConfig.m_texture);
	renderer.BindShader(nullptr);
	renderer.SetModelConstants();
	renderer.DrawVertexArray((int)verts.size(),verts.data());

	for (const auto& childPair : m_children)
	{
		if (childPair.second)
		{
			childPair.second->Draw(renderer);
		}
	}
}

void UIElementBase::Click()
{
	g_theEventSystem->FireEvent(m_elementConfig.m_elementName.GetOriginalString() + (CLICK_PREFIX), m_clickEventArgs);
	m_color = m_elementConfig.m_clickColor;
}

void UIElementBase::Hover()
{
	g_theEventSystem->FireEvent(m_elementConfig.m_elementName.GetOriginalString() + (HOVER_PREFIX), m_hoverEventArgs);
	m_color = m_elementConfig.m_highlightedColor;
}

void UIElementBase::AddChild(UIElementBase* child)
{
	auto it = m_children.find(child->GetElementConfig().m_elementName);

	if (it != m_children.end())
	{
		m_children.erase(it);
	}
	m_children[child->GetElementConfig().m_elementName] = child;
}

void UIElementBase::RemoveChild(UIElementBase* child)
{
	auto it = m_children.find(child->GetElementConfig().m_elementName);

	if (it != m_children.end())
	{
		m_children.erase(it);
	}
}

UIElementBase& UIElementBase::SetElementName(std::string const& name)
{
	m_elementConfig.m_elementName = HashedCaseInsensitiveString(name);
	return *this;
}

UIElementBase& UIElementBase::SetElementBoxCenter(Vec2 const& center)
{
	m_elementConfig.m_elementBox.SetCenter(center);
	return *this;
}

UIElementBase& UIElementBase::SetElementBoxDimension(Vec2 const& dimension)
{
	m_elementConfig.m_elementBox.SetDimensions(dimension);
	return *this;
}

UIElementBase& UIElementBase::SetElementLayer(unsigned int layer)
{
	m_elementConfig.m_layer = layer;
	return *this;
}

UIElementBase& UIElementBase::SetElementTexture(Texture* texture)
{
	m_elementConfig.m_texture = texture;
	return *this;
}

UIElementBase& UIElementBase::SetElementClickable(bool canbeClick)
{
	m_elementConfig.m_canClick = canbeClick;
	return *this;
}

UIElementBase& UIElementBase::SetElementHoverable(bool canbeHover)
{
	m_elementConfig.m_canHover = canbeHover;
	return *this;
}

UIElementBase& UIElementBase::SetElementColor(Rgba8 color)
{
	m_elementConfig.m_defaultColor = color;
	return *this;
}
