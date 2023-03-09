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
#include "IExt.hpp"

#if !defined LUNA_CORE
#include "anubis/ILogger.hpp"
#include "anubis/observer_ptr.hpp"
#endif

namespace Luna
{
    /**
     * @brief API major version
     */
    static constexpr MajorInterfaceVersion MAJOR_VERSION = MajorInterfaceVersion(1);

    /**
     * @brief API minor version
     */
    static constexpr MinorInterfaceVersion MINOR_VERSION = MinorInterfaceVersion(0);

    /**
     * @brief API version
     *
     * Major version is present in the 16 most significant bits.
     * Minor version is present in the 16 least significant bits.
     *
     * @b Examples
     *
     * To extract the major version of the API the following formula can be used
     * @code{cpp}
     * std::uint16_t majorVer = (VERSION >> 16) & 0xFFFF;
     * @endcode
     *
     *
     * To extract the minor version of the API the following formula can be used
     * @code{cpp}
     * std::uint16_t minorVer = VERSION & 0xFFFF;
     * @endcode
     */
    static constexpr InterfaceVersion VERSION = InterfaceVersion(MAJOR_VERSION << 16 | MINOR_VERSION);

#if !defined LUNA_CORE
    LUNA_API nstd::observer_ptr<IExt> Query();
    LUNA_API bool Init(std::unique_ptr<Anubis::ILogger> &&logger);
    LUNA_API void Shutdown();
#else
    using fnQuery = nstd::observer_ptr<IExt> (*)();
    using fnInit = bool (*)(std::unique_ptr<Anubis::ILogger> &&logger);
    using fnShutdown = void (*)();
#endif
} // namespace Luna
