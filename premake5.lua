-- For reference, please refer to the premake wiki:
-- https://github.com/premake/premake-core/wiki

---------------------------------
-- [ WORKSPACE CONFIGURATION   --
---------------------------------
workspace "Mu"                   -- Solution Name
  configurations { "Debug", "Release" }  -- Optimization/General config mode in VS
  platforms      { "x64"}        -- Dropdown platforms section in VS

  -------------------------------
  -- [ COMPILER/LINKER CONFIG] --
  -------------------------------
  warnings "Extra"

  -- see 'filter' in the wiki pages
  filter "configurations:Debug"    defines { "DEBUG" }  symbols  "On"
  filter "configurations:Release"  defines { "NDEBUG" } optimize "On"

  filter { "platforms:*32" } architecture "x86"
  filter { "platforms:*64" } architecture "x64"

  -- when building any visual studio project
  filter { "system:windows", "action:vs*"}
    flags         { "MultiProcessorCompile", "NoMinimalRebuild" }
    linkoptions   { "/ignore:4099" }      -- Ignore library pdb warnings when running in debug

  filter {} -- clear filter when you know you no longer need it!

  -------------------------------
  -- [ PROJECT CONFIGURATION ] --
  ------------------------------- 

  project "Mu"
    location "Mu"
    kind "ConsoleApp" -- "WindowApp" removes console
    language "C++"
    targetdir ("bin_%{cfg.buildcfg}_%{cfg.platform}") -- where the output binary goes.
    targetname "mu" -- the name of the executable saved to targetdir


    --------------------------------------
    -- [ PROJECT FILES CONFIGURATIONS ] --
    --------------------------------------
    local SourceDir = "Mu/src/";
    -- what files the visual studio project/makefile/etc should know about
    files
    { 
      SourceDir .. "**.hpp", 
      SourceDir .. "**.cpp",
    }

    includedirs
    {
      "%{prj.name}/src",
    }

    filter "system:windows"
      cppdialect "C++17"
      staticruntime "On"
      systemversion "latest"

      defines {

      }

    filter {}

    -- setting up visual studio filters (basically virtual folders).
    -- vpaths 
    -- {
    --   ["Header Files/*"] = { SourceDir .. "**.h", SourceDir .. "**.hxx", SourceDir .. "**.hpp" },
    --   ["Source Files/*"] = { SourceDir .. "**.c", SourceDir .. "**.cxx", SourceDir .. "**.cpp" },
    -- }

    -------------------------------------------
    -- [ PROJECT DEPENDENCY CONFIGURATIONS ] --
    -------------------------------------------
    libdirs
    {
      -- add dependency directories here
    }

    links
    {
      -- add depedencies (libraries) here
    }
