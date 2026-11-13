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

bool CImGUILayerWin32::initialize()
{
	assert(m_window_handle);


	if (!ImGui::CreateContext())
	{
		CConsole::the().error("Coudln't create ImGui context.");
		return false;
	}

	auto& io = ImGui::GetIO();


	io.IniFilename = io.LogFilename = nullptr;
	io.ConfigFlags |= m_flags;

	g_gui_fontmgr_i->initialize();

	ImGui::StyleColorsDark();
	g_gui_thememgr_i->initialize_imgui();


	if (!ImGui_ImplWin32_Init(as_hwnd()))
	{
		return false;
	}

	if (!ImGui_ImplOpenGL2_Init())
	{
		return false;
	}

	return true; 
}

void CImGUILayerWin32::destroy()
{
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplOpenGL2_Shutdown();
	ImGui::DestroyContext();

	m_window_handle = nullptr;
}

void CImGUILayerWin32::on_render(const std::function<void()>& post_render_callback)
{
	if (g_gui_fontmgr_i->pre_newframe())
	{

		ImGui_ImplOpenGL2_DestroyDeviceObjects();
		ImGui_ImplOpenGL2_CreateDeviceObjects();
	}


	ImGui_ImplWin32_NewFrame();
	ImGui_ImplOpenGL2_NewFrame();
	ImGui::NewFrame();

	g_gui_fontmgr_i->push_default_font();

	if (m_render_callback)
	{
		m_render_callback();
	}

	if (post_render_callback)
	{
		post_render_callback();
	}

	g_gui_fontmgr_i->pop_default_font();

	ImGui::Render();

	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}
