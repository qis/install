#pragma once
#include <windows.h>
#include <filesystem>
#include <stdexcept>
#include <system_error>
#include <vector>

class process {
public:
  explicit process(HANDLE handle = nullptr);

  process(process&& other);
  process(const process& other) = delete;

  process& operator=(process&& other);
  process& operator=(const process& other) = delete;

  ~process();

  operator bool() const;

  // Returns the process executable path.
  // Requires PROCESS_QUERY_INFORMATION and PROCESS_VM_READ access rights.
  std::filesystem::path path() const;

  // Returns the process executable path.
  // Requires PROCESS_QUERY_INFORMATION and PROCESS_VM_READ access rights.
  std::filesystem::path path(std::error_code& ec) const;

  // Terminates the process.
  // Requires PROCESS_TERMINATE access rights.
  void terminate(UINT exit_code);

  // Terminates the process.
  // Requires PROCESS_TERMINATE access rights.
  void terminate(UINT exit_code, std::error_code& ec);

  // Opens the process.
  static process open(DWORD access, BOOL inherit, DWORD id);

  // Opens the process.
  static process open(DWORD access, BOOL inherit, DWORD id, std::error_code& ec);

  // Returns a list of process IDs.
  static std::vector<DWORD> list();

  // Returns a list of process IDs.
  static std::vector<DWORD> list(std::error_code& ec);

private:
  HANDLE handle_ = nullptr;
};

class process_error : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

const std::error_category& process_error_category();
