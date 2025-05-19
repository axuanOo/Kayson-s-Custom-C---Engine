#include "HashedCaseInsensitiveString.hpp"

HashedCaseInsensitiveString::HashedCaseInsensitiveString(const HashedCaseInsensitiveString& rhs)
{
	m_caseIntactText = rhs.m_caseIntactText;
	m_lowerCaseHash = rhs.GetHash();
}

HashedCaseInsensitiveString::HashedCaseInsensitiveString(char const* text)
{
	m_caseIntactText = std::string(text);
	m_lowerCaseHash = GetHashCode(m_caseIntactText);
}

HashedCaseInsensitiveString::HashedCaseInsensitiveString(std::string const& text)
{
	m_caseIntactText = text;
	m_lowerCaseHash = GetHashCode(m_caseIntactText);
}

unsigned int HashedCaseInsensitiveString::GetHashCode(std::string const& originalString)
{
	const char* c = &originalString[0];

	if (c == nullptr)
	{
		return 0;
	}
	
	unsigned int hashResult = 1;

	while (*c != '\0')
	{
		hashResult *= 31;
		hashResult += (unsigned int) tolower(*c);
		c++;
	}

	return hashResult;
}

bool HashedCaseInsensitiveString::operator<(HashedCaseInsensitiveString const& compare) const
{
	if (m_lowerCaseHash != compare.m_lowerCaseHash)
	{
		return m_lowerCaseHash < compare.m_lowerCaseHash;
	}

	return false;
}

bool HashedCaseInsensitiveString::operator==(HashedCaseInsensitiveString const& compare) const
{
	if (m_lowerCaseHash != compare.m_lowerCaseHash)
	{
		return false;
	}

	return _stricmp(m_caseIntactText.c_str(), compare.m_caseIntactText.c_str()) == 0;
}

bool HashedCaseInsensitiveString::operator!=(HashedCaseInsensitiveString const& compare) const
{
	if (m_lowerCaseHash != compare.m_lowerCaseHash)
	{
		return true;
	}

	return _stricmp(m_caseIntactText.c_str(), compare.m_caseIntactText.c_str()) != 0;
}

bool HashedCaseInsensitiveString::operator==(char const* text) const
{
	unsigned int hashNumber = HashedCaseInsensitiveString::GetHashCode(std::string(text));

	if (hashNumber == m_lowerCaseHash)
	{
		return true;
	}

	return _stricmp(m_caseIntactText.c_str(), text) == 0;
}

bool HashedCaseInsensitiveString::operator!=(char const* text) const
{
	unsigned int hashNumber = HashedCaseInsensitiveString::GetHashCode(std::string(text));

	if (hashNumber != m_lowerCaseHash)
	{
		return true;
	}

	return _stricmp(m_caseIntactText.c_str(), text) != 0;
}

bool HashedCaseInsensitiveString::operator==(std::string const& text) const
{
	unsigned int hashNumber = HashedCaseInsensitiveString::GetHashCode(text);

	if (hashNumber == m_lowerCaseHash)
	{
		return true;
	}

	return _stricmp(m_caseIntactText.c_str(), text.c_str()) == 0;
}

bool HashedCaseInsensitiveString::operator!=(std::string const& text) const
{
	unsigned int hashNumber = HashedCaseInsensitiveString::GetHashCode(text);

	if (hashNumber != m_lowerCaseHash)
	{
		return true;
	}

	return _stricmp(m_caseIntactText.c_str(), text.c_str()) != 0;
}

void HashedCaseInsensitiveString::operator=(HashedCaseInsensitiveString const& assignFrom)
{
	m_caseIntactText = assignFrom.m_caseIntactText;
	m_lowerCaseHash = assignFrom.m_lowerCaseHash;
}

void HashedCaseInsensitiveString::operator=(char const* text)
{
	m_caseIntactText = std::string(text);
	m_lowerCaseHash = HashedCaseInsensitiveString::GetHashCode(m_caseIntactText);
}

void HashedCaseInsensitiveString::operator=(std::string const& text)
{
	m_caseIntactText = text;
	m_lowerCaseHash = HashedCaseInsensitiveString::GetHashCode(m_caseIntactText);
}
