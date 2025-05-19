#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include <stdarg.h>
#include <cctype>
//-----------------------------------------------------------------------------------------------
constexpr int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;
//-----------------------------------------------------------------------------------------------
const std::string Stringf( char const* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}
//-----------------------------------------------------------------------------------------------
const std::string Stringf( int maxLength, char const* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

Strings SplitStringOnDelimiter(std::string const& originalString, char delimiterToSplitOn)
{
	Strings result;

	std::string subStringTemporary;

	char currentChar;

	for (int charIndex = 0; charIndex < originalString.length(); charIndex++)
	{
		currentChar = originalString[charIndex];

		if (currentChar != delimiterToSplitOn)
		{
			subStringTemporary.append(1, currentChar);
			if (charIndex == originalString.length() - 1)
			{
				result.push_back(subStringTemporary);
				subStringTemporary.clear();
			}
		}
		else 
		{
			result.push_back(subStringTemporary);
			subStringTemporary.clear();
		}
	}

	if (originalString.length() == 0 || originalString[originalString.length() - 1] == delimiterToSplitOn)
	{
		subStringTemporary.clear();
		result.push_back(subStringTemporary);
	}

	return result;
}

Strings SplitStringOnDelimiter(std::string const& originalString, std::string const& delimiterToSplitOn, bool removeEmpty /*= false*/)
{
	Strings strings;
	size_t start = 0;
	size_t end = originalString.find(delimiterToSplitOn);

	while (end != std::string::npos)
	{
		if (removeEmpty)
		{
			std::string subString = originalString.substr(start, end - start);

			bool emptyString = false;

			if (subString.size() == 0)
			{
				emptyString = true;
			}

			for (char c: subString)
			{
				if (c != ' ')
				{
					emptyString = false;
					continue;
				}
				emptyString = true;
			}

			if (emptyString == false)
			{
				strings.push_back(originalString.substr(start, end - start));
			}

			start = end + delimiterToSplitOn.size();
			end = originalString.find(delimiterToSplitOn, start);
		}
		else 
		{
			strings.push_back(originalString.substr(start, end - start));
			start = end + delimiterToSplitOn.size();
			end = originalString.find(delimiterToSplitOn, start);
		}
	}

	strings.push_back(originalString.substr(start));

	return strings;
}

std::string ToLower(const std::string& string)
{
	std::string result = string;
	for (char& c : result) {
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	return result;
}

void TrimString(std::string& originalString, char delimiterToTrim)
{
	originalString.erase(0, originalString.find_first_not_of(delimiterToTrim));
	originalString.erase(originalString.find_last_not_of(delimiterToTrim) + 1);
}

std::string GetStringWithQuotes(const std::string& originalString)
{
	std::string extractedContent;
	bool insideQuotes = false;

	for (size_t i = 0; i < originalString.size(); ++i)
	{
		char currentChar = originalString[i];

		if (currentChar == '\"')
		{
			insideQuotes = !insideQuotes; 
		}
		else if (insideQuotes)
		{
			extractedContent += currentChar; 
		}
	}

	if (extractedContent.empty())
	{
		extractedContent = originalString;
	}

	return extractedContent;
}

EventArgs SplitStringInQuotationMarks(std::string originalString)
{
	EventArgs result;

	bool isInsideQute = false;
	bool isAfterEqualMark = false;

	// First step: Remove the first element in this string by find the space
	size_t pos = originalString.find(' ');
	std::string eventName = originalString.substr(0, pos);
	result.SetValue(std::string("QixuanLiangSpecializeEventName"),eventName);
	originalString.erase(originalString.begin(), originalString.begin() + pos + 1);

	// Second step: Parse the remaining parts to get variable stored in the args table
	// If current char is double quote, it will enter the quoting status
	std::string currentArgsName = std::string();
	std::string currentValue = std::string();

	for (int i = 0; i < (int)originalString.size(); i ++)
	{
		char const& c = originalString[i];
		if (c == '"')
		{
			isInsideQute = !isInsideQute;
			continue;
		}
		
		if (isInsideQute)
		{
			currentValue.push_back(c);
		}
		else 
		{
			if (c == ' ')
			{
				result.SetValue(currentArgsName, currentValue);
				isAfterEqualMark = false;
				currentArgsName.clear();
				currentValue.clear();
			}
			else if (c == '=')
			{
				isAfterEqualMark = true;
			}
			else 
			{
				if (isAfterEqualMark == false)
				{
					currentArgsName.push_back(c);
				}
				else 
				{
					currentValue.push_back(c);
				}
			}
		}
	}

	result.SetValue(currentArgsName, currentValue);

	return result;
}

