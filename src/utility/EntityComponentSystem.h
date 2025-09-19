#pragma once

#include <algorithm>
#include <functional>
#include <cassert>
#include <deque>
#include <array>
#include <bitset>
#include <unordered_set>
#include <iostream>
#include <numeric>

#include "utility/TypeList.h"

/**
 * @brief Identifier of an entity managed by the entity component system.
 */
using Entity = std::uint64_t;

/**
 * @tparam ComponentList Typelist of data structures.
 */
template<typename ComponentList, std::size_t N>
class EntityComponentSystem
{
public:

    /**
     * @brief Bitset signifying the components belonging to an entity.
     * 
     * The signature has a bit set for every component the entity has, and
     * cleared for components the entity does not have.
     */
    using Signature = std::bitset<TypeList::Size<ComponentList>>;

    /**
     * @brief Initialise the como
     */
    EntityComponentSystem()
        : m_available_entities(N)
    {
        std::iota(m_available_entities.begin(), m_available_entities.end(), 0);
    }

    /**
     * @brief Create a new empty entity.
     * @returns The new entity.
     */
    inline Entity create_entity()
    {
        assert(!m_available_entities.empty() && "too many entities");

        Entity entity = m_available_entities.front();
        m_available_entities.pop_front();

        m_entity_signatures[entity].reset();
        return entity;
    }

    /**
     * @brief Create an entity with its components.
     * 
     * @param args The components of the entity.
     * @returns The entity identifier.
     */
    template<typename... Args>
    inline Entity create_entity(Args... args)
    {
        Entity entity = create_entity();

        // Add each component.
        (std::get<TypeList::Index<ComponentList, std::remove_cvref<Args>>>(m_components)
            .add(entity, std::forward(args)), ...);

        // Set the signature from the component indexes.
        Signature signature = ((1 + TypeList::Index<ComponentList, std::remove_cvref<Args>>) | ...);
        m_entity_signatures[entity] = signature;

        // Update entity caches.
        for (auto &cache : m_entity_caches)
            if (signature & cache.signature)
                cache.entities.insert(entity);

        return entity;
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
    Signature get_entity_signature(Entity entity)
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
    void remove_entity(Entity entity)
    {
        {
            auto it = std::lower_bound(
                m_available_entities.begin(),
                m_available_entities.end(),
                entity
            );
    
            assert(it == m_available_entities.end() && entity < N && "entity out of range");
        }

        // Remove components from entity.
        std::apply(
            [entity](auto&... array){ (array.remove(entity), ...); },
            m_components
        );

        // Remove entity from caches.
        Signature signature = m_entity_signatures[entity];
        for (auto &cache : m_entity_caches)
            if ((cache.signature & signature).any())
                cache.entities.erase(entity);

        // Make the entity id available again.
        m_available_entities.push_back(entity);
    }

    /**
     * @brief Get the number of entities.
     */
    std::size_t total_entities()
    {
        return N - m_available_entities.size();
    }

    /**
     * @brief Add a component to an entity.
     * 
     * @tparam Component The type of component.
     * @param args The parts of the component.
     */
    template<std::size_t Component, typename... Args>
    inline void add_component(Entity entity, Args&&... args)
    {
        add_component<Component>(
            entity,
            TypeList::Get<ComponentList, Component>(args...)
        );
    }

    /**
     * @brief Add a component to an entity.
     * 
     * @tparam ComponentType The type of component.
     * @param entity The entity to add the component to.
     * @param component The instance of the component.
     */
    template<std::size_t Component>
    inline void add_component(Entity entity, TypeList::Get<ComponentList, Component> &&component)
    {
        std::get<Component>(m_components).add(entity, std::forward<TypeList::Get<ComponentList, Component>>(component));
        Signature &signature = m_entity_signatures[entity].set(Component, true);

        for (auto &cache : m_entity_caches)
            if ((signature & cache.signature).any())
                cache.entities.insert(entity);
    }

    /**
     * @brief Get the component data for an entity.
     * 
     * @tparam ComponentType The type of component.
     * @param entity The entity to get the component from.
     * @return A reference to the component data.
     */
    template<std::size_t Component>
    inline auto &get_component(Entity entity)
    {
        return std::get<Component>(m_components).get(entity);
    }

    /**
     * @brief Remove a component from an entity.
     * 
     * @tparam ComponentType The type of component to remove from the entity.
     * @param entity The entity to remove the component from.
     */
    template<std::size_t Component>
    inline void remove_component(Entity entity)
    {
        std::get<Component>(m_components).remove(entity);
        Signature signature = m_entity_signatures[entity].set(Component, false);

        for (auto &set : m_entity_caches)
            if (!(signature & set.signature))
                set.entities.erase(entity);
    }

    /**
     * @brief Get a cache of entities with a given signature.
     * @returns The cache.
     */
    const std::unordered_set<Entity> &get_cache(Signature signature)
    {
        auto it = m_entity_caches.find(signature);
        if (it == m_entity_caches.end())
            it = create_entity_cache(signature);

        return it->second;
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
            assert(entity < N && "entity out of range");
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

        /**
         * @brief Get the signature of the component array.
         */
        inline constexpr Signature signature()
        {
            return 1 << TypeList::Index<ComponentList, std::remove_cvref<ComponentType>>;
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
     * @brief Create a new entity set.
     * 
     * @param signature The signature of the entity set.
     */
    auto create_entity_cache(Signature signature)
    {
        auto [it, _] = m_entity_caches.emplace_back(signature, {});
        auto available = m_available_entities.begin();

        for (Entity entity = 0; entity < N; ++entity) {

            // Skip non-existing entities.
            if (*available == entity) {
                ++available;
                continue;
            }

            if (m_entity_signatures[entity] & signature)
                it->entities.insert(entity);
        }

        return it;
    }

    /// The queue of available entity identifiers.
    std::deque<Entity> m_available_entities;

    /// The signatures of all registered components.
    std::array<Signature, N> m_entity_signatures;

    /// Cache of entities with a given signature.
    std::unordered_map<Signature, std::unordered_set<Entity>> m_entity_caches;

    /// All the component data arrays.
    TypeList::TupleOf<TypeList::Map<ComponentArray, ComponentList>> m_components;
};
