#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <map>

class NamedStrings 
{
public:
	void			PopulateFromXmlElementAttributes(XmlElement const& element);
	void			SetValue(std::string const& keyName, std::string const& newValue);
	std::string		GetValue(std::string const& keyName, std::string const& defaultValue) const;
	bool			GetValue(std::string const& keyName, bool defaultValue) const;
	int				GetValue(std::string const& keyName, int defaultValue) const;
	float			GetValue(std::string const& keyName, float defaultValue) const;
	std::string		GetValue(std::string const& keyName, char const* defaultValue) const;
	Rgba8			GetValue(std::string const& keyName, Rgba8 const& defaultValue) const;
	Vec2			GetValue(std::string const& keyName, Vec2 const& defaultValue) const;
	Vec3			GetValue(std::string const& keyName, Vec3 const& defaultValue) const;
	IntVec2			GetValue(std::string const& keyName, IntVec2 const& defaultValue) const;
	EulerAngles		GetValue(std::string const& keyName, EulerAngles const& defaultValue) const;
	FloatRange		GetValue(std::string const& keyName, FloatRange const& defaultValue) const;
	char			GetValue(std::string const& keyName, char const& defaultValue) const;
private:
	std::map< std::string, std::string >	m_keyValuePairs;
};