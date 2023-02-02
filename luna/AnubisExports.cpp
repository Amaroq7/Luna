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

#include "AnubisExports.hpp"
#include "PluginSystem.hpp"

nstd::observer_ptr<Anubis::IAnubis> gAnubisApi;
nstd::observer_ptr<Anubis::Game::ILibrary> gGame;
nstd::observer_ptr<Anubis::Engine::ILibrary> gEngine;
std::unique_ptr<Anubis::ILogger> gLogger;
std::unique_ptr<Luna::AnubisPlugin> gPluginInfo;
std::unique_ptr<Luna::PluginSystem> gPluginSystem;

namespace Anubis
{
    nstd::observer_ptr<IPlugin> Query()
    {
        gPluginInfo = std::make_unique<Luna::AnubisPlugin>();
        return gPluginInfo;
    }

    void InstallVHooks()
    {
        gPluginSystem->allowVFuncHooks();
    }

    bool Init(nstd::observer_ptr<IAnubis> api)
    {
        gAnubisApi = api;
        gLogger = gAnubisApi->getLogger(ILogger::VERSION);
        gGame = gAnubisApi->getGame(Game::ILibrary::VERSION);
        gEngine = gAnubisApi->getEngine(Engine::ILibrary::VERSION);
        gLogger->setLogTag("LUNA");
        gLogger->setLogLevel(LogLevel::Debug);

        gPluginSystem = std::make_unique<Luna::PluginSystem>();

        return true;
    }

    void Shutdown()
    {
        gPluginSystem->unloadPlugins();
        gLogger.reset();
    }
}