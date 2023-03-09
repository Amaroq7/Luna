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

#if defined _WIN32
    #define LUNA_API __declspec(dllexport)
#elif defined __linux__
    #define LUNA_API __attribute__((visibility("default")))
#endif

#define LUNA_STRONG_TYPEDEF(_base, _type)                                                                              \
    struct _type                                                                                                       \
    {                                                                                                                  \
        _type() = default;                                                                                             \
        constexpr explicit _type(const _base v) : value(v) {}                                                          \
        _type(const _type &v) = default;                                                                               \
        _type &operator=(const _type &rhs) = default;                                                                  \
        _type &operator=(const _base &rhs)                                                                             \
        {                                                                                                              \
            value = rhs;                                                                                               \
            return *this;                                                                                              \
        }                                                                                                              \
        constexpr operator const _base &() const                                                                       \
        {                                                                                                              \
            return value;                                                                                              \
        }                                                                                                              \
        bool operator==(const _type &rhs) const                                                                        \
        {                                                                                                              \
            return value == rhs.value;                                                                                 \
        }                                                                                                              \
        bool operator==(const _base &rhs) const                                                                        \
        {                                                                                                              \
            return value == rhs;                                                                                       \
        }                                                                                                              \
        bool operator<(const _type &rhs) const                                                                         \
        {                                                                                                              \
            return value < rhs.value;                                                                                  \
        }                                                                                                              \
        _base value;                                                                                                   \
        using BaseType = _base;                                                                                        \
    };

namespace Luna
{
    LUNA_STRONG_TYPEDEF(std::uint32_t, InterfaceVersion)
    LUNA_STRONG_TYPEDEF(std::uint16_t, MajorInterfaceVersion)
    LUNA_STRONG_TYPEDEF(std::uint16_t, MinorInterfaceVersion)
} // namespace Luna
