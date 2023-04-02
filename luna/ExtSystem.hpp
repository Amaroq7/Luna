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

#include <IExt.hpp>
#include <ILogger.hpp>

#include <observer_ptr.hpp>

#include <functional>
#include <filesystem>
#include <memory>

#if defined __linux__
    #include <dlfcn.h>
    #include <unistd.h>
#elif defined _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

namespace Luna
{
    class Extension
    {
    public:
        using SystemHandle = void *;

    public:
        explicit Extension(const std::filesystem::path &path);

        [[nodiscard]] bool initPlugin(std::unique_ptr<Anubis::ILogger> &&logger) const;
        void deinitPlugin() const;
        [[nodiscard]] IExt::Type getType() const;
        [[nodiscard]] std::string_view getName() const;
        [[nodiscard]] std::string_view getVersion() const;
        [[nodiscard]] std::string_view getDate() const;
        [[nodiscard]] std::string_view getUrl() const;
        [[nodiscard]] std::string_view getAuthor() const;
        [[nodiscard]] std::any getImpl() const;

    public:
        template<typename T, typename = std::enable_if_t<std::is_pointer_v<T>>>
        T getSymbol(std::string_view name)
        {
#if defined __linux__
            return reinterpret_cast<T>(dlsym(m_libHandle.get(), name.data()));
#elif defined _WIN32
            return reinterpret_cast<T>(GetProcAddress(reinterpret_cast<HMODULE>(m_libHandle.get()), name.data()));
#endif
        }

        explicit operator SystemHandle();

    private:
#if defined _WIN32
        static constexpr std::string_view FnQuerySgn = "?Query@Luna@@YA?AV?$observer_ptr@VIExt@Luna@@@nstd@@XZ";
        static constexpr std::string_view FnInitSgn = "";
        static constexpr std::string_view FnShutdownSgn = "?Shutdown@Luna@@YAXXZ";
#else
        static constexpr std::string_view FnQuerySgn = "_ZN4Luna5QueryEv";
        static constexpr std::string_view FnInitSgn = "_ZN4Luna4InitEOSt10unique_ptrIN6Anubis7ILoggerESt14default_deleteIS2_EE";
        static constexpr std::string_view FnShutdownSgn = "_ZN4Luna8ShutdownEv";
#endif

    private:
        static std::string_view _getError();

    private:
        void _findPluginFuncs();
        void _queryPlugin();

    private:
        IExt::Type m_plType = IExt::Type::Unknown;
        std::string m_plName;
        std::string m_plVersion;
        std::string m_plDate;
        std::string m_plAuthor;
        std::string m_plUrl;
        std::any m_impl;
        std::function<nstd::observer_ptr<IExt>()> m_queryFn;
        std::function<bool(std::unique_ptr<Anubis::ILogger> &&logger)> m_initFn;
        std::function<void()> m_shutdownFn;
        std::unique_ptr<void, std::function<void(SystemHandle)>> m_libHandle;
    };
}

extern std::vector<Luna::Extension> gExtList;