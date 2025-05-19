#include "Engine/Core/BufferParser.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "ErrorWarningAssert.hpp"

BufferParser::BufferParser(const void* data, size_t size, eBufferEndian endian /*= eBufferEndian::NATIVE*/)
	:m_data(data),m_dataLength(size),m_bufferEndian(endian)
{
	if (endian == eBufferEndian::NATIVE)
	{
		unsigned int value = 1;   
		unsigned char* bytePtr = reinterpret_cast<unsigned char*>(&value);
		
		if (bytePtr[0] == 1)
		{
			m_bufferEndian = eBufferEndian::LITTLE;
		}
		else 
		{
			m_bufferEndian = eBufferEndian::BIG;
		}
	}
}

BufferParser::BufferParser(const std::vector<unsigned char>& data, eBufferEndian endian /*= eBufferEndian::NATIVE*/)
	:BufferParser(data.data(),data.size(),endian)
{

}

char BufferParser::ParseChar()
{
	if (m_currentOffset > m_dataLength)
	{
		ERROR_AND_DIE("Over take the offset from the beginning of the byte array");
	}
	const unsigned char* value = static_cast<const unsigned char*>(m_data);
	char c = static_cast<char>(value[m_currentOffset]);
	++m_currentOffset;
	return c;
}

unsigned char BufferParser::ParseByte()
{
	if (m_currentOffset > m_dataLength)
	{
		ERROR_AND_DIE("Over take the offset from the beginning of the byte array");
	}
	const unsigned char* value = static_cast<const unsigned char*>(m_data);
	unsigned char b = value[m_currentOffset];
	++m_currentOffset;
	return b;
}

unsigned short BufferParser::ParseUShort()
{
	unsigned char b[2];
	b[0] = ParseByte();
	b[1] = ParseByte();
	unsigned short value = 0;
	if (m_bufferEndian == eBufferEndian::LITTLE)
	{
		value = static_cast<unsigned short>(b[0]) | (static_cast<unsigned short>(b[1]) << 8);
	}
	else
	{
		value = (static_cast<unsigned short>(b[0]) << 8) | static_cast<unsigned short>(b[1]);
	}
	return value;
}

bool BufferParser::ParseBool()
{
	if (m_currentOffset > m_dataLength)
	{
		ERROR_AND_DIE("Over take the offset from the beginning of the byte array");
	}

	unsigned char c = ParseByte();

	if (c == 0)
	{
		return false;
	}

	return true;
}

unsigned int BufferParser::ParseUint32()
{
	unsigned char b[4];

	b[0] = ParseByte();
	b[1] = ParseByte();
	b[2] = ParseByte();
	b[3] = ParseByte();

	if (m_bufferEndian == eBufferEndian::LITTLE)
	{
		return  static_cast<unsigned int>(b[0])
			| (static_cast<unsigned int>(b[1]) << 8)
			| (static_cast<unsigned int>(b[2]) << 16)
			| (static_cast<unsigned int>(b[3]) << 24);
	}

	return  (static_cast<unsigned int>(b[0]) << 24) | (static_cast<unsigned int>(b[1]) << 16) | (static_cast<unsigned int>(b[2]) << 8)| static_cast<unsigned int>(b[3]);
}

uint64_t BufferParser::ParseUint64()
{
	unsigned char b[8];
	for (int i = 0; i < 8; ++i)
	{
		b[i] = ParseByte();
	}
	uint64_t value = 0;
	if (m_bufferEndian == eBufferEndian::LITTLE)
	{
		value = static_cast<uint64_t>(b[0])
			| (static_cast<uint64_t>(b[1]) << 8)
			| (static_cast<uint64_t>(b[2]) << 16)
			| (static_cast<uint64_t>(b[3]) << 24)
			| (static_cast<uint64_t>(b[4]) << 32)
			| (static_cast<uint64_t>(b[5]) << 40)
			| (static_cast<uint64_t>(b[6]) << 48)
			| (static_cast<uint64_t>(b[7]) << 56);
	}
	else
	{
		value = (static_cast<uint64_t>(b[0]) << 56)
			| (static_cast<uint64_t>(b[1]) << 48)
			| (static_cast<uint64_t>(b[2]) << 40)
			| (static_cast<uint64_t>(b[3]) << 32)
			| (static_cast<uint64_t>(b[4]) << 24)
			| (static_cast<uint64_t>(b[5]) << 16)
			| (static_cast<uint64_t>(b[6]) << 8)
			| static_cast<uint64_t>(b[7]);
	}
	return value;
}

int64_t BufferParser::ParseInt64()
{
	unsigned char b[8];
	for (int i = 0; i < 8; ++i)
	{
		b[i] = ParseByte();
	}
	uint64_t unsignedValue = 0;
	if (m_bufferEndian == eBufferEndian::LITTLE)
	{
		unsignedValue = static_cast<uint64_t>(b[0])
			| (static_cast<uint64_t>(b[1]) << 8)
			| (static_cast<uint64_t>(b[2]) << 16)
			| (static_cast<uint64_t>(b[3]) << 24)
			| (static_cast<uint64_t>(b[4]) << 32)
			| (static_cast<uint64_t>(b[5]) << 40)
			| (static_cast<uint64_t>(b[6]) << 48)
			| (static_cast<uint64_t>(b[7]) << 56);
	}
	else
	{
		unsignedValue = (static_cast<uint64_t>(b[0]) << 56)
			| (static_cast<uint64_t>(b[1]) << 48)
			| (static_cast<uint64_t>(b[2]) << 40)
			| (static_cast<uint64_t>(b[3]) << 32)
			| (static_cast<uint64_t>(b[4]) << 24)
			| (static_cast<uint64_t>(b[5]) << 16)
			| (static_cast<uint64_t>(b[6]) << 8)
			| static_cast<uint64_t>(b[7]);
	}
	return static_cast<int64_t>(unsignedValue);
}

