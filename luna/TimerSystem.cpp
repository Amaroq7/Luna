/*
 *  Copyright (C) 2023 Karol Szuster
 *
 *  This file is part of Luna.
 *
 *  Luna is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  Luna is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with Luna.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "TimerSystem.hpp"
#include "AnubisExports.hpp"

namespace Luna
{
    Timer::Timer(float interval, Timer::TimerCallback &&cb, std::any &&data, bool repeat, bool execNow)
        : m_interval(interval), m_callback(std::move(cb)), m_data(std::move(data)), m_repeat(repeat)
    {
        if (m_interval < 0.1f)
        {
            m_interval = 0.1f;
        }

        if (execNow)
        {
            exec();
            return;
        }

        m_lastExec = gEngine->getTime();
    }

    bool Timer::exec()
    {
        m_lastExec = gEngine->getTime();

        bool result = m_callback(m_data);

        if (m_repeat)
        {
            return result;
        }

        return false;
    }

    float Timer::getLastExec() const
    {
        return m_lastExec;
    }

    float Timer::getInterval() const
    {
        return m_interval;
    }
}

std::vector<Luna::Timer> gTimers;