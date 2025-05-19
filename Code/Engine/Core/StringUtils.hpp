#pragma once
//-----------------------------------------------------------------------------------------------
#include <vector>
#include <string>

class NamedProperties;
typedef NamedProperties EventArgs;

typedef std::vector<std::string >		Strings; 
//-----------------------------------------------------------------------------------------------
const std::string Stringf( char const* format, ... );
const std::string Stringf( int maxLength, char const* format, ... );

Strings SplitStringOnDelimiter( std::string const& originalString, char delimiterToSplitOn = ',');

// This overload can use multiple delimiter as one, like "'\r\n'"
Strings SplitStringOnDelimiter(std::string const& originalString, std::string const& delimiterToSplitOn, bool removeEmpty = false);

std::string ToLower(const std::string& string);
void		TrimString(std::string& originalString, char delimiterToTrim);
std::string	GetStringWithQuotes(const std::string& originalString);

EventArgs SplitStringInQuotationMarks(std::string originalString);
