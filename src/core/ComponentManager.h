// MIT License, Copyright (c) 2019 Malik Allen

#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "../utility/TemplateHelper.h"
#include "Component.h"
#include "EntityManager.h"
#include "SystemManager.h"

#include <array>
#include <vector>
#include <map>

namespace Nebula
{
	/*
	*	The Component Manager is responsible for creating, destroying and managing the lifetime of components
	*	Along with updating System Manager when the signature of an Entity has changed
	*/
	class ComponentManager
	{
		// All Components created by this component manager
		std::array<Component*, MAX_COMPONENTS> m_components;

		// Components marked for clean up
		std::vector<Component*> m_componentsMarkedForCleanUp;

		// The number of components on this component manager
		uint64_t				m_componentCounter;

		// Entity Manager reference
		EntityManager* m_entityManager;

		using ComponentMap = std::map< uint64_t /*Component Type*/, std::vector<Component*> /*Components*/ >;

		// Map of Components sorted by the component type, where the component type is a key to a list of components of that type
		ComponentMap			m_componentMap;

		// System Manager reference
		SystemManager* m_systemManager;

	public:
		explicit ComponentManager( EntityManager* entityManager, SystemManager* systemManager );

		~ComponentManager();


		/*
		*	Adds a component to the entity with the passed EntityId, returning the created <Component>
		*	@param	<T>:		The type of Component that will be created and added to the entity
		*	@param	EntityId:	The entity id of the entity to add the created component to
		*	@param	Args:		The constructor requirements for the component
		*/
		template<typename T, typename ... Args>
		T* AddComponent( EntityId entityId, Args&& ... args )
		{
			// Complile-time check to see if class T can be converted to class B, 
				// valid for derivation check of class T from class B
			CanConvert_From<T, Component>();

			/* '>=' check work here because we increment component count after adding a component, and the counter begins 0 for the first index of the component map */
			if( m_componentCounter >= MAX_COMPONENTS )	// We are at capacity, return 
			{
				return nullptr;
			}

			Entity* entity = m_entityManager->m_entities[entityId];
			if( entity == nullptr )	// Entity does not exist
			{
				return nullptr;
			}

			if( entity->m_componentCounter >= MAX_COMPONENTS_PER_ENTITY )	// This entity is at its capacity
			{
				return nullptr;
			}

			if(FindComponent<T>(entityId) != nullptr)
			{
				return nullptr;	// Component already exists within the entity
			}

			// Component Classes can support different constructors, 0 -> n number of parameters in their constructor
			T* component = new T( std::forward<Args>( args ) ... );

			if( component == nullptr )	// Could not create component
			{
				return nullptr;
			}

			component->m_ownerId = entityId;
			component->m_componentId = entity->m_componentCounter;
			++entity->m_componentCounter;
			entity->m_components[component->m_componentId] = component;

			component->m_componentManagerId = this->m_componentCounter;
			m_components[component->m_componentManagerId] = component;
			++this->m_componentCounter;

			// Also add a reference of this component to the ComponentMap
			m_componentMap[T::ID].push_back( component );

			if( m_systemManager )
			{
				// This entity's signature has now changed update the system manager's systems
				m_systemManager->OnEntitySignatureChanged( *entity );
			}

			return component;
		}

		/*
		*	@brief	Finds the component of the passed class type on the passed entity
		*	@param	<T>		The type of component to look for
		*	@param	EntityId	The entityId of the entity to search inside of; for the component
		*	@return	T*		Returns the component if found, returning nullptr, if otherwise
		*/
		template<typename T>
		T* FindComponent( EntityId entityId )
		{
			// Complile-time check to see if class T can be converted to class B, 
				// valid for derivation check of class T from class B
			CanConvert_From<T, Component>();

			Entity* entity = m_entityManager->m_entities[entityId];
			if( entity == nullptr )	// Entity does not exist
			{
				return nullptr;
			}

			// Loop through the components of the same type, the id of the component type is used as the key to a vector of components 
			Component* component = nullptr;

			const size_t size = m_componentMap[T::ID].size();
			for( size_t i = 0; i < size; i++ )	// Iterator through vector of components from the same type
			{
				component = m_componentMap[T::ID][i];

				if( component == nullptr )
				{
					continue;
				}

				if( component->m_ownerId == entityId )		// When we find the first component with the same owner, we can return it
				{

					return dynamic_cast<T*>( component );
				}
			}

			return nullptr;
		}

