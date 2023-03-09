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

#include "Connection.hpp"
#include "PreparedStatement.hpp"

#include <conncpp.hpp>

namespace Luna::MDBSQL
{
    Connection::Connection(nstd::observer_ptr<sql::Connection> connection) : m_connection(connection.get()) {}

    std::unique_ptr<IStatement> Connection::createStatement()
    {
        return std::make_unique<Statement>(m_connection->createStatement());
    }

    std::unique_ptr<IPreparedStatement> Connection::prepareStatement(std::string_view sql)
    {
        return std::make_unique<PreparedStatement>(m_connection->prepareStatement(sql.data()));
    }
}