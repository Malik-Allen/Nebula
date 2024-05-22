// MIT License, Copyright (c) 2019 Malik Allen

#include "ComponentManager.h"

namespace Nebula
{
	ComponentManager::ComponentManager( EntityManager* entityManager, SystemManager* systemManager ) :
				m_components(),
				m_componentCounter( 0 ),
				m_entityManager( entityManager ),
				m_systemManager( systemManager )
	{}
	
	ComponentManager::~ComponentManager()
	{
		RemoveAllComponentsOnManager();
		CleanUpComponents();
	}

	void ComponentManager::RemoveAllComponents( EntityId entityId )
	{
		Entity* entity = m_entityManager->m_entities[entityId];
		if( entity == nullptr )	// Entity does not exist
		{
			return;
		}

		for( auto* c : entity->GetComponents() )
		{
			if( c != nullptr )
			{
				RemoveComponent( *entity, c->m_componentType );
			}
			else // The moment we find a null component, we now we are at the end of the array, no need to continue
			{
				break;
			}
		}
	}

	void ComponentManager::RemoveAllComponentsOnManager()
	{
		for( size_t i = 0; i < m_components.size(); i++ )
		{

			if( m_components[i] != nullptr )
			{
				MarkComponentForCleanUp( m_components[i] );
			}

		}

		m_componentMap.clear();
	}

	void ComponentManager::RemoveComponent( const Entity& entity, const uint64_t& componentType )
	{
		// Loop through the components of the same type, the id of the component type is used as the key to a vector of components 
		Component* component = nullptr;
		size_t size = m_componentMap[componentType].size();
		for( size_t i = 0; i < size; i++ )	// Iterator through vector of components from the same type
		{
			component = m_componentMap[componentType][i];

			if( component == nullptr )
			{
				continue;
			}

			if( component->m_ownerId == entity.m_entityId )		// When we find the first component with the same owner, we will remove it
			{

				// Remove the component, from the ComponentMap Vector, replacing the component to be removed with last component in the vector
				m_componentMap[componentType][i] = m_componentMap[componentType][size - 1];
				m_componentMap[componentType].pop_back();

				ComponentId componentId = component->m_componentId;

				// Removing the reference from the list of components on the passed entity
				m_entityManager->m_entities[entity.m_entityId]->m_components[componentId] = nullptr;
				--m_entityManager->m_entities[entity.m_entityId]->m_componentCounter;

				// Now we will perform the similar operation for the Component Manager's Array of Components
				// Just using local function variables that already exist
				componentId = component->m_componentManagerId;
				ComponentId lastComponentId = --this->m_componentCounter;

				// Assign the last component in the array to the removed component's index
				m_components[componentId] = m_components[lastComponentId];
				m_components[lastComponentId] = nullptr;

				if( m_components[componentId] != nullptr )
				{
					m_components[componentId]->m_componentManagerId = componentId;
				}

				if( m_systemManager )
				{
					// Update systems, now that we have removed a component from this entity
					m_systemManager->OnEntitySignatureChanged( entity );
				}

				MarkComponentForCleanUp( component );

				break;
			}
		}
	}

	void ComponentManager::MarkComponentForCleanUp( Component* component )
	{
		if( component == nullptr )
		{
			return;
		}

		component->m_bMarkedForCleanUp = true;
		m_componentsMarkedForCleanUp.push_back( component );
	}

	void ComponentManager::CleanUpComponents()
	{
		size_t size = m_componentsMarkedForCleanUp.size();
		for( size_t i = 0; i < size; ++i )
		{
			if( m_componentsMarkedForCleanUp[i] != nullptr )
			{
				delete m_componentsMarkedForCleanUp[i], m_componentsMarkedForCleanUp[i] = nullptr;
			}
		}

		m_componentsMarkedForCleanUp.clear();
	}

};