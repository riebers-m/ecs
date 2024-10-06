# ECS Library

A lightweight and flexible **Entity-Component-System** (ECS) library for game development or data-driven design. The
library allows for efficient management of entities and their components, supporting multiple component types and
providing a flexible querying system.

## Features

- **Entity Creation and Destruction**: Easily create and manage entities.
- **Component Management**: Add, retrieve, and remove components from entities.
- **Multiple Components Support**: Assign and manage multiple components for an entity.
- **Views**: Query entities based on component types.
- **Efficient Memory Layout**: Uses a compressed memory layout for efficient memory usage.

## Getting Started

### Prerequisites

- **C++20** or later is required.
- **CMake 3.28** or higher for building the library.

### Installation

You can include this library in your project using CMake. Simply add it as a subdirectory or link it using an alias
target `ecs::ecs` as follows:

```bash
git clone https://github.com/yourusername/ecs.git
```

or use CPM: https://github.com/cpm-cmake/CPM.cmake

In your project's CMakeLists.txt:

```cmake
# Add the ECS library as a dependency

add_subdirectory(path_to_ecs_lib)

# Link the ECS library to your executable

add_executable(myapp main.cpp)
target_link_libraries(myapp ecs::ecs)
```

## Usage

Below are some examples to demonstrate the basic usage of the ECS library.

1. Create an Entity

````c++
#include <ecs/ecs.h>

ecs my_ecs;

// Create a new entity
entity my_entity = my_ecs.create();

````

2. Add Components to an Entity

You can add components to an entity using the emplace method. Each component type must be default constructible.

````c++
struct position {
    float x, y;
};

struct velocity {
    float dx, dy;
};

// Add position and velocity components to an entity
auto err = my_ecs.emplace<position, velocity>(my_entity);
if (err != ecs::error::ok) {
    // handle error
}

````

3. Check if an Entity Contains a Component

You can check if an entity contains a specific component type using the contains function.

````c++
if (my_ecs.contains<position>(my_entity)) {
    // The entity has a position component
}

````

4. Retrieve a Single Component

To retrieve a single component, use the get function.

````c++
try {
    position& pos = my_ecs.get<position>(my_entity);
    pos.x = 10.0f;
    pos.y = 5.0f;
} catch(std::out_of_range const& e){}
````

5. Retrieve Multiple Components as a Tuple

To retrieve multiple components, use the get_multiple function.

````c++
try{
    auto& [pos, vel] = my_ecs.get_multiple<position, velocity>(my_entity);
    pos.x = 10.0f;
    vel.dx = 0.5f;
} catch(std::out_of_range const& e){}

````

6. Querying Entities (Views)

You can create a view to query entities that contain specific components.

````c++
for (auto entity : my_ecs.view<position, velocity>()) {
    auto& [pos, vel] = my_ecs.get_multiple<position, velocity>(entity);
    // Process entity components
}

````

7. Remove Components

You can remove a component from an entity using the erase function.

````c++
if(my_ecs.erase<position>(my_entity) != ecs::error::ok) {
    // handle error
}

````

8. Destroy an Entity

To completely destroy an entity and remove all its components:

````c++
if (my_ecs.destroy(my_entity) != ecs::error::ok) {
    // handle error
}

````

## Contributing

Contributions are welcome! Feel free to open an issue or submit a pull request if you have any improvements or find any
bugs.

## License

This project is licensed under the MIT License. See the LICENSE file for details.