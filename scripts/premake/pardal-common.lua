pardal = {}

include "premake-pardal-tests.lua"

BASE_DIR = _MAIN_SCRIPT_DIR .. "/"

function downloadAndExtractZIP(name, url, dstpath)
    if not os.isdir(dstpath) then
        TEMP_ZIP_PATH = BASE_DIR .. "temp/" ..name..".zip"
        io.write("Downloading " .. name .. " (" .. url ..")...")
        os.mkdir(BASE_DIR .. "temp")
        local result_str, response_code = http.download(url, TEMP_ZIP_PATH)
        print("done ("..result_str ..", "  ..response_code..")")
        io.write("Extracting ".. name .. "...")
        zip.extract(TEMP_ZIP_PATH, dstpath)
        print("done")
        os.remove(TEMP_ZIP_PATH)
    end
end

function downloadAndInstallNSIS(name, url, dstpath)
    if not os.isdir(dstpath) then
        TEMP_EXE_PATH = BASE_DIR .. "temp/" ..name..".exe"
        io.write("Downloading " .. name .. " (" .. url ..")...")
        os.mkdir(BASE_DIR .. "temp")
        local result_str, response_code = http.download(url, TEMP_EXE_PATH)
        print("done ("..result_str ..", "  ..response_code..")")
        io.write("installing ".. name .. "...")
        install_command = TEMP_EXE_PATH .. " /S /D="..dstpath
        print(install_command)
        os.execute(install_command)
        print("done")
        os.remove(TEMP_EXE_PATH)
    end
end

function configureCommonExternals()
    filter {}
    includeEASTL()
    includeMimalloc()
    includeGLM()
    includeSTB()
    includeTracy()
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
    vectorextensions "AVX2"
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
    
    filter {}
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
    