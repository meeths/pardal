---
--- Created by sisco.
--- DateTime: 25/05/2025 20:21
---
ASSIMP_VER = "6.0.2"
ASSIMP_DIR = BASE_DIR .. "external/Assimp/" .. ASSIMP_VER

function includeAssimp()
    includedirs { ASSIMP_DIR .. "/include" }
    filter {}
end

function linkAssimp()
    libdirs { ASSIMP_DIR .. "/lib/x64" }
    links "assimp-vc143-mt.lib"
    postbuildcommands { "{copy} " .. ASSIMP_DIR .. "/bin/x64/assimp-vc143-mt.dll %{cfg.buildtarget.directory}" }
    filter {}
end