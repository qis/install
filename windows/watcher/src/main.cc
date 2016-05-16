#include "window.h"
#include <windows.h>
#include <resource.h>
#include <clocale>

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR cmd, int show)
{
  // Initialize the locale.
  std::setlocale(LC_ALL, "");

  // Check if another instance of this application is already running.
  auto event = CreateEvent(NULL, TRUE, FALSE, PRODUCT);
  if (event && GetLastError() == ERROR_ALREADY_EXISTS) {
    MessageBox(nullptr, L"This application is already running.", PROJECT, MB_OK | MB_ICONASTERISK | MB_SETFOREGROUND);
    return 0;
  }

  // Create the main application window.
  window window(instance);

  // Run the main message loop.
  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return static_cast<int>(msg.wParam);
}
