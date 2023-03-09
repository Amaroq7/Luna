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

#include "ExtSystem.hpp"

#include <ILuna.hpp>

#include <fmt/format.h>

std::vector<Luna::Extension> gExtList;

namespace Luna
{
    Extension::Extension(const std::filesystem::path &path)
   {
       using namespace std::string_literals;

       if (!std::filesystem::exists(path))
       {
#if defined __linux__
           throw std::runtime_error("Extension not found "s + path.c_str());
#else
           throw std::runtime_error("Extension not found "s + path.string().c_str());
#endif
       }

#if defined __linux__
       m_libHandle = std::unique_ptr<void, std::function<void(void *)>>(
           dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND),
           [](void *lib)
           {
               if (lib)
               {
                   dlclose(lib);
               }
           });
#elif defined _WIN32
       m_libHandle =
           std::unique_ptr<void, std::function<void(void *)>>(LoadLibraryW(path.c_str()),
                                                              [](void *lib)
                                                              {
                                                                  if (lib)
                                                                  {
                                                                      FreeLibrary(reinterpret_cast<HMODULE>(lib));
                                                                  }
                                                              });
#endif

       if (!m_libHandle)
       {
           throw std::runtime_error(_getError().data());
       }

       _queryPlugin();
       _findPluginFuncs();
   }

   std::string_view Extension::_getError()
   {
#if defined __linux__
       if (char *errorMsg = dlerror(); errorMsg)
       {
           return std::string_view {errorMsg};
       }

       return {""};
#elif defined _WIN32
       static char errorMsg[256];
       FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(), 0, errorMsg,
                     sizeof(errorMsg), nullptr);

       return errorMsg;
#endif
   }

   Extension::operator SystemHandle()
   {
       return m_libHandle.get();
   }

   IExt::Type Extension::getType() const
   {
       return m_plType;
   }

   std::string_view Extension::getName() const
   {
       return m_plName;
   }

   std::string_view Extension::getVersion() const
   {
       return m_plVersion;
   }

   std::string_view Extension::getDate() const
   {
       return m_plDate;
   }

   std::string_view Extension::getUrl() const
   {
       return m_plUrl;
   }

   std::string_view Extension::getAuthor() const
   {
       return m_plAuthor;
   }

   std::any Extension::getImpl() const
   {
       return m_impl;
   }

   void Extension::_queryPlugin()
   {
       m_queryFn = getSymbol<fnQuery>(Extension::FnQuerySgn);
       if (!m_queryFn)
       {
           throw std::runtime_error("Luna::Query function not found");
       }

       nstd::observer_ptr<IExt> plInfo = m_queryFn();
       if (!plInfo)
       {
           throw std::runtime_error("Could not get plugin info");
       }

       InterfaceVersion anubisInterfaceVersion = Luna::VERSION;
       InterfaceVersion pluginInterfaceVersion = plInfo->getInterfaceVersion();

       auto extractMajorVer = [](std::uint32_t version)
       {
           return (version >> 16) & 0xFFFF;
       };

       auto extractMinorVer = [](std::uint32_t version)
       {
           return version & 0xFFFF;
       };

       if (extractMajorVer(anubisInterfaceVersion) != extractMajorVer(pluginInterfaceVersion))
       {
           throw std::runtime_error(fmt::format("Major version mismatch. {} is not compatible with this version of Luna"
                                                "\n    Requested {} got {}",
                                                plInfo->getName(),
                                                extractMajorVer(pluginInterfaceVersion),
                                                extractMajorVer(anubisInterfaceVersion)));
       }
       else if (extractMinorVer(anubisInterfaceVersion) < extractMinorVer(pluginInterfaceVersion))
       {
           throw std::runtime_error(fmt::format("Minor version mismatch. {} is not compatible with this version of Luna"
                                                "\n    Requested at least {} got {}",
                                                plInfo->getName(),
                                                extractMinorVer(pluginInterfaceVersion),
                                                extractMinorVer(anubisInterfaceVersion)));
       }

       m_plName = plInfo->getName();
       m_plAuthor = plInfo->getAuthor();
       m_plType = plInfo->getType();
       m_plDate = plInfo->getDate();
       m_plVersion = plInfo->getVersion();
       m_impl = plInfo->getImpl();
   }

   bool Extension::initPlugin(std::unique_ptr<Anubis::ILogger> &&logger) const
   {
       return m_initFn(std::move(logger));
   }

   void Extension::_findPluginFuncs()
   {
       m_initFn = getSymbol<fnInit>(Extension::FnInitSgn);
       if (!m_initFn)
       {
           throw std::runtime_error("Luna::Init function not found");
       }

       m_shutdownFn = getSymbol<fnShutdown>(Extension::FnShutdownSgn);
       if (!m_shutdownFn)
       {
           throw std::runtime_error("Luna::Shutdown function not found");
       }
   }

   void Extension::deinitPlugin() const
   {
       m_shutdownFn();
   }
} // namespace Luna
