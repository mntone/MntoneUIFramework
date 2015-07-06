#pragma once

class module final
{
public:
	module(::std::wstring module_name, HRESULT& hresult) noexcept;
	~module() noexcept;

private:
	module() = delete;
	module(module const&) = delete;
	
	module& operator=(module const&) = delete;

public:
	HMODULE handle() const noexcept { return handle_; }

private:
	HMODULE handle_;
};