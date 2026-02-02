---
--- Created by sisco.
--- DateTime: 01/04/2025 17:42
---

function includeImGui()
    filter {}

    defines { "PDL_FEATURE_IMGUI", "IMGUI_DEFINE_MATH_OPERATORS","IMGUI_DISABLE_OBSOLETE_KEYIO" }
    includedirs "%{BASE_DIR}external/imgui"
    includedirs "%{BASE_DIR}external/implot"
end

function compileImGui()
    filter {}
    files {
        "%{BASE_DIR}external/imgui/*.cpp",
        "%{BASE_DIR}external/imgui/*.h",
        "%{BASE_DIR}external/implot/*.cpp",
        "%{BASE_DIR}external/implot/*.h",
    }
end

project "imgui"
    kind "StaticLib"
    language "C++"
    targetdir "%{BASE_DIR}libs/%{cfg.buildcfg}"
    
    -- External libraries
    includeImGui()
    includeVulkan()
    compileImGui()
    -- End external libraries

    configureCommonFlags()
    configureCommonExternals()
    
    setConfigurations()
    
    filter {}
