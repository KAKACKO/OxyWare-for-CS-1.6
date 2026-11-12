# OxyWare

This is mainly a build so that the cheat supports non-steam build 8684 which the original doesn't support non-steam.

This is just for fun, everything should work fine. I will maybe update it sometimes but don't expect anything. All credits go to oxiKKK.

OxyWare is a Windows client for Counter-Strike 1.6 non-Steam build 8684. It requires the OpenGL renderer.

The interface uses the Segoe UI fonts installed with Windows. Font files are not included in the source or release package.

Please star this, if you have any questions/suggestions message me on discord: kakacko

## Download and run

Download the Windows ZIP from the repository's **Releases** page and extract the entire archive. Keep `OxyWare.exe` and the DLLs together in the extracted folder. Start Counter-Strike with the OpenGL renderer, run `OxyWare.exe`, then select **Start cheat**. Focus the game and press **Insert** to open the menu.

## Build

Open `OxyWare.sln` in Visual Studio and build **Release | x86** with the v145 toolset. The loader and DLLs are written to `build`. Run `powershell -ExecutionPolicy Bypass -File .\create_release.ps1` to assemble the portable folder, ZIP, and SHA-256 checksum in `deploy`. Upload the ZIP and checksum to a GitHub Release. The `build` and `deploy` folders are excluded from the source repository.

## Data and source

Existing configuration and logs are stored under `%AppData%\oxware`. The legacy folder and internal identifiers are retained so existing settings continue to load.

OxyWare is based on the original [oxWARE project](https://github.com/oxiKKK/oxware) by oxiKKK. The original copyright notices and license remain in the source. See [LICENSE](LICENSE) and [changelog.md](changelog.md).

The interface uses [FreeType](https://freetype.org) and [GLFW](https://www.glfw.org). This software is based in part on the work of the FreeType Team. Their license files are in `src/external/freetype`; GLFW's license is retained in its headers. The bundled Microsoft C++ runtime DLLs are Microsoft redistributables and are licensed separately from the MIT project code.
