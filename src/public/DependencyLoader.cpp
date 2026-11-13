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


#include "DependencyLoader.h"

#include "Console.h"

std::string LoadableModuleObject::get_name_utf8() const
{
	return CStringTools::the().utf16_to_utf8(m_name);
}

bool LoadableModuleObject::load_and_initialize(ELoadType type, const std::filesystem::path& module_path, const std::wstring& module_name)
{
	m_name = module_name;
	m_filepath = module_path;
	m_load_type = type;

	CConsole::the().info("Loading and initializing module '{}'...", get_name_utf8());

	if (!load_by_type())
	{
		return false;
	}

	CConsole::the().info("Module '{}' loaded; resolving initialization export.", get_name_utf8());
	ExposeModuleFn emfn = find_exposemodule_func();
	if (!emfn)
	{
		CConsole::the().error("Failed to find " EXPOSEMODULE_PROCNAME " routine");
		return false;
	}


	if (type == LOADTYPE_MANUALMAP)
	{
		g_libloader_i->register_manualmapped_module(module_name.c_str(), m_injected_module->get_base_address());
	}


	ModuleInitializationContext ctxt
	{
		g_libloader_i, 
		g_devconsole_i
	};


	CConsole::the().info("Calling initialization export for '{}'.", get_name_utf8());
	bool success = emfn(&m_basemodule_ptr, &ctxt);
	if (!success || !m_basemodule_ptr)
	{
		CConsole::the().error("Module initialization failed.");
		return false;
	}


	CConsole::the().info("Module '{}' initialized; resolving interfaces.", get_name_utf8());
	m_getinterfaceinstance_fn = find_getinterfaceinstance_func();

	return true;
}

void LoadableModuleObject::unload()
{
	CConsole::the().info("Unloading module {}...", get_name_utf8());


	m_basemodule_ptr->destroy();

	unload_by_type();
}

bool LoadableModuleObject::is_dll_loaded() const
{
	return g_libloader_i->is_dll_loaded(m_name.c_str());
}

bool LoadableModuleObject::update()
{
	auto update_status = m_basemodule_ptr->update();

	switch (update_status)
	{

		case UPDATE_APP_EXIT:
		{
			return false;
		}
		case UPDATE_CONTINUE:
		default:
		{
			return true;
		}
	}
}

bool LoadableModuleObject::load_by_type()
{
	switch (m_load_type)
	{
		case LOADTYPE_NATIVE:
		{
			auto path = m_filepath.wstring();
			m_handle = g_libloader_i->load_library(path.empty() ? NULL : path.c_str(), m_name.c_str());
			return m_handle != NULL;
		}
		case LOADTYPE_MANUALMAP:
		{
			auto full_path = m_filepath / m_name;
			m_injected_module = new CManualMappedDllCurrentProcess(full_path);
			if (!m_injected_module->start_injection())
			{
				m_injected_module = nullptr;
				return false;
			}

			return true;
		}
	}

	assert(0 && "bad load type specified in " __FUNCTION__ ".");
	return false;
}

void LoadableModuleObject::unload_by_type()
{
	switch (m_load_type)
	{
		case LOADTYPE_NATIVE:
		{
			g_libloader_i->unload_library(m_handle);
			break;
		}
		case LOADTYPE_MANUALMAP:
		{
			assert(m_injected_module);
			m_injected_module->unload(false);
			delete m_injected_module;
			m_injected_module = nullptr;
			break;
		}
		default:
		{
			assert(0 && "bad load type specified in " __FUNCTION__ ".");
			break;
		}
	}
}

ExposeModuleFn LoadableModuleObject::find_exposemodule_func()
{
	ExposeModuleFn func = NULL;
	
	switch (m_load_type)
	{
		case LOADTYPE_NATIVE:
		{
			func = (ExposeModuleFn)g_libloader_i->find_proc_in_target_library((uintptr_t)m_handle, EXPOSEMODULE_PROCNAME);
			break;
		}
		case LOADTYPE_MANUALMAP:
		{
			assert(m_injected_module && m_injected_module->is_successfully_initialized());
			func = m_injected_module->get_ExposeModule_fn();
			break;
		}
		default:
		{
			assert(0 && "bad load type specified in " __FUNCTION__ ".");
			break;
		}
	}

	return func;
}

GetInterfaceInstanceFn LoadableModuleObject::find_getinterfaceinstance_func()
{
	GetInterfaceInstanceFn func = NULL;

	switch (m_load_type)
	{
		case LOADTYPE_NATIVE:
		{
			func = LocateExportedInterfaceGetterFn(m_name.c_str());
			break;
		}
		case LOADTYPE_MANUALMAP:
		{
			assert(m_injected_module && m_injected_module->is_successfully_initialized());
			func = m_injected_module->get_GetInterfaceInstance_fn();
			break;
		}
		default:
		{
			assert(0 && "bad load type specified in " __FUNCTION__ ".");
			break;
		}
	}

	return func;
}


bool CDependencyLoader::load_and_initialize_module(ELoadType type, const std::filesystem::path& module_path, const std::wstring& module_name,
												   std::function<bool(const LoadableModuleObject& mod)> extended_init_callback)
{
	LoadableModuleObject mod;

	if (!mod.load_and_initialize(type, module_path, module_name))
	{
		return false;
	}


	if (extended_init_callback)
	{
		if (!extended_init_callback(mod))
			return false;
	}

	m_loaded_modules.emplace_back(mod);

	return true;
}

void CDependencyLoader::unload_all()
{
	for (auto& module : m_loaded_modules)
	{
		module.unload();
	}

	m_loaded_modules.clear();
}

void CDependencyLoader::unload_module(const std::wstring& module_name)
{
	for (auto& module : m_loaded_modules)
	{
		if (module.get_name() == module_name)
		{
			module.unload();
			return;
		}
	}

	CConsole::the().error("Failed to unload module '{}'. Already unloaded?", CStringTools::the().utf16_to_utf8(module_name));
}

bool CDependencyLoader::update()
{	

	if (!update_modules())
	{
		return false;
	}

	return true;
}

bool CDependencyLoader::update_modules()
{
	for (auto& module : m_loaded_modules)
	{
		if (!module.update())
		{
			CConsole::the().info("Module '{}' requested application shutdown.", module.get_name_utf8());
			return false;
		}
	}

	return true;
}
