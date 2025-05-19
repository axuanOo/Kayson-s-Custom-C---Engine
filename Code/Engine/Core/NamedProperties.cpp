#include "Engine/Core/NamedProperties.hpp"

NamedProperties::~NamedProperties()
{
	for (auto it = m_keyValuePairs.begin(); it != m_keyValuePairs.end(); ++it)
	{
		if (it->second != nullptr)
		{
			delete it->second;
			it->second = nullptr;
		}
	}
}

void NamedProperties::SetValue(std::string const& keyName, const char* value)
{
	SetValue<std::string>(keyName, std::string(value));
}

std::string NamedProperties::GetValue(std::string const& keyName, std::string const& defaultValue) const
{
	return GetValue<std::string>(keyName,defaultValue);
}

void NamedProperties::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	const XmlAttribute* attribute = element.FirstAttribute();

	m_keyValuePairs.clear();

	while (attribute != nullptr)
	{
		SetValue(attribute->Name(), attribute->Value());
		attribute = attribute->Next();
	}

}

NamedPropertiesValueBase::~NamedPropertiesValueBase()
{

}
