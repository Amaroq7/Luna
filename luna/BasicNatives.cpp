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

#include "BasicNatives.hpp"
#include "AnubisExports.hpp"
#include <game/IHooks.hpp>
#include <engine/IEdict.hpp>
#include <fmt/format.h>
#include <game/cstrike/IHooks.hpp>
#include "PluginSystem.hpp"
#include "TimerSystem.hpp"

#include <functional>
#include <unordered_map>

static std::unordered_map<std::string, std::function<void()>> gSrvCommands;

static std::string *gConnectReason;

static int enginePrint(lua_State *L)
{
    std::size_t length;
    const char *msg = luaL_checklstring(L, 1, &length);
    gEngine->print(msg, Anubis::FuncCallType::Direct);
    return 0;
}

static int callNext(lua_State *L)
{
    std::size_t length;
    auto type = static_cast<GameHooks>(luaL_checkinteger(L, 1));

    switch (type)
    {
        case GameHooks::ClientConnect:
        {
            auto hook = reinterpret_cast<Anubis::Game::IClientConnectHook *>(lua_touserdata(L, 2));
            auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 3));
            std::string_view name = luaL_checklstring(L, 4, &length);
            std::string_view ip = luaL_checklstring(L, 5, &length);
            std::string_view reason = luaL_checklstring(L, 6, &length);

            *gConnectReason = reason;

            bool result = hook->callNext(edict, name, ip, gConnectReason);

            lua_pushboolean(L, result);
            return 1;
        }
        case GameHooks::ClientCmd:
        {
            auto hook = reinterpret_cast<Anubis::Game::IClientCmdHook *>(lua_touserdata(L, 2));
            auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 3));
            hook->callNext(edict);
            break;
        }
        case GameHooks::ClientInfoChanged:
        {
            auto hook = reinterpret_cast<Anubis::Game::IClientInfoChangedHook *>(lua_touserdata(L, 2));
            auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 3));
            auto infoBuffer = reinterpret_cast<Anubis::Engine::InfoBuffer *>(lua_touserdata(L, 4));
            hook->callNext(edict, *infoBuffer);
            break;
        }
        case GameHooks::RoundEnd:
        {
            if (!gGame->getHooks()->CSHooks())
            {
                return 0;
            }

            auto hook = reinterpret_cast<Anubis::Game::CStrike::IRoundEndHook *>(lua_touserdata(L, 2));
            auto winStatus = static_cast<Anubis::Game::CStrike::WinStatus>(lua_tointeger(L, 3));
            auto event = static_cast<Anubis::Game::CStrike::ScenarioEventEndRound>(lua_tointeger(L, 4));
            auto delay = static_cast<float>(lua_tonumber(L, 5));
            hook->callNext(winStatus, event, delay);
            break;
        }
        case GameHooks::OnFreezeEnd:
        {
            if (!gGame->getHooks()->CSHooks())
            {
                return 0;
            }

            auto hook = reinterpret_cast<Anubis::Game::CStrike::IRoundFreezeEndHook *>(lua_touserdata(L, 2));
            hook->callNext();
            break;
        }

        default:
            break;
    }

    return 0;
}

static int callOriginal(lua_State *L)
{
    std::size_t length;
    auto type = static_cast<GameHooks>(luaL_checkinteger(L, 1));

    switch (type)
    {
        case GameHooks::ClientConnect:
        {
            auto hook = reinterpret_cast<Anubis::Game::IClientConnectHook *>(lua_touserdata(L, 2));
            auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 3));
            std::string_view name = luaL_checklstring(L, 4, &length);
            std::string_view ip = luaL_checklstring(L, 5, &length);
            bool result = hook->callOriginal(edict, name, ip, gConnectReason);

            lua_pushboolean(L, result);
            return 1;
        }
        case GameHooks::ClientCmd:
        {
            auto hook = reinterpret_cast<Anubis::Game::IClientCmdHook *>(lua_touserdata(L, 2));
            auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 3));
            hook->callOriginal(edict);
            break;
        }
        case GameHooks::ClientInfoChanged:
        {
            auto hook = reinterpret_cast<Anubis::Game::IClientInfoChangedHook *>(lua_touserdata(L, 2));
            auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 3));
            auto infoBuffer = reinterpret_cast<Anubis::Engine::InfoBuffer *>(lua_touserdata(L, 4));
            hook->callOriginal(edict, *infoBuffer);
            break;
        }
        case GameHooks::RoundEnd:
        {
            if (!gGame->getHooks()->CSHooks())
            {
                return 0;
            }

            auto hook = reinterpret_cast<Anubis::Game::CStrike::IRoundEndHook *>(lua_touserdata(L, 2));
            auto winStatus = static_cast<Anubis::Game::CStrike::WinStatus>(lua_tointeger(L, 3));
            auto event = static_cast<Anubis::Game::CStrike::ScenarioEventEndRound>(lua_tointeger(L, 4));
            auto delay = static_cast<float>(lua_tonumber(L, 5));
            hook->callOriginal(winStatus, event, delay);
            break;
        }
        case GameHooks::OnFreezeEnd:
        {
            if (!gGame->getHooks()->CSHooks())
            {
                return 0;
            }

            auto hook = reinterpret_cast<Anubis::Game::CStrike::IRoundFreezeEndHook *>(lua_touserdata(L, 2));
            hook->callOriginal();
            break;
        }

        default:
            break;
    }

    return 0;
}

