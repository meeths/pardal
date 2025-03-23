group "00 Pardal Engine"

include "scripts/premake/external/vulkan.lua"

project "pardal-render"
    kind "StaticLib"
    language "C++"
    targetdir "%{BASE_DIR}libs/%{cfg.buildcfg}"
    
    files {
        "%{BASE_DIR}packages/pardal-render/**.h",
        "%{BASE_DIR}packages/pardal-render/**.hpp",
        "%{BASE_DIR}packages/pardal-render/**.inl",
        "%{BASE_DIR}packages/pardal-render/**.cpp",
        "%{BASE_DIR}packages/pardal-render/**.c"
    }
    
    includedirs {
        "%{BASE_DIR}packages/pardal-render/",
        "%{BASE_DIR}packages/pardal-core/"
    }


    links { "pardal-core" }

    configureCommonFlags()
    configureCommonExternals()
    includeVulkan()
    linkVulkan()
    setConfigurations()
    
    filter {}