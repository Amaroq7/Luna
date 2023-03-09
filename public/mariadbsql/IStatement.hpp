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

#include <string>
#include <vector>
#include <memory>

namespace Luna::MDBSQL
{
    class IResultSet;
    class IWarning;

    class IStatement
    {
    public:
        virtual ~IStatement() = default;

        virtual bool execute(std::string_view sql) = 0;
        virtual std::unique_ptr<IResultSet> executeQuery(std::string_view sql) = 0;
        virtual int32_t executeUpdate(std::string_view sql) = 0;
        virtual int64_t executeLargeUpdate(std::string_view sql) = 0;
        virtual void close() = 0;
        virtual int32_t getMaxRows() = 0;
        virtual void setMaxRows(int32_t max) = 0;
        virtual int64_t getLargeMaxRows() = 0;
        virtual void setLargeMaxRows(int64_t max) = 0;
        virtual void setEscapeProcessing(bool enable) = 0;
        virtual int32_t getQueryTimeout() = 0;
        virtual void setQueryTimeout(int32_t seconds) = 0;
        virtual std::unique_ptr<IResultSet> getResultSet() = 0;
        virtual int32_t getUpdateCount() = 0;
        virtual int64_t getLargeUpdateCount() = 0;
        virtual bool isClosed() = 0;
        virtual void addBatch(std::string_view sql) = 0;
        virtual void clearBatch() = 0;
        virtual std::vector<int32_t> executeBatch() = 0;
        virtual std::vector<int64_t> executeLargeBatch() = 0;
        virtual void closeOnCompletion() = 0;
        virtual bool isCloseOnCompletion() = 0;
        virtual std::unique_ptr<IWarning> getWarnings() = 0;
    };
}