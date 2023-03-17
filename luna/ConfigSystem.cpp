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

#include "ConfigSystem.hpp"

#include <yaml-cpp/yaml.h>

namespace Luna
{
    Config::Config(std::filesystem::path &&cfgFile)
    {
        YAML::Node rootNode;
        try
        {
#if defined __linux__
            rootNode = YAML::LoadFile(cfgFile.c_str());
#elif defined _WIN32
            rootNode = YAML::LoadFile(cfgFile.string().c_str());
#endif
        }
        catch (const std::exception &e [[maybe_unused]])
        {
        }

        for (auto it = rootNode.begin(); it != rootNode.end(); ++it)
        {
            auto nodeName = it->first.as<std::string>();
            if (nodeName == "logging")
            {
                auto level = it->second["level"].as<std::string>();
                std::transform(level.begin(), level.end(), level.begin(),
                               [](unsigned char c)
                               {
                                   return std::tolower(c);
                               });

                if (level == "debug")
                {
                    m_logLevel = LogLevel::Debug;
                }
                else if (level == "info")
                {
                    m_logLevel = LogLevel::Info;
                }
            }
            else if (nodeName == "dirs")
            {
                m_pluginsDirName = it->second["plugins"].as<std::string>();
            }
        }
    }

    std::string_view Config::getPluginsDirName() const
    {
        return m_pluginsDirName;
    }

    Config::LogLevel Config::getLogLevel() const
    {
        return m_logLevel;
    }
}

std::unique_ptr<Luna::Config> gConfig;
