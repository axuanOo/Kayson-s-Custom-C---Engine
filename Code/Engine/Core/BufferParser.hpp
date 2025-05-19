#pragma once
#include <vector>
#include <string>
#include "Engine/Core/EngineCommon.hpp"

class BufferParser
{
public:
	BufferParser(const void* data, size_t size, eBufferEndian endian = eBufferEndian::NATIVE);
	BufferParser(const std::vector<unsigned char>& data, eBufferEndian endian = eBufferEndian::NATIVE);

	char			ParseChar();
	unsigned char	ParseByte();
	unsigned short	ParseUShort();
	bool			ParseBool();
	unsigned int	ParseUint32();
	uint64_t		ParseUint64();
	int64_t			ParseInt64();
	int				ParseInt32();
	float			ParseFloat();
	double			ParseDouble();

	void			ParseStringZeroTerminated(std::string& out);
	void			ParseStringAfter32BitLength(std::string& out);
	void			SkipBytes(size_t numBytes);

	Rgba8			ParseRgba();
	Rgba8			ParseRgb();
	IntVec2			ParseIntVec2();
	Vec2			ParseVec2();
	AABB2			ParseAABB2();
	Vertex_PCU		ParseVertexPCU();
	eBufferEndian	ParseBufferMode();

	void			SetBufferOffset(size_t bufferOffset);
	void			SetEndianMode(eBufferEndian endian);

public:
	const void*		m_data;
	size_t			m_dataLength;
	eBufferEndian	m_bufferEndian;
	size_t			m_currentOffset = 0;
};
