#include "pch.hpp"
#include "module.hpp"

using namespace std;

module::module(wstring module_name, HRESULT& hresult) noexcept
	: handle_(nullptr)
{
	handle_ = LoadLibraryW(module_name.c_str());
	hresult = handle_ != nullptr ? S_OK : HRESULT_FROM_WIN32(GetLastError());
}

module::~module() noexcept
{
	if (handle_) FreeLibrary(handle_);
}