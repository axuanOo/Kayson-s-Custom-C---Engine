#include "Engine/Core/BufferWriter.hpp"

BufferWriter::BufferWriter(std::vector<unsigned char>& buffer, eBufferEndian endian)
	:m_buffer(buffer),m_endian(endian)
{
	if (m_endian == eBufferEndian::NATIVE)
	{
		unsigned int value = 1;
		unsigned char* bytePtr = reinterpret_cast<unsigned char*>(&value);

		if (bytePtr[0] == 1)
		{
			m_endian = eBufferEndian::LITTLE;
		}
		else
		{
			m_endian = eBufferEndian::BIG;
		}
	}
}

void BufferWriter::AppendVec2(Vec2 const& vct)
{
	AppendPrimitive(vct.x);
	AppendPrimitive(vct.y);
}

void BufferWriter::AppendAABB2(AABB2 const& box)
{
	AppendVec2(box.m_mins);
	AppendVec2(box.m_maxs);
}

void BufferWriter::AppendZeroTerminatedString(const std::string& str)
{
	std::string strWithZeroTerm = str;
	strWithZeroTerm.push_back('\0');

	m_buffer.insert(m_buffer.end(), strWithZeroTerm.data(), strWithZeroTerm.data() + strWithZeroTerm.size());
}

void BufferWriter::AppendLengthPrecededString(const std::string& str)
{
	AppendPrimitive<uint32_t>(static_cast<uint32_t>(str.size()));
	m_buffer.insert(m_buffer.end(), str.begin(), str.end());
}

void BufferWriter::OverwriteUInt32(size_t offset, uint32_t value)
{
	if (offset + 4 > m_buffer.size())
		return;

	for (int i = 0; i < 4; i++)
	{
		int index = (m_endian == eBufferEndian::LITTLE) ? i : (3 - i);
		m_buffer[offset + i] = static_cast<unsigned char>((value >> (8 * index)) & 0xFF);
	}
}

void BufferWriter::SetEndianMode(eBufferEndian endianMode)
{
	m_endian = endianMode;
	if (m_endian == eBufferEndian::NATIVE)
	{
		unsigned int value = 1;
		unsigned char* bytePtr = reinterpret_cast<unsigned char*>(&value);

		if (bytePtr[0] == 1)
		{
			m_endian = eBufferEndian::LITTLE;
		}
		else
		{
			m_endian = eBufferEndian::BIG;
		}
	}

}