static int gameFnHook(lua_State *L)
{
    std::size_t length;
    auto type = static_cast<GameHooks>(luaL_checkinteger(L, 1));
    const char *fnName = luaL_checklstring(L, 2, &length);
    auto hookPriority = static_cast<Anubis::HookPriority>(luaL_checkinteger(L, 3));
    nstd::observer_ptr<Anubis::IHookInfo> hookInfo;
    std::string fName {fnName};

    switch (type)
    {
        case GameHooks::ClientConnect:
            hookInfo = gGame->getHooks()->clientConnect()->registerHook(
                [L, fName]
                (const nstd::observer_ptr<Anubis::Game::IClientConnectHook> &hook,
                 nstd::observer_ptr<Anubis::Engine::IEdict> pEdict,
                 std::string_view name, std::string_view ip, nstd::observer_ptr<std::string> reason)
                 {
                    if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
                    {
                        return hook->callNext(pEdict, name, ip, reason);
                    }

                    lua_pushlightuserdata(L, hook.get());
                    lua_pushlightuserdata(L, pEdict.get());
                    lua_pushstring(L, name.data());
                    lua_pushstring(L, ip.data());
                    lua_pushstring(L, reason->c_str());
                    gConnectReason = reason.get();

                    if (lua_pcall(L, 5, 1, 0) != LUA_OK)
                    {
                        return hook->callNext(pEdict, name, ip, reason);
                    }

                    if (!lua_isboolean(L, -1))
                    {
                        return hook->callNext(pEdict, name, ip, reason);
                    }

                    bool result = lua_toboolean(L, -1);
                    lua_pop(L, 1);

                    return result;
                 }, hookPriority);
            break;

        case GameHooks::ClientCmd:
        {
            hookInfo = gGame->getHooks()->clientCmd()->registerHook(
                [L, fName]
                (const nstd::observer_ptr<Anubis::Game::IClientCmdHook> &hook,
                 nstd::observer_ptr<Anubis::Engine::IEdict> pEdict)
                {
                    if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
                    {
                        return hook->callNext(pEdict);
                    }

                    lua_pushlightuserdata(L, hook.get());
                    lua_pushlightuserdata(L, pEdict.get());

                    if (lua_pcall(L, 2, 0, 0) != LUA_OK)
                    {
                        return hook->callNext(pEdict);
                    }
                }, hookPriority);
            break;
        }

        case GameHooks::ClientInfoChanged:
        {
            hookInfo = gGame->getHooks()->clientInfoChanged()->registerHook(
                [L, fName]
                (const nstd::observer_ptr<Anubis::Game::IClientInfoChangedHook> &hook,
                 nstd::observer_ptr<Anubis::Engine::IEdict> pEdict, Anubis::Engine::InfoBuffer infoBuffer)
                {
                    if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
                    {
                        return hook->callNext(pEdict, infoBuffer);
                    }

                    lua_pushlightuserdata(L, hook.get());
                    lua_pushlightuserdata(L, pEdict.get());
                    lua_pushlightuserdata(L, &infoBuffer);

                    if (lua_pcall(L, 3, 0, 0) != LUA_OK)
                    {
                        return hook->callNext(pEdict, infoBuffer);
                    }
                }, hookPriority);
            break;
        }

        case GameHooks::RoundEnd:
        {
            if (!gGame->getHooks()->CSHooks())
            {
                lua_pushnil(L);
                return 1;
            }

            hookInfo = gGame->getHooks()->CSHooks()->roundEnd()->registerHook(
                [L, fName]
                (const nstd::observer_ptr<Anubis::Game::CStrike::IRoundEndHook> &hook, Anubis::Game::CStrike::WinStatus winStatus,
                 Anubis::Game::CStrike::ScenarioEventEndRound event, float delay)
                {
                    if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
                    {
                        return hook->callNext(winStatus, event, delay);
                    }

                    lua_pushlightuserdata(L, hook.get());
                    lua_pushinteger(L, static_cast<lua_Integer>(winStatus));
                    lua_pushinteger(L, static_cast<lua_Integer>(event));
                    lua_pushnumber(L, delay);

                    if (lua_pcall(L, 4, 0, 0) != LUA_OK)
                    {
                        return hook->callNext(winStatus, event, delay);
                    }

                    if (!lua_isboolean(L, -1))
                    {
                        return hook->callNext(winStatus, event, delay);
                    }

                    bool result = lua_toboolean(L, -1);
                    lua_pop(L, 1);

                    return result;
                }, hookPriority);
            break;
        }

        case GameHooks::OnFreezeEnd:
        {
            if (!gGame->getHooks()->CSHooks())
            {
                lua_pushnil(L);
                return 1;
            }

            hookInfo = gGame->getHooks()->CSHooks()->freezeTimeEnd()->registerHook(
                [L, fName]
                (const nstd::observer_ptr<Anubis::Game::CStrike::IRoundFreezeEndHook> &hook)
                {
                    if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
                    {
                        return hook->callNext();
                    }

                    if (lua_pcall(L, 0, 0, 0) != LUA_OK)
                    {
                        return hook->callNext();
                    }
                }, hookPriority);
            break;
        }

        default:
            break;
    }

    lua_pushlightuserdata(L, hookInfo.get());
    return 1;
}

