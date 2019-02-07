/*
 * Copyright (C) 2018-2019 Michał Siejak
 * This file is part of Quartz - a raytracing aspect for Qt3D.
 * See LICENSE file for licensing information.
 */

#pragma once

#include <QQueue>

namespace Qt3DRaytrace {
namespace Utility {

template<typename T>
class MovingAverage
{
public:
    explicit MovingAverage(int limit=100)
        : m_limit(limit)
    {
        Q_ASSERT(limit > 0);
        m_values.reserve(limit);
    }

    T add(T value)
    {
        if(m_values.size() == m_limit) {
            m_average = m_average + (value / m_limit) - (m_values.dequeue() / m_limit);
            m_values.enqueue(value);
        }
        else {
            m_values.enqueue(value);
            m_average += (value - m_average) / m_values.size();
        }
        return m_average;
    }

    void reset()
    {
        m_average = T(0);
        m_values.clear();
    }

    T average() const
    {
        return m_average;
    }

    operator T() const
    {
        return m_average;
    }

private:
    T m_average = T(0);
    QQueue<T> m_values;
    int m_limit;
};

} // Utility
} // Qt3DRaytrace
