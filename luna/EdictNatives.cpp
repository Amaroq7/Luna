/*
 *  Copyright (C) 2013 Karol Szuster
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

#include "EdictNatives.hpp"
#include <observer_ptr.hpp>
#include <engine/IEdict.hpp>
#include "AnubisExports.hpp"

static int setFloatProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::FlProperty>(luaL_checkinteger(L, 2));
    auto value = static_cast<float>(luaL_checknumber(L, 3));

    edict->setFlProperty(property, value);
    return 0;
}

static int setIntProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::IntProperty>(luaL_checkinteger(L, 2));
    auto value = static_cast<std::int32_t>(luaL_checkinteger(L, 3));

    edict->setIntProperty(property, value);
    return 0;
}

static int setVecProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::VecProperty>(luaL_checkinteger(L, 2));
    auto value1 = static_cast<float>(luaL_checknumber(L, 3));
    auto value2 = static_cast<float>(luaL_checknumber(L, 4));
    auto value3 = static_cast<float>(luaL_checknumber(L, 5));

    std::array<float, 3> vector = {value1, value2, value3};

    edict->setVecProperty(property, vector);
    return 0;
}

static int setStrProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::StrProperty>(luaL_checkinteger(L, 2));
    std::size_t length;
    const char *value = luaL_checklstring(L, 3, &length);
    Anubis::Engine::StringOffset strOffset = gEngine->allocString(value, Anubis::FuncCallType::Direct);

    edict->setStrProperty(property, strOffset);
    return 0;
}

static int setShortProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::ShortProperty>(luaL_checkinteger(L, 2));
    auto value = static_cast<std::int16_t>(luaL_checkinteger(L, 3));

    edict->setShortProperty(property, value);
    return 0;
}

static int setUShortProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::UShortProperty>(luaL_checkinteger(L, 2));
    auto value = static_cast<std::uint16_t>(luaL_checkinteger(L, 3));

    edict->setUShortProperty(property, value);
    return 0;
}

static int setByteProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::ByteProperty>(luaL_checkinteger(L, 2));
    auto value = static_cast<std::uint8_t>(luaL_checknumber(L, 3));

    edict->setByteProperty(property, std::byte{value});
    return 0;
}

static int setEdictProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::EdictProperty>(luaL_checkinteger(L, 2));
    auto value = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 3));

    edict->setEdictProperty(property, value);
    return 0;
}

static int getFloatProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::FlProperty>(luaL_checkinteger(L, 2));

    lua_pushnumber(L, edict->getFlProperty(property));
    return 1;
}

static int getIntProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::IntProperty>(luaL_checkinteger(L, 2));

    lua_pushinteger(L, edict->getIntProperty(property));
    return 1;
}

static int getVecProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::VecProperty>(luaL_checkinteger(L, 2));

    std::array<float, 3> result = edict->getVecProperty(property);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    lua_pushnumber(L, result[2]);
    return 3;
}

static int getStrProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::StrProperty>(luaL_checkinteger(L, 2));

    Anubis::Engine::StringOffset strOffset = edict->getStrProperty(property);

    std::string_view result = gEngine->getString(strOffset, Anubis::FuncCallType::Direct);

    (result.data()) ? lua_pushstring(L, result.data()) : lua_pushstring(L, "");
    return 1;
}

static int getShortProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::ShortProperty>(luaL_checkinteger(L, 2));

    lua_pushinteger(L, edict->getShortProperty(property));
    return 1;
}

static int getUShortProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::UShortProperty>(luaL_checkinteger(L, 2));

    lua_pushinteger(L, static_cast<lua_Integer>(edict->getUShortProperty(property)));
    return 1;
}

static int getByteProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::ByteProperty>(luaL_checkinteger(L, 2));

    std::byte result = edict->getByteProperty(property);

    lua_pushinteger(L, std::to_integer<lua_Integer>(result));
    return 1;
}

static int getEdictProperty(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto property = static_cast<Anubis::Engine::IEdict::EdictProperty>(luaL_checkinteger(L, 2));

    lua_pushlightuserdata(L, edict->getEdictProperty(property).get());
    return 1;
}

static int getEdictIndex(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    lua_pushinteger(L, static_cast<lua_Integer>(edict->getIndex()));
    return 1;
}

static int getFixAngle(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    lua_pushinteger(L, static_cast<lua_Integer>(edict->getFixAngle()));
    return 1;
}

static int getModelIndex(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    lua_pushinteger(L, static_cast<lua_Integer>(edict->getModelIndex()));
    return 1;
}

static int getSolidType(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    lua_pushinteger(L, static_cast<lua_Integer>(edict->getSolidType()));
    return 1;
}

static int getEffects(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    lua_pushinteger(L, static_cast<lua_Integer>(edict->getEffects()));
    return 1;
}

static int getController(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    std::array<std::byte, 4> controller = edict->getController();

    lua_pushinteger(L, std::to_integer<lua_Integer>(controller[0]));
    lua_pushinteger(L, std::to_integer<lua_Integer>(controller[1]));
    lua_pushinteger(L, std::to_integer<lua_Integer>(controller[2]));
    lua_pushinteger(L, std::to_integer<lua_Integer>(controller[3]));
    return 4;
}

static int getBlending(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    std::array<std::byte, 2> blending = edict->getBlending();

    lua_pushinteger(L, std::to_integer<lua_Integer>(blending[0]));
    lua_pushinteger(L, std::to_integer<lua_Integer>(blending[1]));
    return 2;
}

static int getRenderMode(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    lua_pushinteger(L, static_cast<lua_Integer>(edict->getRenderMode()));
    return 1;
}

static int getDeadFlag(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    lua_pushinteger(L, static_cast<lua_Integer>(edict->getDeadFlag()));
    return 1;
}

static int getSpawnFlag(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    lua_pushinteger(L, static_cast<lua_Integer>(edict->getSpawnFlag()));
    return 1;
}

static int setFixAngle(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto value = static_cast<Anubis::Engine::FixAngle>(luaL_checkinteger(L, 2));

    edict->setFixAngle(value);
    return 0;
}

static int setModelIndex(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto value = static_cast<Anubis::Engine::PrecacheId>(luaL_checkinteger(L, 2));

    edict->setModelIndex(value);
    return 0;
}

static int setSolidType(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto value = static_cast<Anubis::Engine::IEdict::SolidType>(luaL_checkinteger(L, 2));

    edict->setSolidType(value);
    return 0;
}

static int setEffects(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto value = static_cast<Anubis::Engine::IEdict::Effects>(luaL_checkinteger(L, 2));

    edict->setEffects(value);
    return 0;
}

static int setController(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    std::array<std::byte, 4> controller = {
        std::byte{static_cast<std::uint8_t>(luaL_checkinteger(L, 2))},
        std::byte{static_cast<std::uint8_t>(luaL_checkinteger(L, 3))},
        std::byte{static_cast<std::uint8_t>(luaL_checkinteger(L, 4))},
        std::byte{static_cast<std::uint8_t>(luaL_checkinteger(L, 5))}
    };

    edict->setController(controller);
    return 0;
}

static int setBlending(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));

    std::array<std::byte, 2> blending = {
        std::byte{static_cast<std::uint8_t>(luaL_checkinteger(L, 2))},
        std::byte{static_cast<std::uint8_t>(luaL_checkinteger(L, 3))}
    };

    edict->setBlending(blending);
    return 0;
}

static int setRenderMode(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto value = static_cast<Anubis::Engine::IEdict::RenderMode>(luaL_checkinteger(L, 2));

    edict->setRenderMode(value);
    return 0;
}

static int setDeadFlag(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto value = static_cast<Anubis::Engine::IEdict::DeadFlag>(luaL_checkinteger(L, 2));

    edict->setDeadFlag(value);
    return 0;
}

static int setSpawnFlag(lua_State *L)
{
    auto edict = reinterpret_cast<Anubis::Engine::IEdict *>(lua_touserdata(L, 1));
    auto value = static_cast<Anubis::Engine::IEdict::SpawnFlag>(luaL_checkinteger(L, 2));

    edict->setSpawnFlag(value);
    return 0;
}

LuaAdapterCFunction gEdictNatives[] = {
    {"setFloatProperty", setFloatProperty},
    {"setIntProperty", setIntProperty},
    {"setVecProperty", setVecProperty},
    {"setStrProperty", setStrProperty},
    {"setShortProperty", setShortProperty},
    {"setUShortProperty", setUShortProperty},
    {"setByteProperty", setByteProperty},
    {"setEdictProperty", setEdictProperty},

    {"getFloatProperty", getFloatProperty},
    {"getIntProperty", getIntProperty},
    {"getVecProperty", getVecProperty},
    {"getStrProperty", getStrProperty},
    {"getShortProperty", getShortProperty},
    {"getUShortProperty", getUShortProperty},
    {"getByteProperty", getByteProperty},
    {"getEdictProperty", getEdictProperty},

    {"getEdictIndex", getEdictIndex},
    {"getFixAngle", getFixAngle},
    {"getModelIndex", getModelIndex},
    {"getSolidType", getSolidType},
    {"getEffects", getEffects},
    {"getController", getController},
    {"getBlending", getBlending},
    {"getRenderMode", getRenderMode},
    {"getDeadFlag", getDeadFlag},
    {"getSpawnFlag", getSpawnFlag},

    {"setFixAngle", setFixAngle},
    {"setModelIndex", setModelIndex},
    {"setSolidType", setSolidType},
    {"setEffects", setEffects},
    {"setController", setController},
    {"setBlending", setBlending},
    {"setRenderMode", setRenderMode},
    {"setDeadFlag", setDeadFlag},
    {"setSpawnFlag", setSpawnFlag},

    {nullptr, nullptr}
};