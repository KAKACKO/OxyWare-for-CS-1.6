/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
* 
*	This program is licensed under the MIT license. By downloading, copying, 
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a 
*	copy of this software and associated documentation files (the "Software"), 
*	to deal in the Software without restriction, including without limitation 
*	the rights to use, copy, modify, merge, publish, distribute, sublicense, 
*	and/or sell copies of the Software, and to permit persons to whom the 
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included 
*	in all copies or substantial portions of the Software. 
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
*	IN THE SOFTWARE.
*/

#include "precompiled.h"

VarBoolean ui_background_rain("ui_background_rain", "Toggles on/off the background rain when UI is up", true);
VarBoolean ui_background_fade("ui_background_fade", "Toggles on/off the background fade animation when UI is up", true);
VarBoolean ui_render_feature_list("ui_render_feature_list", "Toggles on/off rendering of the feature list", true);

BaseCommand ui_toggle_menu(
	"ui_toggle_menu", "Toggles menu on/off",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		COxWareUI::the().find_context("menu")->toggle_rendering();
	}
);

BaseCommand ui_toggle_console(
	"ui_toggle_console", "Toggles console on/off",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		COxWareUI::the().find_context("console")->toggle_rendering();
	}
);

#ifdef OX_ENABLE_CODE_PROFILE
BaseCommand ui_toggle_perf_profiler_visualization(
	"ui_toggle_perf_profiler_visualization", "Toggles performance profiler visualization on/off",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		COxWareUI::the().find_context("performace_profiler_visualization")->toggle_rendering();
	}
);
#endif

void COxWareUI::swapbuffers_detour(HDC hdc)
{
	if (COxWare::the().is_cheat_exiting())
	{
		return;
	}

	m_hdc = hdc;

	if (!m_initialized)
	{
		initialize(WindowFromDC(hdc));

		m_initialized = true;
	}

	run_ui();
}

void COxWareUI::destroy()
{
	if (!m_cheat_context || !m_original_context)
	{
		return;
	}

	CConsole::the().info("Destroying cheat UI...");


	if (!COxWare::the().is_game_exiting_or_restarting())
	{
		CEngineInput::the().toggle_ingame_clientdll_mouse(true);
		CEngineInput::the().toggle_ingame_input(true);


		auto ibaseui = CHLInterfaceHook::the().IBaseUI();
		if (ibaseui)
		{
			if (!CEngineInput::the().is_gameui_rendering())
			{
				ibaseui->HideGameUI();
				ibaseui->ActivateGameUI();
			}
		}
	}

	g_texture_manager_i->shutdown();

	destroy_rendering_contexts();

	g_window_msg_handler_i->destroy();
	g_user_input_i->destroy();

	g_imgui_platform_layer_i->destroy_layer();

	m_initialized = false;
}

