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

#include "Natives.hpp"
#include "../ExtSystem.hpp"

#include <vector>
#include <cstddef>

#include <mariadbsql/IDriver.hpp>
#include <mariadbsql/IConnection.hpp>
#include <mariadbsql/IStatement.hpp>
#include <mariadbsql/IResultSet.hpp>
#include <mariadbsql/IPreparedStatement.hpp>

std::vector<std::unique_ptr<Luna::MDBSQL::IConnection>> gConnections;
std::vector<std::unique_ptr<Luna::MDBSQL::IStatement>> gStatements;
std::vector<std::unique_ptr<Luna::MDBSQL::IResultSet>> gResultSets;

static int getDriver(lua_State *L)
{
    auto itExt = std::find_if(gExtList.begin(), gExtList.end(), [](const Luna::Extension &ext) {
        return ext.getName() == "MariaDB Extension";
    });

    if (itExt == gExtList.end())
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushlightuserdata(L, std::any_cast<nstd::observer_ptr<Luna::MDBSQL::IDriver>>(itExt->getImpl()).get());
    }

    return 1;
}

static int connect(lua_State *L)
{
    auto driver = reinterpret_cast<Luna::MDBSQL::IDriver *>(lua_touserdata(L, 1));

    if (!driver)
    {
        lua_pushnil(L);
    }
    else
    {
        size_t length;
        const char *host = luaL_checklstring(L, 2, &length);
        const char *user = luaL_checklstring(L, 3, &length);
        const char *pwd = luaL_checklstring(L, 4, &length);

        if (auto connection = driver->connect(host, user, pwd); connection)
        {
            auto &con = gConnections.emplace_back(std::move(connection));

            lua_pushlightuserdata(L, con.get());
        }
        else
        {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int disconnect(lua_State *L)
{
    auto conn = reinterpret_cast<Luna::MDBSQL::IConnection *>(lua_touserdata(L, 1));

    if (conn)
    {
        auto it = std::find_if(gConnections.begin(), gConnections.end(), [conn](const std::unique_ptr<Luna::MDBSQL::IConnection> &_conn) {
            return conn == _conn.get();
        });

        if (it != gConnections.end())
        {
            gConnections.erase(it);
        }
    }

    return 0;
}

static int isValidUrl(lua_State *L)
{
    auto driver = reinterpret_cast<Luna::MDBSQL::IDriver *>(lua_touserdata(L, 1));

    if (!driver)
    {
        lua_pushnil(L);
    }
    else
    {
        size_t length;
        const char *url = luaL_checklstring(L, 2, &length);
        lua_pushboolean(L, driver->acceptsURL(url));
    }

    return 1;
}

static int getDriverName(lua_State *L)
{
    auto driver = reinterpret_cast<Luna::MDBSQL::IDriver *>(lua_touserdata(L, 1));

    if (!driver)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushstring(L, driver->getName().data());
    }

    return 1;
}

static int createStatementInternal(lua_State *L, bool prepared)
{
    auto conn = reinterpret_cast<Luna::MDBSQL::IConnection *>(lua_touserdata(L, 1));

    if (!conn)
    {
        lua_pushnil(L);
    }
    else
    {
        const char *sql;
        if (prepared)
        {
            size_t length;
            sql = luaL_checklstring(L, 2, &length);
        }

        if (auto statement = (!prepared) ? conn->createStatement() : conn->prepareStatement(sql); statement)
        {
            auto &stmt = gStatements.emplace_back(std::move(statement));
            lua_pushlightuserdata(L, stmt.get());
        }
        else
        {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int createStatement(lua_State *L)
{
    return createStatementInternal(L, false);
}

static int destroyStatement(lua_State *L)
{
    auto stmt = reinterpret_cast<Luna::MDBSQL::IStatement *>(lua_touserdata(L, 1));

    if (stmt)
    {
        auto it = std::find_if(gStatements.begin(), gStatements.end(), [stmt](const std::unique_ptr<Luna::MDBSQL::IStatement> &_stmt) {
               return stmt == _stmt.get();
        });

        if (it != gStatements.end())
        {
            gStatements.erase(it);
        }
    }

    return 0;
}

static int executeStatement(lua_State *L)
{
    auto stmt = reinterpret_cast<Luna::MDBSQL::IStatement *>(lua_touserdata(L, 1));

    if (!stmt)
    {
        lua_pushnil(L);
    }
    else
    {
        const char *sql = nullptr;
        Luna::MDBSQL::IPreparedStatement *pStmt = nullptr;
        if (lua_gettop(L) == 2)
        {
            size_t length;
            sql = luaL_tolstring(L, 2, &length);
        }
        else
        {
            pStmt = dynamic_cast<Luna::MDBSQL::IPreparedStatement *>(stmt);
        }

        lua_pushboolean(L, sql ? stmt->execute(sql) : pStmt->execute());
    }

    return 1;
}

static int executeQueryStatement(lua_State *L)
{
    auto stmt = reinterpret_cast<Luna::MDBSQL::IStatement *>(lua_touserdata(L, 1));

    if (!stmt)
    {
        lua_pushnil(L);
    }
    else
    {
        const char *sql = nullptr;
        Luna::MDBSQL::IPreparedStatement *pStmt = nullptr;
        if (lua_gettop(L) == 2)
        {
            size_t length;
            sql = luaL_tolstring(L, 2, &length);
        }
        else
        {
            pStmt = dynamic_cast<Luna::MDBSQL::IPreparedStatement *>(stmt);
        }


        if (auto resultSet = (sql ? stmt->executeQuery(sql) : pStmt->executeQuery()); resultSet)
        {
            auto &result = gResultSets.emplace_back(std::move(resultSet));
            lua_pushlightuserdata(L, result.get());
        }
        else
        {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int executeUpdateStatementInternal(lua_State *L, bool large)
{
    auto stmt = reinterpret_cast<Luna::MDBSQL::IStatement *>(lua_touserdata(L, 1));

    if (!stmt)
    {
        lua_pushnil(L);
    }
    else
    {
        const char *sql = nullptr;
        Luna::MDBSQL::IPreparedStatement *pStmt = nullptr;
        if (lua_gettop(L) == 2)
        {
            size_t length;
            sql = luaL_tolstring(L, 2, &length);
        }
        else
        {
            pStmt = dynamic_cast<Luna::MDBSQL::IPreparedStatement *>(stmt);
        }

        if (auto result = (!large) ? (sql ? stmt->executeUpdate(sql) : pStmt->executeUpdate())
                                   : (sql ? stmt->executeLargeUpdate(sql) : pStmt->executeLargeUpdate()); result)
        {
            lua_pushinteger(L, result);
        }
        else
        {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int executeUpdateStatement(lua_State *L)
{
    return executeUpdateStatementInternal(L, false);
}

static int executeUpdateLargeStatement(lua_State *L)
{
    return executeUpdateStatementInternal(L, true);
}

static int statementClose(lua_State *L)
{
    auto stmt = reinterpret_cast<Luna::MDBSQL::IStatement *>(lua_touserdata(L, 1));

    if (stmt)
    {
        stmt->close();
    }

    return 0;
}

static int statementIsClosed(lua_State *L)
{
    auto stmt = reinterpret_cast<Luna::MDBSQL::IStatement *>(lua_touserdata(L, 1));

    if (stmt)
    {
        lua_pushboolean(L, stmt->isClosed());
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int createPreparedStatement(lua_State *L)
{
    return createStatementInternal(L, true);
}

enum class TypePStmt : std::uint8_t
{
    Bool = 0,
    Byte,
    Short,
    String,
    Int,
    Long,
    UInt64,
    UInt,
    Float,
    Double,
    BigInt,
    Null
};

static int preparedStatementInternal(lua_State *L, TypePStmt type)
{
    auto stmt = reinterpret_cast<Luna::MDBSQL::IStatement *>(lua_touserdata(L, 1));
    Luna::MDBSQL::IPreparedStatement *pStmt;
    try
    {
        pStmt = dynamic_cast<Luna::MDBSQL::IPreparedStatement *>(stmt);
        if (pStmt)
        {
            auto pos = static_cast<int32_t>(luaL_checkinteger(L, 2));
            switch (type)
            {
                case TypePStmt::Bool:
                {
                    pStmt->setBoolean(pos, lua_toboolean(L, 3));
                    break;
                }
                case TypePStmt::Byte:
                {
                    pStmt->setByte(pos, static_cast<std::byte>(lua_tointeger(L, 3)));
                    break;
                }
                case TypePStmt::Short:
                {
                    pStmt->setShort(pos, static_cast<std::int16_t>(lua_tointeger(L, 3)));
                    break;
                }
                case TypePStmt::String:
                {
                    size_t length;
                    const char *string = luaL_checklstring(L, 3, &length);
                    pStmt->setString(pos, string);
                    break;
                }
                case TypePStmt::Int:
                {
                    pStmt->setInt(pos, static_cast<std::int32_t>(lua_tointeger(L, 3)));
                    break;
                }
                case TypePStmt::Long:
                {
                    pStmt->setLong(pos, lua_tointeger(L, 3));
                    break;
                }
                case TypePStmt::UInt64:
                {
                    pStmt->setUInt64(pos, static_cast<std::uint64_t>(lua_tointeger(L, 3)));
                    break;
                }
                case TypePStmt::UInt:
                {
                    pStmt->setUInt(pos, static_cast<std::uint32_t>(lua_tointeger(L, 3)));
                    break;
                }
                case TypePStmt::Float:
                {
                    pStmt->setFloat(pos, static_cast<float>(lua_tonumber(L, 3)));
                    break;
                }
                case TypePStmt::Double:
                {
                    pStmt->setDouble(pos, lua_tonumber(L, 3));
                    break;
                }
                case TypePStmt::BigInt:
                {
                    size_t length;
                    const char *bigInt = luaL_checklstring(L, 3, &length);
                    pStmt->setBigInt(pos, bigInt);
                    break;
                }
                default:
                    break;
            }

        }
    }
    catch (const std::exception &e)
    {
    }

    return 0;
}

static int preparedStatementBool(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::Bool);
}

static int preparedStatementByte(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::Byte);
}

static int preparedStatementShort(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::Short);
}

static int preparedStatementString(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::String);
}

static int preparedStatementInt(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::Int);
}

static int preparedStatementLong(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::Long);
}

static int preparedStatementUInt64(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::UInt64);
}

static int preparedStatementUInt(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::UInt);
}

static int preparedStatementFloat(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::Float);
}

static int preparedStatementDouble(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::Double);
}

static int preparedStatementBigInt(lua_State *L)
{
    return preparedStatementInternal(L, TypePStmt::BigInt);
}

static int escapeProcessing(lua_State *L)
{
    auto stmt = reinterpret_cast<Luna::MDBSQL::IStatement *>(lua_touserdata(L, 1));

    if (stmt)
    {
        stmt->setEscapeProcessing(lua_toboolean(L, 2));
    }

    return 0;
}

static int resultSetNext(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));

    if (rs)
    {
        lua_pushboolean(L, rs->next());
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int isFirstInternal(lua_State *L, bool before)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));

    if (rs)
    {
        lua_pushboolean(L, (!before) ? rs->isFirst() : rs->isBeforeFirst());
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int resultSetIsFirst(lua_State *L)
{
    return isFirstInternal(L, false);
}

static int resultSetIsBeforeFirst(lua_State *L)
{
    return isFirstInternal(L, true);
}

static int isLastInternal(lua_State *L, bool after)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));

    if (rs)
    {
        lua_pushboolean(L, (!after) ? rs->isLast() : rs->isAfterLast());
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int resultSetIsLast(lua_State *L)
{
    return isLastInternal(L, false);
}

static int resultSetIsAfterLast(lua_State *L)
{
    return isLastInternal(L, true);
}

static int resultSetBeforeFirst(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));
    if (rs)
    {
        rs->beforeFirst();
    }

    return 0;
}

