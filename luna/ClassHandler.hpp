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

#include <game/IBaseEntity.hpp>
#include <game/IBasePlayer.hpp>
#include <engine/IEdict.hpp>
#include <memory>
#include <vector>
#include <type_traits>

class ClassHandler
{
    private:
        class EntityHandle
        {
        public:
            explicit EntityHandle() = default;

            explicit EntityHandle(std::unique_ptr<Anubis::Game::IBaseEntity> &&pEntity)
                : m_pEntity(std::move(pEntity)), m_edict(m_pEntity->edict()), m_serialNumber(m_edict->getSerialNumber()) {}

            [[nodiscard]] const std::unique_ptr<Anubis::Game::IBaseEntity> &get() const
            {
                return m_pEntity;
            }

            [[nodiscard]] bool isValid() const
            {
                return m_pEntity->isValid() && m_serialNumber == m_edict->getSerialNumber();
            }

        private:
            std::unique_ptr<Anubis::Game::IBaseEntity> m_pEntity{};
            nstd::observer_ptr<Anubis::Engine::IEdict> m_edict{};
            std::uint32_t m_serialNumber{};
        };

    public:
        ClassHandler() = default;
        ~ClassHandler() = default;

        template<typename T = Anubis::Game::IBaseEntity, typename = std::enable_if_t<std::is_base_of_v<Anubis::Game::IBaseEntity, T>>>
        std::add_pointer_t<T> create(std::add_rvalue_reference_t<std::unique_ptr<T>> pEntity)
        {
            const auto &[iter, inserted] = m_entHandles.try_emplace(pEntity.get(), std::move(pEntity));

            return dynamic_cast<std::add_pointer_t<T>>(iter->first);
        }

        template<typename T = Anubis::Game::IBaseEntity, typename = std::enable_if_t<std::is_base_of_v<Anubis::Game::IBaseEntity, T>>>
        std::add_pointer_t<T> get(std::add_pointer_t<T> pEntity)
        {
            for (auto iter = m_entHandles.begin(); iter != m_entHandles.end(); iter++)
            {
                if (!(iter->second).isValid())
                {
                    if (iter = m_entHandles.erase(iter); iter == m_entHandles.end())
                    {
                        break;
                    }
                }
                if ((iter->second).get().get() == pEntity)
                {
                    try
                    {
                        return dynamic_cast<std::add_pointer_t<T>>((iter->second).get().get());
                    }
                    catch (const std::bad_cast &)
                    {
                        return {};
                    }
                }
            }

            return {};
        }

    private:
        std::unordered_map<Anubis::Game::IBaseEntity *, EntityHandle> m_entHandles;
};

extern ClassHandler gClassHandler;