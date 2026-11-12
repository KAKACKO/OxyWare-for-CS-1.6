#include "precompiled.h"

namespace nitro_compat
{
namespace
{
class Api
{
public:
    virtual ~Api() = default;
    virtual void WriteLog(const char*, const char*) = 0;
    virtual void SetSEHCallback(void (*)(void*)) = 0;
    virtual void (*GetSEHCallback())(void*) = 0;
    virtual bool Initialize(void*, void*, void*) = 0;
    virtual bool UnInitialize() = 0;
    virtual bool IsInitialized() = 0;
    virtual void* GetClientData() = 0;
    virtual void* GetEngineData() = 0;
    virtual void* GetWindowsData() = 0;
    virtual void* GetSDL2Data() = 0;
    virtual int* GetBuildVersion() = 0;
    virtual void GetVersion(char*, int) = 0;
};

bool read_memory(const void* source, void* destination, size_t size)
{
    SIZE_T read = 0;
    return ReadProcessMemory(GetCurrentProcess(), source, destination, size, &read) && read == size;
}

bool valid_object(void* object, uintptr_t module, uintptr_t end, size_t slots)
{
    uintptr_t table = 0, entries[13]{};
    if (!read_memory(object, &table, sizeof(table)) || table < module || table >= end || slots > 13 ||
        !read_memory(reinterpret_cast<void*>(table), entries, slots * sizeof(uintptr_t))) return false;
    for (size_t i = 0; i < slots; ++i)
    {
        MEMORY_BASIC_INFORMATION mbi{};
        if (entries[i] < module || entries[i] >= end ||
            !VirtualQuery(reinterpret_cast<void*>(entries[i]), &mbi, sizeof(mbi)) ||
            !(mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))
            return false;
    }
    return true;
}

constexpr size_t function_offsets[] = {732, 2228, 1892, 1332};
constexpr uintptr_t original_rvas[] = {0x57610, 0x45390, 0x8c210, 0x11a50};

struct PendingOperation
{
    bool (*function)(void*) = nullptr;
    void* context = nullptr;
    std::atomic<int> state{0};
    bool result = false;
    HANDLE done = nullptr;
};
std::atomic<PendingOperation*> pending{nullptr};
std::atomic<unsigned> dispatchers{0};
std::atomic<unsigned> operation_readers{0};
UINT operation_message = 0;
std::atomic<DWORD> engine_thread{0};

struct WindowSearch
{
    DWORD process;
    HWND window = nullptr;
};

BOOL CALLBACK find_process_window(HWND candidate, LPARAM context)
{
    auto search = reinterpret_cast<WindowSearch*>(context);
    DWORD owner = 0;
    if (IsWindowVisible(candidate) && GetWindowThreadProcessId(candidate, &owner) && owner == search->process)
    {
		wchar_t class_name[64]{};
		GetClassNameW(candidate, class_name, _countof(class_name));
		if (!_wcsicmp(class_name, L"SDL_app"))
		{
			search->window = candidate;
			return FALSE;
		}
    }
    return TRUE;
}

LRESULT CALLBACK dispatch_hook(int code, WPARAM wp, LPARAM lp)
{
    dispatchers.fetch_add(1, std::memory_order_acq_rel);
    if (code >= 0 && reinterpret_cast<MSG*>(lp)->message == operation_message)
        pump_pending_operation();
    auto result = CallNextHookEx(nullptr, code, wp, lp);
    dispatchers.fetch_sub(1, std::memory_order_release);
    return result;
}
}

bool Compatibility::initialize(const wchar_t* engine, int build)
{
    m_enabled = false;
	m_crosshair = {};
    auto module = GetModuleHandleW(L"nitro_api2.dll");
    if (!module || build != 8684 || _wcsicmp(engine, L"hw.dll")) return false;
    auto reject = [](const char* reason) {
        CConsole::the().warning("Nitro compatibility check failed: {}", reason);
        return false;
    };
    auto factory = reinterpret_cast<void* (__cdecl*)(const char*, int*)>(GetProcAddress(module, "CreateInterface"));
    if (!factory) { CConsole::the().warning("Nitro has no CreateInterface export."); return false; }
    auto [nitro_base, nitro_end] = g_libloader_i->get_loaded_dll_address_space(L"nitro_api2.dll", SPACE_FULL);
    auto [engine_base, engine_end] = g_libloader_i->get_loaded_dll_address_space(engine, SPACE_FULL);
    auto api = static_cast<Api*>(factory("NITRO_API_001", nullptr));
    if (!valid_object(api, nitro_base, nitro_end, 13)) return reject("unexpected NITRO_API_001 interface layout");
    if (!api->IsInitialized()) return reject("Nitro is not initialized yet");
    int api_build = 0;
    if (!read_memory(api->GetBuildVersion(), &api_build, sizeof(api_build)) || api_build != build)
        return reject("Nitro and the engine report different builds");
    auto data = static_cast<unsigned char*>(api->GetEngineData());
    if (!read_memory(data + 1324, &m_host_initialized, sizeof(m_host_initialized)) ||
        reinterpret_cast<uintptr_t>(m_host_initialized) != engine_base + 0x643e44 ||
        engine_base + 0x643e48 > engine_end) return reject("host_initialized does not match this engine layout");
    int initialized = 0;
    if (!read_memory(m_host_initialized, &initialized, sizeof(initialized)) || (initialized != 0 && initialized != 1))
        return reject("host_initialized is not a readable boolean");

    for (size_t i = 0; i < 4; ++i)
    {
        Function fn;
        if (!read_memory(data + function_offsets[i], &fn, sizeof(fn)) ||
            !valid_object(fn.chain, nitro_base, nitro_end, 11) ||
            !valid_object(fn.hook, nitro_base, nitro_end, 5))
        {
            CConsole::the().warning("Nitro function {} has an unexpected interface layout.", i);
            return false;
        }
        auto chain = static_cast<Chain<>*>(fn.chain);
        if (!chain->IsHookAttached() || chain->GetHook() != fn.hook || !fn.hook->IsHookActive() ||
            fn.hook->GetOriginalFunctionAddress() != engine_base + original_rvas[i])
        {
            CConsole::the().warning("Nitro function {} is inactive or its original address differs from this engine layout.", i);
            return false;
        }
        m_functions[i] = fn;
    }
	auto [client_base, client_end] = g_libloader_i->get_loaded_dll_address_space(L"client.dll", SPACE_FULL);
	auto client_data = static_cast<unsigned char*>(api->GetClientData());
	Function crosshair;
	if (client_base && client_end && client_data && read_memory(client_data + 32, &crosshair, sizeof(crosshair)) &&
		valid_object(crosshair.chain, nitro_base, nitro_end, 11) &&
		valid_object(crosshair.hook, nitro_base, nitro_end, 5))
	{
		auto chain = static_cast<ResultChain<int, void*, float, int>*>(crosshair.chain);
		if (chain->IsHookAttached() && chain->GetHook() == crosshair.hook &&
			crosshair.hook->IsHookActive() &&
			crosshair.hook->GetOriginalFunctionAddress() == client_base + 0x426c0)
		{
			m_crosshair = crosshair;
			CConsole::the().info("Validated Nitro client crosshair handler chain.");
		}
	}
	if (!m_crosshair.chain)
		CConsole::the().warning("Nitro client crosshair chain is unavailable for this client.dll; custom crosshair replacement will be omitted.");
    char version[128]{};
    api->GetVersion(version, sizeof(version));
    version[sizeof(version) - 1] = 0;
    CConsole::the().info("Validated Nitro API adapter: {}, engine build {}.", version, api_build);
    m_enabled = true;
    return true;
}

void pump_pending_operation()
{
    if (GetCurrentThreadId() != engine_thread.load(std::memory_order_acquire)) return;
    operation_readers.fetch_add(1);
    struct ReaderScope
    {
        ~ReaderScope() { operation_readers.fetch_sub(1); }
    } reader_scope;
    auto job = pending.load();
    if (!job) return;
    int expected = 0;
    if (!job->state.compare_exchange_strong(expected, 1)) return;
    job->result = job->function(job->context);
    SetEvent(job->done);
    job->state.store(2, std::memory_order_release);
}

bool run_on_engine_thread(bool (*operation)(void*), void* context)
{
	DWORD selected_thread = engine_thread.load(std::memory_order_acquire);
	if (selected_thread && GetCurrentThreadId() == selected_thread)
	{
		auto succeeded = operation(context);
		if (!succeeded) CConsole::the().error("Nitro dispatch: handler mutation returned false on the engine thread.");
		return succeeded;
	}
	HWND window = nullptr;
	if (CMemoryHookMgr::the().pmainwindow().is_installed())
	{
		uintptr_t window_pointer = 0;
		if (read_memory(CMemoryHookMgr::the().pmainwindow().get(), &window_pointer, sizeof(window_pointer)) && window_pointer)
			read_memory(reinterpret_cast<void*>(window_pointer), &window, sizeof(window));
	}
	if (!IsWindow(window))
	{
		WindowSearch search{GetCurrentProcessId()};
		EnumWindows(find_process_window, reinterpret_cast<LPARAM>(&search));
		window = search.window;
	}
	DWORD process = 0;
	DWORD window_thread = window ? GetWindowThreadProcessId(window, &process) : 0;
	bool has_window = window_thread && process == GetCurrentProcessId();
	if (has_window)
	{
		selected_thread = window_thread;
		engine_thread.store(selected_thread, std::memory_order_release);
	}
	else if (!selected_thread)
	{
		CConsole::the().error("Nitro dispatch: window {} is invalid or belongs to process {} (ours {}).", (uintptr_t)window, process, GetCurrentProcessId());
		return false;
	}
	if (GetCurrentThreadId() == selected_thread)
	{
		auto succeeded = operation(context);
		if (!succeeded) CConsole::the().error("Nitro dispatch: handler mutation returned false on the engine thread.");
		return succeeded;
	}
	if (!operation_message) operation_message = RegisterWindowMessageW(L"OxyWare.Nitro.ChainOperation");
	if (!operation_message)
	{
		CConsole::the().error("Nitro dispatch: RegisterWindowMessage failed ({}).", GetLastError());
		return false;
	}

    PendingOperation job;
    job.function = operation;
    job.context = context;
    job.done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
	if (!job.done)
	{
		CConsole::the().error("Nitro dispatch: CreateEvent failed ({}).", GetLastError());
		return false;
	}
    PendingOperation* empty = nullptr;
	if (!pending.compare_exchange_strong(empty, &job))
	{
		CConsole::the().error("Nitro dispatch: another handler mutation is already pending.");
		CloseHandle(job.done);
		return false;
	}
	HHOOK hook = nullptr;
	if (has_window)
	{
		hook = SetWindowsHookExW(WH_GETMESSAGE, dispatch_hook, nullptr, selected_thread);
		if (!hook)
			CConsole::the().error("Nitro dispatch: SetWindowsHookEx failed ({}), window {}, thread {}.", GetLastError(), (uintptr_t)window, selected_thread);
		else if (!PostMessageW(window, operation_message, 0, 0))
			CConsole::the().error("Nitro dispatch: PostMessage failed ({}), window {}, thread {}.", GetLastError(), (uintptr_t)window, selected_thread);
	}
	if (hook || !has_window)
	{
		auto waited = WaitForSingleObject(job.done, 5000);
		if (waited != WAIT_OBJECT_0)
			CConsole::the().error("Nitro dispatch: engine thread did not execute the operation (wait {}).", waited);
	}
    int queued = 0;
    job.state.compare_exchange_strong(queued, 3);
    while (job.state.load(std::memory_order_acquire) == 1) Sleep(1);
    pending.store(nullptr);
    if (hook) UnhookWindowsHookEx(hook);
    while (dispatchers.load(std::memory_order_acquire) || operation_readers.load()) Sleep(1);
    CloseHandle(job.done);
    if (!job.result) CConsole::the().error("Nitro chain operation failed or the engine thread did not respond.");
    return job.result;
}
}
