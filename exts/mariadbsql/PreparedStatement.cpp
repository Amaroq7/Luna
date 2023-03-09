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

#include "PreparedStatement.hpp"
#include "ResultSet.hpp"

#include <conncpp.hpp>
#include <cstddef>

namespace Luna::MDBSQL
{
    PreparedStatement::PreparedStatement(nstd::observer_ptr<sql::PreparedStatement> preparedStmt)
    : Statement(preparedStmt.get()) {}

    bool PreparedStatement::execute()
    {
        return operator sql::PreparedStatement *()->execute();
    }

    bool PreparedStatement::execute(std::string_view sql)
    {
        return Statement::execute(sql);
    }

    int32_t PreparedStatement::executeUpdate()
    {
        return operator sql::PreparedStatement *()->executeUpdate();
    }

    int64_t PreparedStatement::executeLargeUpdate()
    {
        return operator sql::PreparedStatement *()->executeLargeUpdate();
    }

    std::unique_ptr<IResultSet> PreparedStatement::executeQuery()
    {
        return std::make_unique<ResultSet>(operator sql::PreparedStatement *()->executeQuery());
    }

    void PreparedStatement::addBatch()
    {
        operator sql::PreparedStatement *()->addBatch();
    }

    void PreparedStatement::clearParameters()
    {
        operator sql::PreparedStatement *()->clearParameters();
    }

    void PreparedStatement::setNull(int32_t parameterIndex, int32_t sqlType)
    {
        operator sql::PreparedStatement *()->setNull(parameterIndex, sqlType);
    }

    void PreparedStatement::setBoolean(int32_t parameterIndex, bool value)
    {
        operator sql::PreparedStatement *()->setBoolean(parameterIndex, value);
    }

    void PreparedStatement::setByte(int32_t parameterIndex, std::byte bit)
    {
        operator sql::PreparedStatement *()->setByte(parameterIndex, std::to_integer<int8_t>(bit));
    }

    void PreparedStatement::setShort(int32_t parameterIndex, int16_t value)
    {
        operator sql::PreparedStatement *()->setShort(parameterIndex, value);
    }

    void PreparedStatement::setString(int32_t parameterIndex, std::string_view str)
    {
        operator sql::PreparedStatement *()->setString(parameterIndex, str.data());
    }

    void PreparedStatement::setInt(int32_t parameterIndex, int32_t value)
    {
        operator sql::PreparedStatement *()->setInt(parameterIndex, value);
    }

    void PreparedStatement::setLong(int32_t parameterIndex, int64_t value)
    {
        operator sql::PreparedStatement *()->setLong(parameterIndex, value);
    }

    void PreparedStatement::setInt64(int32_t parameterIndex, int64_t value)
    {
        operator sql::PreparedStatement *()->setInt64(parameterIndex, value);
    }

    void PreparedStatement::setUInt64(int32_t parameterIndex, uint64_t value)
    {
        operator sql::PreparedStatement *()->setUInt64(parameterIndex, value);
    }

    void PreparedStatement::setUInt(int32_t parameterIndex, uint32_t value)
    {
        operator sql::PreparedStatement *()->setUInt(parameterIndex, value);
    }

    void PreparedStatement::setFloat(int32_t parameterIndex, float value)
    {
        operator sql::PreparedStatement *()->setFloat(parameterIndex, value);
    }

    void PreparedStatement::setDouble(int32_t parameterIndex, double value)
    {
        operator sql::PreparedStatement *()->setDouble(parameterIndex, value);
    }

    void PreparedStatement::setBigInt(int32_t parameterIndex, std::string_view value)
    {
        operator sql::PreparedStatement *()->setBigInt(parameterIndex, value.data());
    }

    PreparedStatement::operator sql::PreparedStatement *()
    {
        return dynamic_cast<sql::PreparedStatement *>(m_statement.get());
    }

    int32_t PreparedStatement::executeUpdate(std::string_view sql)
    {
        return Statement::executeUpdate(sql);
    }

    int64_t PreparedStatement::executeLargeUpdate(std::string_view sql)
    {
        return Statement::executeLargeUpdate(sql);
    }

    std::unique_ptr<IResultSet> PreparedStatement::executeQuery(std::string_view sql [[maybe_unused]])
    {
        return {};
    }

    void PreparedStatement::addBatch(std::string_view sql)
    {
        Statement::addBatch(sql);
    }

}