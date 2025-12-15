group "99 Pardal Scripts"

project "pardal-scripts"
    kind "None"
    files { 
        "%{BASE_DIR}*.yml",
        "%{BASE_DIR}*.lua", 
        "%{BASE_DIR}*.bat", 
        "%{BASE_DIR}scripts/**.*" 
    }
    filter {}