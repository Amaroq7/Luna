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

#include "LunaExports.hpp"

std::unique_ptr<Anubis::ILogger> gLogger;
std::unique_ptr<Luna::IExt> gPluginInfo;

namespace Luna
{
    nstd::observer_ptr<IExt> Query()
    {
        gPluginInfo = std::make_unique<Extension>();
        return gPluginInfo;
    }

    bool Init(std::unique_ptr<Anubis::ILogger> &&logger)
    {
        gLogger = std::move(logger);
        gLogger->setLogLevel(Anubis::LogLevel::Debug);

        return true;
    }

    void Shutdown()
    {
        gLogger.reset();
    }
}