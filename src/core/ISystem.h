// MIT License, Copyright (c) 2019 Malik Allen

#ifndef NEBULA_ISYSTEM_H
#define NEBULA_ISYSTEM_H

namespace Nebula 
{
	class ISystem
	{
		ISystem(const ISystem&) = delete;
		ISystem(ISystem&&) = delete;
		ISystem& operator=(const ISystem&) = delete;
		ISystem& operator=(ISystem&&) = delete;

		friend class SystemManager;

		// Unique Identifier Managed by the SystemManager
		uint64_t		m_systemManagerId;

		// Unique Identifier Managed by the SystemManager
		uint64_t		m_systemId;

		// The world this system exists in
		class World*			m_world;

	public:

		explicit ISystem(uint64_t systemID):
			m_systemManagerId(0),
			m_systemId(systemID),
			m_world(nullptr)
		{};
		virtual ~ISystem() = default;

		virtual void Update(float deltaTime) = 0;

		virtual void OnEntitySignatureChanged( const struct Entity& entity ) = 0;

	protected:

		inline World* GetWorld() const
		{
			return m_world;
		};

	};
	
}




#endif // !NEBULA_ISYSTEM_H



