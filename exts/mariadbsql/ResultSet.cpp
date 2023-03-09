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

#include "ResultSet.hpp"
#include "Statement.hpp"
#include "Warning.hpp"

#include <conncpp.hpp>

namespace Luna::MDBSQL
{
    ResultSet::ResultSet(nstd::observer_ptr<sql::ResultSet> resultSet)
        : m_resultSet(resultSet.get())
    {}

    void ResultSet::close()
    {
        m_resultSet->close();
    }

    bool ResultSet::next()
    {
        return m_resultSet->next();
    }

    std::unique_ptr<IWarning> ResultSet::getWarnings()
    {
        return std::make_unique<Warning>(m_resultSet->getWarnings());
    }

    void ResultSet::clearWarnings()
    {
        m_resultSet->clearWarnings();
    }

    bool ResultSet::isBeforeFirst() const
    {
        return m_resultSet->isBeforeFirst();
    }

    bool ResultSet::isFirst() const
    {
        return m_resultSet->isFirst();
    }

    bool ResultSet::isLast()
    {
        return m_resultSet->isLast();
    }

    bool ResultSet::isAfterLast()
    {
        return m_resultSet->isAfterLast();
    }

    void ResultSet::beforeFirst()
    {
        m_resultSet->beforeFirst();
    }

    void ResultSet::afterLast()
    {
        m_resultSet->afterLast();
    }

    bool ResultSet::first()
    {
        return m_resultSet->first();
    }

    bool ResultSet::last()
    {
        return m_resultSet->last();
    }

    std::int32_t ResultSet::getRow()
    {
        return m_resultSet->getRow();
    }

    bool ResultSet::previous()
    {
        return m_resultSet->previous();
    }

    std::int32_t ResultSet::getFetchDirection() const
    {
        return m_resultSet->getFetchDirection();
    }

    void ResultSet::setFetchDirection(FetchDirection fetchDirection)
    {
        return m_resultSet->setFetchDirection(static_cast<std::uint16_t>(fetchDirection));
    }

    std::int32_t ResultSet::getFetchSize() const
    {
        return m_resultSet->getFetchSize();
    }

    void ResultSet::setFetchSize(std::int32_t fetchSize)
    {
        m_resultSet->setFetchSize(fetchSize);
    }

    bool ResultSet::isClosed() const
    {
        return m_resultSet->isClosed();
    }

    std::unique_ptr<IStatement> ResultSet::getStatement()
    {
        return std::make_unique<Statement>(m_resultSet->getStatement());
    }

    bool ResultSet::isNull(std::int32_t columnIndex) const
    {
        return m_resultSet->isNull(columnIndex);
    }

    bool ResultSet::isNull(std::string_view columnLabel) const
    {
        return m_resultSet->isNull(columnLabel.data());
    }

    std::string ResultSet::getString(std::int32_t columnIndex) const
    {
        return m_resultSet->getString(columnIndex).c_str();
    }

    std::string ResultSet::getString(std::string_view columnLabel) const
    {
        return m_resultSet->getString(columnLabel.data()).c_str();
    }

    std::int32_t ResultSet::getInt(std::int32_t columnIndex) const
    {
        return m_resultSet->getInt(columnIndex);
    }

    std::int32_t ResultSet::getInt(std::string_view columnLabel) const
    {
        return m_resultSet->getInt(columnLabel.data());
    }

    std::uint32_t ResultSet::getUInt(std::int32_t columnIndex) const
    {
        return m_resultSet->getUInt(columnIndex);
    }

    std::uint32_t ResultSet::getUInt(std::string_view columnLabel) const
    {
        return m_resultSet->getUInt(columnLabel.data());
    }

    std::int64_t ResultSet::getLong(std::string_view columnLabel) const
    {
        return m_resultSet->getLong(columnLabel.data());
    }

    std::int64_t ResultSet::getLong(std::int32_t columnIndex) const
    {
        return m_resultSet->getLong(columnIndex);
    }

    std::int64_t ResultSet::getInt64(std::string_view columnLabel) const
    {
        return m_resultSet->getInt64(columnLabel.data());
    }

    std::int64_t ResultSet::getInt64(std::int32_t columnIndex) const
    {
        return m_resultSet->getInt64(columnIndex);
    }

    std::uint64_t ResultSet::getUInt64(std::string_view columnLabel) const
    {
        return m_resultSet->getUInt64(columnLabel.data());
    }

    std::uint64_t ResultSet::getUInt64(std::int32_t columnIndex) const
    {
        return m_resultSet->getUInt64(columnIndex);
    }

    float ResultSet::getFloat(std::string_view columnLabel) const
    {
        return m_resultSet->getFloat(columnLabel.data());
    }

    float ResultSet::getFloat(std::int32_t columnIndex) const
    {
        return m_resultSet->getFloat(columnIndex);
    }

    long double ResultSet::getDouble(std::string_view columnLabel) const
    {
        return m_resultSet->getDouble(columnLabel.data());
    }

    long double ResultSet::getDouble(std::int32_t columnIndex) const
    {
        return m_resultSet->getDouble(columnIndex);
    }

    bool ResultSet::getBoolean(std::int32_t index) const
    {
        return m_resultSet->getBoolean(index);
    }

    bool ResultSet::getBoolean(std::string_view columnLabel) const
    {
        return m_resultSet->getBoolean(columnLabel.data());
    }

    std::byte ResultSet::getByte(std::int32_t index) const
    {
        return std::byte(m_resultSet->getByte(index));
    }

    std::byte ResultSet::getByte(std::string_view columnLabel) const
    {
        return std::byte(m_resultSet->getByte(columnLabel.data()));
    }

    std::int16_t ResultSet::getShort(std::int32_t index) const
    {
        return m_resultSet->getShort(index);
    }

    std::int16_t ResultSet::getShort(std::string_view columnLabel) const
    {
        return m_resultSet->getShort(columnLabel.data());
    }

    std::int32_t ResultSet::findColumn(std::string_view columnLabel) const
    {
        return m_resultSet->findColumn(columnLabel.data());
    }
}