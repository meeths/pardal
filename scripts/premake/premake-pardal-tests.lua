
function pardal.create_test_project(projectname)
    project ( projectname .. "-tests" )
    kind "ConsoleApp"

    filter {}
    links { projectname }

    language "C++"
    targetdir ("%{BASE_DIR}bin/" .. projectname .."-tests/%{cfg.buildcfg}")

    files{
        "%{BASE_DIR}packages/" .. projectname.."-tests/**.cpp",
        "%{BASE_DIR}external/googletest/googletest/src/gtest-all.cc",
        "%{BASE_DIR}external/googletest/googletest/src/gtest_main.cc",
        "%{BASE_DIR}external/googletest/googlemock/src/gmock-all.cc",
    }

    includedirs {
        "%{BASE_DIR}packages/" .. projectname,
        "%{BASE_DIR}external/googletest/googletest/include",
        "%{BASE_DIR}external/googletest/googlemock/include",
        "%{BASE_DIR}external/googletest/googletest",
        "%{BASE_DIR}external/googletest/googlemock"
    }

    -- External libraries
    -- End external libraries

    configureCommonFlags()
    configureCommonExternals()
    setConfigurations()

    filter {}

end 