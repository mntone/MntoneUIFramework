#include "pch.hpp"
#include "window.hpp"
#include "layouts.hpp"
#include "button.hpp"
#include "text_box.hpp"

int APIENTRY wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hprevious_instance, _In_ LPWSTR command_line, _In_ int command_show)
{
	using namespace std;
	using namespace mnfx;

	// auto release (by parent elements)
	text_box* tb = new text_box(L"initial text.");
	button* btn = new button(L"push button sample");
	grid* root = new grid(
	{ grid_length(), 28 },
	{ },
	{
		make_tuple(0, 0, tb),
		make_tuple(1, 0, btn),
	});

	unique_ptr<window> wnd = make_unique<window>();
	wnd->set_child(root);
	wnd->set_margin(9);
	wnd->set_text(L"guitest window");

	wnd->initialize(hinstance);
	return wnd->run();
}