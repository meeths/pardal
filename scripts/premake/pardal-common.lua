pardal = {}

include "premake-pardal-tests.lua"

BASE_DIR = _MAIN_SCRIPT_DIR .. "/"

function configureCommonExternals()
    filter {}
    includeGLM()
end

function configureCommonFlags()
    flags { "MultiProcessorCompile" }
    fatalwarnings { "All" }    
    filter {"configurations:Release"}
        linktimeoptimization "On"         
    filter {}
    toolset "clang"
    cppdialect "C++23"
    editAndContinue "Off"
    exceptionhandling "Off"
    characterset "Unicode"
end

function setConfigurations()

    filter "platforms:Win64"
        defines { "PDL_PLATFORM_WINDOWS" }

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG", "PDL_DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG", "PDL_RELEASE" }
        optimize "Speed"
        runtime "Release"        

    filter "configurations:Profile"
        defines { "NDEBUG", "PDL_PROFILE" }
        optimize "Speed"
        runtime "Release"        
    end

workspace "pardal"
    location "%{BASE_DIR}projects"
    configurations { "Debug", "Profile", "Release" }
    platforms { "Win64" }

    targetdir "%{BASE_DIR}bin/pardal/%{cfg.buildcfg}"

    filter { "platforms:Win64" }
        system "Windows"
        architecture "x64"

    filter {}
    
include "scripts/premake/pardal-external.lua"
    