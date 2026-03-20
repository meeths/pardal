group "00 Pardal Engine"

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

    defines {"PDL_VULKAN"}

    includedirs {
        "%{BASE_DIR}packages/pardal-render/",
        "%{BASE_DIR}packages/pardal-core/"
    }


    links { "pardal-core", "imgui" }

    getVulkan()
    getSlang()

    configureCommonFlags()
    configureCommonExternals()
    includeVulkan()
    includeImGui()
    includeSlang()
    includeAssimp()
    linkVulkan()
    setConfigurations()
    
    filter {}

pardal.create_test_project("pardal-render")

    includedirs {
        "%{BASE_DIR}packages/pardal-render/",
        "%{BASE_DIR}packages/pardal-core/"
    }

    links { "pardal-core", "pardal-render", "imgui" }

    includeVulkan()
    includeImGui()
    includeSlang()
    linkVulkan()
    linkSlang()

    links {"Tracy"}
