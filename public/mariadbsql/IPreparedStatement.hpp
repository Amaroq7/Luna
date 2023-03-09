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

#include "IStatement.hpp"

namespace Luna::MDBSQL
{
    class IPreparedStatement : public virtual IStatement
    {
    public:
        virtual ~IPreparedStatement() = default;

        virtual bool execute() = 0;
        virtual bool execute(std::string_view sql) = 0;
        virtual int32_t executeUpdate() = 0;
        virtual int64_t executeLargeUpdate() = 0;
        virtual std::unique_ptr<IResultSet> executeQuery() = 0;
        virtual void addBatch() = 0;
        virtual void clearParameters() = 0;
        virtual void setNull(int32_t parameterIndex, int32_t sqlType) = 0;

        virtual void setBoolean(int32_t parameterIndex, bool value) = 0;
        virtual void setByte(int32_t parameterIndex, std::byte bit) = 0;
        virtual void setShort(int32_t parameterIndex, int16_t value) = 0;
        virtual void setString(int32_t parameterIndex, std::string_view str) = 0;
        virtual void setInt(int32_t parameterIndex, int32_t value) = 0;
        virtual void setLong(int32_t parameterIndex, int64_t value) = 0;
        virtual void setInt64(int32_t parameterIndex, int64_t value) = 0;
        virtual void setUInt64(int32_t parameterIndex, uint64_t value) = 0;
        virtual void setUInt(int32_t parameterIndex, uint32_t value) = 0;
        virtual void setFloat(int32_t parameterIndex, float value) = 0;
        virtual void setDouble(int32_t parameterIndex, double value) = 0;
        virtual void setBigInt(int32_t parameterIndex, std::string_view value) = 0;
    };
}