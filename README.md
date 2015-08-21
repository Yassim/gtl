# gtl
Game (or performance) oriented stl replacement

# Work In progress.
# Do NOT use in production

## TODO

 - [x] heap
 - [x] aligned heap (msvc)
 - [x] vector
 - [ ] array
 - [ ] paged_vector
 - [ ] map (in progress)
 - [ ] unordered_map
 - [ ] queue
 - [ ] pooled_string
 - [ ] set ?
 - [ ] list ??
 - [ ] atomic
    - [ ] counter
    - [ ] pointer
 - [ ] serialisation
    - [ ] text (xml)
    - [ ] binary (native endian)
 - [ ] persistant
   - [x] devirtualised base
   - [ ] persistant heap
   - [ ] persistant_ptr
 - [ ] spatial
   - [ ] r-tree

## building
Visual studio. Open the GTL project. press F5
Clang. command line of "clang++ src/utilities.cc src_test/*.cc src_test/persistant/*.cc src_test/meta/*.cc" works for me so far (Not it does produce warnings in the tests because I'm using C++11 funky ness in them, but not in the main headers)