static int resultSetAfterLast(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));
    if (rs)
    {
        rs->afterLast();
    }

    return 0;
}

static int resultSetFirst(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));
    if (rs)
    {
        lua_pushboolean(L, rs->first());
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int resultSetLast(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));
    if (rs)
    {
        lua_pushboolean(L, rs->last());
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int resultSetGetRow(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));
    if (rs)
    {
        lua_pushinteger(L, rs->getRow());
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int resultSetPrevious(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));
    if (rs)
    {
        lua_pushboolean(L, rs->previous());
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int resultSetGetFetchDirection(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));
    if (rs)
    {
        lua_pushnumber(L, rs->getFetchDirection());
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int resultSetSetFetchDirection(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));
    if (rs)
    {
        rs->setFetchDirection(static_cast<Luna::MDBSQL::IResultSet::FetchDirection>(lua_tonumber(L, 2)));
    }

    return 0;
}

static int resultSetGet(lua_State *L, TypePStmt type, bool name)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));
    if (rs)
    {
        std::int32_t pos;
        const char *colName;
        if (!name)
        {
            pos = static_cast<int32_t>(luaL_checkinteger(L, 2));
        }
        else
        {
            size_t length;
            colName = luaL_checklstring(L, 2, &length);
        }

        switch (type)
        {
            case TypePStmt::Bool:
            {
                lua_pushboolean(L, (!name) ? rs->getBoolean(pos) : rs->getBoolean(colName));
                break;
            }
            case TypePStmt::Byte:
            {
                lua_pushinteger(L, (!name) ? std::to_integer<lua_Integer>(rs->getByte(pos)) : std::to_integer<lua_Integer>(rs->getByte(colName)));
                break;
            }
            case TypePStmt::Short:
            {
                lua_pushinteger(L, (!name) ? rs->getShort(pos) : rs->getShort(colName));
                break;
            }
            case TypePStmt::String:
            {
                lua_pushstring(L, (!name) ? rs->getString(pos).c_str() : rs->getString(colName).c_str());
                break;
            }
            case TypePStmt::Int:
            {
                lua_pushinteger(L, (!name) ? rs->getInt(pos) : rs->getInt(colName));
                break;
            }
            case TypePStmt::Long:
            {
                lua_pushinteger(L, (!name) ? rs->getLong(pos) : rs->getLong(colName));
                break;
            }
            case TypePStmt::UInt64:
            {
                lua_pushinteger(L, (!name) ? static_cast<lua_Integer>(rs->getUInt64(pos)) : static_cast<lua_Integer>(rs->getUInt64(colName)));
                break;
            }
            case TypePStmt::UInt:
            {
                lua_pushinteger(L, (!name) ? static_cast<lua_Integer>(rs->getUInt(pos)) : static_cast<lua_Integer>(rs->getUInt(colName)));
                break;
            }
            case TypePStmt::Float:
            {
                lua_pushnumber(L, (!name) ? rs->getFloat(pos) : rs->getFloat(colName));
                break;
            }
            case TypePStmt::Double:
            {
                lua_pushnumber(L, (!name) ? rs->getDouble(pos) : rs->getDouble(colName));
                break;
            }
            case TypePStmt::Null:
            {
                lua_pushboolean(L, (!name) ? rs->isNull(pos) : rs->isNull(colName));
                break;
            }
            default:
                break;
        }

    }

    return 1;
}

