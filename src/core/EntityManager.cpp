// MIT License, Copyright (c) 2019 Malik Allen

#include "EntityManager.h"

namespace Nebula
{
	EntityManager::EntityManager() :
		m_entityCounter( 0 )
	{
		for( uint64_t i = 0; i < MAX_ENTITIES; ++i )
		{
			m_entityPool.CreateNewObjectInPool();
		}
	}

	EntityManager::~EntityManager()
	{
		MarkAllEntitiesForCleanUp();
		CleanUpEntities();
	}

	EntityId EntityManager::CreateEntity()
	{

		/* '>' check works for entity counter, because the 0 entity id is reserved for an invalid entity id, and an entity id is incremented as its assigned*/
		if( m_entityCounter > MAX_ENTITIES )
		{
			return 0;
		}

		Entity* entity = GetNewEntity();

		if( entity == nullptr )
		{
			return 0;
		}

		entity->m_entityId = ++m_entityCounter;
		m_entities[m_entityCounter] = entity;

		return m_entityCounter;

	}


	bool EntityManager::MarkEntityForCleanUp( EntityId entityId )
	{
		Entity* entity = m_entities[entityId];

		// Entity does not exist, returning
		if( entity == nullptr )
		{
			return false;
		}

		m_entities.erase( entityId );

		MarkEntityForCleanUp( entity );

		--m_entityCounter;

		return true;
	}

	void EntityManager::MarkEntityForCleanUp( Entity* entity )
	{
		entity->m_bMarkedForCleanUp = true;
		entity->m_entityId = 0;
		m_entitiesMarkedForCleanUp.push_back( entity );
	}

	void EntityManager::MarkAllEntitiesForCleanUp()
	{
		Entity* entity = nullptr;
		for( uint64_t i = 0; i < m_entityCounter; i++ )
		{
			entity = m_entities[i];

			// Entity does not exist, returning
			if( entity == nullptr )
			{
				return;
			}

			MarkEntityForCleanUp( entity );
		}
		m_entities.clear();
		m_entityCounter = 0;
	}

	Entity* EntityManager::GetNewEntity()
	{
		Entity* entity = nullptr;

		size_t numEntitiesMarkedForCleanUp = m_entitiesMarkedForCleanUp.size();
		if( numEntitiesMarkedForCleanUp > 0 )
		{
			// Grab the last index of entities marked for clean up
			entity = m_entitiesMarkedForCleanUp[numEntitiesMarkedForCleanUp - 1];
			entity->m_bMarkedForCleanUp = false;
			m_entitiesMarkedForCleanUp.pop_back();
			return entity;
		}

		// Otherwise, grab an entity from the entity pool
		entity = m_entityPool.GetObject();

		if( entity == nullptr )
		{
			return nullptr;
		}

		return entity;
	}

	void EntityManager::CleanUpEntities()
	{
		Entity* entity = nullptr;
		size_t numEntitiesMarkedForCleanUp = m_entitiesMarkedForCleanUp.size();
		for( int i = 0; i < numEntitiesMarkedForCleanUp; ++i )
		{
			if( m_entitiesMarkedForCleanUp[i] != nullptr )
				m_entitiesMarkedForCleanUp[i]->m_bMarkedForCleanUp = false;

			m_entityPool.ReturnObject( m_entitiesMarkedForCleanUp[i] );
		}
		m_entitiesMarkedForCleanUp.clear();
	}

};