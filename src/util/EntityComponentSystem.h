#pragma once

#include <cassert>
#include <deque>
#include <array>
#include <bitset>
#include <unordered_set>

#include "util/TypeList.h"

/**
 * @brief Identifier of an entity managed by the entity component system.
 */
using Entity = std::uint64_t;

struct System {
    std::unordered_set<Entity> m_entities;
};

/**
 * @tparam Components Typelist of data structures.
 * @tparam Systems Typelist of all the systems.
 */
template<typename Components, std::size_t N>
class EntityComponentSystem
{
public:

    /**
     * @brief Bitset signifying the components belonging to an entity.
     * 
     * The signature has a bit set for every component the entity has, and
     * cleared for components the entity does not have.
     */
    using Signature = std::bitset<TypeList::Size<Components>>;

    void add_system(std::unique_ptr<System> &&)

    /**
     * @brief Create a new entity.
     * @returns The new entity.
     */
    inline Entity create_entity()
    {
        assert(!m_available.empty() && "too many entities");

        Entity entity = m_available.front();
        m_available.pop_front();
        ++m_size;

        return entity;
    }

    /**
     * @brief Add a component to an entity.
     * 
     * @tparam ComponentType The type of component.
     * @param entity The entity to add the component to.
     * @param component The instance of the component.
     */
    template<std::size_t Component>
    inline void add_component(Entity entity, TypeList::Get<Components, Component> &&component) {
        std::get<Component>(m_components).add(entity, std::forward(component));
        Signature &signature = m_entity_signatures[entity].set(Component, true);
        update_system_entities(entity, signature);
    }

    /**
     * @brief Get the component data for an entity.
     * 
     * @tparam ComponentType The type of component.
     * @param entity The entity to get the component from.
     * @return A reference to the component data.
     */
    template<std::size_t Component>
    inline auto &get_component(Entity entity) {
        return std::get<Component>(m_components).get(entity);
    }

    /**
     * @brief Remove a component from an entity.
     * 
     * @tparam ComponentType The type of component to remove from the entity.
     * @param entity The entity to remove the component from.
     */
    template<std::size_t Component>
    inline void remove_component(Entity entity) {
        std::get<Component>(m_components).remove(entity);
        Signature signature = m_entity_signatures[entity].set(Component, false);
        update_system_entities(entity, signature);
    }

    /**
     * @brief Get the signature of a component.
     * 
     * The signature has a bit set for every component the entity has, and
     * cleared for components the entity does not have.
     * 
     * The entity has a Component if 1 << Component is set.
     * 
     * @param entity The entity.
     * @returns The entities component signature.
     */
    Signature get_signature(Entity entity)
    {
        assert(entity < N && "get entity signature out of range");
        return m_entity_signatures[entity];
    }

    /**
     * @brief Remove an entity from the entity component system.
     * 
     * Frees up the entities component identifier, data storage, and updates
     * system entity collections.
     * 
     * @param entity The entity to remove.
     */
    void destroy_entity(Entity entity)
    {
        // Ensure the identifier exists.
        assert(
            std::find(m_available.begin(), m_available.end(), entity) == std::end(m_available) &&
            "entity out of range"
        );

        m_entity_signatures[entity].reset();
        m_available.push_back(entity);
        --m_size;

        // Remove entity from each component array.
        std::apply(
            []<typename T>(ComponentArray<T> &array){ array.remove(entity); },
            m_components
        );

        // Update system entity arrays.
        update_system_entities(entity, 0);
    }

    /**
     * @brief Get the number of entities.
     */
    std::size_t total_entities()
    {
        return m_size;
    }

    /**
     * @brief Get a pointer to a system.
     */
    template<std::size_t System>
    inline auto &get_system()
    {
        return std::get<System>(m_systems).system;
    }

private:

    /**
     * @brief An array of a single type of component, that stores a component for
     * each entity if it has one.
     */
    template<typename ComponentType>
    class ComponentArray
    {
    public:

        /**
         * @brief Add a component to an entity.
         * 
         * @param entity The entity to add the component to.
         * @param component The component to add.
         */
        inline void add(Entity entity, ComponentType component)
        {
            assert(entity < N && "too many entities");

            std::size_t index = m_size;
            m_components[index] = component;

            m_entity_to_index[entity] = index;
            m_index_to_entity[index] = entity;

            ++m_size;
        }

        /**
         * @brief Get the component of an entity.
         * 
         * @param entity The entity to get the component of.
         * @returns The component of the entity.
         */
        ComponentType &get(Entity entity)
        {
            return m_components[m_entity_to_index[entity]];
        }

        /**
         * @brief Reset an entities data.
         * 
         * @param entity The entity to reset.
         */
        inline void remove(Entity entity)
        {
            std::size_t index = m_entity_to_index[entity];
            std::size_t last_index = m_size - 1;
            Entity last_entity = m_index_to_entity[last_index];

            // Maintain contiguous by moving last component and entity to removed.
            m_components[index] = m_components[last_index];
            m_entity_to_index[last_entity] = index;
            m_index_to_entity[index] = last_entity;

            m_size--;
        }

    private:

        /// An instance of the component for every entity. Sparse.
        std::array<ComponentType, N> m_components;

        /// Mapping of entity identifiers to their index in components.
        std::array<std::size_t, N> m_entity_to_index;

        /// Mapping of indexes to the entity they point to.
        std::array<std::size_t, N> m_index_to_entity;

        /// Total number of valid entries in the array.
        std::size_t m_size;
    };

    /**
     * @brief Updates all the systems with an entity.
     * 
     * @param entity The entity that changed.
     * @param signature The entities component signature.
     */
    void update_system_entities(Entity entity, Signature signature)
    {
        const static auto update = [&]<typename T>(T &system){
            if (T::SIGNATURE & signature)
                system.m_entities.insert(entity);
            else
                system.m_entities.erase(entity);
        };

        std::apply(update, m_systems);
    }

    /// The number of existing entities.
    std::uint64_t m_size;

    /// The queue of available entity identifiers.
    std::deque<Entity> m_available;

    /// The signatures of all registered components.
    std::array<Signature, N> m_entity_signatures;

    /// The signatures of all systems.
    std::vector<Signature> m_system_signatures;

    /// All the systems.
    std::vector<std::unique_ptr<System>> m_systems;

    /// All the component data arrays.
    TypeList::TupleOf<TypeList::Apply<ComponentArray, Components>> m_components;
};
