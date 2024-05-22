// MIT License, Copyright (c) 2019 Malik Allen

#ifndef NEBULA_WORLD_H
#define NEBULA_WORLD_H

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

#include "../utility/TemplateHelper.h"

#include <vector>

namespace Nebula
{
	class World
	{
		EntityManager* m_enityManager;

		SystemManager* m_systemManager;

		ComponentManager* m_componentManager;

		template<typename ... T>
		friend struct Parser;

	public:
		World() :
			m_enityManager( new EntityManager() ),
			m_systemManager( new SystemManager() ),
			m_componentManager( new ComponentManager( m_enityManager, m_systemManager ) )
		{
			m_systemManager->SetWorld( this );
		}
		
		~World()
		{
			// Each Manager will handle the destruction of their items

			// Systems get deleted first
			if ( m_systemManager )
			{
				delete m_systemManager;
				m_systemManager = nullptr;
			}

			// Now we remove all components from the component manager
			if ( m_componentManager )
			{
				delete m_componentManager;
				m_componentManager = nullptr;
			}

			// Finally clear out all entities
			if ( m_enityManager )
			{
				delete m_enityManager;
				m_enityManager = nullptr;
			}
		}

		// Will create the number of entities passed, given that you do not exceed entity limits
		std::vector<EntityId> CreateEntities( uint64_t numberOfEntities )
		{
			std::vector<EntityId> createdEntities;
			EntityId currentEntityId;

			for ( int i = 0; i < numberOfEntities; ++i )
			{
				currentEntityId = m_enityManager->CreateEntity();

				if ( currentEntityId != 0 )
					// A valid Entity Id is a non-zero value
				{
					createdEntities.push_back( currentEntityId );
				}

			}

			return createdEntities;
		}


		// Will create 'n' number of entities with the passed components added to it, Returns vector of the entityIds
		template<class ... Components>
		std::vector<EntityId> CreateEntitiesWithComponents( uint64_t numberOfEntities )
		{
			std::vector<EntityId> createdEntities;
			EntityId currentEntityId = 0;
			for ( int i = 0; i < numberOfEntities; ++i )
			{
				currentEntityId = m_enityManager->CreateEntity();

				if ( currentEntityId != 0 )
					// A valid Entity Id is a non-zero number, therefore add components to valid entities
				{

					AddNewComponentToEntity< 0, Components ... >( currentEntityId );

					createdEntities.push_back( currentEntityId );
				}
			}
			return createdEntities;
		}

		// Destroys Entity with the passed EntityId, removing all components in the process
		void DestroyEntity( EntityId entityId )
		{
			m_componentManager->RemoveAllComponents( entityId );
			m_enityManager->MarkEntityForCleanUp( entityId );
		}

		// Adds Component to entity with passed EntityId
		template<typename T, typename ... Args>
		T* AddComponentToEntity( EntityId entityId, Args&& ... args )
		{
			return m_componentManager->AddComponent<T, Args ...>( entityId, std::forward<Args>( args ) ... );
		}

		// Removes component from entity with passed EntityId, if Component exists on entity
		template<typename T>
		void RemoveComponentFromEntity( EntityId entityId )
		{
			m_componentManager->RemoveComponent<T>( entityId );
		}

		// Find the component class on the passed entity, returning the component if it exists
		template<typename T>
		T* FindComponentInEntity( EntityId entityId )
		{
			return m_componentManager->FindComponent<T>( entityId );
		}


		// Registers Systems, inside of system manager
		template<typename T>
		T* RegisterSystem()
		{
			return m_systemManager->RegisterSystem<T>();
		}

		// Deregisters system from system manager
		template<typename T>
		void UnregisterSystem()
		{
			m_systemManager->UnregisterSystem<T>();
		}

		// Registers Systems, inside of system manager
		template<typename T>
		T* GetSystem()
		{
			return m_systemManager->GetSystem<T>();
		}


		// Update World Systems
		void Update( float deltaTime )
		{
			m_systemManager->Update( deltaTime );
		}

	private:
		// Recursively adds components to the entity with the passed id
		template<size_t INDEX, typename ComponentClass, typename ... Components>
		void AddNewComponentToEntity( EntityId entityId )
		{
			// Complile-time check to see if class T can be converted to class B, 
				// valid for derivation check of class T from class B
			CanConvert_From<ComponentClass, Component>();

			if ( m_componentManager->AddComponent<ComponentClass>( entityId ) != nullptr )
				// When we successfully add a component, carry on with the process
			{
				AddNewComponentToEntity< INDEX + 1, Components ... >( entityId );
			}
		}

		template<size_t INDEX>
		void AddNewComponentToEntity( EntityId entityId )
		{}
	};
}



#endif // !NEBULA_WORLD_H

