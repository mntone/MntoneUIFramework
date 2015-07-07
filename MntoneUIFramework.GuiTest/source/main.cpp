#include "pch.hpp"
#include "window.hpp"
#include "layouts.hpp"
#include "button.hpp"
#include "text_box.hpp"
#include "label.hpp"

int APIENTRY wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE /*hprevious_instance*/, _In_ LPWSTR /*command_line*/, _In_ int /*command_show*/)
{
	using namespace std;
	using namespace mnfx;

#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// auto release (by parent elements)
	text_box* tb = new text_box(L"initial text.‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ‚ ");
	button* btn = new button(L"push button sample");
	size_t id = 0;
	btn->click().add([](button const& s, event_args /*e*/)
	{
		MessageBoxW(s.root().hwnd(), L"clicked.", L"sample dialog", MB_OK);
		return S_OK;
	}, id);

	label* firstname = new label();
	firstname->set_text(L"&to: ");
	text_box* firstvalue = new text_box();
	firstvalue->set_text(L"Taro");
	label* secondname = new label();
	secondname->set_text(L"&e-mail: ");
	text_box* secondvalue = new text_box();
	secondvalue->set_text(L"example@test.com");
	grid* table = new grid(
	{ grid_length(), grid_length() },
	{ 80, grid_length() },
	{
		make_tuple(0, 0, firstname),
		make_tuple(0, 1, firstvalue),
		make_tuple(1, 0, secondname),
		make_tuple(1, 1, secondvalue),
	});

	grid* root = new grid(
	{ 60, grid_length(), 28 },
	{ },
	{
		make_tuple(0, 0, table),
		make_tuple(1, 0, tb),
		make_tuple(2, 0, btn),
	});

	unique_ptr<window> wnd = make_unique<window>();
	wnd->set_child(root);
	wnd->set_margin(9);
	wnd->set_text(L"guitest window");
	wnd->initialize(hinstance);
	return wnd->run();
}