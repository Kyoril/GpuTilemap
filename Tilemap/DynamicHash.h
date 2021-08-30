#pragma once

#include <cstdint>

class DynamicHash final
{
public:
	void Add64(uint64_t value)
	{
		m_hash ^= value + 0x9e3779b9 + (value << 6) + (value >> 2);
	}

	void Add32(uint32_t value)
	{
		Add64(static_cast<size_t>(value));
	}

	void AddFloat(float value)
	{
		Add32(*reinterpret_cast<uint32_t*>(&value));
	}

	operator size_t() const
	{
		return m_hash;
	}

private:
	size_t m_hash = 0;
};