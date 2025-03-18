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
    
    includedirs {
        "%{BASE_DIR}packages/pardal-render/include",
        "%{BASE_DIR}packages/pardal-core/include"
    }


    links { "pardal-core" }

    configureCommonFlags()
    setConfigurations()
    
    filter {}