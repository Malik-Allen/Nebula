// MIT License, Copyright (c) 2019 Malik Allen

#ifndef NEBULA_OBJECTPOOL_H
#define NEBULA_OBJECTPOOL_H

#include <vector>
#include <map>

namespace Nebula
{
	/*
	* This Object Pool class can be used to contain a simple object pool for the given template classe
	* ATM, there is no limit to the size of the object pool, it can be used to create a fixed or dynamic pool
	*/
	template <typename T>
	class ObjectPool
	{
	public:
		ObjectPool() = default;

		~ObjectPool()
		{
			const size_t poolSize = objects.size();
			for( size_t i = 0; i < poolSize; ++i )
			{
				if( objects[i] != nullptr )
				{
					delete objects[i], objects[i] = nullptr;
				}
			}
			objects.clear();
		}

		/*
		*  Utility function to create a new object and adds it to this pool
		*/
		template<typename ... Args>
		void CreateNewObjectInPool( Args&& ... args )
		{
			T* object = new T( std::forward<Args>( args ) ... );

			if( object == nullptr )	// Could not create object
			{
				return;
			}
			objects.push_back( object );
		}

		/*
		*	Creates a new object for this pool.
		*	WARNING, if this function's return value is not stored, return it back to the pool.
		*	TODO: Upgrade the concept/workflow here, avoid dangling pointers and track newly created object: MAP?
		*	@return T*:		The object created from this pool
		*/
		template<typename ... Args>
		T* CreateNewObject( Args&& ... args )
		{
			T* object = new T( std::forward<Args>( args ) ... );

			if( object == nullptr )	// Could not create object
			{
				return nullptr;
			}
			return object;
		}

		/*
		*	Returns the last available object from inside this object pool, removing it from the pool altogether
		*/
		T* GetObject()
		{
			T* object = nullptr;
			if( objects.empty() )
			{
				return object;
			}
			object = objects[objects.size() - 1];
			objects.pop_back();
			return object;
		}

		/*
		*	Returns the passed object to this object pool
		*	@param	Object:		The object that will be returned to the pool, ONLY valid object pointers will be return the pool successfully
		*/
		void ReturnObject( T* object )
		{
			if( object == nullptr )
			{
				return;
			}

			for( const auto* obj : objects )
			{
				if( obj == object )
				{
					return;
				}
			}
			objects.push_back( object );
		}

	private:
		ObjectPool( const ObjectPool& ) = delete;
		ObjectPool& operator=( const ObjectPool& ) = delete;
		ObjectPool( ObjectPool&& ) = delete;
		ObjectPool& operator=( ObjectPool&& ) = delete;

		// Objects inside of pool
		std::vector<T*> objects;

		// Objects taken from the pool
		// std::map<T*, int> objectsTaken;
	};
}

#endif // !NEBULA_OBJECTPOOL_H

