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

namespace Luna::MDBSQL
{
    class IWarning
    {
    public:
        virtual ~IWarning() = default;
        virtual std::unique_ptr<IWarning> getNextWarning() const = 0;
        virtual std::string_view getSQLState() const = 0;
        virtual std::int32_t getErrorCode() const = 0;
        virtual std::string_view getMessage() const = 0;
    };
}