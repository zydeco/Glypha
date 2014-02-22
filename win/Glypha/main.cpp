#include <windows.h>
#include <gl/gL.h>
#include "../../game/GLGame.h"
#include "../../game/GLUtils.h"
#include "resources.h"

class AppController {
public:
    AppController();
    ~AppController();
    bool init(HINSTANCE hInstance);
    void run();
    void handleGameEvent(GL::Game::Event event);
private:
    HINSTANCE hInstance;
    HWND win;
    HACCEL accelerators;
    GL::Game game;

    HGLRC hRC;
    HDC hDC;

    static LRESULT CALLBACK AppController::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void onRender();
    void onResize(UINT width, UINT height);
    void onMenu(WORD cmd);
    void onKey(WPARAM key, bool down);
    void onMouseDown(UINT x, UINT y);
};

namespace {
void gameCallback(GL::Game::Event event, void *context)
{
    static_cast<AppController*>(context)->handleGameEvent(event);
}
}

AppController::AppController()
    : game(gameCallback, this)
{
}

AppController::~AppController()
{
}

bool AppController::init(HINSTANCE hInstance)
{
    // Register the window class
    WNDCLASSEX winClass;
    ZeroMemory(&winClass, sizeof(WNDCLASSEX));
    winClass.cbSize = sizeof(winClass);
    winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    winClass.lpfnWndProc = AppController::WndProc;
    winClass.cbWndExtra = sizeof(LONG_PTR);
    winClass.hInstance = hInstance;
    winClass.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    winClass.lpszClassName = L"MainWindow";
    winClass.lpszMenuName = MAKEINTRESOURCEW(IDR_MAINMENU);
    if (RegisterClassExW(&winClass) == 0) {
        return false;
    }

    // Load the accelerators from the resource file so menu keyboard shortcuts work.
    accelerators = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(ID_MENU_ACCELERATORS));
    if (accelerators == NULL) {
        return false;
    }

    // Create the window centered
    int w = 640, h = 460;
    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
    win = CreateWindowW(winClass.lpszClassName, L"Glypha III", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, x, y, w, h, NULL, NULL, hInstance, this);
    if (win == NULL) {
        return false;
    }
    
    // Setup OpenGL
    hDC = GetDC(win);
    if (hDC == NULL) {
        return false;
    }
    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };
    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (pixelFormat == 0 || SetPixelFormat(hDC, pixelFormat, &pfd) == FALSE) {
        return false;
    }
    hRC = wglCreateContext(hDC);
    if (hRC == NULL || wglMakeCurrent(hDC, hRC) == FALSE) {
        return false;
    }

    // Readjust the window so the client size matches our desired size
    RECT rcClient, rcWindow;
    POINT ptDiff;
    (void)GetClientRect(win, &rcClient);
    (void)GetWindowRect(win, &rcWindow);
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    (void)MoveWindow(win, rcWindow.left, rcWindow.top, w + ptDiff.x, h + ptDiff.y, TRUE);

    // Show the window
    (void)ShowWindow(win, SW_SHOWNORMAL);
    (void)UpdateWindow(win);

    return true;
}

void AppController::run()
{
    MSG msg;
    for (;;) {
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            // Check for keystrokes for the menus
            if (!TranslateAcceleratorW(win, accelerators, &msg)) {
                (void)TranslateMessage(&msg);
                (void)DispatchMessageW(&msg);
            }
        }
        if (msg.message == WM_QUIT) {
            break;
        }
        (void)InvalidateRect(win, NULL, FALSE);
    }
}

void AppController::handleGameEvent(GL::Game::Event event)
{
    switch (event) {
    case GL::Game::EventStarted:
        (void)EnableMenuItem(GetSubMenu(GetMenu(win), 0), ID_MENU_NEW_GAME, MF_DISABLED | MF_GRAYED);
        (void)EnableMenuItem(GetSubMenu(GetMenu(win), 0), ID_MENU_END_GAME, MF_ENABLED);
        (void)EnableMenuItem(GetSubMenu(GetMenu(win), 1), ID_MENU_HELP, MF_DISABLED | MF_GRAYED);
        break;
    case GL::Game::EventEnded:
        (void)EnableMenuItem(GetSubMenu(GetMenu(win), 0), ID_MENU_NEW_GAME, MF_ENABLED);
        (void)EnableMenuItem(GetSubMenu(GetMenu(win), 0), ID_MENU_END_GAME, MF_DISABLED | MF_GRAYED);
        (void)EnableMenuItem(GetSubMenu(GetMenu(win), 1), ID_MENU_HELP, MF_ENABLED);
        break;
    }
}

void AppController::onKey(WPARAM key, bool down)
{
    GL::Game::Key gameKey;
    switch (key) {
    case VK_SPACE: gameKey = GL::Game::KeySpacebar; break;
    case VK_DOWN: gameKey = GL::Game::KeyDownArrow; break;
    case VK_LEFT: gameKey = GL::Game::KeyLeftArrow; break;
    case VK_RIGHT: gameKey = GL::Game::KeyRightArrow; break;
    case 'A': gameKey = GL::Game::KeyA; break;
    case 'S': gameKey = GL::Game::KeyS; break;
    case VK_OEM_1: gameKey = GL::Game::KeyColon; break;
    case VK_OEM_7: gameKey = GL::Game::KeyQuote; break;
    default:
	    return;
    }
    if (down) {
	    game.handleKeyDownEvent(gameKey);
    } else {
	    game.handleKeyUpEvent(gameKey);
    }
}

void AppController::onMouseDown(UINT x, UINT y)
{
    game.handleMouseDownEvent(GL::Point(x, y));
}

LRESULT CALLBACK AppController::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        AppController *appController = (AppController *)pcs->lpCreateParams;
        (void)SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(appController));
        result = 1;
    } else {
        AppController *appController = reinterpret_cast<AppController *>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));
        bool wasHandled = false;

        if (appController != NULL) {
            switch (message) {
            case WM_SIZE:
                appController->onResize(LOWORD(lParam), HIWORD(lParam));
                result = 0;
                wasHandled = true;
                break;

            case WM_DISPLAYCHANGE:
                (void)InvalidateRect(hwnd, NULL, FALSE);
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
                appController->onRender();
                (void)ValidateRect(hwnd, NULL);
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                PostQuitMessage(0);
                result = 1;
                wasHandled = true;
                break;

            case WM_COMMAND:
                appController->onMenu(LOWORD(wParam));
                result = 1;
                wasHandled = true;
                break;

            case WM_KEYDOWN:
            case WM_KEYUP:
	            appController->onKey(wParam, message == WM_KEYDOWN);
	            result = 0;
	            wasHandled = true;
	            break;

            case WM_LBUTTONDOWN:
                appController->onMouseDown(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF);
                break;
            }
        }

        if (wasHandled == false) {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

void AppController::onRender()
{
    game.run();
    (void)SwapBuffers(hDC);
}

void AppController::onResize(UINT width, UINT height)
{
    game.renderer()->resize(width, height);
}

void AppController::onMenu(WORD cmd)
{
    switch(cmd) {
    case ID_MENU_NEW_GAME:
        game.newGame();
        break;
    case ID_MENU_EXIT:
        PostMessage(win, WM_CLOSE, 0, 0);
        break;
    case ID_MENU_END_GAME:
        game.endGame();
        break;
    case ID_MENU_HELP:
        game.showHelp();
        break;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    AppController appController;
    if (appController.init(hInstance) == false) {
        (void)MessageBoxW(NULL, L"Failed to initialize.", NULL, MB_OK | MB_ICONERROR);
        return 0;
    }
    appController.run();
    return 0;
}
