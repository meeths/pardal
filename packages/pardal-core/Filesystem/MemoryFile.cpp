
#include <Filesystem/MemoryFile.h>
#include <Math/Functions.h>
// Created on 2019-01-10 by fmacias

namespace pdl
{
	void MemoryFile::Open(FileConstants::OpenMode openMode)
	{
		m_IsOpen = true;
		m_OpenMode = openMode;
	}

	bool MemoryFile::IsOpen()
	{
		return m_IsOpen;
	}

	void MemoryFile::Close()
	{
		m_IsOpen = false;
	}

	unsigned long MemoryFile::Read(void * buffer, unsigned long size, unsigned long elementSize)
	{
		if(!(m_OpenMode & (int)FileConstants::OpenMode::Read))
			return 0;

		auto readSize = Math::Min(size * elementSize, (unsigned long)m_Buffer.size() - m_Cursor);

		if(readSize <= 0)
			return 0;

		std::memcpy(buffer, m_Buffer.data(), readSize);
		m_Cursor += readSize;
		return readSize;
	}

	unsigned long MemoryFile::Write(const void * buffer, unsigned long size, unsigned long elementSize)
	{
		if((static_cast<int>(m_OpenMode) & static_cast<int>(FileConstants::OpenMode::Write)) == 0 )
			return 0;

		if(m_OpenMode & FileConstants::OpenMode::Append)
			m_Cursor = (unsigned long) m_Buffer.size();

		auto writeSize = size * elementSize;
		if(writeSize + m_Cursor > m_Buffer.size())
			m_Buffer.resize(writeSize + m_Cursor);
		std::memcpy(m_Buffer.data(), buffer, writeSize);
		m_Cursor = m_Cursor + writeSize;
		return writeSize;
	}

	void MemoryFile::SetPosition(unsigned long position, FileConstants::PositionOrigin origin)
	{
		switch(origin)
		{
		case FileConstants::PositionOrigin::Start:
			m_Cursor = position;
			break;
		case FileConstants::PositionOrigin::End:
			m_Cursor = (unsigned long) m_Buffer.size() - position;
			break;
		case FileConstants::PositionOrigin::Current:
			m_Cursor = m_Cursor + position;
			break;
		}
	}
}

