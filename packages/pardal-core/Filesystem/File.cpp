#include <Filesystem/File.h>
#include <Base/DebugHelpers.h>

namespace pdl
{
File::~File()
{
	if(File::IsOpen())
	{
		File::Close();
	}
}

void File::Open(FileConstants::OpenMode openMode)
{
	int stdBitmask = 0;
	if(openMode & FileConstants::OpenMode::Read) stdBitmask |= std::ios::in;
	if(openMode & FileConstants::OpenMode::Write) stdBitmask |= std::ios::out;
	if(openMode & FileConstants::OpenMode::Binary) stdBitmask |= std::ios::binary;
	if(openMode & FileConstants::OpenMode::AtEnd) stdBitmask |= std::ios::ate;
	if(openMode & FileConstants::OpenMode::Truncate) stdBitmask |= std::ios::trunc;

	m_Stream.open(m_FileName, stdBitmask);
}

bool File::IsOpen()
{
	return m_Stream.is_open();
}

void File::Close()
{
	m_Stream.close();
}

unsigned long File::Read(void* buffer, unsigned long size, unsigned long elementSize)
{
	pdlAssert(IsOpen());

	m_Stream.read(static_cast<char*>(buffer), size * elementSize);
	if(m_Stream)
		return size * elementSize;
	// Could not read the requested number of bytes
	return static_cast<unsigned long>(m_Stream.gcount());
}

unsigned long File::Write(const void* buffer, unsigned long size, unsigned long elementSize)
{
	pdlAssert(IsOpen());

	m_Stream.write(static_cast<const char*>(buffer), size * elementSize);
	return size * elementSize;
}

	unsigned long File::GetPosition()
{
	pdlAssert(IsOpen());
	return static_cast<unsigned long>(m_Stream.tellg());
}

void File::SetPosition(unsigned long position, FileConstants::PositionOrigin origin)
{
	std::ios::seekdir seekDir;
	switch(origin) 
	{
		case FileConstants::PositionOrigin::End: 
			seekDir = std::ios::end; break;
		case FileConstants::PositionOrigin::Current: 
			seekDir = std::ios::cur; break;
		case FileConstants::PositionOrigin::Start:
		default:  
			seekDir = std::ios::beg; break;
	}

	m_Stream.seekg(position, seekDir);
}
}