int BufferParser::ParseInt32()
{
	unsigned char b[4];

	b[0] = ParseByte();
	b[1] = ParseByte();
	b[2] = ParseByte();
	b[3] = ParseByte();

	if (m_bufferEndian == eBufferEndian::LITTLE)
	{
		return  static_cast<int>(b[0])
			| (static_cast<int>(b[1]) << 8)
			| (static_cast<int>(b[2]) << 16)
			| (static_cast<int>(b[3]) << 24);
	}

	return  (static_cast<int>(b[0]) << 24) | (static_cast<int>(b[1]) << 16) | (static_cast<int>(b[2]) << 8) | static_cast<int>(b[3]);
}

float BufferParser::ParseFloat()
{
	unsigned char b[4];

	b[0] = ParseByte();
	b[1] = ParseByte();
	b[2] = ParseByte();
	b[3] = ParseByte();

	unsigned int value = 0;
	if (m_bufferEndian == eBufferEndian::LITTLE)
	{
		value = static_cast<unsigned int>(b[0])
			| (static_cast<unsigned int>(b[1]) << 8)
			| (static_cast<unsigned int>(b[2]) << 16)
			| (static_cast<unsigned int>(b[3]) << 24);
	}
	else 
	{
		value = (static_cast<unsigned int>(b[0]) << 24)
			| (static_cast<unsigned int>(b[1]) << 16)
			| (static_cast<unsigned int>(b[2]) << 8)
			| static_cast<unsigned int>(b[3]);
	}

	float result = *reinterpret_cast<float*>(&value);
	return result;
}

double BufferParser::ParseDouble()
{
	unsigned char b[8];
	b[0] = ParseByte();
	b[1] = ParseByte();
	b[2] = ParseByte();
	b[3] = ParseByte();
	b[4] = ParseByte();
	b[5] = ParseByte();
	b[6] = ParseByte();
	b[7] = ParseByte();

	unsigned long long value = 0;
	if (m_bufferEndian == eBufferEndian::LITTLE)
	{
		value = static_cast<unsigned long long>(b[0])
			| (static_cast<unsigned long long>(b[1]) << 8)
			| (static_cast<unsigned long long>(b[2]) << 16)
			| (static_cast<unsigned long long>(b[3]) << 24)
			| (static_cast<unsigned long long>(b[4]) << 32)
			| (static_cast<unsigned long long>(b[5]) << 40)
			| (static_cast<unsigned long long>(b[6]) << 48)
			| (static_cast<unsigned long long>(b[7]) << 56);
	}
	else  
	{
		value = (static_cast<unsigned long long>(b[0]) << 56)
			| (static_cast<unsigned long long>(b[1]) << 48)
			| (static_cast<unsigned long long>(b[2]) << 40)
			| (static_cast<unsigned long long>(b[3]) << 32)
			| (static_cast<unsigned long long>(b[4]) << 24)
			| (static_cast<unsigned long long>(b[5]) << 16)
			| (static_cast<unsigned long long>(b[6]) << 8)
			| static_cast<unsigned long long>(b[7]);
	}

	double result;
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

void BufferParser::ParseStringZeroTerminated(std::string& out)
{
	char c = ParseChar();

	while (c != '\0')
	{
		out.push_back(c);
		c = ParseChar();
	}
}

void BufferParser::ParseStringAfter32BitLength(std::string& out)
{
	unsigned int l = ParseUint32();
	
	for (unsigned int i = 0; i < l; i++)
	{
		char c = ParseChar();
		out.push_back(c);
	}
}

void BufferParser::SkipBytes(size_t numBytes)
{
	
	GUARANTEE_OR_DIE(
		m_currentOffset + numBytes <= m_dataLength,
		Stringf("BufferParser::SkipBytes out of range (offset=%zu + skip=%zu > length=%zu)",
			m_currentOffset, numBytes, m_dataLength)
	);

	m_currentOffset += numBytes;


}

Rgba8 BufferParser::ParseRgba()
{
	unsigned char r = ParseByte();
	unsigned char g = ParseByte();
	unsigned char b = ParseByte();
	unsigned char a = ParseByte();

	return Rgba8(r,g,b,a);
}

Rgba8 BufferParser::ParseRgb()
{
	unsigned char r = ParseByte();
	unsigned char g = ParseByte();
	unsigned char b = ParseByte();

	return Rgba8(r,g,b);
}

IntVec2 BufferParser::ParseIntVec2()
{
	int x = ParseInt32();
	int y = ParseInt32();

	return IntVec2(x,y);
}

Vec2 BufferParser::ParseVec2()
{
	float x = ParseFloat();
	float y = ParseFloat();

	return Vec2(x,y);
}

AABB2 BufferParser::ParseAABB2()
{
	Vec2 minXY = ParseVec2();
	Vec2 maxXY = ParseVec2();

	return AABB2(minXY, maxXY);
}

Vertex_PCU BufferParser::ParseVertexPCU()
{
	float x = ParseFloat();
	float y = ParseFloat();
	float z = ParseFloat();

	Rgba8 color = ParseRgba();

	Vec2 uv = ParseVec2();

	return Vertex_PCU(Vec3(x, y, z), color, uv);
}

eBufferEndian BufferParser::ParseBufferMode()
{
	unsigned char b = ParseByte();

	return static_cast<eBufferEndian>(b);
}

void BufferParser::SetBufferOffset(size_t bufferOffset)
{
	m_currentOffset = bufferOffset;
}

void BufferParser::SetEndianMode(eBufferEndian endian)
{
	m_bufferEndian = endian;
}