static int gameFnUnhook(lua_State *L)
{
    auto type = static_cast<GameHooks>(luaL_checkinteger(L, 1));

    switch (type)
    {
        case GameHooks::ClientConnect:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            gGame->getHooks()->clientConnect()->unregisterHook(hookInfo);
            break;
        }
        case GameHooks::ClientCmd:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            gGame->getHooks()->clientCmd()->unregisterHook(hookInfo);
            break;
        }
        case GameHooks::ClientInfoChanged:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            gGame->getHooks()->clientInfoChanged()->unregisterHook(hookInfo);
            break;
        }
        case GameHooks::RoundEnd:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            gGame->getHooks()->CSHooks()->roundEnd()->unregisterHook(hookInfo);
            break;
        }
        case GameHooks::OnFreezeEnd:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            gGame->getHooks()->CSHooks()->freezeTimeEnd()->unregisterHook(hookInfo);
            break;
        }

        default:
            break;
    }

    return 0;
}

static void srvCommand()
{
    for (const auto &[key, func] : gSrvCommands)
    {
        std::string_view cmdName = gEngine->cmdArgv(0, Anubis::FuncCallType::Direct);
        if (cmdName != key)
        {
            continue;
        }

        func();
        break;
    }
}

static int addSrvCommand(lua_State *L)
{
    std::size_t length;
    const char *cmdName = luaL_checklstring(L, 1, &length);
    const char *fnName = luaL_checklstring(L, 2, &length);

    std::string fName {fnName};

    const auto &[iter, inserted] = gSrvCommands.try_emplace(cmdName, [L, fName] {
         if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
         {
             return;
         }

         if (lua_pcall(L, 0, 0, 0) != LUA_OK)
         {
             return;
         }
    });

    if (!inserted)
    {
        lua_pushboolean(L, 0);
    }
    else
    {
        gEngine->registerSrvCommand(cmdName, srvCommand, Anubis::FuncCallType::Direct);
        lua_pushboolean(L, 1);
    }

    return 1;
}

static int rmvSrvCommand(lua_State *L)
{
    std::size_t length;
    const char *cmdName = luaL_checklstring(L, 1, &length);

    gEngine->removeCmd(cmdName);
    gSrvCommands.erase(cmdName);

    return 0;
}

static int cmdArgv(lua_State *L)
{
    lua_Integer argv = luaL_checkinteger(L, 1);
    std::string_view result = gEngine->cmdArgv(static_cast<std::uint8_t>(argv), Anubis::FuncCallType::Direct);

    (result.data()) ? lua_pushstring(L, result.data()) : lua_pushstring(L, "");
    return 1;
}

static int cmdArgc(lua_State *L)
{
    std::uint8_t result = gEngine->cmdArgc(Anubis::FuncCallType::Direct);

    lua_pushinteger(L, static_cast<lua_Integer>(result));
    return 1;
}

static int cmdArgs(lua_State *L)
{
    std::string_view result = gEngine->cmdArgs(Anubis::FuncCallType::Direct);

    (result.data()) ? lua_pushstring(L, result.data()) : lua_pushstring(L, "");
    return 1;
}

