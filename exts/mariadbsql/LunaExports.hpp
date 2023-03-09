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

#include <ILuna.hpp>
#include <SQLInfo.hpp>
#include <anubis/ILogger.hpp>
#include <Driver.hpp>

extern std::unique_ptr<Anubis::ILogger> gLogger;

class Extension final : public Luna::IExt
{
public:
   ~Extension() final = default;

   [[nodiscard]] Luna::InterfaceVersion getInterfaceVersion() const final
   {
       return Luna::VERSION;
   }

   [[nodiscard]] Type getType() const final
   {
       return IExt::Type::SQLMariaDB;
   }

   [[nodiscard]] std::string_view getName() const final
   {
       return "MariaDB Extension";
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

   [[nodiscard]] std::string_view getLogTag() const final
   {
       return "MARIADB";
   }

   [[nodiscard]] std::any getImpl() const final
   {
       static auto driver = std::make_unique<Luna::MDBSQL::Driver>();

       return nstd::observer_ptr<Luna::MDBSQL::IDriver>(driver);
   }
};

extern std::unique_ptr<Luna::IExt> gPluginInfo;