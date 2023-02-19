option("tests", { default = false, description = "Enable tests" })
option("cpp-lazy-use-modules", { default = false, description = "Use cpp-lazy with C++20's modules" })

add_requires("fmt >=5")

if has_config("tests") then
    add_requires("catch2 v2.13.10")
end

add_rules("mode.debug", "mode.release")

target("cpp-lazy")
    add_includedirs("include", { public = true })
    add_packages("fmt", { public = true })
    if has_config("cpp-lazy-use-modules") then
        add_files("src/lz.cppm")
        set_languages("c++20")
        set_kind("static")
    else 
        set_kind("headeronly")
    end
    add_headerfiles("include/(Lz/*.hpp)")
    add_headerfiles("include/(Lz/detail/*.hpp)")
target_end()

if has_config("tests") then
    target("cpp-lazy tests")
        set_kind("binary")
        add_files("tests/test.cpp")
        add_deps("cpp-lazy")
        add_packages("catch2")
        add_cxxflags("-Wpedantic -Wextra -Wall -Wshadow -Wno-unused-function -Werror -Wconversion", { tools = { "gcc", "clang" } })
        add_cxxflags("/W4 /permissive- /WX", { tools = { "clang-cl", "msvc" } })
end
