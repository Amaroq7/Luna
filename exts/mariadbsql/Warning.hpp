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

#include <IWarning.hpp>

#include <anubis/observer_ptr.hpp>

namespace sql
{
    class SQLWarning;
}

namespace Luna::MDBSQL
{
    class Warning final : public IWarning
    {
    public:
        explicit Warning(nstd::observer_ptr<sql::SQLWarning> warning);
        ~Warning() final = default;

        std::unique_ptr<IWarning> getNextWarning() const final;
        std::string_view getSQLState() const final;
        std::int32_t getErrorCode() const final;
        std::string_view getMessage() const final;

    private:
        std::unique_ptr<sql::SQLWarning> m_warning;
    };
}