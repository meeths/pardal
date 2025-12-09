---
--- Created by sisco.
--- DateTime: 25/05/2025 20:21
---
SLANG_VER = "2025.9.2"
SLANG_DIR = BASE_DIR .. "external/downloaded/slang"..SLANG_VER

function getSlang()
    SLANG_ZIPFILE = BASE_DIR .. "temp/slang.zip"
    SLANG_URL = "https://github.com/shader-slang/slang/releases/download/v"..SLANG_VER.."/slang-"..SLANG_VER.."-windows-x86_64.zip"
    downloadAndExtractZIP("slang", SLANG_URL, SLANG_DIR)
end

function includeSlang()
    includedirs { SLANG_DIR .. "/include" }
    filter {}
end

function linkSlang()
    libdirs { SLANG_DIR .. "/lib" }
    links "slang.lib"

    postbuildcommands { "{copy} %{wks.location}../external/downloaded/slang" .. SLANG_VER .. "/bin/slang.dll %{cfg.buildtarget.directory}" }

    filter {}
end