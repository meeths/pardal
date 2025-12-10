#include "Log/Log.h"
#include "Log/LoggerStdout.h"
#include "Memory/Memory.h"
#include "generated/EditorMainWindow.h"

int main(int argc, char** argv)
{
    pdl::Memory::Initialize();
#ifndef PDL_RELEASE
    pdl::Log::Instance().RegisterLogger(pdl::MakeSharedPointer<pdl::LoggerStdout>());
#endif

    auto editorWindow = EditorMainWindow::create();
    editorWindow->run();
    return 0;
    
}
