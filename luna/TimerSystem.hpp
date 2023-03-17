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

#pragma once

#include <vector>
#include <functional>
#include <any>

namespace Luna
{
    class Timer
    {
    public:
        using TimerCallback = std::function<bool(std::any)>;

    public:
        Timer(float interval, TimerCallback &&cb, std::any &&data, bool repeat, bool execNow);
        bool exec();
        float getLastExec() const;
        float getInterval() const;

    private:
        float m_interval;
        TimerCallback m_callback;
        std::any m_data;
        bool m_repeat;
        float m_lastExec;
    };
}

extern std::vector<Luna::Timer> gTimers;