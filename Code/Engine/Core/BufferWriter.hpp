#pragma once
#include "Engine/Core/EngineCommon.hpp"

struct IntVec3;
struct Plane2;

class BufferWriter 
{
public:
	BufferWriter(std::vector<unsigned char>& buffer, eBufferEndian endian = eBufferEndian::NATIVE);

	template<typename T>
	void AppendPrimitive(T value)
	{
		unsigned char bytes[sizeof(T)];

		if (m_endian == eBufferEndian::LITTLE)
		{
			for (size_t i = 0; i < sizeof(T); i++)
			{
				bytes[i] = static_cast<unsigned char>((value >> (8 * i)) & 0xFF);
			}
		}
		else
		{
			for (size_t i = 0; i < sizeof(T); i++)
			{
				bytes[sizeof(T) - 1 - i] = static_cast<unsigned char>((value >> (8 * i)) & 0xFF);
			}
		}

		m_buffer.insert(m_buffer.end(), bytes, bytes + sizeof(T));
	}

	template<>
	void AppendPrimitive<float>(float value)
	{
		uint32_t intValue;
		std::memcpy(&intValue, &value, sizeof(float));
		AppendPrimitive<uint32_t>(intValue);
	}
	
	template<>
	void AppendPrimitive<double>(double value)
	{
		uint64_t intValue;
		std::memcpy(&intValue, &value, sizeof(double));
		AppendPrimitive<uint64_t>(intValue);
	}
	
	void AppendVec2(Vec2 const& vct);
	void AppendAABB2(AABB2 const& box);

	uint32_t GetBufferSize() { return (uint32_t)m_buffer.size(); }

	void AppendZeroTerminatedString(const std::string& str);
	void AppendLengthPrecededString(const std::string& str);

	void OverwriteUInt32(size_t offset, uint32_t value);
	void SetEndianMode( eBufferEndian endianMode );

	std::vector<unsigned char>& m_buffer;
	eBufferEndian m_endian;
};

