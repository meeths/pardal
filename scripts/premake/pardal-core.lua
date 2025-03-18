group "00 Pardal Engine"

project "pardal-core"
    kind "StaticLib"
    language "C++"
    targetdir "%{BASE_DIR}libs/%{cfg.buildcfg}"

    files { 
        "%{BASE_DIR}packages/pardal-core/**.h", 
        "%{BASE_DIR}packages/pardal-core/**.hpp", 
        "%{BASE_DIR}packages/pardal-core/**.inl", 
        "%{BASE_DIR}packages/pardal-core/**.cpp", 
        "%{BASE_DIR}packages/pardal-core/**.c" 
    }

    includedirs {
        "%{BASE_DIR}packages/pardal-core/include/"
    }

    configureCommonFlags()
    setConfigurations()

    filter {}

pardal.create_test_project("pardal-core")