IRenderingContext* COxWareUI::find_context(const std::string& id) const
{
	for (auto ctx : m_rendering_contexts)
	{
		if (id == ctx->get_id())
			return ctx;
	}
	
	assert(0);
	return nullptr;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void COxWareUI::initialize(HWND wnd)
{
	m_hwnd = wnd;
	char class_name[64]{};
	char window_title[128]{};
	GetClassNameA(wnd, class_name, sizeof(class_name));
	GetWindowTextA(wnd, window_title, sizeof(window_title));
	DWORD window_process = 0;
	auto window_thread = GetWindowThreadProcessId(wnd, &window_process);
	CConsole::the().info("Cheat UI target window: {}, class '{}', title '{}', thread {}, process {}.",
		(uintptr_t)wnd, class_name, window_title, window_thread, window_process);


	g_window_msg_handler_i->initialize(wnd);
	g_window_msg_handler_i->add_msg_callback(
		[this](UINT message, WPARAM key, LPARAM)
		{
			if (key == VK_INSERT && (message == WM_KEYDOWN || message == WM_SYSKEYDOWN ||
				message == WM_KEYUP || message == WM_SYSKEYUP))
				CConsole::the().info("Insert key message {} received by game window {} (foreground {}).",
					message, (uintptr_t)m_hwnd, (uintptr_t)GetForegroundWindow());
		});

	if (!g_imgui_platform_layer_i->create_new_layer(IMPLATFORM_win32, wnd, [this]() { render_imgui(); }, ImGuiConfigFlags_NoMouseCursorChange))
	{
		COxWare::the().end_cheat_execution();
		return;
	}


	g_window_msg_handler_i->add_msg_return_callback(
		[this](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* lpReturnVal)
		{
			if (m_is_any_interactible_rendering_context_active)
			{
				if (msg == WM_SETCURSOR)
				{


					if (LOWORD(lParam) == HTCLIENT)
					{

						::SetCursor(nullptr);
						*lpReturnVal = TRUE;
					}
					else
					{

						*lpReturnVal = FALSE;
					}

					return WNDPROCRET_Custom;
				}

				if (g_imgui_platform_layer_i->imgui_wndproc_handler(hWnd, msg, wParam, lParam))
				{
					*lpReturnVal = TRUE;
					return WNDPROCRET_Custom;
				}
			}

			return WNDPROCRET_Generic;
		});

	instantiate_rendering_contexts();


	g_user_input_i->add_key_press_callback(
		"toggle_menu", VK_INSERT, 
		[](const std::string& id, UserKey_t* key)
		{
			auto menu = COxWareUI::the().find_context("menu");
			auto before = menu->should_render();
			auto blocked = menu->is_interaction_blocked();
			menu->toggle_rendering();
			CConsole::the().info("Insert menu state: blocked {}, visible {} -> {}.", blocked, before, menu->should_render());
		});
	g_user_input_i->add_key_press_callback(
		"toggle_console", VK_OEM_3, 
		[](const std::string& id, UserKey_t* key)
		{
			COxWareUI::the().find_context("console")->toggle_rendering();
		});
#ifdef OX_ENABLE_CODE_PROFILE
	g_user_input_i->add_key_press_callback(
		"toggle_perf_profiler_visualization", VK_F11, 
		[](const std::string& id, UserKey_t* key)
		{
			COxWareUI::the().find_context("performace_profiler_visualization")->toggle_rendering();
		});
#endif

	m_original_context = wglGetCurrentContext();
	m_cheat_context = wglCreateContext(m_hdc);
	CConsole::the().info("Cheat GL contexts: original {}, overlay {}, create error {}.",
		(uintptr_t)m_original_context, (uintptr_t)m_cheat_context, m_cheat_context ? 0 : GetLastError());

	CEngineInput::the().initialize();

	CESP::the().initialize_gui();
	CFlashbangFadeModifier::the().initialize_gui();
	CBulletTrace::the().initialize_gui();
	CClientMovementPacketPlot::the().initialize_gui();

	CConsole::the().info("Cheat UI fully initialized.");
}

void COxWareUI::run_ui()
{

	if (CAntiScreen::the().hide_visuals())
	{
		return;
	}

	m_is_any_interactible_rendering_context_active = false;
	m_contexts_to_be_rendered.clear();

	handle_popups_inputs();

	for (IRenderingContext* ctx : m_rendering_contexts)
	{
		if (ctx->should_render())
		{
			m_contexts_to_be_rendered.push_back(ctx);


			if (ctx->is_interactible() && !m_is_any_interactible_rendering_context_active)
			{
				m_is_any_interactible_rendering_context_active = true;
			}
		}
	}
	static bool previous_menu_rendering = false;
	bool menu_rendering = find_context("menu")->should_render();
	if (menu_rendering != previous_menu_rendering)
	{
		CConsole::the().info("Menu render state {}, active contexts {}, HDC {}.", menu_rendering,
			m_contexts_to_be_rendered.size(), (uintptr_t)m_hdc);
		previous_menu_rendering = menu_rendering;
	}

	CUIWindowPopups::the().pre_render();

	if (!m_contexts_to_be_rendered.empty() || CUIWindowPopups::the().is_in_popup_dialog())
	{

		if (!wglMakeCurrent(m_hdc, m_cheat_context))
		{
			static bool reported_context_error = false;
			if (!reported_context_error)
			{
				CConsole::the().error("Cannot activate cheat GL context ({}).", GetLastError());
				reported_context_error = true;
			}
		}
		
		if (g_imgui_platform_layer_i)
		{
			g_imgui_platform_layer_i->render();
		}
		

		wglMakeCurrent(m_hdc, m_original_context);
	}

	post_render();
}

void COxWareUI::render_imgui()
{
	g_gui_widgets_i->block_input_on_all_except_popup(CUIWindowPopups::the().is_in_popup_dialog() || g_user_input_i->is_in_key_binding_mode());


	initialize_textures();

	for (IRenderingContext* ctx : m_contexts_to_be_rendered)
	{
		ctx->on_render();
	}

	CUIWindowPopups::the().render();

	if (g_user_input_i->is_in_key_binding_mode())
	{

		g_imgui_platform_layer_i->override_cursor(GUICURSOR_None);
	}

	g_imgui_platform_layer_i->should_render_imgui_cursor(m_is_any_interactible_rendering_context_active);
}

void COxWareUI::post_render()
{
	handle_ingame_mouseevents();
}

void COxWareUI::initialize_textures()
{
	static bool f = false;
	if (!f)
	{
		g_texture_manager_i->initialize();
		f = true;
	}
}

void COxWareUI::instantiate_rendering_contexts()
{

	m_rendering_contexts.reserve(64);

	instantiate_single_rendering_context(&ctx_Menu, new CUIMenu("menu"));
	instantiate_single_rendering_context(&ctx_FeatureList, new CUIFeatureList("feature_list", ctx_Menu));
	instantiate_single_rendering_context(&ctx_Console, new CUIConsole("console", ctx_Menu));
#ifdef OX_ENABLE_CODE_PROFILE
	instantiate_single_rendering_context(&ctx_PerfProfilerVisualization, new CUIPerfProfilerVisualization("performace_profiler_visualization", ctx_Menu));
#endif
	instantiate_single_rendering_context(&ctx_MenuBackground, new CUIMenuBackground("menu_background", ctx_Menu, 250ms));

	instantiate_single_rendering_context(&ctx_BackgroundRendering, new CUIBackgroundRendering("background_rendering"));


	ctx_FeatureList->set_not_interactible();
	ctx_MenuBackground->set_not_interactible();
	ctx_BackgroundRendering->set_not_interactible();
}

void COxWareUI::destroy_rendering_contexts()
{
	for (IRenderingContext* ctx : m_rendering_contexts)
	{
		CConsole::the().info("Destroying {} rendering context...", ctx->get_id());
		ctx->on_destroy();
		delete ctx;
	}

	m_rendering_contexts.clear();
}

void COxWareUI::handle_ingame_mouseevents()
{


	static bool last = false;
	if (last != m_is_any_interactible_rendering_context_active)
	{

		if (CGameUtil::the().is_fully_connected())
		{
			CGameUtil::the().reset_all_in_states();
			CMemoryFnHookMgr::the().ClearIOStates().call();
		}


		CEngineInput::the().surface_centerize_cursor_position();

		if (m_is_any_interactible_rendering_context_active)
		{
			CEngineInput::the().toggle_ingame_input(false);
		}
		else
		{
			CEngineInput::the().toggle_ingame_input(true);
		}

		last = m_is_any_interactible_rendering_context_active;
	}


	if (CGameUtil::the().is_fully_connected() && GetFocus() == m_hwnd)
	{
		CEngineInput::the().toggle_ingame_clientdll_mouse(!m_is_any_interactible_rendering_context_active);
	}

#if 0


	int deltaX, deltaY;
	CEngineInput::the().SDL_GetRelativeMouseState(&deltaX, &deltaY);

	CConsole::the().info("delta: relative: {} | [{}, {}]", (bool)CEngineInput::the().SDL_GetRelativeMouseMode(), deltaX, deltaY);
#endif
}

void COxWareUI::lock_interaction_on_all_contexts()
{
	for (auto context : m_rendering_contexts)
	{
		context->lock_interaction();
	}
}

void COxWareUI::unlock_interaction_on_all_contexts()
{
	for (auto context : m_rendering_contexts)
	{
		context->unlock_interaction();
	}
}

void COxWareUI::handle_popups_inputs()
{
	static bool was_in_popup = false;
	if (CUIWindowPopups::the().is_in_popup_dialog() && !was_in_popup)
	{
		lock_interaction_on_all_contexts();
		was_in_popup = true;
	}
	else if (!CUIWindowPopups::the().is_in_popup_dialog() && was_in_popup)
	{
		unlock_interaction_on_all_contexts();
		was_in_popup = false;
	}
}
