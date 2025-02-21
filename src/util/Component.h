#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <memory>
#include <numeric>
#include <queue>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

#include "engine/TypeList.h"

/** 
 * @note This implementation is based on
 * https://austinmorlan.com/posts/entity_component_system/#the-component
 * 
 * Changes include compile time type lists for component and system arrays, and
 * an entity wrapper for more intuitive usage.
 */

/**
 * @brief A numeric identifier for representing entities.
 */
using Entity = std::uint64_t;

/**
 * @brief The maximum number of components.
 */
static const constexpr std::size_t MAX_COMPONENTS = 64;

/**
 * @brief The signature of an entity that determines if a component is
 * compatible with a system. Each entity is a flag in a bit field, and checking
 * if an entity is compatible is a single bit operation.
 */
using Signature = std::bitset<MAX_COMPONENTS>;

/**
 * @brief The maximum number of entities.
 */
static const constexpr Entity MAX_ENTITIES = 4096;

/**
 * @brief The entity manager maintains unique entity identifiers and entity type
 * information.
 */
class EntityManager
{
    inline EntityManager()
        : m_entities(0)
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
        ++m_entities;

        return entity;
    }

    /**
     * @brief Remove an entity from the signatures and make the identifier
     * available for allocation.
     * 
     * @param entity The identifier to remove.
     */
    inline void entity_destroyed(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "entity out of range");

        m_signatures[entity].reset();
        m_available.push(entity);
        --m_entities;
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
        return m_entities;
    }

private:

    /// The queue of available entity identifiers.
    std::queue<Entity> m_available;

    /// The signatures of all registered components.
    std::array<Signature, MAX_ENTITIES> m_signatures;

    /// The number of existing entities.
    std::uint64_t m_entities;
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
 * 
 * @tparam Components A type list of the component types being managed.
 */
template<typename Components>
class ComponentManager
{
public:

    /**
     * @brief Add a component to an entity.
     * 
     * @tparam ComponentType The type of component.
     * @param entity The entity to add the component to.
     * @param component The instance of the component.
     */
    template<std::size_t Component>
    inline void add_component(Entity entity, TypeList::Get<Components, Component> component) {
        std::get<Component>(m_components).add(entity, component);
    }

    /**
     * @brief Get the component data for an entity.
     * 
     * @tparam ComponentType The type of component.
     * @param entity The entity to get the component from.
     * @return A reference to the component data.
     */
    template<std::size_t Component>
    inline TypeList::Get<Components, Component> &get_component(Entity entity) {
        return std::get<Component>(m_components).get(entity);
    }

    /**
     * @brief Get the signature of a component.
     */
    template<std::size_t Component>
    inline Signature get_component_signature()
    {
        return 1 << Component;
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
    }

    /**
     * @brief Callback for when an entity is destroyed.
     * @param entity The entity to remove the component from.
     */
    // inline void entity_destroyed(Entity entity) {
    //     (get_component<Components>().entity_destroyed(entity), ...);
    // }

private:

    /// A tuple of the component arrays.
    Typelist::TupleOf<Typelist::Apply<ComponentArray, Components>> m_components;
};

/**
 * @brief A generic system which is derived from to implement each system
 * functionality.
 */
struct System {

    /// The entities belonging to this system.
    std::unordered_set<Entity> m_entities;
};

/**
 * @brief Managers system signatures and storage.
 * @tparam Systems A typelist of the systems being managed.
 */
template<typename Systems>
class SystemManager
{
public:

    /**
     * @brief Get a pointer to a system.
     */
    template<std::size_t System>
    inline TypeList::Get<Systems, System> &get_system()
    {
        return std::get<Systems, System>(m_systems);
    }

    /**
     * @brief Set the signature of a system.
     */
    template<std::size_t System>
    void set_signature(Signature signature)
    {
        m_signatures[System] = signature;
    }

    /**
     * @brief Get the signature of a system.
     */
    template<std::size_t System>
    Signature get_signature()
    {
        return m_signatures[System];
    }

