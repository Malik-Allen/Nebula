# NEBULA

## Entity Component System

### Quick Start

`Entity`, `Component`, `System` are all instantiated and managed through a `World` object.

All user-defined Components and Systems must contain the following static member:

`static constexpr uint32_t ID = GENERATE_ID( "ExampleClassName" );`

The `ID` allows the ECS to generate a unique identifier for the given component or system at compile-time.

This `ID` must be passed as a paramter to the parent constructor call in classes derived from `System` and `Component`, see example below:

```
class FooComponent : public Nebula::Component {
public:
	static constexpr uint32_t ID = GENERATE_ID( "FooComponent" );

	FooComponent(): 
        Component(ID) 
    {};

	~FooComponent() = default;
};
```

Once instantiated, the `Nebula::World` object is responsible for creating entities and adding/removing components from entities at run-time.

All systems should be registered before their components are added to entities.

`Nebula::Parser<...>` can be used on a `World` object to obtain all entities with the matching `Component` signature, see example below:

`Nebula::Parser<AudioComponent, PhysicsComponent>( WorldObject );`


### Systems

All Systems are defined by its `Component Signature`, the components that need to be present on the Entity to be updated within this System, see example below:

```
class FooSystem : public Nebula::System<FooComponent, FoobarComponent>
{
public:
	static constexpr uint64_t ID = GENERATE_ID("FooSystem");

	FooSystem(): 
        System(ID) 
    {}
	~FooSystem() = default;
};
```
All user-defined systems must be registered with a `World` object at run-time.

`std::get<FooComponent*>` can be used to obtain elements of component signatures (std::tuple)

### Features

Custom constructors are supported for user-defined Component and System classes.