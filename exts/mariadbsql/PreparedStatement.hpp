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

#include <IPreparedStatement.hpp>
#include "Statement.hpp"

namespace sql
{
    class PreparedStatement;
}

namespace Luna::MDBSQL
{
    class PreparedStatement final : public Statement, public virtual IPreparedStatement
    {
    public:
        explicit PreparedStatement(nstd::observer_ptr<sql::PreparedStatement> preparedStmt);
        ~PreparedStatement() final = default;

        bool execute() final;
        bool execute(std::string_view sql) final;
        int32_t executeUpdate() final;
        int32_t executeUpdate(std::string_view sql) override;
        int64_t executeLargeUpdate() final;
        int64_t executeLargeUpdate(std::string_view sql) override;
        std::unique_ptr<IResultSet> executeQuery() final;
        std::unique_ptr<IResultSet> executeQuery(std::string_view sql) override;
        void addBatch() final;
        void addBatch(std::string_view sql) override;
        void clearParameters() final;
        void setNull(int32_t parameterIndex, int32_t sqlType) final;

        void setBoolean(int32_t parameterIndex, bool value) final;
        void setByte(int32_t parameterIndex, std::byte bit) final;
        void setShort(int32_t parameterIndex, int16_t value) final;
        void setString(int32_t parameterIndex, std::string_view str) final;
        void setInt(int32_t parameterIndex, int32_t value) final;
        void setLong(int32_t parameterIndex, int64_t value) final;
        void setInt64(int32_t parameterIndex, int64_t value) final;
        void setUInt64(int32_t parameterIndex, uint64_t value) final;
        void setUInt(int32_t parameterIndex, uint32_t value) final;
        void setFloat(int32_t parameterIndex, float value) final;
        void setDouble(int32_t parameterIndex, double value) final;
        void setBigInt(int32_t parameterIndex, std::string_view value) final;

    private:
        explicit operator sql::PreparedStatement *();
    };
}