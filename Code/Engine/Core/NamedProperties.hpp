#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/HashedCaseInsensitiveString.hpp"
#include "Engine/Math/Vec2.hpp"
#include <map>
#include <type_traits>
#include "Engine/Core/XmlUtils.hpp"

template<typename T> 
class Check_Set_From_Text 
{
	typedef char yes[1];
	typedef char no[2];

	template <typename C>
	static yes& test(decltype(&C::SetFromText));

	template <typename C> 
	static no& test(...);

public:

	static constexpr bool isValid() { return sizeof(test<T>(0)) == sizeof(yes);}
};

struct NamedPropertiesValueBase
{
	virtual ~NamedPropertiesValueBase();

	virtual void SignalForAbstractClass() {};
};

template<typename T>
struct NamedPropertiesValue : NamedPropertiesValueBase 
{
	NamedPropertiesValue(T const& value) : m_value(value) {};
	virtual ~NamedPropertiesValue() {};

	T m_value;
};


class NamedProperties 
{
public:
	NamedProperties() = default;
	virtual ~NamedProperties();

	template<typename T> 
	void 	SetValue(std::string const& keyName, T const& value);
	template<typename T>
	void	SetValue(HashedCaseInsensitiveString const& keyName, T const& value);

	void	SetValue(std::string const& keyName, const char* value);

	template<typename T>
	T		GetValue(std::string const& keyName, T const& defaultValue) const;
	template<typename T>
	T		GetValue(HashedCaseInsensitiveString const& keyName, T const& defaultValue) const;

	std::string GetValue(std::string const& keyName, std::string const& defaultValue) const;

	void			PopulateFromXmlElementAttributes(XmlElement const& element);
public:
	std::map<HashedCaseInsensitiveString,NamedPropertiesValueBase*> m_keyValuePairs;
};

template<typename T>
void NamedProperties::SetValue(std::string const& keyName, T const& value)
{
	HashedCaseInsensitiveString hcis = HashedCaseInsensitiveString(keyName);

	auto it = m_keyValuePairs.find(hcis);
	if (it != m_keyValuePairs.end())
	{
		delete it->second;
		m_keyValuePairs.erase(it);
	}

	m_keyValuePairs[hcis] = new NamedPropertiesValue<T>(value);
}

template<typename T>
void NamedProperties::SetValue(HashedCaseInsensitiveString const& keyName, T const& value)
{
	auto it = m_keyValuePairs.find(keyName);
	if (it != m_keyValuePairs.end())
	{
		delete it->second;
		m_keyValuePairs.erase(it);
	}

	m_keyValuePairs[keyName] = new NamedPropertiesValue<T>(value);

}

template<typename T>
T NamedProperties::GetValue(HashedCaseInsensitiveString const& keyName, T const& defaultValue) const
{
	auto it = m_keyValuePairs.find(keyName);

	if (it == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	NamedPropertiesValue<T>* valuePtr = dynamic_cast<NamedPropertiesValue<T>*>(it->second);

	if (valuePtr)
	{
		return valuePtr->m_value;
	}

	NamedPropertiesValue<std::string>* stringValueType = dynamic_cast<NamedPropertiesValue<std::string>*>(it->second);

	if (stringValueType)
	{
		std::string value = stringValueType->m_value;

		if constexpr (std::is_same_v<T, bool>)
		{
			if (ToLower(value) == "true")
				return true;
			else if (ToLower(value) == "false")
				return false;
			else
				throw std::runtime_error("Invalid boolean text");
		}
		else if constexpr (std::is_integral_v<T>)
		{
			return atoi(value.c_str());
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			return (float)atof(value.c_str());
		}
		else if constexpr (Check_Set_From_Text<T>::isValid())
		{
			T typeValue;
			typeValue.SetFromText(value.c_str());
			return typeValue;
		}
		else
		{
			return defaultValue;
		}
	}
	else
	{
		return defaultValue;
	}

}

template<typename T>
T NamedProperties::GetValue(std::string const& keyName, T const& defaultValue) const
{
	HashedCaseInsensitiveString hcis = HashedCaseInsensitiveString(keyName);

	auto it = m_keyValuePairs.find(hcis);

	if (it == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	NamedPropertiesValue<T>* valuePtr = dynamic_cast<NamedPropertiesValue<T>*>(it->second);

	if (valuePtr)
	{
		return valuePtr->m_value;
	}

	NamedPropertiesValue<std::string>* stringValueType = dynamic_cast<NamedPropertiesValue<std::string>*>(it->second);

	if (stringValueType)
	{
		std::string value = stringValueType->m_value;
		
		if constexpr (std::is_same_v<T, bool>)
		{
			if (ToLower(value) == "true")
				return true;
			else if (ToLower(value) == "false")
				return false;
			else
				throw std::runtime_error("Invalid boolean text");
		}
		else if constexpr(std::is_integral_v<T>)
		{
			return atoi(value.c_str());
		}
		else if constexpr(std::is_floating_point_v<T>)
		{
			return (float)atof(value.c_str());
		}
		else if constexpr(Check_Set_From_Text<T>::isValid())
		{
			T typeValue;
			typeValue.SetFromText(value.c_str());
			return typeValue;
		}
		else 
		{
			return defaultValue;
		}
	}
	else
	{
		return defaultValue;
	}

}
