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

#include "Warning.hpp"

#include <conncpp.hpp>

namespace Luna::MDBSQL
{
    Warning::Warning(nstd::observer_ptr<sql::SQLWarning> warning)
    : m_warning(warning.get())
    {}

    std::unique_ptr<IWarning> Warning::getNextWarning() const
    {
        return std::make_unique<Warning>(m_warning->getNextWarning());
    }

    std::string_view Warning::getSQLState() const
    {
        return m_warning->getSQLState().c_str();
    }

    std::int32_t Warning::getErrorCode() const
    {
        return m_warning->getErrorCode();
    }

    std::string_view Warning::getMessage() const
    {
        return m_warning->getMessage().c_str();
    }
}