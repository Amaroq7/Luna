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

#include <memory>
#include <cinttypes>
#include <string>

namespace Luna::MDBSQL
{
    class IWarning;
    class IStatement;

    class IResultSet
    {
    public:
        enum class FetchDirection : std::uint16_t
        {
            Forward = 1000,
            Reverse,
            Unknown
        };

        virtual ~IResultSet() = default;

        virtual void close() = 0;
        virtual bool next() = 0;

        virtual std::unique_ptr<IWarning> getWarnings() = 0;
        virtual void clearWarnings() = 0;

        virtual bool isBeforeFirst() const = 0;
        virtual bool isFirst() const = 0;

        virtual bool isLast() = 0;
        virtual bool isAfterLast() = 0;

        virtual void beforeFirst() = 0;
        virtual void afterLast() = 0;
        virtual bool first() = 0;
        virtual bool last() = 0;
        virtual std::int32_t getRow() = 0;
        virtual bool previous() = 0;

        virtual std::int32_t getFetchDirection() const = 0;
        virtual void setFetchDirection(FetchDirection fetchDirection) = 0;
        virtual std::int32_t getFetchSize() const = 0;
        virtual void setFetchSize(std::int32_t fetchSize) = 0;
        virtual bool isClosed() const = 0;
        virtual std::unique_ptr<IStatement> getStatement() = 0;

        virtual bool isNull(std::int32_t columnIndex) const = 0;
        virtual bool isNull(std::string_view columnLabel) const = 0;
        virtual std::string getString(std::int32_t columnIndex) const = 0;
        virtual std::string getString(std::string_view columnLabel) const = 0;
        virtual std::int32_t getInt(std::int32_t columnIndex) const = 0;
        virtual std::int32_t getInt(std::string_view columnLabel) const = 0;
        virtual std::uint32_t getUInt(std::int32_t columnIndex) const = 0;
        virtual std::uint32_t getUInt(std::string_view columnLabel) const = 0;
        virtual std::int64_t getLong(std::string_view columnLabel) const = 0;
        virtual std::int64_t getLong(std::int32_t columnIndex) const = 0;
        virtual std::int64_t getInt64(std::string_view columnLabel) const = 0;
        virtual std::int64_t getInt64(std::int32_t columnIndex) const = 0;
        virtual std::uint64_t getUInt64(std::string_view columnLabel) const = 0;
        virtual std::uint64_t getUInt64(std::int32_t columnIndex) const = 0;
        virtual float getFloat(std::string_view columnLabel) const = 0;
        virtual float getFloat(std::int32_t columnIndex) const = 0;
        virtual long double getDouble(std::string_view columnLabel) const = 0;
        virtual long double getDouble(std::int32_t columnIndex) const = 0;
        virtual bool getBoolean(std::int32_t index) const = 0;
        virtual bool getBoolean(std::string_view columnLabel) const = 0;
        virtual std::byte getByte(std::int32_t index) const = 0;
        virtual std::byte getByte(std::string_view columnLabel) const = 0;
        virtual std::int16_t getShort(std::int32_t index) const = 0;
        virtual std::int16_t getShort(std::string_view columnLabel) const = 0;

        virtual std::int32_t findColumn(std::string_view columnLabel) const = 0;
    };
}