# Entity-Componten-System

Simple ECS lib

## API

### Entity

```c++
ecs::ecs ecs;
```

To get a new entity from the ecs, simply call *create*.

````c++
ecs::entity entity = ecs.create();
````

To destroy a entity with all its components.

````c++
ecs::error err = ecs.destroy(entity);
````

If an entity is released, the ecs reuses it freely internally.

To destroy all entities and its components at once.

````c++
ecs::error err = ecs.clear();
````

### Component

All component types have to be aggregate types.
Adding a zero initialized component for entity.

````c++
ecs::error err = ecs.emplace<position>(entity);
````

Adding a custom component for entity.

````c++
ecs::error err = ecs.insert<position>(entity, {0,0});
````

Adding multiple components for an entity at once. All components are zero initialized.

````c++
ecs::error err = ecs.emplace<position, velocity>(entity);
````

To check if an entity owns components.

````c++
bool ok = ecs.all_of<position, velocity>(entity);
````

Or to check if one component in a set is owned.

````c++
bool ok = ecs.any_of<position, velocity>(entity);
````

Remove component from owned entity.

````c++
ecs::error err = ecs.erase<position>(entity);
````

To get a component owned by an entity.

````c++
auto const& position = ecs.get<position>(entity);
````

It is also possible to get multiple components at once.

````c++
auto const [pos, vel] = ecs.get<position, velocity>(entity);
````

To get all entities owning a specific component.

````c++
auto view = ecs.view<position>();
````

Then iterating ower all components like this.

````c++
for(auto entity:view) {
    auto& pos = view.get<position>(entity);
}
````

To manipulate multiple components owned by an entity.

````c++
ecs.view<position,velocity>([dt](auto& pos, auto& vel){
    pos.x += vel.x * dt;
    pos.y += vel.y * dt;
});
````
