add_requires("catch2 v2.13.10", "fmt >=5")

target("cpp-lazy")
    set_kind("headeronly")
    add_includedirs("include", { interface = true })
    add_packages("fmt", { public = true })

target("cpp-lazy tests")
    set_kind("binary")
    add_files("tests/*.cpp")
    add_deps("cpp-lazy")
    add_packages("catch2")
    add_cxxflags("-Wpedantic -Wextra -Wall -Wshadow -Wno-unused-function -Werror -Wconversion", { tools = { "gcc", "clang"}})
    add_cxxflags("/W4 /permissive- /WX", { tools = { "clang-cl", "msvc" } })
