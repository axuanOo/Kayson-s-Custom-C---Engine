#include "Engine/Core/Window.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "ThirdParty/imgui/imgui_impl_win32.h"
#define  WIN32_LEAN_AND_MEAN		// Always define this before #including <windows.h>
#include <windows.h>
#include <iostream>

Window* Window:: s_mainWindow = nullptr;


Window::Window()
{

}

Window::Window(WindowConfig const& config)
{
	m_config = config;
	s_mainWindow = this;
}

Window::~Window()
{

}

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called back by Windows whenever we tell it to (by calling DispatchMessage).
//
// #SD1ToDo: We will move this function to a more appropriate place (Engine/Renderer/Window.cpp) later on...
//
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(windowHandle, wmMessageCode, wParam, lParam))
		return true;
	// Get the current window instance
	Window* window = Window::GetMainWindowInstance();
	GUARANTEE_OR_DIE ( window != nullptr, "Window was null!");

	// Ask the Window for a pointer to the InputSystem it was created with (in its InputSystemConfig )
	InputSystem* input = window->GetConfig().m_inputSystem;
	GUARANTEE_OR_DIE (  input != nullptr, "Window's inputsystem pointer was null!");


	// Respond accordingly based on whatever message Windows has left for us (key pressed/released, close/X clicked, last foucus, etc.)
	switch (wmMessageCode)
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:
	{
		g_theEventSystem->FireEvent("Close");
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	case WM_CHAR:
	{
		EventArgs args;
		args.SetValue(std::string("CharName"), Stringf("%d", (unsigned char)wParam));
		FireEvent("CharInput", args);
		return 0;
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		EventArgs args;
		args.SetValue(std::string("KeyCode"), Stringf("%d", (unsigned char)wParam));
		FireEvent("KeyPressed",args);
		return 0;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		EventArgs args;
		args.SetValue(std::string("KeyCode"), Stringf("%d", (unsigned char)wParam));
		FireEvent("KeyReleased", args);
		return 0;
	}
	
	case WM_LBUTTONDOWN:
	{
		unsigned char keycode = KEYCODE_LEFT_MOUSE;
		if (input) 
		{
			input->HandleKeyPressed(keycode);
		}
		break;
	}
	case WM_LBUTTONUP: 
	{
		unsigned char keycode = KEYCODE_LEFT_MOUSE;
		if (input)
		{
			input->HandleKeyReleased(keycode);
		}
		break;
	}

	case WM_RBUTTONDOWN:
	{
		unsigned char keycode = KEYCODE_RIGHT_MOUSE;
		if (input)
		{
			input->HandleKeyPressed(keycode);
		}
		break;
	}

	case WM_RBUTTONUP:
	{
		unsigned char keycode = KEYCODE_RIGHT_MOUSE;
		if (input)
		{
			input->HandleKeyReleased(keycode);
		}
		break;
	}

	case WM_MOUSEWHEEL: 
	{
		short delta = GET_WHEEL_DELTA_WPARAM(wParam);
		EventArgs args;
		args.SetValue(std::string("ScrollValue"), Stringf("%i", (signed short)delta));
		FireEvent("MouseScroll", args);
		return 0;
	}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}





void Window::Startup()
{
	CreateOSWindow();
}

void Window::BeginFrame()
{
	Clock::TickSystemClock();
	RunMessagePump();
}

void Window::EndFrame()
{

}

void Window::Shutdown()
{

}

WindowConfig const& Window::GetConfig() const
{
	return m_config;
}


float Window::GetAspect() const
{
	return m_config.m_clientAspect;
}

void* Window::GetHwnd() const
{
	return m_hwnd;
}

IntVec2 Window::GetClientDimensions() const
{
	return m_clientDimensions;
}

IntVec2 Window::GetClientPosition() const
{
	return m_clientPosition;
}

void* Window::GetWindowDC() const
{
	return m_dc;
}

Window* Window::GetMainWindowInstance()
{
	return s_mainWindow;
}


Vec2 Window::GetNormalizedCursorPos() const
{
	HWND windowHandle = HWND(m_hwnd);
	POINT cursorCoords;
	RECT clientRect;
	::GetCursorPos(&cursorCoords); // in screen coordinates, (0,0) top-left
	::ScreenToClient(windowHandle, &cursorCoords); // relative to the window interior
	::GetClientRect(windowHandle,&clientRect); // size of window interior (0,0 to width,height)
	float cursorX = float(cursorCoords.x)/float(clientRect.right); // normalized x position
	float cursorY = float(cursorCoords.y)/float(clientRect.bottom); // normalized y position
	return Vec2(cursorX, 1.0f - cursorY); // we want (0,0) in the bottom-left
}

bool Window::GetWindowFocus() const
{
	return ::GetActiveWindow() == (HWND)m_hwnd;
}

void Window::CreateOSWindow()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	HMODULE applicationInstanceHandle = GetModuleHandle(NULL);
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = applicationInstanceHandle;
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	float clientAspect = GetConfig().m_clientAspect;
	float clientMarginX = 0.f;
	float clientMarginY = 0.f;

	if (m_config.m_isFullscreen)
	{
		clientWidth = desktopWidth;
		clientHeight = desktopHeight;
		clientAspect = desktopAspect;
		// Update window style for full screen
		windowStyleFlags = WS_POPUP;
		windowStyleExFlags = 0;
	}
	else 
	{
		if (GetConfig().m_clientSize != IntVec2::ZERO)
		{
			clientWidth = (float)GetConfig().m_clientSize.x;
			clientHeight = (float)GetConfig().m_clientSize.y;
			clientAspect = clientWidth / clientHeight;
		}

		if (GetConfig().m_clientPosition != IntVec2::ZERO)
		{
			clientMarginX = (float)GetConfig().m_clientPosition.x;
			clientMarginY = (float)GetConfig().m_clientPosition.y;
		}
	}

	if (clientAspect > desktopAspect)
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	if (clientMarginX == 0.0f && clientMarginY == 0.0f)
	{
		// Calculate client rect bounds by centering the client area
		clientMarginX = 0.5f * (desktopWidth - clientWidth);
		clientMarginY = 0.5f * (desktopHeight - clientHeight);
	}

	m_clientDimensions.x = (int)clientWidth;
	m_clientDimensions.y = (int)clientHeight;
	m_clientPosition.x = (int)clientMarginX;
	m_clientPosition.y = (int)clientMarginY;
	m_config.m_clientAspect = clientAspect;

	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, GetConfig().m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));

	//HWND g_hWnd = static_cast<HWND>(m_hwnd);
	m_hwnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		(HINSTANCE)applicationInstanceHandle,
		NULL);

	ShowWindow((HWND)m_hwnd, SW_SHOW);
	SetForegroundWindow((HWND)m_hwnd);
	SetFocus((HWND)m_hwnd);

	m_dc = ::GetDC((HWND)m_hwnd);

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);
}

void Window::RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}
		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}
