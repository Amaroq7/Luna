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

#include "ClassNatives.hpp"
#include "AnubisExports.hpp"
#include <game/IHooks.hpp>
#include <engine/IEdict.hpp>
#include <game/IEntitiesHooks.hpp>
#include <engine/ITraceResult.hpp>
#include <game/IBaseEntity.hpp>
#include <game/IBasePlayer.hpp>
#include "ClassHandler.hpp"

static float *gFlTakeDamage;

static int playerClassCall(lua_State *L, bool original)
{
    auto type = static_cast<PlayerClassHooks>(luaL_checkinteger(L, 1));

    switch (type)
    {
        case PlayerClassHooks::Spawn:
        {
            auto hook = reinterpret_cast<Anubis::Game::IBasePlayerSpawnHook *>(lua_touserdata(L, 2));
            auto player = reinterpret_cast<Anubis::Game::IBasePlayer *>(lua_touserdata(L, 3));
            original ? hook->callOriginal(player) : hook->callNext(player);

            break;
        }
        case PlayerClassHooks::TakeDamage:
        {
            auto hook = reinterpret_cast<Anubis::Game::IBasePlayerTakeDamageHook *>(lua_touserdata(L, 2));
            auto player = reinterpret_cast<Anubis::Game::IBasePlayer *>(lua_touserdata(L, 3));
            auto inflictor = reinterpret_cast<Anubis::Game::IBaseEntity *>(lua_touserdata(L, 4));
            auto attacker = reinterpret_cast<Anubis::Game::IBaseEntity *>(lua_touserdata(L, 5));
            auto dmg = static_cast<float>(luaL_checknumber(L, 6));

            *gFlTakeDamage = dmg;

            auto dmgType = static_cast<Anubis::Game::DmgType>(luaL_checkinteger(L, 7));
            bool result = original ? hook->callOriginal(player, inflictor, attacker, *gFlTakeDamage, dmgType) :
                                    hook->callNext(player, inflictor, attacker, *gFlTakeDamage, dmgType);

            lua_pushboolean(L, static_cast<int>(result));
            return 1;
        }
        case PlayerClassHooks::TraceAttack:
        {
            static std::unique_ptr<Anubis::Engine::ITraceResult> tempTr;
            std::ignore = tempTr.release();

            auto hook = reinterpret_cast<Anubis::Game::IBasePlayerTraceAttackHook *>(lua_touserdata(L, 2));
            auto player = reinterpret_cast<Anubis::Game::IBasePlayer *>(lua_touserdata(L, 3));
            auto attacker = reinterpret_cast<Anubis::Game::IBaseEntity *>(lua_touserdata(L, 4));
            auto dmg = static_cast<float>(luaL_checknumber(L, 5));
            auto vecDir = reinterpret_cast<float *>(lua_touserdata(L, 6));
            tempTr.reset(reinterpret_cast<Anubis::Engine::ITraceResult *>(lua_touserdata(L, 7)));
            auto dmgType = static_cast<Anubis::Game::DmgType>(luaL_checkinteger(L, 8));
            original ? hook->callOriginal(player, attacker, dmg, vecDir, tempTr, dmgType) :
                        hook->callNext(player, attacker, dmg, vecDir, tempTr, dmgType);

            break;
        }
        case PlayerClassHooks::Killed:
        {
            auto hook = reinterpret_cast<Anubis::Game::IBasePlayerKilledHook *>(lua_touserdata(L, 2));
            auto player = reinterpret_cast<Anubis::Game::IBasePlayer *>(lua_touserdata(L, 3));
            auto attacker = reinterpret_cast<Anubis::Game::IBaseEntity *>(lua_touserdata(L, 4));
            auto gibType = static_cast<Anubis::Game::GibType>(luaL_checkinteger(L, 5));
            original ? hook->callOriginal(player, attacker, gibType) : hook->callNext(player, attacker, gibType);

            break;
        }

        default:
            break;
    }

    return 0;
}

static int playerClassCallNext(lua_State *L)
{
    return playerClassCall(L, false);
}

static int playerClassCallOriginal(lua_State *L)
{
    return playerClassCall(L, true);
}

