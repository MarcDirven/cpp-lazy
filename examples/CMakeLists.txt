cmake_minimum_required(VERSION 3.12.4)

project(examples)

function(create_example_project name)
    add_executable(IteratorExample${name} ${name}.cpp)
    target_link_libraries(IteratorExample${name} PUBLIC cpp-lazy)
    set_property(TARGET IteratorExample${name} PROPERTY CXX_STANDARD 14)
endfunction()


create_example_project(Choose)
create_example_project(Enumerate)
create_example_project(Except)
create_example_project(Filter)
create_example_project(Generate)
create_example_project(Map)
create_example_project(Random)
create_example_project(Range)
create_example_project(Repeat)
create_example_project(StringSplitter)
create_example_project(Take)
create_example_project(TakeEvery)
create_example_project(Zip)
