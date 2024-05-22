// MIT License, Copyright (c) 2019 Malik Allen

#ifndef NEBULA_ENTITY_H
#define NEBULA_ENTITY_H

#include "Constants.h"

#include <array>

namespace Nebula
{
	/*
	*  The Entity represents an EntityId that contains Components
	*/
	struct Entity
	{
		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
		Entity& operator=( const Entity& ) = delete;
		Entity& operator=(Entity&&) = delete;
		
		Entity() : m_entityId( 0 ), m_componentCounter( 0 ), m_components(), m_bMarkedForCleanUp(false) {}
		~Entity() = default;	

		inline const EntityId& GetId() const { return m_entityId; }
		inline const uint64_t& GetComponentCount() const { return m_componentCounter; }
		inline const std::array<class Component*, MAX_COMPONENTS_PER_ENTITY>& GetComponents() const { return m_components; }

		friend bool operator== ( const Entity& e1, const Entity& e2 )
		{
			return e1.GetId() == e2.GetId();
		}

		friend bool operator!= ( const Entity& e1, const Entity& e2 )
		{
			return e1.GetId() != e2.GetId();
		}

	private:

		friend class EntityManager;
		friend class ComponentManager;

		// Unique identifier for this entity
		EntityId			m_entityId;

		// Number of components on this entity
		uint64_t			m_componentCounter;

		// Components attached to this entity
		std::array<Component*, MAX_COMPONENTS_PER_ENTITY> m_components;

		// Used to determine when an entity has been marked for clean up by the EntityManager
		bool				m_bMarkedForCleanUp;

	};
}

#endif // NEBULA_ENTITY_H