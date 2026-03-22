
#include <iostream>
#include "Base/ServiceLocator.h"
#include "Engine/CoreSystems.h"
#include "Engine/Engine.h"
#include "Engine/EngineOptions.h"

int main(int argc, char** argv)
{
    auto coreSystemsInitializedResults = pdl::CoreSystems::Initialize();
    
    if (!coreSystemsInitializedResults)
    {
        std::cout << coreSystemsInitializedResults.error().data();
        return -1;
    }
    
    auto& engineOptions = pdl::ServiceLocator<pdl::EngineOptions>::Ref();
    
    engineOptions.SetOption("window_title", "Pardal Engine");
    engineOptions.SetOption("render_backend", "Vulkan");
    engineOptions.SetOption("enable_validation", "true");
    engineOptions.SetOption("vsync", "true");
    
    pdl::Engine engine(engineOptions);
    engine.Run();
    
    return 0;
    
}
