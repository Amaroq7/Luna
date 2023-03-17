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
#include "ExtSystem.hpp"
#include "TimerSystem.hpp"
#include "ConfigSystem.hpp"

nstd::observer_ptr<Anubis::IAnubis> gAnubisApi;
nstd::observer_ptr<Anubis::Game::ILibrary> gGame;
nstd::observer_ptr<Anubis::Engine::ILibrary> gEngine;
std::unique_ptr<Anubis::ILogger> gLogger;
std::unique_ptr<Luna::AnubisPlugin> gPluginInfo;
std::unique_ptr<Luna::PluginSystem> gPluginSystem;

namespace
{
    void loadExts()
    {
        std::filesystem::path extsPath {gPluginInfo->getPath().parent_path().parent_path() / "exts"};

        for (const auto &entry : std::filesystem::directory_iterator{extsPath})
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const auto &path = entry.path();

            gExtList.emplace_back(path);
        }
    }

    void ServerFrame(const std::unique_ptr<Anubis::Game::IStartFrameHook> &hook)
    {
        hook->callNext();

        for (auto it = gTimers.begin(); it != gTimers.end(); it++)
        {
            auto &timer = *it;
            if (timer.getLastExec() + timer.getInterval() <= gEngine->getTime())
            {
                bool result = timer.exec();
                if (!result)
                {
                    it = gTimers.erase(it);
                }
            }
        }
    }
}

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

        std::filesystem::path cfgFile {gPluginInfo->getPath().parent_path().parent_path() / "configs" / "general.yaml"};
        gConfig = std::make_unique<Luna::Config>(std::move(cfgFile));

        gLogger->setLogLevel(static_cast<Anubis::LogLevel>(gConfig->getLogLevel()));

        loadExts();
        gPluginSystem = std::make_unique<Luna::PluginSystem>(gConfig->getPluginsDirName());
        gGame->getHooks()->startFrame()->registerHook(ServerFrame, Anubis::HookPriority::Default);

        return true;
    }

    void Shutdown()
    {
        gPluginSystem->unloadPlugins();
        gLogger.reset();
    }
}