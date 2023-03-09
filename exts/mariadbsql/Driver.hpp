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

#include <IDriver.hpp>

#include <anubis/observer_ptr.hpp>

namespace sql
{
    class Driver;
}

namespace Luna::MDBSQL
{
   class Driver final : public IDriver
   {
   public:
       Driver();
       ~Driver() final = default;

       std::unique_ptr<IConnection> connect(std::string_view url, const Properties &props) final;
       std::unique_ptr<IConnection> connect(std::string_view host, std::string_view user, std::string_view pwd) final;
       std::unique_ptr<IConnection> connect(const Properties &props) final;
       bool acceptsURL(std::string_view url) final;
       uint32_t getMajorVersion() final;
       uint32_t getMinorVersion() final;
       bool jdbcCompliant() final;
       std::string_view getName() final;

   private:
       nstd::observer_ptr<sql::Driver> m_sqlDriver;
   };
}