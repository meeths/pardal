
#include "Engine/Engine.h"
#include "Engine/EngineOptions.h"

int main(int argc, char** argv)
{
    pdl::EngineOptions engineOptions(pdl::EngineOptions::EInitFrom::All);
    
    engineOptions.SetOption("window_title", "Pardal Engine Test App");

    engineOptions.SetOption("render_backend", "Vulkan");
    engineOptions.SetOption("enable_validation", "true");
    engineOptions.SetOption("vsync", "true");
    
    pdl::Engine engine(engineOptions);
    engine.Run();
    
    return 0;
    
}
