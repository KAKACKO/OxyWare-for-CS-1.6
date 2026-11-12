#pragma once

#include <Windows.h>
#include <shellapi.h>
#include <filesystem>
#include <string>

// Prefer an explicit game argument. Custom launchers may omit it; in that
// case identify the game from the client DLL actually loaded by the engine.
inline std::wstring resolve_game_directory(const wchar_t* command_line,
                                           const std::filesystem::path& client_module)
{
	int argc = 0;
	wchar_t** argv = CommandLineToArgvW(command_line, &argc);
	if (!argv)
		return {};

	for (int i = 1; i < argc; ++i)
	{
		if (_wcsicmp(argv[i], L"-game") != 0)
			continue;

		std::wstring directory;
		if (i + 1 < argc && argv[i + 1][0] != L'-')
			directory = argv[i + 1];
		LocalFree(argv);
		return directory;
	}
	LocalFree(argv);

	const auto client_directory = client_module.parent_path();
	if (_wcsicmp(client_module.filename().c_str(), L"client.dll") != 0 ||
		_wcsicmp(client_directory.filename().c_str(), L"cl_dlls") != 0)
		return {};

	return client_directory.parent_path().filename().wstring();
}
