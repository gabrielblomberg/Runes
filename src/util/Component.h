#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <typeindex>
#include <unordered_map>

#include "util/TypeList.h"

/**
 * @note This implementation is based on
 * 
 * https://austinmorlan.com/posts/entity_component_system/#the-component
 */

/**
 * @brief An entity is defined by its identifier.
 */
using Entity = std::uint64_t;

/**
 * @brief The maximum number of entities.
 */
static const constexpr Entity MAX_ENTITIES = 4096;

/**
 * @brief Each component has its own identifier.
 */
using Component = std::uint64_t;

/**
 * @brief The maximum number of components.
 */
static const constexpr Component MAX_COMPONENTS = 32;

/**
 * @brief The signature of an entity that determines if a component is
 * compatible with a system. Each entity is a flag in a bit field, and checking
 * if an entity is compatible is a single bit operation.
 */
using Signature = std::bitset<MAX_COMPONENTS>;

/**
 * @brief The entity manager maintains unique entity identifiers and entity type
 * information.
 */
class EntityManager
{
    inline EntityManager()
        : m_existing(0)
    {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
            m_available.push(entity);
    }

    /**
     * @brief Create a new entity.
     * 
     * @return The allocated entity identifier.
     */
    inline Entity create_entity()
    {
        assert(!m_available.empty() && "too many entities");

        Entity entity = m_available.front();
        m_available.pop();
        ++m_existing;

        return entity;
    }

    /**
     * @brief Remove an entity from the signatures and make the identifier
     * available for allocation.
     * 
     * @param entity The identifier to remove.
     */
    inline void destroy_entity(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "entity out of range");

        m_signatures[entity].reset();
        m_available.push(entity);
        --m_existing;
    }

    /**
     * @brief Set the component signature of an entity.
     * 
     * @param entity The identifier of the entity.
     * @param signature The signature of the entity.
     */
    void set_signature(Entity entity, Signature signature)
    {
        assert(entity < MAX_ENTITIES && "entity out of range");
        m_signatures[entity] = signature;
    }

    /**
     * @brief Get the signature of an entity.
     * 
     * @param entity The identifier of the entity to get.
     * @returns The component signature of the entity.
     */
    Signature get_signature(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "entity out of range");
        return m_signatures[entity];
    }

    /**
     * @brief Get the number of existing entities.
     */
    std::uint64_t size()
    {
        return m_existing;
    }

private:

    /// The queue of available entity identifiers.
    std::queue<Entity> m_available;

    /// The signatures of all registered components.
    std::array<Signature, MAX_ENTITIES> m_signatures;

    /// The number of existing entities.
    std::uint64_t m_existing;
};

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
        assert(entity < MAX_ENTITIES && "too many entities");

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

        // Remove the entity.
        m_entity_to_index.erase(entity);
        m_index_to_entity.erase(last_index);
    }

    /**
     * @brief Called when an entity is destroyed to destroy its components.
     * @param entity The entity that was destroyed.
     */
    void entity_destroyed(Entity entity) override
    {
        if (m_entity_to_index.find(entity) != m_entity_to_index.end())
            remove(entity);
    }

private:

    /// An instance of the component for every entity. Sparse.
    std::array<ComponentType, MAX_ENTITIES> m_components;

    /// Mapping from each entity to the index in the component array.
    std::unordered_map<Entity, std::size_t> m_entity_to_index;

    /// Mapping of indexes to the entity they point to.
    std::unordered_map<std::size_t, Entity> m_index_to_entity;

    /// Total number of valid entries in the array.
    std::size_t m_size;
};

/**
 * @brief Manages all the different types of components, and entities that own
 * instances of those components.
 */
template<typename... Components>
class ComponentManager
{
public:

    static_assert(sizeof...(Components) < MAX_COMPONENTS);

    /**
     * @brief Add a component to an entity.
     * 
     * @tparam ComponentType The type of component.
     * @param entity The entity to add the component to.
     * @param component The instance of the component.
     */
    template<typename ComponentType>
    inline void add(Entity entity, ComponentType component) {
        constexpr Component Index = TypeList::Find<Components, ComponentType>;
        std::get<Index>(m_components).add(entity, component);
    }

    /**
     * @brief Remove a component from an entity.
     * 
     * @tparam ComponentType The type of component to remove from the entity.
     * @param entity The entity to remove the component from.
     */
    template<typename ComponentType>
    inline void remove(Entity entity) {
        constexpr Component Index = TypeList::Find<Components, ComponentType>;
        std::get<Index>(m_components).remove(entity);
    }

    /**
     * @brief Get the component data for an entity.
     * 
     * @tparam ComponentType The type of component.
     * @param entity The entity to get the component from.
     * @return A reference to the component data.
     */
    template<typename ComponentType>
    inline ComponentType &get(Entity entity) {
        get_components<ComponentType>()->get(entity);
    }

private:

    /**
     * @brief Type list of all component arrays.
     */
    using ComponentArrays = Typelist::Apply<ComponentArray, Components>;

    TypeList::TupleOf<ComponentArrays> m_components;
};

class System
{
public:

    std::set<Entity> m_entities;
};

class SystemManager
{
public:

};
