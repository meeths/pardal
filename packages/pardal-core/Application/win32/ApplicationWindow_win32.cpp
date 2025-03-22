#ifdef PDL_PLATFORM_WINDOWS
#include <Application/ApplicationWindow.h>
#include <Base/BaseTypes.h>
#include <Base/DebugHelpers.h>
#include <Containers/UnorderedMap.h>
#include <String/StringUtils.h>

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

// Created on 2025-03-22 by sisco
namespace pdl
{
    class Impl
    {
    public:
        static UnorderedMap<HWND, ApplicationWindow*> m_createdWin32Windows;

        void InitializeWindow(ApplicationWindow* applicationWindow)
        {
            m_applicationWindow = applicationWindow;
            IApplicationWindow::InitInfoBase& initInfo = m_applicationWindow->m_initInfo;

            WNDCLASSEX oWinClass;
            m_applicationHandle = static_cast<HINSTANCE>(GetModuleHandle(nullptr));

            oWinClass.cbSize = sizeof(WNDCLASSEX);
            oWinClass.lpfnWndProc = static_cast<WNDPROC>(Win32MessageLoop);
            oWinClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
            oWinClass.cbClsExtra = 0;
            oWinClass.cbWndExtra = 0;
            oWinClass.hInstance = m_applicationHandle;
            oWinClass.hIcon = LoadIcon(m_applicationHandle, IDI_APPLICATION);
            oWinClass.hCursor = initInfo.m_showCursor ? LoadCursor(nullptr, IDC_ARROW) : NULL;
            oWinClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
            oWinClass.lpszMenuName = nullptr;
            oWinClass.hIconSm = LoadIcon(m_applicationHandle, IDI_APPLICATION);
            oWinClass.lpszClassName = L"pdlApplicationWindow";


            int registerResults = RegisterClassEx(&oWinClass);
            pdlAssert(registerResults);

            if (initInfo.m_fullScreen) // Attempt Fullscreen Mode?
            {
                DEVMODE dmScreenSettings; // Device Mode
                memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); // Makes Sure Memory's Cleared
                dmScreenSettings.dmSize = sizeof(dmScreenSettings); // Size Of The Devmode Structure
                dmScreenSettings.dmPelsWidth = initInfo.m_windowSize.x; // Selected Screen Width
                dmScreenSettings.dmPelsHeight = initInfo.m_windowSize.y; // Selected Screen Height
                dmScreenSettings.dmBitsPerPel = 32; // Selected Bits Per Pixel
                dmScreenSettings.dmDisplayFrequency = 60;
                dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

                // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
                if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
                {
                    // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
                    if (MessageBox(
                        NULL,
                        L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",
                        L"pdlEngine",MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
                    {
                        initInfo.m_fullScreen = FALSE; // Windowed Mode Selected.  Fullscreen = FALSE
                    }
                    else
                    {
                        // Pop Up A Message Box Letting User Know The Program Is Closing.
                        pdlAssert(0 && "Could not start in full screen mode.");
                    }
                }
            }


            DWORD dwExStyle; // Window Extended Style
            DWORD dwStyle; // Window Style

            if (initInfo.m_fullScreen)
            {
                dwExStyle = WS_EX_APPWINDOW; // Window Extended Style
                dwStyle = WS_POPUP; // Windows Style
            }
            else
            {
                dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Window Extended Style
                dwStyle = WS_OVERLAPPEDWINDOW; // Windows Style
            }

            RECT windowRect = {0, 0, initInfo.m_windowSize.x, initInfo.m_windowSize.y};
            AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle); // Adjust Window To True Requested Size

            windowRect.bottom -= windowRect.top;
            windowRect.top = 0;

            m_hWnd = CreateWindow( // Extended style 
                oWinClass.lpszClassName,
                StringUtils::ToWstring(initInfo.m_windowTitle).c_str(), // Title
                dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // Window Style
                windowRect.left, // Initial X
                windowRect.top, // Initial Y
                windowRect.right - windowRect.left, // Width
                windowRect.bottom - windowRect.top, // Height
                NULL, // Handle to parent
                NULL, // Handle to menu
                m_applicationHandle, // Instance of app
                this);

            pdlAssert(m_hWnd);
            m_createdWin32Windows[m_hWnd] = applicationWindow;

            ::ShowWindow(m_hWnd, SW_SHOW);
            ::SetForegroundWindow(m_hWnd);
            ::SetFocus(m_hWnd);
        }

