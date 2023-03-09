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

#include "Statement.hpp"
#include "ResultSet.hpp"
#include "Warning.hpp"

#include <conncpp.hpp>

namespace Luna::MDBSQL
{
    Statement::Statement(nstd::observer_ptr<sql::Statement> statement) : m_statement(statement.get()) {}

    bool Statement::execute(std::string_view sql)
    {
        return m_statement->execute(sql.data());
    }

    std::unique_ptr<IResultSet> Statement::executeQuery(std::string_view sql)
    {
        return std::make_unique<ResultSet>(m_statement->executeQuery(sql.data()));
    }

    int32_t Statement::executeUpdate(std::string_view sql)
    {
        return m_statement->executeUpdate(sql.data());
    }

    int64_t Statement::executeLargeUpdate(std::string_view sql)
    {
        return m_statement->executeLargeUpdate(sql.data());
    }

    void Statement::close()
    {
        m_statement->close();
    }

    int32_t Statement::getMaxRows()
    {
        return m_statement->getMaxRows();
    }

    void Statement::setMaxRows(int32_t max)
    {
        m_statement->setMaxRows(max);
    }

    int64_t Statement::getLargeMaxRows()
    {
        return m_statement->getLargeMaxRows();
    }

    void Statement::setLargeMaxRows(int64_t max)
    {
        m_statement->setLargeMaxRows(max);
    }

    void Statement::setEscapeProcessing(bool enable)
    {
        m_statement->setEscapeProcessing(enable);
    }

    int32_t Statement::getQueryTimeout()
    {
        return m_statement->getQueryTimeout();
    }

    void Statement::setQueryTimeout(int32_t seconds)
    {
        m_statement->setQueryTimeout(seconds);
    }

    std::unique_ptr<IResultSet> Statement::getResultSet()
    {
        return {};
    }

    int32_t Statement::getUpdateCount()
    {
        return m_statement->getUpdateCount();
    }

    int64_t Statement::getLargeUpdateCount()
    {
        return m_statement->getLargeUpdateCount();
    }

    bool Statement::isClosed()
    {
        return m_statement->isClosed();
    }

    void Statement::addBatch(std::string_view sql)
    {
        m_statement->addBatch(sql.data());
    }

    void Statement::clearBatch()
    {
        m_statement->clearBatch();
    }

    std::vector<int32_t> Statement::executeBatch()
    {
        const sql::Ints &results = m_statement->executeBatch();

        return {results.begin(), results.end()};
    }

    std::vector<int64_t> Statement::executeLargeBatch()
    {
        const sql::Longs &results = m_statement->executeLargeBatch();

        return {results.begin(), results.end()};
    }

    void Statement::closeOnCompletion()
    {
        m_statement->closeOnCompletion();
    }

    bool Statement::isCloseOnCompletion()
    {
        return m_statement->isCloseOnCompletion();
    }

    std::unique_ptr<IWarning> Statement::getWarnings()
    {
        return std::make_unique<Warning>(m_statement->getWarnings());
    }

}