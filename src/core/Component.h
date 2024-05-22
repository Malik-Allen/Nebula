// MIT License, Copyright (c) 2019 Malik Allen

#ifndef NEBULA_COMPONENT_H
#define NEBULA_COMPONENT_H

#include "Constants.h"

namespace Nebula
{
	class Component
	{
		Component(const Component &) = delete;
		Component(Component &&) = delete;
		Component& operator=( const Component& ) = delete;
		Component &operator=(Component &&) = delete;

		friend class ComponentManager;

		// The owning entity's id
		EntityId m_ownerId;

		// The unique identier for this component
		ComponentId m_componentId;

		// The unique identier for this component, used only by the component manager
		uint64_t m_componentManagerId;

		// This component's unique type identifier
		uint64_t m_componentType;

		// Used by the ComponentManager to show when a component is ready to be cleaned up
		bool m_bMarkedForCleanUp;

	public:
		explicit Component(uint64_t componentType) : m_ownerId(0),
													 m_componentId(0),
													 m_componentManagerId(0),
													 m_componentType(componentType),
													 m_bMarkedForCleanUp(false)
		{};

		virtual ~Component() {}

		inline const EntityId &GetOwnerEntity() const { return m_ownerId; }

		inline const ComponentId &GetComponentId() const { return m_componentId; }

		inline const uint64_t &GetComponentType() const { return m_componentType; }
	};

}

#endif // ! NEBULA_COMPONENT_H