        static LRESULT Win32MessageLoop(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            ApplicationWindow* applicationWindow = NULL;
            if (m_createdWin32Windows.find(hWnd) != m_createdWin32Windows.end())
            {
                applicationWindow = m_createdWin32Windows[hWnd];
            }

            switch (msg)
            {
            case WM_DESTROY:
                {
                    applicationWindow->m_closeCallbacks();
                    PostQuitMessage(0);
                    return 0;
                }
            case WM_ACTIVATE:
            case WM_ACTIVATEAPP:
                {
                    if (wParam)
                    {
                        applicationWindow->m_focusCallbacks();
                    }
                    else
                    {
                        applicationWindow->m_lostFocusCallbacks();
                    }
                    break;
                }
            case WM_SIZE:
                {
                    if (wParam == SIZE_MINIMIZED)
                    {
                        applicationWindow->m_lostFocusCallbacks();
                    }
                    else if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
                    {
                        applicationWindow->m_resizeCallbacks(Math::Vector2{LOWORD(lParam), HIWORD(lParam)});
                    }
                    break;
                }
            case WM_MOUSEWHEEL:
                applicationWindow->m_mouseWheelVCallbacks(
                    static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA));
                return 0;
            case WM_MOUSEHWHEEL:
                applicationWindow->m_mouseWheelHCallbacks(
                    static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA));
                return 0;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                applicationWindow->m_keyDownCallbacks(static_cast<int16>(wParam));
                return 0;
            case WM_KEYUP:
            case WM_SYSKEYUP:
                applicationWindow->m_keyUpCallbacks(static_cast<int16>(wParam));
                return 0;
            case WM_CHAR:
                if (wParam > 0 && wParam < 0x10000)
                {
                    applicationWindow->m_keyInputCallbacks(static_cast<int16>(wParam));
                }
                return 0;
            case WM_SETCURSOR:
                {
                    WORD ht = LOWORD(lParam);

                    if (HTCLIENT == ht && applicationWindow->m_cursorVisible && !applicationWindow->m_initInfo.m_showCursor)
                    {
                        applicationWindow->m_cursorVisible = false;
                        ShowCursor(false);
                    }
                    else if (HTCLIENT != ht && !applicationWindow->m_cursorVisible)
                    {
                        applicationWindow->m_cursorVisible = true;
                        ShowCursor(true);
                    }
                }

            default: break;
            }

            return DefWindowProc(hWnd, msg, wParam, lParam);
        }

        void Update()
        {
            MSG msg;
            ZeroMemory(&msg, sizeof(msg));
            if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    m_applicationWindow->RequestClose();
                }

                if (msg.message == WM_MOUSEMOVE ||
                    msg.message == WM_LBUTTONDOWN ||
                    msg.message == WM_LBUTTONUP ||
                    msg.message == WM_RBUTTONDOWN ||
                    msg.message == WM_RBUTTONUP ||
                    msg.message == WM_MBUTTONDOWN ||
                    msg.message == WM_MBUTTONUP)
                {
                    const bool lButton = msg.wParam & MK_LBUTTON;
                    const bool rButton = msg.wParam & MK_RBUTTON;
                    const bool mButton = msg.wParam & MK_MBUTTON;
                    const glm::vec2 mousePos(static_cast<float>(LOWORD(msg.lParam)), static_cast<float>(HIWORD(msg.lParam)));

                    const bool lAlt = GetAsyncKeyState(VK_LMENU) & 0x8000;
                    const bool lCtrl = GetAsyncKeyState(VK_LCONTROL) & 0x8000;
                    const bool lShift = GetAsyncKeyState(VK_LSHIFT) & 0x8000;

                    const bool rAlt = GetAsyncKeyState(VK_RMENU) & 0x8000;
                    const bool rCtrl = GetAsyncKeyState(VK_RCONTROL) & 0x8000;
                    const bool rShift = GetAsyncKeyState(VK_RSHIFT) & 0x8000;

                    const unsigned int modifiers =
                        (lAlt ? 1 << 0 : 0) |
                        (lCtrl ? 1 << 1 : 0) |
                        (lShift ? 1 << 2 : 0) |
                        (rAlt ? 1 << 3 : 0) |
                        (rCtrl ? 1 << 4 : 0) |
                        (rShift ? 1 << 5 : 0);


                    m_applicationWindow->m_mouseMoveCallbacks(mousePos, lButton, rButton, mButton, modifiers);
                }


                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        ApplicationWindow* m_applicationWindow;
        HWND m_hWnd;
        HINSTANCE m_applicationHandle;
    };

    UnorderedMap<HWND, ApplicationWindow*> Impl::m_createdWin32Windows;
    
    ApplicationWindow::ApplicationWindow(InitInfoBase initInfo) : m_initInfo(std::move(initInfo))
    {
        m_windowImpl = MakeUniquePointer<Impl>();
        m_windowImpl->InitializeWindow(this);
    }

    ApplicationWindow::~ApplicationWindow()
    {
        pdlAssert(m_windowImpl);
        DestroyWindow(m_windowImpl->m_hWnd);
    }

    void ApplicationWindow::Update()
    {
        pdlAssert(m_windowImpl);
        m_windowImpl->Update();
    }

    void ApplicationWindow::SetWindowSize(const Math::Vector2i& size)
    {
        pdlAssert(m_windowImpl);
        SetWindowPos(m_windowImpl->m_hWnd, HWND_TOP, 0, 0, size.x, size.y, SWP_SHOWWINDOW);
    }

    void ApplicationWindow::SetWindowTitle(StringView title)
    {
        pdlAssert(m_windowImpl);
        ::SetWindowTextA(m_windowImpl->m_hWnd, title.data());
    }

    void ApplicationWindow::SetFullScreen(bool fullScreen)
    {
        pdlAssert(m_windowImpl);
        pdlNotImplemented();
    }

    Math::Vector2i ApplicationWindow::GetWindowSize() const
    {
        pdlAssert(m_windowImpl);
        RECT windowRect;
        ::GetWindowRect(m_windowImpl->m_hWnd, &windowRect);
        return { windowRect.right - windowRect.left, windowRect.bottom - windowRect.top };
    }

    String ApplicationWindow::GetWindowTitle() const
    {
        pdlAssert(m_windowImpl);
        char title[256];
        ::GetWindowTextA(m_windowImpl->m_hWnd, title, 256);
        return title;
    }

    bool ApplicationWindow::IsFullScreen() const
    {
        pdlAssert(m_windowImpl);
        pdlNotImplemented();
        return false;
    }

    void* ApplicationWindow::GetNativeWindow()
    {
        pdlAssert(m_windowImpl);
        return m_windowImpl->m_hWnd;
    }
}

#endif
