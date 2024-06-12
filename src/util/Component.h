#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <memory>
#include <queue>
#include <unordered_map>
#include <typeindex>
#include <set>

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
 * @brief The entity manager provides information about entity types.
 * 
 * It returns bit sets with flags set for each component an entity has,
 * contained within contiguous memory for minimal cache misses.
 */
class EntityManager
{
    EntityManager();

    /**
     * @brief Create a new entity.
     * 
     * @return The allocated entity identifier.
     */
    Entity create_entity();

    /**
     * @brief Remove an entity from the signatures and make the identifier
     * available for allocation.
     * 
     * @param entity The identifier to remove.
     */
    void destroy_entity(Entity entity);

    /**
     * @brief Set the component signature of an entity.
     * 
     * @param entity THe identifier of the entity.
     * @param signature The signature of the entity.
     */
    void set_signature(Entity entity, Signature signature);

    /**
     * @brief Get the signature of an entity.
     * 
     * @param entity The identifier of the entity to get.
     * @returns The component signature of the entity.
     */
    Signature get_signature(Entity entity);

private:

    /// The queue of available entity identifiers.
    std::queue<Entity> m_available;

    /// The signatures of all registered components.
    std::array<Signature, MAX_ENTITIES> m_signatures;
};

/**
 * @brief 
 * 
 */
class IComponentArray
{
public:

	virtual ~IComponentArray() = default;

    /**
     * @brief Callback to check if 
     * 
     * @param entity 
     */
	virtual void EntityDestroyed(Entity entity) = 0;
};

/**
 * @brief An array of a single type of component, that stores a component for
 * each entity if it has one.
 */
template<typename ComponentType>
class ComponentArray : public IComponentArray
{
public:

    /**
     * @brief Add a component to an entity.
     * 
     * @param entity The entity to add the component to.
     * @param component The component to add.
     */
    void add(Entity entity, ComponentType component);

    /**
     * @brief Get the component of an entity.
     * 
     * @param entity The entity to get the component of.
     * @returns The component of the entity.
     */
    ComponentType &get(Entity entity);

    /**
     * @brief Reset an entities data.
     * 
     * @param entity The entity to reset.
     */
    void remove(Entity entity);

    /**
     * @brief Called when an entity is destroyed to destroy its components.
     * @param entity The entity that was destroyed.
     */
    void entity_destroyed(Entity entity) override;

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

template<typename ComponentType>
void ComponentArray<ComponentType>::add(Entity entity, ComponentType component)
{
    std::size_t index = m_size;
    m_entity_to_index[entity] = index;
    m_index_to_entity[index] = entity;
    m_components[index] = component;
    ++m_size;
}

template<typename ComponentType>
ComponentType &ComponentArray<ComponentType>::get(Entity entity)
{
    return m_components[m_entity_to_index[entity]];
}

template<typename ComponentType>
void ComponentArray<ComponentType>::remove(Entity entity)
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

template<typename ComponentType>
void ComponentArray<ComponentType>::entity_destroyed(Entity entity)
{
    if (m_entity_to_index.find(entity) != m_entity_to_index.end()) {
        remove(entity);
    }
}

/**
 * @brief Manages all the different types of components, and entities that own
 * instances of those components.
 */
class ComponentManager
{
public:

    ComponentManager()
        : m_next_component(0)
    {}

    /**
     * @brief Register a new component that an entity can own.
     * 
     * @tparam T The type of the component.
     * @param name The name of the component.
     */
    template<typename ComponentType>
    void register_component();

    /**
     * @brief Get the type identifier of a component.
     * 
     * @tparam ComponentType The type of component.
     * @return The type identifier of the component. 
     */
    template<typename ComponentType>
    inline Component type() {
        std::type_index index = std::type_index(typeid(ComponentType));
        assert(m_component_types.find(index) != m_component_types.end());
        return m_component_types[index];
    }

    /**
     * @brief Add a component to an entity.
     * 
     * @tparam ComponentType The type of component.
     * @param entity The entity to add the component to.
     * @param component The instance of the component.
     */
    template<typename ComponentType>
    inline void add(Entity entity, ComponentType component) {
        get_components<ComponentType>()->add(entity, component);
    }

    /**
     * @brief Remove a component from an entity.
     * 
     * @tparam ComponentType The type of component to remove from the entity.
     * @param entity The entity to remove the component from.
     */
    template<typename ComponentType>
    inline void remove(Entity entity) {
        get_components<ComponentType>()->remove(entity);
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

    /**
     * @brief Get the number of registered components.
     * @return The number of registered components. 
     */
    inline std::size_t size() {
        return m_next_component;
    } 

private:

    /**
     * @brief Get a pointer to the the component array of a component type.
     * 
     * @tparam ComponentType The type of component.
     * @return The array of components for each entity.
     */
    template<typename ComponentType>
    inline std::shared_ptr<ComponentArray<ComponentType>> get_components() {
        return std::static_pointer_cast<ComponentArray<ComponentType>>(
            m_component_arrays[std::type_index(typeid(ComponentType))]
        );
    }

    /// Types to their allocated component type.
    std::unordered_map<std::type_index, Component> m_component_types;

    /// The array of component instances of each entity for each component type.
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> m_component_arrays;

    /// The next component type identifier. Used for setting Signature bit fields.
    std::size_t m_next_component;
};

template<typename ComponentType>
void ComponentManager::register_component()
{
    std::type_index index = std::type_index(typeid(ComponentType));
    assert(m_component_names.find(index) != m_component_arrays.end());

    m_component_types.emplace(index, m_next_component);
    m_component_arrays.emplace(index, std::make_shared<ComponentArray<ComponentType>>());
    ++m_next_component;
}

class System
{
public:
    std::set<Entity> m_entities;
};

class SystemManager
{

};
