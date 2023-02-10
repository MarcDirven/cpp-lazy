option("tests", {default = false, description = "Enable tests"})

add_requires("fmt >=5")

if has_config("tests") then
    add_requires("catch2 v2.13.10")
end

target("cpp-lazy")
    set_kind("headeronly")
    add_includedirs("include", { public = true })
    add_packages("fmt", { public = true })
    add_headerfiles("include/Lz/*.hpp")
    add_headerfiles("include/Lz/detail/*.hpp")
target_end()

if has_config("tests") then
    target("cpp-lazy tests")
        set_kind("binary")
        add_files("tests/*.cpp")
        add_deps("cpp-lazy")
        add_packages("catch2")
        add_cxxflags("-Wpedantic -Wextra -Wall -Wshadow -Wno-unused-function -Werror -Wconversion", { tools = { "gcc", "clang"}})
        add_cxxflags("/W4 /permissive- /WX", { tools = { "clang-cl", "msvc" } })
end
