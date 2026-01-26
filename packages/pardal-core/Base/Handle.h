
#pragma once
#include "BaseDefines.h"
#include "Base/BaseTypes.h"

// Created on 2026-01-25 by sisco

namespace pdl
{
template <typename T>
class Handle
{
public:
    Handle() = default;
    
    explicit Handle(void* ptr) : 
        m_index(reinterpret_cast<ptrdiff_t>(ptr) & 0xffffffff),
        m_generation((reinterpret_cast<ptrdiff_t>(ptr) >> 32) & 0xffffffff) {}
    
    pdlNoDiscard bool IsEmpty() const 
    {
        return m_generation == 0;
    }
    pdlNoDiscard bool IsValid() const 
    {
        return m_generation != 0;
    }
    pdlNoDiscard uint32_t Index() const {
        return m_index;
    }
    pdlNoDiscard uint32_t Gen() const {
        return m_generation;
    }
    pdlNoDiscard void* GetIndexAsVoid() const 
    {
        return reinterpret_cast<void*>(static_cast<ptrdiff_t>(m_index));
    }
    pdlNoDiscard void* GetHandleAsVoid() const 
    {
        static_assert(sizeof(void*) >= sizeof(uint64_t));
        return reinterpret_cast<void*>((static_cast<ptrdiff_t>(m_generation) << 32) + static_cast<ptrdiff_t>(m_index));
    }
    pdlNoDiscard bool operator==(const Handle<T>& other) const 
    {
        return m_index == other.m_index && m_generation == other.m_generation;
    }
    pdlNoDiscard bool operator!=(const Handle<T>& other) const 
    {
        return m_index != other.m_index || m_generation != other.m_generation;
    }

    pdlNoDiscard explicit operator bool() const 
    {
        return IsValid();
    }

private:
    Handle(uint32_t index, uint32_t generation) : m_index(index), m_generation(generation) {}

    template<typename T_, typename ImplObjectType>
    friend class Pool;
    
    uint32 m_index = 0;
    uint32 m_generation = 0;
} ;

}

