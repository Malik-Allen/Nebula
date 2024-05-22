// MIT License, Copyright (c) 2019 Malik Allen

#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "Entity.h"
#include "../utility/ObjectPool.h"

#include <map>
#include <vector>

namespace Nebula
{
	/*
	*	Entity Manager is responsible for managing the lifetime, creation, and destruction of entities
	*/
	class EntityManager
	{
		template<typename ... T >
		friend struct Parser;

		friend class ComponentManager;

		// The entities created by this Entity Manager
		std::map<EntityId, Entity*>    m_entities;

		// The number of entities in this entity manager
		uint64_t				m_entityCounter;

		// Entities that have been removed from the 'm_entities' map and have been marked for clean up
		std::vector<Entity*>	m_entitiesMarkedForCleanUp;

		// Object pool used to manage the creation and deletion of entities
		ObjectPool<Entity>		m_entityPool;

	public:

		EntityManager();
		~EntityManager();

		/*
		*	Creates an Entity, returning the EntityId for the created entity
		*	@return	EntityId:	The EntityId of the created entity, if an Entity could not be created an EntityId of 0 will be returned
		*/
		EntityId CreateEntity();
		
		/*
		*	Marks the Entity with the identical EntityId that has been passed for clean up
		*	@param	EntityId:	The EntityId of the Entity that should be marked for clean up
		*	@return	bool:	Returns true, if the passed EntityId exists and has been successfully marked for clean up. Returns false, if otherwise
		*/
		bool MarkEntityForCleanUp( EntityId entityId );

	private:

		/*
		*	This utility function marks the passed entity for clean up and adds the passed entity to the marked for clean up array of entities
		*	@param	Entity:		The Entity to be marked for clean up
		*/
		void MarkEntityForCleanUp( Entity* entity );

		/*
		*	Marks all live entities for clean up
		*/
		void MarkAllEntitiesForCleanUp();

		/*
		*	Returns a pointer to a new Entity, ready to be used
		*/
		Entity* GetNewEntity();

		/*
		*	Cleans up the entities marked for clean up
		*/
		void CleanUpEntities();

	};

}


#endif // ENTITYMANAGER_H