set_languages("c++20")
add_rules("mode.debug", "mode.release")
-- set_optimize("fastest")

local libs = { "cxxopts", "spdlog", "nlohmann_json", "gtest" }

add_includedirs("include")
add_requires(table.unpack(libs))

target("pile")
  set_kind("static")
  add_files("source/**/*.cpp")
  add_packages(table.unpack(libs))

target("peak")
  set_kind("binary")
  add_files("standalone/main.cpp")
  add_packages(table.unpack(libs))
  add_deps("pile")

target("test")
  set_kind("binary")
  add_files("test/*.cpp")
  add_packages(table.unpack(libs))
  add_deps("pile")
