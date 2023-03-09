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

#include <IResultSet.hpp>

#include <anubis/observer_ptr.hpp>

namespace sql
{
    class ResultSet;
}

namespace Luna::MDBSQL
{
    class ResultSet final : public IResultSet
    {
    public:
        explicit ResultSet(nstd::observer_ptr<sql::ResultSet> resultSet);
        ~ResultSet() final = default;

        void close() final;
        bool next() final;

        std::unique_ptr<IWarning> getWarnings() final;
        void clearWarnings() final;

        bool isBeforeFirst() const final;
        bool isFirst() const final;

        bool isLast() final;
        bool isAfterLast() final;

        void beforeFirst() final;
        void afterLast() final;
        bool first() final;
        bool last() final;
        std::int32_t getRow() final;
        bool previous() final;

        std::int32_t getFetchDirection() const final;
        void setFetchDirection(FetchDirection fetchDirection) final;
        std::int32_t getFetchSize() const final;
        void setFetchSize(std::int32_t fetchSize) final;
        bool isClosed() const final;
        std::unique_ptr<IStatement> getStatement() final;

        bool isNull(std::int32_t columnIndex) const final;
        bool isNull(std::string_view columnLabel) const final;
        std::string getString(std::int32_t columnIndex) const final;
        std::string getString(std::string_view columnLabel) const final;
        std::int32_t getInt(std::int32_t columnIndex) const final;
        std::int32_t getInt(std::string_view columnLabel) const final;
        std::uint32_t getUInt(std::int32_t columnIndex) const final;
        std::uint32_t getUInt(std::string_view columnLabel) const final;
        std::int64_t getLong(std::string_view columnLabel) const final;
        std::int64_t getLong(std::int32_t columnIndex) const final;
        std::int64_t getInt64(std::string_view columnLabel) const final;
        std::int64_t getInt64(std::int32_t columnIndex) const final;
        std::uint64_t getUInt64(std::string_view columnLabel) const final;
        std::uint64_t getUInt64(std::int32_t columnIndex) const final;
        float getFloat(std::string_view columnLabel) const final;
        float getFloat(std::int32_t columnIndex) const final;
        long double getDouble(std::string_view columnLabel) const final;
        long double getDouble(std::int32_t columnIndex) const final;
        bool getBoolean(std::int32_t index) const final;
        bool getBoolean(std::string_view columnLabel) const final;
        std::byte getByte(std::int32_t index) const final;
        std::byte getByte(std::string_view columnLabel) const final;
        std::int16_t getShort(std::int32_t index) const final;
        std::int16_t getShort(std::string_view columnLabel) const final;

        std::int32_t findColumn(std::string_view columnLabel) const final;

    private:
        std::unique_ptr<sql::ResultSet> m_resultSet;
    };
}