group "20 Pardal Apps"

project "pardal-test-app"
    kind "ConsoleApp"
    links { "pardal-core", "pardal-render", "imgui" }
    language "C++"
    targetdir "%{BASE_DIR}bin/pardal-test-app/%{cfg.buildcfg}"
    
    files {
        "%{BASE_DIR}packages/pardal-test-app/**.h",
        "%{BASE_DIR}packages/pardal-test-app/**.hpp",
        "%{BASE_DIR}packages/pardal-test-app/**.inl",
        "%{BASE_DIR}packages/pardal-test-app/**.cpp",
        "%{BASE_DIR}packages/pardal-test-app/**.c"
    }
    
    includedirs {
        "%{BASE_DIR}packages/pardal-core",
        "%{BASE_DIR}packages/pardal-render",
        "%{BASE_DIR}packages/pardal-test-app"
    }
    
    configureCommonFlags()
    configureCommonExternals()
    
    includeVulkan()
    includeImGui()
    linkVulkan()
     
    setConfigurations()
    
    filter {}