    /**
     * @brief Update each systems set of entities when the signature of an
     * entity changes.
     * 
     * @param entity The entity whose signature changed.
     * @param signature The new signature of the entity.
     */
    void entity_signature_changed(Entity entity, Signature signature)
    {
        static auto update = [entity, signature]<typename T>(T &system){
            if (m_signatures[i] & signature)
                system.m_entities.insert(entity);
            else
                system.m_entities.erase(entity);
        };

        // std::apply(update, m_systems);
    }

private:

    /// The signature for each system.
    std::array<Signature, TypeList::Size<Systems>> m_signatures;

    TypeList::TupleOf<Systems> m_systems;
};

/**
 * @brief The main.
 */
template<typename Components, typename Systems>
class EntityComponentSystem
{
public:

    /**
     * @brief Wrapper around an entity identifier, interfacing with the entity
     * component system.
     */
    class Entity
    {
    public:

        /**
         * @brief Add a component to an entity.
         * 
         * @tparam ComponentType The type of the component.
         * @param entity The entity to add the component to.
         * @param component The component data.
         */
        template<std::size_t Component>
        void add_component(TypeList::Get<Components, Component> component)
        {
            // Add the component.
            m_ecs->m_component_manager.add_component(m_entity, component);

            // Update the signature of the component.
            auto signature = m_ecs->m_entity_manager.get_signature(m_entity);
            constexpr Component Index = TypeList::Find<Components, ComponentType>;
            signature.set(Index);

            // Update the entities belonging to the system with added component.
            /// @TODO: Use component type to update system appropriately.
            m_ecs->m_system_manager.entity_signature_changed(m_entity, signature);
        }

        /**
         * @brief Get a component of an entity.
         * 
         * Returns a reference to component data of an entity.
         * 
         * @tparam ComponentType The component data type.
         * @param entity The entity to get the component data from.
         * 
         * @returns The entities component data.
         */
        template<typename ComponentType>
        inline ComponentType &get_component(Entity entity)
        {
            return m_ecs->m_component_manager.get_component<ComponentType>(entity);
        }

        /**
         * @brief Remove a component from an entity. Dynamically removes an
         * attribute from an entity.
         * 
         * @tparam ComponentType The type of the component.
         * @param entity The entity to remove the component data from.
         */
        template<typename ComponentType>
        void remove_component(Entity entity)
        {
            m_ecs->m_component_manager.remove_component<ComponentType>(entity);
        }

        /**
         * @brief Get the signature of this entity.
         */
        Signature get_signature()
        {
            return m_ecs->m_entity_manager.get_signature(m_entity);
        }

    private:

        friend class EntityComponentSystem<Components, Systems>;

        Entity(::Entity entity, EntityComponentSystem<Components, Systems> *ecs)
            : m_entity(entity)
            , m_ecs(ecs)
        {}

        ~Entity()
        {
            m_ecs->m_entity_manager.destroy_entity(entity);
            m_ecs->m_component_manager.entity_destroyed(entity);
            m_ecs->m_system_manager.entity_signature_changed(entity);
        }

        /// Entity identifier of this entity.
        ::Entity m_entity;

        /// Pointer to the entity component system.
        EntityComponentSystem<Components, Systems> *m_ecs;
    };

    /**
     * @brief Create a new entity and returns its identifier.
     */
    inline Entity create_entity()
    {
        return Entity(m_entity_manager.create_entity(), this);
    }

    /**
     * @brief Get the signature of a component.
     */
    template<typename ComponentType>
    Component get_component_signature()
    {
        return m_ecs->m_component_manager.get_component_signature();
    }

    /**
     * @brief Set the signature of a system.
     */
    template<typename SystemType>
    void set_system_signature(Signature signature)
    {
        m_system_manager.set_signature<SystemType>(signature);
    }

private:

    /// The entity manager keeps track of entity identifiers.
    EntityManager m_entity_manager;

    /// The component manager handles entity attribute data.
    ComponentManager<Components> m_component_manager;

    /// The system manager updates the entities belonging to a system.
    SystemManager<Systems> m_system_manager;
};
