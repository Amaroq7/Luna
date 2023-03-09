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

#include "PluginSystem.hpp"
#include "AnubisExports.hpp"
#include "BasicNatives.hpp"
#include "EdictNatives.hpp"
#include "ClassNatives.hpp"
#include "sql/Natives.hpp"

#include <fmt/format.h>

#include <iostream>

namespace Luna
{
    Plugin::Plugin(PluginInfo &&pluginInfo, nstd::observer_ptr<lua_State> luaState, Plugin::ID pid)
        : m_pluginInfo(std::move(pluginInfo)), m_luaState(luaState), m_id(pid)
    {
            luaL_openlibs(m_luaState.get());
            std::size_t i = 0;
            do
            {
                lua_register(m_luaState.get(), gBasicNatives[i].name, gBasicNatives[i].func);
            } while (gBasicNatives[++i].func);

            i = 0;
            do
            {
                lua_register(m_luaState.get(), gEdictNatives[i].name, gEdictNatives[i].func);
            } while (gEdictNatives[++i].func);

            i = 0;
            do
            {
                lua_register(m_luaState.get(), gClassNatives[i].name, gClassNatives[i].func);
            } while (gClassNatives[++i].func);

            i = 0;
            do
            {
                lua_register(m_luaState.get(), gSQLNatives[i].name, gSQLNatives[i].func);
            } while (gSQLNatives[++i].func);

            if (lua_getglobal(m_luaState.get(), "maxClients") != LUA_TNIL)
            {
                lua_pushinteger(m_luaState.get(), gEngine->getMaxClients());
                lua_setglobal(m_luaState.get(), "maxClients");
                lua_pop(m_luaState.get(), 1);
            }

            if (lua_getglobal(m_luaState.get(), "__start") == LUA_TNIL)
            {
                luaL_error(m_luaState.get(), "__start function was not found");
            }

            if (lua_pcall(m_luaState.get(), 0, 0, 0) != LUA_OK)
            {
                luaL_error(m_luaState.get(), "__start could not be executed");
            }
    }

    Plugin::~Plugin()
    {
        lua_close(m_luaState.get());
    }

    void Plugin::allowVFuncHooks() const
    {
        if (lua_getglobal(m_luaState.get(), "__installVFuncHooks") != LUA_TNIL)
        {
            lua_pcall(m_luaState.get(), 0, 0, 0);
        }
    }

    PluginSystem::PluginSystem()
    {
        _loadPlugins();
    }

    void PluginSystem::unloadPlugins()
    {
        m_plugins.clear();
    }

    void PluginSystem::_loadPlugins()
    {
        std::filesystem::path pluginsPath {gPluginInfo->getPath().parent_path().parent_path() / "plugins"};

        for (const auto &entry : std::filesystem::directory_iterator{pluginsPath})
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const auto &path = entry.path();

            const std::string ext =
#if defined __linux__
                path.extension().c_str();
#elif defined _WIN32
                path.extension().string().c_str();
#endif

            if (ext != Plugin::EXTENSION)
            {
                std::string warn = fmt::format("Could not load {}. Expected .luac got {}.", path.string(), ext);
                gLogger->logMsg(Anubis::LogDest::ConsoleFile, Anubis::LogLevel::Warning, warn);
                continue;
            }

            nstd::observer_ptr<lua_State> luaState = luaL_newstate();

#if defined __linux__
            int loadResult = luaL_dofile(luaState.get(), path.c_str());
#elif defined _WIN32
            int loadResult = luaL_dofile(luaState.get(), path.string().c_str());
#endif
            if (loadResult != LUA_OK)
            {
                std::string warn = fmt::format("Could not load {}. Wrong format.", path.string());
                gLogger->logMsg(Anubis::LogDest::ConsoleFile, Anubis::LogLevel::Warning, warn);
                continue;
            }

            auto results = _readPluginInfo(path, luaState);

            if (!results.second)
            {
                continue;
            }

            std::string info = fmt::format("Loaded {}.", results.first.name);
            gLogger->logMsg(Anubis::LogDest::ConsoleFile, Anubis::LogLevel::Info, info);
            m_plugins.emplace_back(std::make_unique<Plugin>(std::move(results.first), luaState, m_plugins.size()));
        }
    }

    std::pair<PluginInfo, bool> PluginSystem::_readPluginInfo(const std::filesystem::path &path,
                                                              nstd::observer_ptr<lua_State> luaState) const
    {
        if (lua_getglobal(luaState.get(), "pluginInfo") != LUA_TTABLE)
        {
            std::string warn = fmt::format("Could not load {}. Cannot find plugin info.", path.string());
            gLogger->logMsg(Anubis::LogDest::ConsoleFile, Anubis::LogLevel::Warning, warn);

            return {{}, false};
        }

        PluginInfo pluginInfo;

        lua_getfield(luaState.get(), -1, Plugin::FIELD_NAME);
        pluginInfo.name = lua_tostring(luaState.get(), -1);
        lua_pop(luaState.get(), 1);

        lua_getfield(luaState.get(), -1, Plugin::FIELD_VERSION);
        pluginInfo.version = lua_tostring(luaState.get(), -1);
        lua_pop(luaState.get(), 1);

        lua_getfield(luaState.get(), -1, Plugin::FIELD_AUTHOR);
        pluginInfo.author = lua_tostring(luaState.get(), -1);
        lua_pop(luaState.get(), 1);

        lua_getfield(luaState.get(), -1, Plugin::FIELD_URL);
        pluginInfo.url = lua_tostring(luaState.get(), -1);
        lua_pop(luaState.get(), 2);

        return {pluginInfo, true};
    }

    void PluginSystem::allowVFuncHooks() const
    {
        for (const auto &plugin : m_plugins)
        {
            plugin->allowVFuncHooks();
        }
    }
}


