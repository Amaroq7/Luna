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

#include <cinttypes>
#include <string>
#include <memory>
#include <map>

namespace Luna::MDBSQL
{
    class IConnection;

    using Properties = std::map<std::string, std::string>;

    class IDriver
    {
    public:
        virtual ~IDriver() = default;

        virtual std::unique_ptr<IConnection> connect(std::string_view url, const Properties &props) = 0;
        virtual std::unique_ptr<IConnection> connect(std::string_view host, std::string_view user, std::string_view pwd) = 0;
        virtual std::unique_ptr<IConnection> connect(const Properties &props) = 0;
        virtual bool acceptsURL(std::string_view url) = 0;
        virtual uint32_t getMajorVersion() = 0;
        virtual uint32_t getMinorVersion() = 0;
        virtual bool jdbcCompliant() = 0;
        virtual std::string_view getName() = 0;
    };
}