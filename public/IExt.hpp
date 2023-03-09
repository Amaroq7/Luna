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

#include "Common.hpp"

#include <filesystem>
#include <string_view>
#include <any>

namespace Luna
{
    class IExt
    {
    public:
        enum class Type : std::uint8_t
        {
            Unknown = 0,
            SQLMariaDB
        };

    public:
        virtual ~IExt() = default;

        [[nodiscard]] virtual InterfaceVersion getInterfaceVersion() const = 0;
        [[nodiscard]] virtual Type getType() const = 0;
        [[nodiscard]] virtual std::string_view getName() const = 0;
        [[nodiscard]] virtual std::string_view getVersion() const = 0;
        [[nodiscard]] virtual std::string_view getDate() const = 0;
        [[nodiscard]] virtual std::string_view getAuthor() const = 0;
        [[nodiscard]] virtual std::string_view getUrl() const = 0;
        [[nodiscard]] virtual std::string_view getLogTag() const = 0;
        [[nodiscard]] virtual std::any getImpl() const = 0;

    };
} // namespace Luna