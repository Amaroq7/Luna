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

#include <string_view>
#include <cinttypes>
#include <type_traits>
#include <cstdio>

#include <IAnubis.hpp>
#include <LunaInfo.hpp>
#include <ILogger.hpp>
#include <game/ILibrary.hpp>
#include <engine/ILibrary.hpp>
#include <game/IHooks.hpp>

extern nstd::observer_ptr<Anubis::IAnubis> gAnubisApi;
extern nstd::observer_ptr<Anubis::Game::ILibrary> gGame;
extern nstd::observer_ptr<Anubis::Engine::ILibrary> gEngine;
extern std::unique_ptr<Anubis::ILogger> gLogger;

namespace Luna
{
   class AnubisPlugin final : public Anubis::IPlugin
   {
   public:
       ~AnubisPlugin() final = default;

       [[nodiscard]] Anubis::InterfaceVersion getInterfaceVersion() const final
       {
           return Anubis::IAnubis::VERSION;
       }

       [[nodiscard]] Type getType() const final
       {
           return IPlugin::Type::Extension;
       }

       [[nodiscard]] std::string_view getName() const final
       {
           return "Lua Library";
       }

       [[nodiscard]] std::string_view getVersion() const final
       {
           return LUNA_VERSION;
       }

       [[nodiscard]] std::string_view getDate() const final
       {
           return __DATE__;
       }

       [[nodiscard]] std::string_view getAuthor() const final
       {
           return "Karol Szuster";
       }

       [[nodiscard]] std::string_view getUrl() const final
       {
           return "https://github.com/Amaroq7/luna";
       }
   };
} // namespace Luna

extern std::unique_ptr<Luna::AnubisPlugin> gPluginInfo;