/*
 *  Copyright (C) 2013 Karol Szuster
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

#include <cinttypes>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <observer_ptr.hpp>

namespace Luna
{
    struct PluginInfo
    {
        std::string name;
        std::string version;
        std::string author;
        std::string url;
    };

    class Plugin
    {
    public:
        static constexpr const char *FIELD_NAME = "name";
        static constexpr const char *FIELD_VERSION = "version";
        static constexpr const char *FIELD_AUTHOR = "author";
        static constexpr const char *FIELD_URL = "url";

        static constexpr const char *EXTENSION = ".luac";
        using ID = std::uint32_t;

    public:
        Plugin(PluginInfo &&pluginInfo, nstd::observer_ptr<lua_State> luaState, ID pid);
        ~Plugin();

        void allowVFuncHooks() const;

    private:
        PluginInfo m_pluginInfo{};
        nstd::observer_ptr<lua_State> m_luaState{};
        ID m_id{};
    };

    class PluginSystem
    {
    public:
        PluginSystem();

        void unloadPlugins();
        void allowVFuncHooks() const;

    private:
        void _loadPlugins();
        [[nodiscard]] std::pair<PluginInfo, bool> _readPluginInfo(const std::filesystem::path &path,
                                                                  nstd::observer_ptr<lua_State> luaState) const;

    private:
        std::vector<std::unique_ptr<Plugin>> m_plugins;
    };
}

extern std::unique_ptr<Luna::PluginSystem> gPluginSystem;