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

#include <IStatement.hpp>

#include <anubis/observer_ptr.hpp>

namespace sql
{
    class Statement;
}

namespace Luna::MDBSQL
{
    class Statement : public virtual IStatement
    {
    public:
        explicit Statement(nstd::observer_ptr<sql::Statement> statement);
        ~Statement() override = default;

        bool execute(std::string_view sql) override;
        std::unique_ptr<IResultSet> executeQuery(std::string_view sql) override;
        int32_t executeUpdate(std::string_view sql) override;
        int64_t executeLargeUpdate(std::string_view sql) override;
        void close() override;
        int32_t getMaxRows() override;
        void setMaxRows(int32_t max) override;
        int64_t getLargeMaxRows() override;
        void setLargeMaxRows(int64_t max) override;
        void setEscapeProcessing(bool enable) override;
        int32_t getQueryTimeout() override;
        void setQueryTimeout(int32_t seconds) override;
        std::unique_ptr<IResultSet> getResultSet() override;
        int32_t getUpdateCount() override;
        int64_t getLargeUpdateCount() override;
        bool isClosed() override;
        void addBatch(std::string_view sql) override;
        void clearBatch() override;
        std::vector<int32_t> executeBatch() override;
        std::vector<int64_t> executeLargeBatch() override;
        void closeOnCompletion() override;
        bool isCloseOnCompletion() override;
        std::unique_ptr<IWarning> getWarnings() override;

    protected:
        std::unique_ptr<sql::Statement> m_statement;
    };
}