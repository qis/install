#pragma once
#include <windows.h>
#include <shellapi.h>

class window {
public:
  window(HINSTANCE instance);

  void on_create();
  void on_destroy();
  void on_timer(UINT_PTR id);
  void on_command(UINT id);

  void on_tray(UINT id);
  void on_tray(UINT id, int x, int y);

private:
  LRESULT handle(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  HINSTANCE instance_;
  HWND hwnd_ = nullptr;

  HICON main_ = nullptr;
  HICON grey_ = nullptr;

  NOTIFYICONDATA tray_ = {};
};
