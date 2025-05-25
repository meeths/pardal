---
--- Created by sisco.
--- DateTime: 25/05/2025 20:21
---

function getSlang()
    SLANG_VER = "2025.9.2"
    SLANG_DIR = BASE_DIR .. "external/downloaded/slang"..SLANG_VER
    SLANG_ZIPFILE = BASE_DIR .. "temp/slang.zip"
    SLANG_URL = "https://github.com/shader-slang/slang/releases/download/v"..SLANG_VER.."/slang-"..SLANG_VER.."-windows-x86_64.zip"
    downloadAndExtract("slang", SLANG_URL, SLANG_DIR)
end
