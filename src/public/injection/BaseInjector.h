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

#ifndef BASEINJECTOR_H
#define BASEINJECTOR_H
#pragma once

#include <unordered_map>

#include "mmap/ManualMappedDll.h"
#include "mmap/ManualMappedDllCurrentProcess.h"
#include "loadlibrary/LoadLibrareredDll.h"

enum EModuleUnloadWaitResult
{
	MODWAIT_Error,
	MODWAIT_CanUnload,
	MODWAIT_Hang,
};

class CBaseInjector : public IBaseInjector
{
public:
	CBaseInjector(EInjectionTechnique t) : IBaseInjector(t)
	{
	}

	void destroy();

	static constexpr size_t k_update_threshold_ms = 250;
	void update();
	bool is_reinjection_pending() const override { return m_should_reinject; }

	bool inject_to_target_process(const char* execuatable_name, const std::filesystem::path& dll_path);

	IInjectableModuleObject* get_injected_dll(const std::filesystem::path& dll_path)
	{
		for (auto& [key, dll] : m_injected_dlls)
		{
			if (key == dll_path)
				return dll;
		}

		return nullptr;
	}

private:
	void prepare_for_reinjection(const char* execuatable_name, const std::filesystem::path& dll_path);
	bool m_should_reinject = false;

	struct reinjection_data_t
	{
		std::string exe_name;
		std::filesystem::path dll_path;
		uint32_t requested_at = 0;
		uint32_t ready_since = 0;
		uint32_t ready_process_id = 0;
	};
	reinjection_data_t m_reinjection_data;

private:


#ifdef _DEBUG
	inline static constexpr size_t k_max_module_unload_wait_time_sec = 9999;
#else
	inline static constexpr size_t k_max_module_unload_wait_time_sec = 10;
#endif
	EModuleUnloadWaitResult hang_till_module_unloads(const std::filesystem::path& module_path, IInjectableModuleObject* module,
													 std::chrono::high_resolution_clock::time_point wait_start);

	inline IInjectableModuleObject* create_injected_dll(const char* execuatable_name, const std::filesystem::path& dll_path)
	{
		switch (m_technique)
		{
			case INJECT_LOADLIBRARY:
			{
				return new CLoadLibrareredDll(execuatable_name, dll_path);
			}
			case INJECT_MANUALMAP:
			{
				return new CManualMappedDll(execuatable_name, dll_path);
			}
		}

		assert(0);
		return nullptr;
	}

private:
	std::unordered_map<std::filesystem::path, IInjectableModuleObject*> m_injected_dlls;
};

#endif
