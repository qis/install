#include "window.h"
#include "process.h"
#include <windowsx.h>
#include <resource.h>
#include <stdexcept>
#include <string>
#include <cwchar>

#define WM_APP_TRAY (WM_APP + 1)
#define WATCHER_TIMER 1

window::window(HINSTANCE instance) : instance_(instance)
{
  // Load the window icons.
  main_ = LoadIcon(instance, MAKEINTRESOURCE(IDI_MAIN));
  grey_ = LoadIcon(instance, MAKEINTRESOURCE(IDI_GREY));

  // Initialize the systray icon.
  tray_.cbSize = sizeof(tray_);
  tray_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
  tray_.uCallbackMessage = WM_APP_TRAY;
  tray_.hIcon = main_;
  std::wcsncpy(tray_.szTip, PROJECT, ARRAYSIZE(tray_.szTip));

  // Register the main application window class.
  WNDCLASSEX wc = {};
  wc.cbSize = sizeof(wc);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
    auto self = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (msg == WM_CREATE) {
      self = reinterpret_cast<window*>(reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else if (msg == WM_DESTROY) {
      SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
    }
    return self ? self->handle(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
  };
  wc.hInstance = instance;
  wc.hIcon = main_;
  wc.hIconSm = main_;
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
  wc.lpszMenuName = MAKEINTRESOURCE(IDM_MAIN);
  wc.lpszClassName = PRODUCT;

  if (!RegisterClassEx(&wc)) {
    MessageBox(nullptr, L"Could not register the main application window class.", PROJECT, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
    PostQuitMessage(1);
    return;
  }

  // Create the main application window.
  auto es = 0x0L;
  auto ws = WS_OVERLAPPEDWINDOW;

  if (!CreateWindowEx(es, PRODUCT, PROJECT, ws, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, instance, this)) {
    MessageBox(nullptr, L"Could not create the main application window.", PROJECT, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
    PostQuitMessage(1);
    return;
  }
}

void window::on_create()
{
  // Create the systray icon.
  tray_.hWnd = hwnd_;
  Shell_NotifyIcon(NIM_ADD, &tray_);

  // Set the systray icon version.
  tray_.uVersion = NOTIFYICON_VERSION_4;
  Shell_NotifyIcon(NIM_SETVERSION, &tray_);

  // Update the systray icon.
  on_timer(WATCHER_TIMER);

  // Start the timer.
  SetTimer(hwnd_, WATCHER_TIMER, 1000, nullptr);
}

void window::on_destroy()
{
  // Destroy the systray icon.
  Shell_NotifyIcon(NIM_DELETE, &tray_);

  // Stop the main message loop.
  PostQuitMessage(0);
}

void window::on_timer(UINT_PTR id)
{
  if (id != WATCHER_TIMER) {
    return;
  }

  // Check if a process is running.
  bool match = false;
  for (auto id : process::list()) {
    // Open the process.
    std::error_code ec;
    auto process = process::open(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, id, ec);
    if (ec) {
      continue;
    }

    // Get the process path.
    auto path = process.path(ec);
    if (ec) {
      continue;
    }

    // Check if the path matches.
    auto name = path.filename();
    if (name == "VsHub.exe" || name == "vctip.exe") {
      match = true;
      break;
    }
  }

  // Update the systray icon.
  if (match) {
    if (tray_.hIcon != main_) {
      tray_.hIcon = main_;
      Shell_NotifyIcon(NIM_MODIFY, &tray_);
    }
  } else {
    if (tray_.hIcon != grey_) {
      tray_.hIcon = grey_;
      Shell_NotifyIcon(NIM_MODIFY, &tray_);
    }
  }
}

void window::on_command(UINT id)
{
  // Handle windows commands.
  switch (id) {
  case IDM_EXIT:
    PostMessage(hwnd_, WM_CLOSE, 0, 0);
    break;
  }
}

void window::on_tray(UINT id)
{
  // Handle the systray icon select message.
  for (auto id : process::list()) {
    // Open the process.
    std::error_code ec;
    auto process = process::open(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, id, ec);
    if (ec) {
      continue;
    }

    // Get the process path.
    auto path = process.path(ec);
    if (ec) {
      continue;
    }

    // Terminate the process if necessary.
    auto name = path.filename();
    if (name == "VsHub.exe" || name == "vctip.exe" || name == "PerfWatson2.exe") {
      process.terminate(0, ec);
    }
  }

  // Update the systray icon.
  if (tray_.hIcon != grey_) {
    tray_.hIcon = grey_;
    Shell_NotifyIcon(NIM_MODIFY, &tray_);
  }
}

void window::on_tray(UINT id, int x, int y)
{
  // Handle the systray icon context menu message.
  if (auto menu = LoadMenu(instance_, MAKEINTRESOURCE(IDM_MAIN))) {
    if (auto submenu = GetSubMenu(menu, 0)) {
      SetForegroundWindow(hwnd_);
      auto align = (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0) ? TPM_RIGHTALIGN : TPM_LEFTALIGN;
      TrackPopupMenuEx(submenu, TPM_RIGHTBUTTON | align, x, y, hwnd_, nullptr);
    }
    DestroyMenu(menu);
  }
}

LRESULT window::handle(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  // Handle windows messages.
  try {
    switch (msg) {
    case WM_CREATE:
      hwnd_ = hwnd;
      on_create();
      return 0;
    case WM_DESTROY:
      on_destroy();
      hwnd_ = nullptr;
      return 0;
    case WM_TIMER:
      on_timer(wparam);
      return 0;
    case WM_COMMAND:
      on_command(LOWORD(wparam));
      return 0;
    case WM_APP_TRAY:
      switch (LOWORD(lparam)) {
      case NIN_SELECT:
        on_tray(HIWORD(lparam));
        break;
      case WM_CONTEXTMENU:
        on_tray(HIWORD(lparam), GET_X_LPARAM(wparam), GET_Y_LPARAM(wparam));
        break;
      }
      return 0;
    }
  }
  catch (const std::exception& e) {
    std::wstring msg;
    msg.resize(MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, nullptr, 0) + 1);
    msg.resize(MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, &msg[0], static_cast<int>(msg.size())));
    MessageBox(hwnd, msg.c_str(), PROJECT, MB_OK | MB_ICONERROR);
    DestroyWindow(hwnd);
  }
  return DefWindowProc(hwnd, msg, wparam, lparam);
}
