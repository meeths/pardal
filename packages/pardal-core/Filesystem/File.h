#pragma once
#include <Base/BaseDefines.h>
#include <Filesystem/IFile.h>
#include <String/String.h>
#include <fstream>

namespace pdl
{

class File : public IFile
{
public:
	explicit File(StringView filename) : m_FileName(filename) {}
	DeclareNonCopyable(File);
	DeclareDefaultMoveable(File);


	~File() override;
	void Open(FileConstants::OpenMode openMode) override;
	bool IsOpen() override;
	void Close() override;
	unsigned long Read(void* buffer, unsigned long size, unsigned long elementSize) override;
	unsigned long Write(const void* buffer, unsigned long size, unsigned long elementSize) override;
	unsigned long GetPosition() override;
	void SetPosition(unsigned long position, FileConstants::PositionOrigin origin) override;

	std::fstream& GetStream() { return m_Stream; }
private:
	std::fstream m_Stream;
	String m_FileName;

};

}
