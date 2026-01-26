
#pragma once
#include "Vector.h"

// Created on 2026-01-25 by sisco

namespace pdl
{

template <typename T>
class CircularVector
{
public:
    CircularVector(int maxSize = 100) 
    {
        m_maxSize = maxSize;
        m_offset = 0;
        samples.reserve(maxSize);
    }
    void Push(T sample) {
        if (samples.size() < m_maxSize)
            samples.push_back(sample);
        else {
            samples[m_offset] = sample;
            m_offset =  (m_offset + 1) % m_maxSize;
        }
    }
    void Erase() {
        if (samples.size() > 0) {
            samples.clear();
            m_offset  = 0;
        }
    }
    int GetOffset() const { return m_offset; }
    const Vector<T>& GetSamples() const { return samples; }
private:
    int m_maxSize;
    int m_offset;
    Vector<T> samples;
};

};