static int infoKeyValue(lua_State *L)
{
    auto infoBuffer = reinterpret_cast<Anubis::Engine::InfoBuffer *>(lua_touserdata(L, 1));
    std::size_t length;
    const char *keyName = luaL_checklstring(L, 2, &length);

    std::string_view result = gEngine->infoKeyValue(*infoBuffer, keyName, Anubis::FuncCallType::Direct);

    lua_pushstring(L, result.data());
    return 1;
}

static int clientPrint(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto printType = static_cast<Anubis::Engine::PrintType>(luaL_checkinteger(L, 2));
    std::size_t length;
    const char *msg = luaL_checklstring(L, 3, &length);

    gEngine->clientPrint(edict, printType, msg, Anubis::FuncCallType::Direct);

    return 0;
}

static int execFunc(lua_State *L)
{
    size_t length;
    const char *name = luaL_checklstring(L, 1, &length);

    lua_pop(L, 1);

    int paramsNum = lua_gettop(L);

    for (const auto &plugin : gPluginSystem->getPlugins())
    {
        lua_State *otherPl = plugin->getState().get();

        if (lua_getglobal(otherPl, name) == LUA_TNIL)
        {
            continue;
        }

        for (int i = 1; i <= paramsNum; i++)
        {
            switch (lua_type(L, i))
            {
                case LUA_TNIL:
                {
                    lua_pushnil(otherPl);
                    break;
                }
                case LUA_TBOOLEAN:
                {
                    lua_pushboolean(otherPl, lua_toboolean(L, i));
                    break;
                }
                case LUA_TNUMBER:
                {
                    lua_pushnumber(otherPl, lua_tonumber(L, i));
                    break;
                }
                case LUA_TSTRING:
                {
                    size_t len;
                    const char *str = lua_tolstring(L, i, &len);
                    lua_pushlstring(otherPl, str, len);
                    break;
                }
                default:
                    break;
            }
        }

        lua_pcall(otherPl, paramsNum, 0, 0);
    }

    return 0;
}

static int createTimer(lua_State *L)
{
    std::size_t length;
    auto interval = static_cast<float>(lua_tonumber(L, 1));
    const char *fnName = luaL_checklstring(L, 2, &length);
    std::any data;
    auto repeat = static_cast<bool>(lua_toboolean(L, 4));
    auto execNow = static_cast<bool>(lua_toboolean(L, 5));

    switch (lua_type(L, 3))
    {
        case LUA_TBOOLEAN:
        {
            data = static_cast<bool>(lua_toboolean(L, 3));
            break;
        }
        case LUA_TNUMBER:
        {
            data = lua_tonumber(L, 3);
            break;
        }
        case LUA_TSTRING:
        {
            std::string tempData;
            size_t len;

            const char *str = lua_tolstring(L, 3, &len);
            tempData = str;

            data = std::move(tempData);
            break;
        }
        default:
            break;
    }

    std::string fName {fnName};

    gTimers.emplace_back(interval, [L, fName](std::any data) {
         if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
         {
             return false;
         }

         if (data.type() == typeid(bool))
         {
             lua_pushboolean(L, std::any_cast<bool>(data));
         }
         else if (data.type() == typeid(double))
         {
             lua_pushnumber(L, std::any_cast<double>(data));
         }
         else if (data.type() == typeid(std::string))
         {
             lua_pushstring(L, std::any_cast<std::string>(data).c_str());
         }
         else
         {
             lua_pushnil(L);
         }

         if (lua_pcall(L, 1, 1, 0) != LUA_OK)
         {
             return false;
         }

         auto result = static_cast<bool>(lua_toboolean(L, -1));
         lua_pop(L, 1);

         return result;
    }, std::move(data), repeat, execNow);

    return 0;
}

LuaAdapterCFunction gBasicNatives[] = {
    {"enginePrint", enginePrint},
    {"gameFnHook", gameFnHook},
    {"callNext", callNext},
    {"callOriginal", callOriginal},
    {"gameFnUnhook", gameFnUnhook},
    {"addSrvCommand", addSrvCommand},
    {"rmvSrvCommand", rmvSrvCommand},
    {"cmdArgv", cmdArgv},
    {"cmdArgc", cmdArgc},
    {"cmdArgs", cmdArgs},
    {"infoKeyValue", infoKeyValue},
    {"clientPrint", clientPrint},
    {"execFunc", execFunc},
    {"createTimer", createTimer},
    {nullptr, nullptr}
};