		/*
		*	Removes the passed component type from the entity with the passed entity id
		*	@param	<T>:		The type of Component to remove, if multiple components of the same type exist, the first component of type <T> found will be removed
		*	@param	EntityId:	The entity id of the entity to remove the component from
		*/
		template<typename T>
		void RemoveComponent( EntityId entityId )
		{
			// Complile-time check to see if class T can be converted to class B, 
				// valid for derivation check of class T from class B
			CanConvert_From<T, Component>();

			Entity* entity = m_entityManager->m_entities[entityId];
			if( entity == nullptr )	// Entity does not exist
			{
				return;
			}

			// Loop through the components of the same type, the id of the component type is used as the key to a vector of components 
			Component* component = nullptr;

			const size_t size = m_componentMap[T::ID].size();
			for( size_t i = 0; i < size; i++ )	// Iterator through vector of components from the same type
			{
				component = m_componentMap[T::ID][i];

				if( component == nullptr )
				{
					continue;
				}

				if( component->m_ownerId == entityId )		// When we find the first component with the same owner, we will remove it
				{

					// Remove the component, from the ComponentMap Vector, replacing the component to be removed with last component in the vector
					m_componentMap[T::ID][i] = m_componentMap[T::ID][size - 1];
					m_componentMap[T::ID].pop_back();

					ComponentId componentId = component->m_componentId;

					// Save this for the swapping later
					uint64_t lastComponentId = --m_entityManager->m_entities[entityId]->m_componentCounter;

					// Assign the component index of the component we are about to delete to the last component on this entity
					m_entityManager->m_entities[entityId]->m_components[componentId] = m_entityManager->m_entities[entityId]->m_components[lastComponentId];

					// If this component is a valid component, then we give it a new component id
					if( m_entityManager->m_entities[entityId]->m_components[componentId] != nullptr )
					{
						m_entityManager->m_entities[entityId]->m_components[componentId]->m_componentId = componentId;
					}

					// Making sure we clean up what we left behind
					m_entityManager->m_entities[entityId]->m_components[lastComponentId] = nullptr;

					// Now we will perform the similar operation for the Component Manager's Array of Components
					// Just using local function variables that already exist
					componentId = component->m_componentManagerId;
					lastComponentId = --this->m_componentCounter;

					// Assign the last component in the array to the removed component's index
					m_components[componentId] = m_components[lastComponentId];
					m_components[lastComponentId] = nullptr;

					if( m_components[componentId] != nullptr )
					{
						m_components[componentId]->m_componentManagerId = componentId;
					}

					// Update systems, now that we have removed a component from this entity
					m_systemManager->OnEntitySignatureChanged( *entity );

					MarkComponentForCleanUp( component );

					break;
				}
			}

		}


		/*
		*	Removes all components from the entity with the passed entity id
		*	@param	EntityId:		The entity id of the entity that will have its components removed
		*/
		void RemoveAllComponents( EntityId entityId );


	private:

		/*
		*	Removes all components from this component manager
		*/
		void RemoveAllComponentsOnManager();


		// Utility function for the ComponentManager to remove components given their component type
		/*
		*	Utility function for removing the passed component type from the entity with the passed entity id
		*	@param	ComponentType:		The type of Component to remove, if multiple components of the same type exist, the first component of the ComponentType found will be removed
		*	@param	EntityId:	The entity id of the entity to remove the component from
		*/
		void RemoveComponent( const Entity& entity, const uint64_t& componentType );

		/*
		*	Marks a component for cleanup and moves it to the clean up vector of components
		*	@param	Component:		The Component that will be marked to cleanup
		*/
		void MarkComponentForCleanUp( Component* component );

		/*
		*	Cleans up all components marked for clean up
		*/
		void CleanUpComponents();

	};

}


#endif // COMPONENTMANAGER_H

