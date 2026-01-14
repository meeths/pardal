#ifdef PDL_PLATFORM_WINDOWS
#include <Application/ProgramArguments.h>
#include <windows.h>
#include <string>

// Avoid macro collision with Windows API GetCommandLine macro
#ifdef GetCommandLine
#undef GetCommandLine
#endif

namespace pdl
{
    String ProgramArguments::GetCommandLine()
    {
        LPCWSTR wcmd = ::GetCommandLineW();
        if (!wcmd)
            return {};

        int needed = ::WideCharToMultiByte(CP_UTF8, 0, wcmd, -1, nullptr, 0, nullptr, nullptr);
        if (needed <= 1)
            return {};

        String utf8;
        utf8.resize(static_cast<size_t>(needed - 1));
        ::WideCharToMultiByte(CP_UTF8, 0, wcmd, -1, utf8.data(), needed, nullptr, nullptr);
        return utf8;
    }
}

#endif
