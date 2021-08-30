#pragma once

class BufferBase
{
public:
	virtual ~BufferBase() = default;

public:
	virtual void* Map() = 0;
	virtual void Unmap() = 0;
};

template<typename T>
class ScopedBufferLock final
{
public:
	ScopedBufferLock(BufferBase& buffer)
		: m_buffer(buffer)
	{
		m_memory = reinterpret_cast<T*>(m_buffer.Map());
	}

	~ScopedBufferLock()
	{
		m_buffer.Unmap();
	}

public:
	T* Get()
	{
		return m_memory;
	}

	T* operator[](size_t index)
	{
		return m_memory + index;
	}

	T* operator->() const { return Get(); }

private:
	BufferBase& m_buffer;
	T* m_memory;
};
