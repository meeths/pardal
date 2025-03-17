group "99 scripts"

project "lyra.scripts"
    kind "Utility"
    files { 
        "%{BASE_DIR}*.yml",
        "%{BASE_DIR}*.lua", 
        "%{BASE_DIR}*.bat", 
        "%{BASE_DIR}scripts/**.*" 
    }
    filter {}