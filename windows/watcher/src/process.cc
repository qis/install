#include "process.h"
#include <psapi.h>
#include <sstream>
#include <string>

namespace {

class process_error_category_impl : public std::error_category {
public:
  virtual const char* name() const noexcept override
  {
    return "process_error_category";
  }

  virtual std::string message(int e) const override
  {
    DWORD error = static_cast<DWORD>(e);
    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
    LPTSTR str = nullptr;
    auto size = FormatMessage(flags, nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&str), 0, nullptr);
    if (size > 0 && str) {
      std::string msg;
      msg.resize(WideCharToMultiByte(CP_UTF8, 0, str, static_cast<int>(size), nullptr, 0, nullptr, nullptr) + 1);
      msg.resize(WideCharToMultiByte(CP_UTF8, 0, str, static_cast<int>(size), &msg[0], static_cast<int>(msg.size()), nullptr, nullptr));
      LocalFree(str);
      return msg;
    }
    std::ostringstream oss;
    oss << "Error: 0x" << std::hex << static_cast<unsigned>(e);
    return oss.str();
  }
};

std::error_code make_error(DWORD error)
{
  return std::error_code(static_cast<int>(error), process_error_category());
}

}  // namespace


process::process(HANDLE handle) :
  handle_(handle)
{}

process::process(process&& other) :
  process()
{
  std::swap(handle_, other.handle_);
}

process& process::operator=(process&& other)
{
  std::swap(handle_, other.handle_);
  return *this;
}

process::~process()
{
  if (handle_) {
    CloseHandle(handle_);
  }
}

process::operator bool() const
{
  return handle_ ? true : false;
}

std::filesystem::path process::path() const
{
  std::error_code ec;
  auto ret = path(ec);
  if (ec) {
    throw process_error(ec.message());
  }
  return ret;
}

std::filesystem::path process::path(std::error_code& ec) const
{
  if (!handle_) {
    ec = make_error(ERROR_INVALID_HANDLE);
    return {};
  }
  DWORD size = 0;
  std::wstring path;
  path.resize(1024 * 8);
  path.resize(GetModuleFileNameEx(handle_, 0, &path[0], static_cast<DWORD>(path.size())));
  if (path.empty()) {
    ec = make_error(GetLastError());
    return {};
  }
  return std::filesystem::canonical(path);
}

void process::terminate(UINT exit_code)
{
  std::error_code ec;
  terminate(exit_code, ec);
  if (ec) {
    throw process_error(ec.message());
  }
}

void process::terminate(UINT exit_code, std::error_code& ec)
{
  if (!handle_) {
    ec = make_error(ERROR_INVALID_HANDLE);
    return;
  }
  if (!TerminateProcess(handle_, exit_code)) {
    ec = make_error(GetLastError());
    return;
  }
  CloseHandle(handle_);
  handle_ = nullptr;
}

process process::open(DWORD access, BOOL inherit, DWORD id)
{
  std::error_code ec;
  auto ret = open(access, inherit, id, ec);
  if (ec) {
    throw process_error(ec.message());
  }
  return ret;
}

process process::open(DWORD access, BOOL inherit, DWORD id, std::error_code& ec)
{
  auto handle = OpenProcess(access, inherit, id);
  if (!handle) {
    ec = make_error(GetLastError());
  }
  return process(handle);
}

std::vector<DWORD> process::list()
{
  std::error_code ec;
  auto ret = list(ec);
  if (ec) {
    throw process_error(ec.message());
  }
  return ret;
}

std::vector<DWORD> process::list(std::error_code& ec)
{
  DWORD bytes = 0;
  std::vector<DWORD> list;
  list.resize(4096);
  if (!EnumProcesses(list.data(), static_cast<DWORD>(list.size()), &bytes)) {
    ec = make_error(GetLastError());
    return{};
  }
  list.resize(std::min(list.size(), bytes / sizeof(DWORD)));
  return list;
}

const std::error_category& process_error_category()
{
  static process_error_category_impl instance;
  return instance;
}