static int resultSetFindColumn(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));
    if (rs)
    {
        size_t length;
        const char *name = luaL_checklstring(L, 2, &length);
        lua_pushinteger(L, rs->findColumn(name));
    }

    return 1;
}

static int SQLResultSetIsNullById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Null, false);
}

static int SQLResultSetIsNullByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Null, true);
}

static int SQLResultSetGetStringById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::String, false);
}

static int SQLResultSetGetStringByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::String, true);
}

static int SQLResultSetGetIntById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Int, false);
}

static int SQLResultSetGetIntByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Int, true);
}

static int SQLResultSetGetUIntById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::UInt, false);
}

static int SQLResultSetGetUIntByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::UInt, true);
}

static int SQLResultSetGetLongById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Long, false);
}

static int SQLResultSetGetLongByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Long, true);
}

static int SQLResultSetGetUInt64ById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::UInt64, false);
}

static int SQLResultSetGetUInt64ByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::UInt64, true);
}

static int SQLResultSetGetFloatById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Float, false);
}

static int SQLResultSetGetFloatByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Float, true);
}

static int SQLResultSetGetDoubleById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Double, false);
}

static int SQLResultSetGetDoubleByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Double, true);
}

static int SQLResultSetGetBoolById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Bool, false);
}

static int SQLResultSetGetBoolByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Bool, true);
}

static int SQLResultSetGetByteById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Byte, false);
}

static int SQLResultSetGetByteByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Byte, true);
}

static int SQLResultSetGetShortById(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Short, false);
}

static int SQLResultSetGetShortByName(lua_State *L)
{
    return resultSetGet(L, TypePStmt::Short, true);
}

static int resultSetDestroy(lua_State *L)
{
    auto rs = reinterpret_cast<Luna::MDBSQL::IResultSet *>(lua_touserdata(L, 1));

    if (rs)
    {
        auto it = std::find_if(gResultSets.begin(), gResultSets.end(), [rs](const std::unique_ptr<Luna::MDBSQL::IResultSet> &_rs) {
                                   return rs == _rs.get();
                               });

        if (it != gResultSets.end())
        {
            gResultSets.erase(it);
        }
    }

    return 0;
}

LuaAdapterCFunction gSQLNatives[] = {
    {"getSQLDriver", getDriver},
    {"getSQLDriverName", getDriverName},
    {"isSQLValidUrl", isValidUrl},
    {"SQLConnect", connect},
    {"SQLDisconnect", disconnect},
    {"SQLCreateStatement", createStatement},
    {"SQLDestroyStatement", destroyStatement},
    {"SQLStatementExecute", executeStatement},
    {"SQLStatementExecuteQuery", executeQueryStatement},
    {"SQLStatementExecuteUpdate", executeUpdateStatement},
    {"SQLStatementExecuteUpdateLarge", executeUpdateLargeStatement},
    {"SQLStatementClose", statementClose},
    {"SQLStatementIsClosed", statementIsClosed},
    {"SQLCreatePreparedStatement", createPreparedStatement},
    {"SQLPreparedStatementSetBool", preparedStatementBool},
    {"SQLPreparedStatementSetByte", preparedStatementByte},
    {"SQLPreparedStatementSetShort", preparedStatementShort},
    {"SQLPreparedStatementSetString", preparedStatementString},
    {"SQLPreparedStatementSetInt", preparedStatementInt},
    {"SQLPreparedStatementSetLong", preparedStatementLong},
    {"SQLPreparedStatementSetUInt64", preparedStatementUInt64},
    {"SQLPreparedStatementSetUInt", preparedStatementUInt},
    {"SQLPreparedStatementSetFloat", preparedStatementFloat},
    {"SQLPreparedStatementSetDouble", preparedStatementDouble},
    {"SQLPreparedStatementSetBigInt", preparedStatementBigInt},
    {"SQLStatementEscapeProcessing", escapeProcessing},
    {"SQLResultSetNext", resultSetNext},
    {"SQLResultSetIsFirst", resultSetIsFirst},
    {"SQLResultSetIsBeforeFirst", resultSetIsBeforeFirst},
    {"SQLResultSetIsLast", resultSetIsLast},
    {"SQLResultSetIsAfterLast", resultSetIsAfterLast},
    {"SQLResultSetBeforeFirst", resultSetBeforeFirst},
    {"SQLResultSetAfterLast", resultSetAfterLast},
    {"SQLResultSetFirst", resultSetFirst},
    {"SQLResultSetLast", resultSetLast},
    {"SQLResultGetRow", resultSetGetRow},
    {"SQLResultPrevious", resultSetPrevious},
    {"SQLResultGetFetchDirection", resultSetGetFetchDirection},
    {"SQLResultSetFetchDirection", resultSetSetFetchDirection},
    {"SQLResultSetGetBoolByName", SQLResultSetGetBoolByName},
    {"SQLResultSetGetBoolById", SQLResultSetGetBoolById},
    {"SQLResultSetGetByteByName", SQLResultSetGetByteByName},
    {"SQLResultSetGetByteById", SQLResultSetGetByteById},
    {"SQLResultSetGetShortByName", SQLResultSetGetShortByName},
    {"SQLResultSetGetShortById", SQLResultSetGetShortById},
    {"SQLResultSetGetStringByName", SQLResultSetGetStringByName},
    {"SQLResultSetGetStringById", SQLResultSetGetStringById},
    {"SQLResultSetGetIntByName", SQLResultSetGetIntByName},
    {"SQLResultSetGetIntById", SQLResultSetGetIntById},
    {"SQLResultSetGetLongByName", SQLResultSetGetLongByName},
    {"SQLResultSetGetLongById", SQLResultSetGetLongById},
    {"SQLResultSetGetUInt64ByName", SQLResultSetGetUInt64ByName},
    {"SQLResultSetGetUInt64ById", SQLResultSetGetUInt64ById},
    {"SQLResultSetGetUIntByName", SQLResultSetGetUIntByName},
    {"SQLResultSetGetUIntById", SQLResultSetGetUIntById},
    {"SQLResultSetGetFloatByName", SQLResultSetGetFloatByName},
    {"SQLResultSetGetFloatById", SQLResultSetGetFloatById},
    {"SQLResultSetGetDoubleByName", SQLResultSetGetDoubleByName},
    {"SQLResultSetGetDoubleById", SQLResultSetGetDoubleById},
    {"SQLResultSetIsNullByName", SQLResultSetIsNullByName},
    {"SQLResultSetIsNullById", SQLResultSetIsNullById},
    {"SQLResultSetFindColumn", resultSetFindColumn},
    {"SQLResultSetDestroy", resultSetDestroy},
    {nullptr, nullptr},
};