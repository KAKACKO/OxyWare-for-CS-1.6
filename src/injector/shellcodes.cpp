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


#include <platform.h>

#include <interface/IBaseModule.h>

#include <injection/IBaseInjector.h>
#include <injection/BaseInjector.h>

#include <tier/MessageBox.h>

#include <BytePattern.h>

#include "RtlIIFT_bytepattern_search.h"


DISABLE_SAFEBUFFERS HINSTANCE __stdcall CLoadLibrareredDll::shellcode_routine(loadlibrary_shellcode_execution_context_t* context)
{
	context->pfnOutputDebugStringA(context->debug_messages[0]);


	context->pfnOutputDebugStringA(context->debug_messages[1]);

	HMODULE module = NULL;


	context->pfnOutputDebugStringA(context->m_injected_dll_name);
	module = context->pfnLoadLibraryExA(context->m_injected_dll_name, NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!module)
	{

		module = context->pfnLoadLibraryExA(context->m_injected_dll_name, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (!module)
		{

			module = context->pfnLoadLibraryExA(context->m_injected_dll_name, NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);


		}
	}

	if (!module)
	{
		return NULL;
	}

	context->m_loaded_module = module;


	uint8_t* base = (uint8_t*)module;
	auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
	auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(base + dos->e_lfanew);
	auto op = &nt->OptionalHeader;

	auto teb = GET_CURRENT_TEB();
	auto peb = teb->ProcessEnvironmentBlock;

	pfnCommunicativeDllEntryPoint_t pfnCommunicativeDllEntryPoint = nullptr;
	pfnPreDllLoad_t pfnPreDllLoad = nullptr;


	if (op->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
	{
		auto exports_directory = (PIMAGE_EXPORT_DIRECTORY)((uint8_t*)base + op->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);


		auto function_table_base = reinterpret_cast<uint32_t*>((uint8_t*)base + exports_directory->AddressOfFunctions);
		auto name_table_base = reinterpret_cast<uint32_t*>((uint8_t*)base + exports_directory->AddressOfNames);
		auto ordinal_table_base = reinterpret_cast<uint16_t*>((uint8_t*)base + exports_directory->AddressOfNameOrdinals);


		for (size_t i = 0; i < exports_directory->NumberOfNames; i++)
		{
			auto export_procname = reinterpret_cast<const char*>((uint8_t*)base + name_table_base[i]);

			if (!context->pfn_stricmp(export_procname, context->export_names[0]))
			{
				context->pfnExposeModule = reinterpret_cast<ExposeModuleFn>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
			else if (!context->pfn_stricmp(export_procname, context->export_names[1]))
			{
				context->pfnGetInterfaceInstance = reinterpret_cast<GetInterfaceInstanceFn>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
			else if (!context->pfn_stricmp(export_procname, context->export_names[2]))
			{
				pfnCommunicativeDllEntryPoint = reinterpret_cast<pfnCommunicativeDllEntryPoint_t>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
			else if (!context->pfn_stricmp(export_procname, context->export_names[3]))
			{
				pfnPreDllLoad = reinterpret_cast<pfnPreDllLoad_t>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
		}
	}

	context->pfnOutputDebugStringA(context->debug_messages[2]);


	if (pfnPreDllLoad != nullptr)
	{
		if (!pfnPreDllLoad())
		{
			return NULL;
		}
	}

	context->pfnOutputDebugStringA(context->debug_messages[3]);


	if (pfnCommunicativeDllEntryPoint != nullptr)
	{
		context->m_information_package.m_ipc_block_ptr->m_technique = INJECT_LOADLIBRARY;

		if (!pfnCommunicativeDllEntryPoint(&context->m_information_package))
		{
			return NULL;
		}
	}

	context->pfnOutputDebugStringA(context->debug_messages[4]);

	return (HINSTANCE)module;
}


DISABLE_SAFEBUFFERS HINSTANCE CManualMappedDllCurrentProcess::shellcode_routine()
{
	auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(m_allocation_block_ptr);
	auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(m_allocation_block_ptr + dos->e_lfanew);
	auto op = &nt->OptionalHeader;

	auto teb = GET_CURRENT_TEB();
	auto peb = teb->ProcessEnvironmentBlock;

	auto pfnDllMain = reinterpret_cast<pfnDllMain_t>(m_allocation_block_ptr + op->AddressOfEntryPoint);

	pfnPreDllLoad_t pfnPreDllLoad = nullptr;


	if (op->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		auto import_desc = (IMAGE_IMPORT_DESCRIPTOR*)(m_allocation_block_ptr + op->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

		while (import_desc->Name)
		{
			char* module_name = (char*)(m_allocation_block_ptr + import_desc->Name);


			HINSTANCE module_handle = (HINSTANCE)LoadLibraryA(module_name);

			auto thunk_ref = (ULONG_PTR*)(m_allocation_block_ptr + import_desc->OriginalFirstThunk);
			auto func_ref = (ULONG_PTR*)(m_allocation_block_ptr + import_desc->FirstThunk);

			if (!import_desc->OriginalFirstThunk)
			{
				thunk_ref = func_ref;
			}

			for (; *thunk_ref; ++thunk_ref, ++func_ref)
			{
				if (IMAGE_SNAP_BY_ORDINAL(*thunk_ref))
				{
					*func_ref = (ULONG_PTR)GetProcAddress(module_handle, (char*)IMAGE_ORDINAL32(*thunk_ref));
				}
				else
				{
					auto import = (IMAGE_IMPORT_BY_NAME*)(m_allocation_block_ptr + (*thunk_ref));
					*func_ref = (ULONG_PTR)GetProcAddress(module_handle, import->Name);
				}
			}

			import_desc++;
		}
	}


	if (op->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
	{
		auto exports_directory = (PIMAGE_EXPORT_DIRECTORY)((uint8_t*)m_allocation_block_ptr + op->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);


		auto function_table_base = reinterpret_cast<uint32_t*>((uint8_t*)m_allocation_block_ptr + exports_directory->AddressOfFunctions);
		auto name_table_base = reinterpret_cast<uint32_t*>((uint8_t*)m_allocation_block_ptr + exports_directory->AddressOfNames);
		auto ordinal_table_base = reinterpret_cast<uint16_t*>((uint8_t*)m_allocation_block_ptr + exports_directory->AddressOfNameOrdinals);


		for (size_t i = 0; i < exports_directory->NumberOfNames; i++)
		{
			auto export_procname = reinterpret_cast<const char*>((uint8_t*)m_allocation_block_ptr + name_table_base[i]);
			if (!_stricmp(export_procname, EXPOSEMODULE_PROCNAME))
			{
				m_pfnExposeModuleFn = reinterpret_cast<ExposeModuleFn>((uint8_t*)m_allocation_block_ptr + function_table_base[ordinal_table_base[i]]);

			}
			else if (!_stricmp(export_procname, INTERFACEINSTANCEGETTER_PROCNAME))
			{
				m_pfnGetInterfaceInstanceFn = reinterpret_cast<GetInterfaceInstanceFn>((uint8_t*)m_allocation_block_ptr + function_table_base[ordinal_table_base[i]]);

			}
			else if (!_stricmp(export_procname, PRE_DLL_LOAD_PROCNAME))
			{
				pfnPreDllLoad = reinterpret_cast<pfnPreDllLoad_t>((uint8_t*)m_allocation_block_ptr + function_table_base[ordinal_table_base[i]]);
			}
		}
	}


	if (!RtlIIFT_BytePattern_Search::the().resolve_bytepatterns())
	{
		return NULL;
	}


	CBytePatternRuntime RtlIIFT_pattern({ RtlIIFT_BytePattern_Search::the().m_RtlIIFT_bytepattern.c_str(), RtlIIFT_BytePattern_Search::the().m_RtlIIFT_bytepattern.length() });

	DWORD ntdll_base = (DWORD)GetModuleHandleA("ntdll.dll");
	DWORD size_of_ntdll_image = (DWORD)((PIMAGE_NT_HEADERS)((uint8_t*)ntdll_base + ((PIMAGE_DOS_HEADER)ntdll_base)->e_lfanew))->OptionalHeader.SizeOfImage;


	void(__fastcall * RtlInsertInvertedFunctionTable)(DWORD ImageBase, DWORD SizeOfImage);
	RtlInsertInvertedFunctionTable = (decltype(RtlInsertInvertedFunctionTable))RtlIIFT_pattern.search_in_loaded_address_space(ntdll_base, ntdll_base + size_of_ntdll_image);

	if (RtlInsertInvertedFunctionTable)
	{
		DWORD size_of_image_current = (DWORD)((PIMAGE_NT_HEADERS)((uint8_t*)m_allocation_block_ptr + ((PIMAGE_DOS_HEADER)m_allocation_block_ptr)->e_lfanew))->OptionalHeader.SizeOfImage;
		RtlInsertInvertedFunctionTable((DWORD)m_allocation_block_ptr, size_of_image_current);
	}
	else
	{
		CMessageBox::display_error("Couldn't find RtlInsertInvertedFunctionTable function. "
								   "This function is mandatory. Aborting injection...");
		return NULL;
	}


	if (pfnPreDllLoad != nullptr)
	{
		if (!pfnPreDllLoad())
		{
			return NULL;
		}
	}


	if (!pfnDllMain((HINSTANCE)m_allocation_block_ptr, DLL_PROCESS_ATTACH, NULL))
	{
		return NULL;
	}

	return (HINSTANCE)m_allocation_block_ptr;
}


DISABLE_SAFEBUFFERS HINSTANCE __stdcall CManualMappedDll::shellcode_routine(manualmap_shellcode_execution_context_t* context)
{
	uint8_t* base = context->module_base;
	auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
	auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(base + dos->e_lfanew);
	auto op = &nt->OptionalHeader;

	context->pfnOutputDebugStringA(context->debug_messages[0]);

	auto teb = GET_CURRENT_TEB();
	auto peb = teb->ProcessEnvironmentBlock;

	auto pfnDllMain = reinterpret_cast<pfnDllMain_t>(base + op->AddressOfEntryPoint);

	pfnCommunicativeDllEntryPoint_t pfnCommunicativeDllEntryPoint = nullptr;
	pfnPreDllLoad_t pfnPreDllLoad = nullptr;

	context->pfnOutputDebugStringA(context->debug_messages[1]);


	if (op->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		auto import_desc = (IMAGE_IMPORT_DESCRIPTOR*)(base + op->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

		while (import_desc->Name)
		{
			char* module_name = (char*)(base + import_desc->Name);


			HINSTANCE module_handle = (HINSTANCE)context->pfnLoadLibraryA(module_name);

			auto thunk_ref = (ULONG_PTR*)(base + import_desc->OriginalFirstThunk);
			auto func_ref = (ULONG_PTR*)(base + import_desc->FirstThunk);

			if (!import_desc->OriginalFirstThunk)
			{
				thunk_ref = func_ref;
			}

			for (; *thunk_ref; ++thunk_ref, ++func_ref)
			{
				if (IMAGE_SNAP_BY_ORDINAL(*thunk_ref))
				{
					*func_ref = (ULONG_PTR)context->pfnGetProcAddress(module_handle, (char*)IMAGE_ORDINAL32(*thunk_ref));
				}
				else
				{
					auto import = (IMAGE_IMPORT_BY_NAME*)(base + (*thunk_ref));
					*func_ref = (ULONG_PTR)context->pfnGetProcAddress(module_handle, import->Name);

				}
			}

			import_desc++;
		}
	}

	context->pfnOutputDebugStringA(context->debug_messages[2]);


	if (op->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
	{
		auto exports_directory = (PIMAGE_EXPORT_DIRECTORY)((uint8_t*)base + op->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);


		auto function_table_base = reinterpret_cast<uint32_t*> ((uint8_t*)base + exports_directory->AddressOfFunctions);
		auto name_table_base = reinterpret_cast<uint32_t*>((uint8_t*)base + exports_directory->AddressOfNames);
		auto ordinal_table_base = reinterpret_cast<uint16_t*>((uint8_t*)base + exports_directory->AddressOfNameOrdinals);


		for (size_t i = 0; i < exports_directory->NumberOfNames; i++)
		{
			auto export_procname = reinterpret_cast<const char*>((uint8_t*)base + name_table_base[i]);

			if (!context->pfn_stricmp(export_procname, context->export_names[0]))
			{
				context->pfnExposeModule = reinterpret_cast<ExposeModuleFn>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
			else if (!context->pfn_stricmp(export_procname, context->export_names[1]))
			{
				context->pfnGetInterfaceInstance = reinterpret_cast<GetInterfaceInstanceFn>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
			else if (!context->pfn_stricmp(export_procname, context->export_names[2]))
			{
				pfnCommunicativeDllEntryPoint = reinterpret_cast<pfnCommunicativeDllEntryPoint_t>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
			else if (!context->pfn_stricmp(export_procname, context->export_names[3]))
			{
				pfnPreDllLoad = reinterpret_cast<pfnPreDllLoad_t>((uint8_t*)base + function_table_base[ordinal_table_base[i]]);
			}
		}
	}


	context->pfnOutputDebugStringA(context->debug_messages[3]);

	uintptr_t ntdll_base = NULL;
	auto topmost_entry = &peb->Ldr->InMemoryOrderModuleList;
	auto front_link = topmost_entry->Flink;

	do
	{
		auto module_entry = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>((uint8_t*)front_link - (sizeof(LIST_ENTRY)));

		if (!context->pfn_wcsicmp(module_entry->BaseDllName.Buffer, context->dll_names[0]))
		{
			ntdll_base = (uintptr_t)module_entry->DllBase;
			break;
		}

		front_link = front_link->Flink;
	} while (front_link != topmost_entry);


	context->pfnOutputDebugStringA(context->debug_messages[4]);


	auto BPattern_RtlIIFT = &context->byte_patterns[BPattern_RtlIIFT_Idx];

	DWORD size_of_ntdll_image = (DWORD)((PIMAGE_NT_HEADERS)((uint8_t*)ntdll_base + ((PIMAGE_DOS_HEADER)ntdll_base)->e_lfanew))->OptionalHeader.SizeOfImage;


	void(__fastcall * RtlInsertInvertedFunctionTable)(DWORD ImageBase, DWORD SizeOfImage);

	context->pfnOutputDebugStringA(context->debug_messages[5]);


	uintptr_t* address = nullptr;
	bool found = false;
	for (uintptr_t i = ntdll_base; i < (ntdll_base + size_of_ntdll_image) - BPattern_RtlIIFT->length; i++)
	{
		found = true;
		for (size_t k = 0; k < BPattern_RtlIIFT->length; k++)
		{
			if (BPattern_RtlIIFT->mask[k] == 'x' && BPattern_RtlIIFT->bytepattern[k] != *(char*)(i + k))
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			address = (uintptr_t*)i;
			break;
		}
	}

	context->pfnOutputDebugStringA(context->debug_messages[6]);

	RtlInsertInvertedFunctionTable = (decltype(RtlInsertInvertedFunctionTable))address;

	if (RtlInsertInvertedFunctionTable)
	{
		DWORD size_of_image_current = (DWORD)((PIMAGE_NT_HEADERS)((uint8_t*)base + ((PIMAGE_DOS_HEADER)base)->e_lfanew))->OptionalHeader.SizeOfImage;
		RtlInsertInvertedFunctionTable((DWORD)base, size_of_image_current);
	}
	else
	{

		return NULL;
	}

	context->pfnOutputDebugStringA(context->debug_messages[7]);


	if (pfnPreDllLoad != nullptr)
	{
		if (!pfnPreDllLoad())
		{
			return NULL;
		}
	}

	context->pfnOutputDebugStringA(context->debug_messages[8]);


	if (!pfnDllMain((HINSTANCE)base, DLL_PROCESS_ATTACH, NULL))
	{
		return NULL;
	}

	context->pfnOutputDebugStringA(context->debug_messages[9]);


	if (pfnCommunicativeDllEntryPoint != nullptr)
	{
		context->m_information_package.m_ipc_block_ptr->m_technique = INJECT_MANUALMAP;

		if (!pfnCommunicativeDllEntryPoint(&context->m_information_package))
		{
			return NULL;
		}
	}

	context->pfnOutputDebugStringA(context->debug_messages[10]);


	return (HINSTANCE)base;
}
