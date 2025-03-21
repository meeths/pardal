#pragma once
#include <Filesystem/FileConstants.h>

namespace pdl
{
class IFile
{
public:
    virtual ~IFile() = default;

    virtual void Open(FileConstants::OpenMode openMode) = 0;
    virtual bool IsOpen() = 0;
    virtual void Close() = 0;

    virtual unsigned long Read(void* buffer, unsigned long size, unsigned long elementSize = 1) = 0;
    virtual unsigned long Write(const void* buffer, unsigned long size, unsigned long elementSize = 1) = 0;
    virtual unsigned long GetPosition() = 0;
    virtual void SetPosition(unsigned long  position, FileConstants::PositionOrigin origin = FileConstants::PositionOrigin::Start) = 0;

    template <typename T>
    unsigned long ReadObject(T& _object) { return Read(static_cast<void*>(&_object), sizeof(T)); };

    template <typename T>
    unsigned long WriteObject(const T& _object) { return Write(static_cast<const void*>(&_object), sizeof(T)); };

};

}