set_languages("c++20")
add_rules("mode.debug", "mode.release")
-- set_optimize("fastest")

local libs = { "cxxopts", "spdlog" }
add_requires(table.unpack(libs))
add_includedirs("include")

target("pile")
  set_kind("static")
  add_files("source/**/*.cpp")
  add_packages(table.unpack(libs))

target("peak")
  set_kind("binary")
  add_files("standalone/main.cpp")
  add_packages(table.unpack(libs))
  add_deps("pile")
