# Ditto

Generic reusable C++ software components for both embedded and general purpose software. Curated with love and care. In contrast with other libraries, it takes advantage of the latest C++20 features and library components. Therefore, it is best used with a recent compiler with C++20 support.

## Available components

  * `Ditto::LinkedList`: Implementation of a doubly-linked list. Currently it is not very suitable for embedded use, it uses new/delete.
  * `Ditto::Box`: Implementation of a non-null owned pointer, similar to `std::unique_ptr`, but is always valid. When moved, a new object is default constracted in the object that is being moved from.
  * `Ditto::Pair`: Pair implementation. Not much to be seen here.
  * `Ditto::HashMap`: Simple hash map implementation. It uses fixed size buckets and each contains a list to handle hash collisions. Much could be improved in this implementation. Especially to make it more suitable for embedded use. This is still WIP.
  * `Ditto::LinearMap`: More suitable map implementation for embedded systems. It is fully statically allocated and performs linear search for keys so lookup is O(N).
  * `Ditto::PolymorphicObject`: Data structure that can hold different objects statically. You can think of it as an `Ditto::Box<Base>` that can hold derived types statically. For that, it needs to know at compile-time what classes could be instantiated, to make sure it reserves enough memory for them and it is aligned appropriately for all of these derived classes. This is particularly useful for embedded systems when we have heavy objects that implement a given interface and are mutually exclusive (They won't be instantiated at the same time). An example for a usecase is a screen in a graphics system. When switching screens, all widgets in the previous screen will no longer be used and therefore, all screens could be a `Ditto::PolymorphicObject`.
  * `Ditto::CircularQueue`: Implementation of a Circular FIFO Queue statically allocated.
  * `Ditto::StateMachine`: Generic implementation of a FSM where states are represented as an `Ditto::PolymorphicObject`.
  * `Ditto::NonNullPtr`: Implementation of a pointer that cannot be `nullptr`. It asserts that the pointer it is constructed from is not null. It does not own the memory, it's just a wrapper over a raw pointer to imply that it cannot be null.
  * `Ditto::EventLoop`: Implementation of an event loop for embedded use. It follows the observer pattern for Events and loops through an event queue distributing events to its subscriptors.
  * `Ditto::Badge`: Implements the Badge pattern. Functions taking a Badge object can only be called from the templated class of the Badge, since a badge can only be constructed from this templated class.
  * `Ditto::Result`: Encapsulation of the return type of a function. Could either be and Ok value or an Error and has method to find out which one it is and obtain the value.
  * `Ditto::FixedPoint`: Implements a fixed point integer with support for common operations.
  * `Ditto::ResourceLock`: Implements a wrapper of an object and a mutex. The underlying object can only be accessed when the mutex has been locked, ensuring that the access to the resource is always mutually exclusive. It treats all accesses as potentially changing the state of the object, therefore one must also lock for reads even if no other thread is mutating the state of the underlying object.
  * It features a custom assert implementation that can be overriden by the user.


## Using Ditto

This library is distributed with `CMake`. In order to link your project against it you can just use it as an external project:

```CMake
include(FetchContent)

FetchContent_Declare(
  Ditto
    GIT_REPOSITORY https://github.com/Javier-varez/Ditto
    GIT_TAG main)

FetchContent_MakeAvailable(Ditto)

# Now you can link it against your target:
target_link_libraries(<TARGET_NAME> PRIVATE Ditto)
```

## Contributing

Feel free to contribute to this project and open issues. Appreciated contributions include, but are not limited to:
  * Bug reports
  * Code contributions
  * Documentation contributions
  * Issues
  * Feature requests

Happy coding!

