#pragma once
#include <Filesystem/IFile.h>
#include <Base/BaseDefines.h>
#include <Containers/Vector.h>
// Created on 2019-01-10 by fmacias

namespace pdl
{

class MemoryFile : public IFile
{
public:
    explicit MemoryFile(Vector<char>& _buffer) : m_Buffer(_buffer), m_Cursor(0), m_IsOpen(false), m_OpenMode(FileConstants::Read)
    {
    }
    DeclareNonCopyable(MemoryFile);


    void Open(FileConstants::OpenMode openMode) override;
    bool IsOpen() override;
    void Close() override;
    unsigned long Read(void* buffer, unsigned long size, unsigned long elementSize) override;
    unsigned long Write(const void* buffer, unsigned long size, unsigned long elementSize) override;
    unsigned long GetPosition() override { return m_Cursor; };
    void SetPosition(unsigned long position, FileConstants::PositionOrigin origin) override;

private:
    Vector<char>& m_Buffer;
    unsigned long m_Cursor;
    bool m_IsOpen;
    FileConstants::OpenMode m_OpenMode;
};

}

