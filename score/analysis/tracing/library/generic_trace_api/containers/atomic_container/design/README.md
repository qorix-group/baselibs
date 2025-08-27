# Design Atomic Container

## Introduction

The `AtomicContainer` is a class that is used to atomically store data from multiple threads and whose size is set at compile time.
This setup solves both the issue of avoiding dynamic allocations during run time and the concurrency problem.


## Static description

The class needs 3 templates:
- `class T`: The type of the data the user wants to store
- `std::size_t AtomicContainerSize` : The pre-defined size of the container
- `template <class> class AtomicIndirectorType` : This is a special template used only in unit tests to inject a `std::atomic` mock. See this [testing section](#testing-considerations)
  Please see [here](broken_link_g/swh/ddad_platform/blob/master/aas/lib/memory/shared/atomic_indirector.h) for more information. 

The class contains 3 private members:
- `std::atomic<std::size_t> first_empty_` : Index of the first available element (not guaranteed accurate in concurrent usage)
- `std::atomic<std::size_t> empty_elements_count_` : Number of available elements in the container
- `std::array<AtomicContainerElement, AtomicContainerSize> atomic_container_`: 64 bytes aligned array that contains the data which is of custom type `AtomicContainerElement`

The data is stored by wrapping it inside `AtomicContainerElement` which is defined as:
```c++
class AtomicContainerElement
    {
      public:
        T data_; // Client data type
        std::atomic<bool> is_occupied_;  // Occupancy flag
    };
```
With this setup we ensure that the data type can be atomically marked as occupied without needing any extra bookkeeping structures. 

The current available operations for the container are : 
- `[](size_t index)`: Overloaded `[]` operator that returns a mutable or constant reference to element but only to data of type `T` preventing access to the occupancy flag. 
- `Acquire()`: Returns a mutable reference to an unoccupied element in `atomic_container_` and marks it as occupied. Also decreases `empty_elements_count_`. 
- `Release(size_t index)`: Updates occupancy flag in element referenced to `false` and increases `empty_elements_count_` 
- `Release(const T& to_release)`: Finds reference passed by `to_release`, updates its occupancy flag in element referenced to `false` and increases `empty_elements_count_`.
- `FindIf(function predicate)` : Find if method that takes a generic predicate and returns a mutable or constant reference to data of type `T` if found, returns `std::nullopt` otherwise. 
- `IsFreeAt(size_t index)`: Check if element is free at the given index
- `GetEmptyElementsCount`: Returns number of empty elements, i.e. it returns `empty_elements_count_`
- `Size()`: Returns size of the array i.e. template parameter `AtomicContainerSize`

## Dynamic Description
The `Atomic Container` is thread-safe only to acquire and release elements as only the occupancy flag is atomic. 
It does not guarantee that `data_` is also protected against race conditions. 
If two threads get a mutable reference to an element and modify the data concurrently it is a race condition as there are no mechanism that protect the reference. 
Therefore, in a multi-thread context data should only be modified once on acquisition or different fields should be updated in the data. 

The only two thread-safe methods are therefore `Acquire()` and `Release()`. 

`Acquire()` Sequence :
1. Check if there is at least one empty elements by verifying that `empty_elements_count_ > 0`. 
2. If there are no empty elements return `std::nullopt`, otherwise continue. 
3. Load the first potential empty element by doing `(first_empty_ + 1 ) % AtomicContainerSize`.
4. If the element is not already occupied set the `is_occupied_` flag to `true` by doing an atomic compare exchange.
5. If compare exchange is successful then:
   1. Decrease `empty_elements_count_` by 1. 
   2. Update `first_empty` index to refer to the next element.
   3. Return a reference to the `data_` field of that acquired element.
6. If is fails then try again with the next element in the array (go to point 4) until the whole container has been traversed. 
7. If no empty elements could be found return `std::nullopt`.

`Release(std::size_t index)` Sequence:
1. Check if index is within expected bounds
2. If the element at the desired index is occupied set `is_occupied_` flag to `false` by doing an atomic compare exchange. 
3. If compare exchange is successful then increase `empty_elements_count_` by 1.

`Release(T& to_release)` Sequence:
1. Search for a matching reference to `to_release`. If no match is found the function will just return. 
2. Once the element to release has been identified, sequence is same as for `Release(std::size_t index)`. 

## Testing Considerations
To be able to easily mock the atomic operations provided by `std::atomic`, the `AtomicContainer` uses the facilities provided by the `AtomicIndirector` via template injection. 
By default, template resolves to `AtomicIndirectorReal` but in unit tests, `AtomicIndirectorMock` is injected.
More information on the `AtomicIndirector` can be found in its [documentation](broken_link_g/swh/ddad_platform/blob/master/aas/lib/memory/shared/atomic_indirector.h).