static int playerClassFnHook(lua_State *L)
{
    std::size_t length;
    auto type = static_cast<PlayerClassHooks>(luaL_checkinteger(L, 1));
    const char *fnName = luaL_checklstring(L, 2, &length);
    auto hookPriority = static_cast<Anubis::HookPriority>(luaL_checkinteger(L, 3));
    nstd::observer_ptr<Anubis::IHookInfo> hookInfo;
    std::string fName {fnName};

    switch (type)
    {
        case PlayerClassHooks::Spawn:
        {
            hookInfo = gGame->getCBasePlayerHooks()->spawn()->registerHook(
                [L, fName](const nstd::observer_ptr<Anubis::Game::IBasePlayerSpawnHook> &hook,
                          nstd::observer_ptr<Anubis::Game::IBasePlayer> player)
                {
                    if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
                    {
                        return hook->callNext(player);
                    }

                    lua_pushlightuserdata(L, hook.get());
                    lua_pushlightuserdata(L, player.get());

                    if (lua_pcall(L, 2, 0, 0) != LUA_OK)
                    {
                        return hook->callNext(player);
                    }
                },
                hookPriority);
            break;
        }
        case PlayerClassHooks::TakeDamage:
        {
            hookInfo = gGame->getCBasePlayerHooks()->takeDamage()->registerHook(
                [L, fName]
                (const nstd::observer_ptr<Anubis::Game::IBasePlayerTakeDamageHook> &hook,
                nstd::observer_ptr<Anubis::Game::IBasePlayer> player,
                nstd::observer_ptr<Anubis::Game::IBaseEntity> inflictor,
                nstd::observer_ptr<Anubis::Game::IBaseEntity> attacker,
                float &dmg,
                Anubis::Game::DmgType dmgType)
                {
                    if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
                    {
                        return hook->callNext(player, inflictor, attacker, dmg, dmgType);
                    }

                    lua_pushlightuserdata(L, hook.get());
                    lua_pushlightuserdata(L, player.get());
                    lua_pushlightuserdata(L, inflictor.get());
                    lua_pushlightuserdata(L, attacker.get());
                    lua_pushnumber(L, dmg);
                    lua_pushinteger(L, static_cast<lua_Integer>(dmgType));

                    gFlTakeDamage = &dmg;

                    if (lua_pcall(L, 6, 1, 0) != LUA_OK)
                    {
                        return hook->callNext(player, inflictor, attacker, dmg, dmgType);
                    }

                    bool result = lua_toboolean(L, -1);
                    lua_pop(L, 1);

                    return result;
                }, hookPriority);
            break;
        }
        case PlayerClassHooks::TraceAttack:
        {
            hookInfo = gGame->getCBasePlayerHooks()->traceAttack()->registerHook(
                [L, fName]
                (const nstd::observer_ptr<Anubis::Game::IBasePlayerTraceAttackHook> &hook,
                nstd::observer_ptr<Anubis::Game::IBasePlayer> player, nstd::observer_ptr<Anubis::Game::IBaseEntity> attacker,
                float flDamage, float *vecDir, const std::unique_ptr<Anubis::Engine::ITraceResult> &tr, Anubis::Game::DmgType dmgType)
                {
                    if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
                    {
                        return hook->callNext(player, attacker, flDamage, vecDir, tr, dmgType);
                    }

                    lua_pushlightuserdata(L, hook.get());
                    lua_pushlightuserdata(L, player.get());
                    lua_pushlightuserdata(L, attacker.get());
                    lua_pushnumber(L, flDamage);
                    lua_pushlightuserdata(L, vecDir);
                    lua_pushlightuserdata(L, tr.get());
                    lua_pushinteger(L, static_cast<lua_Integer>(dmgType));

                    if (lua_pcall(L, 7, 0, 0) != LUA_OK)
                    {
                        return hook->callNext(player, attacker, flDamage, vecDir, tr, dmgType);
                    }
                }, hookPriority);
            break;
        }
        case PlayerClassHooks::Killed:
        {
            hookInfo = gGame->getCBasePlayerHooks()->killed()->registerHook(
               [L, fName]
               (const nstd::observer_ptr<Anubis::Game::IBasePlayerKilledHook> &hook,
                nstd::observer_ptr<Anubis::Game::IBasePlayer> player, nstd::observer_ptr<Anubis::Game::IBaseEntity> attacker,
                Anubis::Game::GibType gibType)
                {
                    if (lua_getglobal(L, fName.c_str()) == LUA_TNIL)
                    {
                        return hook->callNext(player, attacker, gibType);
                    }

                    lua_pushlightuserdata(L, hook.get());
                    lua_pushlightuserdata(L, player.get());
                    lua_pushlightuserdata(L, attacker.get());
                    lua_pushinteger(L, static_cast<lua_Integer>(gibType));

                    if (lua_pcall(L, 4, 0, 0) != LUA_OK)
                    {
                        return hook->callNext(player, attacker, gibType);
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

static int playerClassFnUnhook(lua_State *L)
{
    auto type = static_cast<PlayerClassHooks>(luaL_checkinteger(L, 1));

    switch (type)
    {
        case PlayerClassHooks::Spawn:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            gGame->getCBasePlayerHooks()->spawn()->unregisterHook(hookInfo);
            break;
        }
        case PlayerClassHooks::TakeDamage:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            gGame->getCBasePlayerHooks()->takeDamage()->unregisterHook(hookInfo);
            break;
        }
        case PlayerClassHooks::TraceAttack:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            gGame->getCBasePlayerHooks()->traceAttack()->unregisterHook(hookInfo);
            break;
        }
        case PlayerClassHooks::Killed:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            gGame->getCBasePlayerHooks()->killed()->unregisterHook(hookInfo);
            break;
        }
        case PlayerClassHooks::GiveShield:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            if (gGame->getCBasePlayerHooks()->giveShield())
            {
                gGame->getCBasePlayerHooks()->giveShield()->unregisterHook(hookInfo);
            }
            break;
        }
        case PlayerClassHooks::DropShield:
        {
            auto hookInfo = reinterpret_cast<Anubis::IHookInfo *>(lua_touserdata(L, 2));
            if (gGame->getCBasePlayerHooks()->dropShield())
            {
                gGame->getCBasePlayerHooks()->dropShield()->unregisterHook(hookInfo);
            }
            break;
        }

        default:
            break;
    }

    return 0;
}

static int getEdictFromPlayerClass(lua_State *L)
{
    auto entity = reinterpret_cast<Anubis::Game::IBasePlayer *>(lua_touserdata(L, 1));

    if (gClassHandler.get<Anubis::Game::IBasePlayer>(entity))
    {
        lua_pushlightuserdata(L, entity->edict().get());
        return 1;
    }

    return 0;
}

static int getEdictFromBaseClass(lua_State *L)
{
    auto entity = reinterpret_cast<Anubis::Game::IBaseEntity *>(lua_touserdata(L, 1));

    if (gClassHandler.get<Anubis::Game::IBaseEntity>(entity))
    {
        lua_pushlightuserdata(L, entity->edict().get());
        return 1;
    }

    return 0;
}

static int getEdictFromPlayerClassInHook(lua_State *L)
{
    auto entity = reinterpret_cast<Anubis::Game::IBasePlayer *>(lua_touserdata(L, 1));

    lua_pushlightuserdata(L, entity->edict().get());
    return 1;
}

static int getEdictFromBaseClassInHook(lua_State *L)
{
    auto entity = reinterpret_cast<Anubis::Game::IBaseEntity *>(lua_touserdata(L, 1));

    lua_pushlightuserdata(L, entity->edict().get());
    return 1;
}

static int getPlayerFromEdict(lua_State *L)
{
    auto entity = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    auto player = gClassHandler.create<Anubis::Game::IBasePlayer>(gGame->getBasePlayer(entity));

    lua_pushlightuserdata(L, player);
    return 1;
}

static int spawnPlayerClass(lua_State *L)
{
    auto entity = reinterpret_cast<Anubis::Game::IBasePlayer *>(lua_touserdata(L, 1));

    if (gClassHandler.get<Anubis::Game::IBasePlayer>(entity))
    {
        entity->spawn();
    }

    return 0;
}

static int giveNamedItemToPlayer(lua_State *L)
{
    auto player = reinterpret_cast<Anubis::Game::IBasePlayer *>(lua_touserdata(L, 1));

    if (!gClassHandler.get<Anubis::Game::IBasePlayer>(player))
    {
        return 0;
    }

    std::size_t length;
    const char *item = luaL_checklstring(L, 2, &length);

    std::optional<std::unique_ptr<Anubis::Game::IBaseEntity>> entity = player->giveNamedItem(item);

    if (!entity)
    {
        return 0;
    }

    lua_pushlightuserdata(L, gClassHandler.create<Anubis::Game::IBaseEntity>(std::move(*entity)));
    return 1;
}

LuaAdapterCFunction gClassNatives[] = {
    {"playerClassFnHook", playerClassFnHook},
    {"playerClassCallNext", playerClassCallNext},
    {"playerClassCallOriginal", playerClassCallOriginal},
    {"playerClassFnUnhook", playerClassFnUnhook},
    {"getEdictFromPlayerClass", getEdictFromPlayerClass},
    {"getEdictFromBaseClass", getEdictFromBaseClass},
    {"getEdictFromPlayerClassInHook", getEdictFromPlayerClassInHook},
    {"getEdictFromBaseClassInHook", getEdictFromBaseClassInHook},
    {"getPlayerFromEdict", getPlayerFromEdict},
    {"spawnPlayerClass", spawnPlayerClass},
    {"giveNamedItemToPlayer", giveNamedItemToPlayer},
    {nullptr, nullptr}
};