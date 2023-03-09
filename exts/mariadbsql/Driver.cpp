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

#include "Driver.hpp"
#include "Connection.hpp"

#include <conncpp.hpp>

namespace Luna::MDBSQL
{
       Driver::Driver() : m_sqlDriver(sql::mariadb::get_driver_instance()) {}

       std::unique_ptr<IConnection> Driver::connect(std::string_view url, const Properties &props)
       {
           sql::Properties sqlProps;
           for (const auto &[key, value] : props)
           {
               sqlProps.emplace(key, value);
           }

           return std::make_unique<Connection>(m_sqlDriver->connect(url.data(), sqlProps));
       }

       std::unique_ptr<IConnection> Driver::connect(std::string_view host, std::string_view user, std::string_view pwd)
       {
           return std::make_unique<Connection>(m_sqlDriver->connect(host.data(), user.data(), pwd.data()));
       }

       std::unique_ptr<IConnection> Driver::connect(const Properties &props)
       {
           sql::Properties sqlProps;
           for (const auto &[key, value] : props)
           {
               sqlProps.emplace(key, value);
           }

           return std::make_unique<Connection>(m_sqlDriver->connect(sqlProps));
       }

       bool Driver::acceptsURL(std::string_view url)
       {
           return m_sqlDriver->acceptsURL(url.data());
       }

       uint32_t Driver::getMajorVersion()
       {
           return m_sqlDriver->getMajorVersion();
       }

       uint32_t Driver::getMinorVersion()
       {
           return m_sqlDriver->getMinorVersion();
       }

       bool Driver::jdbcCompliant()
       {
           return m_sqlDriver->jdbcCompliant();
       }

       std::string_view Driver::getName()
       {
           return m_sqlDriver->getName().c_str();
       }
}