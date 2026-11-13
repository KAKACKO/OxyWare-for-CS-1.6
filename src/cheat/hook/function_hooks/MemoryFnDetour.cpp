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

bool CMemoryFnDetourMgr::install_hooks()
{
	if (!_Host_Frame().install()) return false;


	CEngineSynchronization::the().put_engine_in_sleep();


	glBegin().install();
	glReadPixels().install();
	wglSwapBuffers().install();
	glColor4f().install();
	if (COxWare::the().get_build_number() < FIRST_SDL_BUILD)
	{
		VGui_CallEngineSurfaceAppHandler4554().install();
	}
	else
	{
		VGui_CallEngineSurfaceAppHandler().install();
	}
	Key_Event().install();
	Host_Noclip_f().install();
	CL_CreateMove().install();
	CL_ReallocateDynamicData().install();
	CL_DeallocateDynamicData().install();
	MYgluPerspective().install();
	if (COxWare::the().get_build_number() >= 4554)
	{
		if (!R_ForceCVars().install()) { CEngineSynchronization::the().resume_engine(); return false; }
	}
	V_CalcRefdef().install();

	HUD_Redraw().install();
	R_GLStudioDrawPoints().install();
	V_FadeAlpha().install();
	V_ApplyShake().install();
	if (!S_StartDynamicSound().install()) { CEngineSynchronization::the().resume_engine(); return false; }
	R_DrawViewModel().install();
	CPartSmokeGrenade__Create().install();
	CreateGasSmoke().install();
	CEngine__Unload().install();
	SCR_CalcRefdef().install();
	SCR_UpdateScreen().install();
	if (!SPR_Set().install()) { CEngineSynchronization::the().resume_engine(); return false; }
	CGame__AppActivate().install();
	CHudAmmo__DrawCrosshair().install();
	R_StudioDrawPlayer().install();
	CL_SendConsistencyInfo().install();
	SCR_DrawFPS().install();
	Cmd_AddMallocCommand().install();
	hudRegisterVariable().install();
	MSG_WriteUsercmd().install();
	CHudSniperScope__Draw().install();
	CL_IsThirdPerson().install();
	CL_ProcessEntityUpdate().install();
	HUD_PostRunCmd().install();
	HUD_CreateEntities().install();
	HUD_DrawTransparentTriangles().install();
	MakeSkyVec().install();
	HUD_Frame().install();
	R_DrawEntitiesOnList().install();
	R_StudioSetupLighting().install();
	VGui_ViewportPaintBackground().install();
#ifdef INTERCEPT_STEAM_LOGGING
	if (COxWare::the().get_build_number() >= 8684)
	{
		CLogInstance__log().install();
	}
#endif
	Steam_GSInitiateGameConnection().install();

	CEngineSynchronization::the().resume_engine();

	return true;
}

void CMemoryFnDetourMgr::uninstall_hooks()
{
	m_unloading_hooks_mutex = true;


	glBegin().uninstall();
	glReadPixels().uninstall();
	wglSwapBuffers().uninstall();
	glColor4f().uninstall();
	if (COxWare::the().get_build_number() < FIRST_SDL_BUILD)
	{
		VGui_CallEngineSurfaceAppHandler4554().uninstall();
	}
	else
	{
		VGui_CallEngineSurfaceAppHandler().uninstall();
	}
	Key_Event().uninstall();
	Host_Noclip_f().uninstall();
	CL_CreateMove().uninstall();
	CL_ReallocateDynamicData().uninstall();
	CL_DeallocateDynamicData().uninstall();
	MYgluPerspective().uninstall();
	if (COxWare::the().get_build_number() >= 4554)
	{
		R_ForceCVars().uninstall();
	}
	V_CalcRefdef().uninstall();

	HUD_Redraw().uninstall();
	R_GLStudioDrawPoints().uninstall();
	V_FadeAlpha().uninstall();
	V_ApplyShake().uninstall();
	S_StartDynamicSound().uninstall();
	R_DrawViewModel().uninstall();
	CPartSmokeGrenade__Create().uninstall();
	CreateGasSmoke().uninstall();

	if (!m_unloading_from_CEngine__Unload)
	{

		CEngine__Unload().uninstall();
	}

	SCR_CalcRefdef().uninstall();
	SCR_UpdateScreen().uninstall();
	SPR_Set().uninstall();
	CGame__AppActivate().uninstall();
	CHudAmmo__DrawCrosshair().uninstall();
	R_StudioDrawPlayer().uninstall();
	CL_SendConsistencyInfo().uninstall();
	SCR_DrawFPS().uninstall();
	Cmd_AddMallocCommand().uninstall();
	hudRegisterVariable().uninstall();
	MSG_WriteUsercmd().uninstall();
	CHudSniperScope__Draw().uninstall();
	CL_IsThirdPerson().uninstall();
	CL_ProcessEntityUpdate().uninstall();
	HUD_PostRunCmd().uninstall();
	HUD_CreateEntities().uninstall();
	HUD_DrawTransparentTriangles().uninstall();
	MakeSkyVec().uninstall();
	HUD_Frame().uninstall();
	R_DrawEntitiesOnList().uninstall();
	R_StudioSetupLighting().uninstall();
	VGui_ViewportPaintBackground().uninstall();
#ifdef INTERCEPT_STEAM_LOGGING
	if (COxWare::the().get_build_number() >= 8684)
	{
		CLogInstance__log().uninstall();
	}
#endif
	Steam_GSInitiateGameConnection().uninstall();


	_Host_Frame().uninstall();

	m_unloading_hooks_mutex = false;
}


bool wglSwapBuffers_FnDetour_t::install()
{


	if (COxWare::the().get_build_number() >= FIRST_SDL_BUILD)
	{
		initialize("SwapBuffers", L"gdi32.dll");
		return detour_using_exported_name((uintptr_t*)wglSwapBuffers, "SwapBuffers");
	}

	initialize("wglSwapBuffers", L"opengl32.dll");
	return detour_using_exported_name((uintptr_t*)wglSwapBuffers, "wglSwapBuffers");
}

BOOL APIENTRY wglSwapBuffers_FnDetour_t::wglSwapBuffers(HDC hdc)
{
	OX_PROFILE_SCOPE("swapbuffers");

	COxWareUI::the().swapbuffers_detour(hdc);

	return CMemoryFnDetourMgr::the().wglSwapBuffers().call(hdc);
}


bool glBegin_FnDetour_t::install()
{
	initialize("glBegin", L"opengl32.dll");
	return detour_using_exported_name((uintptr_t*)glBegin, "glBegin");
}

void APIENTRY glBegin_FnDetour_t::glBegin(GLenum mode)
{


	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		if (CGameUtil::the().is_fully_connected())
		{
			if (world_visuals_dimlight_world.get_value() && mode == GL_POLYGON)
			{
				CWorldVisuals::the().dim();
			}
		}
	}

	CMemoryFnDetourMgr::the().glBegin().call(mode);
}


bool glReadPixels_FnDetour_t::install()
{
	initialize("glReadPixels", L"opengl32.dll");
	return detour_using_exported_name((uintptr_t*)glReadPixels, "glReadPixels");
}

void APIENTRY glReadPixels_FnDetour_t::glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{


	if (!antiscreen_enable.get_value())
	{
		CMemoryFnDetourMgr::the().glReadPixels().call(x, y, width, height, format, type, pixels);
		return;
	}

	if (CAntiScreen::the().glreadpixels_called_by_antiscreen())
	{

		CMemoryFnDetourMgr::the().glReadPixels().call(x, y, width, height, format, type, pixels);
	}
	else
	{

		CAntiScreen::the().detour_pixels((uint8_t*)pixels);
	}
}


bool glColor4f_FnDetour_t::install()
{
	initialize("glColor4f", L"opengl32.dll");
	return detour_using_exported_name((uintptr_t*)glColor4f, "glColor4f");
}

void glColor4f_FnDetour_t::glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	CModelChams::the().update_studio_colors(&r, &g, &b, &a);

	CMemoryFnDetourMgr::the().glColor4f().call(r, g, b, a);
}


bool VGui_CallEngineSurfaceAppHandler_FnDetour_t::install()
{
	initialize("VGui_CallEngineSurfaceAppHandler", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)VGui_CallEngineSurfaceAppHandler);
}

void VGui_CallEngineSurfaceAppHandler_FnDetour_t::VGui_CallEngineSurfaceAppHandler(void* event, void* userData)
{


	if (COxWareUI::the().should_disable_ingame_input())
	{
		return;
	}

	CMemoryFnDetourMgr::the().VGui_CallEngineSurfaceAppHandler().call(event, userData);
}


bool VGui_CallEngineSurfaceAppHandler4554_FnDetour_t::install()
{
	initialize("VGui_CallEngineSurfaceAppHandler4554", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)VGui_CallEngineSurfaceAppHandler4554);
}

LRESULT VGui_CallEngineSurfaceAppHandler4554_FnDetour_t::VGui_CallEngineSurfaceAppHandler4554(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{


	if (COxWareUI::the().should_disable_ingame_input())
	{
		return 0;
	}

	return CMemoryFnDetourMgr::the().VGui_CallEngineSurfaceAppHandler4554().call(hwnd, msg, wparam, lparam);
}


bool Key_Event_FnDetour_t::install()
{
	initialize("Key_Event", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)Key_Event);
}

void Key_Event_FnDetour_t::Key_Event(int key, hl::qboolean down)
{


	if (COxWareUI::the().should_disable_ingame_input())
	{
		return;
	}


	if (g_in_commands_i->should_block_engine_key(g_user_input_i->engine_key_to_virtual_key(key)))
	{
		return;
	}


	if (g_bindmgr_i->should_block_engine_key(g_user_input_i->engine_key_to_virtual_key(key)))
	{
		return;
	}

	CMemoryFnDetourMgr::the().Key_Event().call(key, down);
}


bool Host_Noclip_f_FnDetour_t::install()
{
	initialize("Host_Noclip_f", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)Host_Noclip_f);
}

void Host_Noclip_f_FnDetour_t::Host_Noclip_f()
{
	CForceEnableDisabled::the().force_enable_noclip_pre();

	CMemoryFnDetourMgr::the().Host_Noclip_f().call();

	CForceEnableDisabled::the().force_enable_noclip_post();
}


bool CL_CreateMove_FnDetour_t::install()
{
	initialize("CL_CreateMove", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)CL_CreateMove, (uintptr_t*)CMemoryHookMgr::the().cl_funcs()->pfnHUD_CL_CreateMove);
}

void CL_CreateMove_FnDetour_t::CL_CreateMove(float frametime, hl::usercmd_t *cmd, int active)
{


	CPanic::the().update();

	if (active)
	{
		CLocalState::the().update_pre_clientmove(frametime, cmd);

		if (bypass_constrain_renderer_glclear.get_value())
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}

	CMemoryFnDetourMgr::the().CL_CreateMove().call(frametime, cmd, active);

	if (active)
	{
		CLocalState::the().update_clientmove(frametime, cmd);


		g_in_commands_i->update_activation_conditions();

		if (!CGameUtil::the().is_spectator())
		{
			CNetchanSequenceHistory::the().update();

			CClientMovementPacket::the().update_clientmove(cmd);

			CEngineSpeedControl::the().update();

			if (COxWareUI::the().should_disable_ingame_input())
			{


				cmd->buttons = 0;
				cmd->forwardmove = cmd->sidemove = cmd->upmove = 0;
			}

			CNonSteamFpsFixer::the().fix_fps();

			CAutomation::the().update();
			CTriggerbot::the().update(cmd);

			CMovement::the().update_clientmove(frametime, cmd);
		}

#if 0
		if (CUserMSGDetourMgr::the().ReceiveW_fn().is_installed())
		{
			uint8_t value = 2;
			CUserMSGDetourMgr::the().ReceiveW_fn().call("ReceiveW", sizeof(uint8_t), &value);
		}
#endif

		if (debug_render_info.get_value())
		{
			CEngineFontRendering::the().render_information();
		}
	}
}


bool _Host_Frame_FnDetour_t::install()
{
	initialize("_Host_Frame", L"hw.dll");
	if (nitro_compat::Compatibility::get().enabled())
		return install_nitro(nitro_compat::FunctionId::Frame, _Host_Frame);
	return detour_using_bytepattern((uintptr_t*)_Host_Frame);
}

void _Host_Frame_FnDetour_t::_Host_Frame(float time)
{

	static bool was_connected = false;
	bool is_connected = (CMemoryHookMgr::the().cls()->state == hl::ca_active);
	if (was_connected != is_connected)
	{
		CGameEvents::the().on_connect();
		was_connected = is_connected;
	}

	if (!CEngineSynchronization::the().engine_frame())
	{

		return;
	}


	OX_PROFILE_SCOPE("engine_frame");

	CForceEnableDisabled::the().update_disable_sponly_cvars();

	CVideoModeUtil::the().update();

	CMemoryFnDetourMgr::the()._Host_Frame().call(time);
}


bool CL_ReallocateDynamicData_FnDetour_t::install()
{
	initialize("CL_ReallocateDynamicData", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CL_ReallocateDynamicData);
}

void CL_ReallocateDynamicData_FnDetour_t::CL_ReallocateDynamicData(int nMaxClients)
{


	CMemoryFnDetourMgr::the().CL_ReallocateDynamicData().call(nMaxClients);


	CEntityMgr::the().erase();
	CEntityMgr::the().init();

	CSoundEsp::the().reset_cache();
}


bool CL_DeallocateDynamicData_FnDetour_t::install()
{
	initialize("CL_DeallocateDynamicData", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CL_DeallocateDynamicData);
}

void CL_DeallocateDynamicData_FnDetour_t::CL_DeallocateDynamicData()
{


	CEntityMgr::the().erase();

	CMemoryFnDetourMgr::the().CL_DeallocateDynamicData().call();
}


bool MYgluPerspective_FnDetour_t::install()
{
	initialize("MYgluPerspective", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)MYgluPerspective);
}

void MYgluPerspective_FnDetour_t::MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble out_zFar = zFar;
	GLdouble out_aspect = aspect;

	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{

		auto renderdist_scaled = CForceEnableDisabled::the().force_max_viewable_renderdistance();
		if (renderdist_scaled)
		{
			out_zFar = zFar * renderdist_scaled.value();
		}


		auto aspect_scaled = CAspectRatioChanger::the().scale_aspect();
		if (aspect_scaled)
		{
			out_aspect = aspect * aspect_scaled.value();
		}
	}

	CMemoryFnDetourMgr::the().MYgluPerspective().call(fovy, out_aspect, zNear, out_zFar);
}


bool R_ForceCVars_FnDetour_t::install()
{
	initialize("R_ForceCVars", L"hw.dll");
	if (nitro_compat::Compatibility::get().enabled())
		return install_nitro(nitro_compat::FunctionId::ForceCVars, R_ForceCVars);
	return detour_using_bytepattern((uintptr_t*)R_ForceCVars);
}

void R_ForceCVars_FnDetour_t::R_ForceCVars(hl::qboolean mp)
{
	if (CForceEnableDisabled::the().disable_renderer_cvar_constrain())
	{
		return;
	}

	CMemoryFnDetourMgr::the().R_ForceCVars().call(mp);
}


bool V_CalcRefdef_FnDetour_t::install()
{
	initialize("V_CalcRefdef", L"client.dll");
	return detour_using_memory_address((uintptr_t*)V_CalcRefdef, (uintptr_t*)CMemoryHookMgr::the().cl_funcs()->pfnHUD_CalcRef);
}

void V_CalcRefdef_FnDetour_t::V_CalcRefdef(hl::ref_params_t *pparams)
{
	CMemoryFnDetourMgr::the().V_CalcRefdef().call(pparams);

	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		CThirdPerson::the().update(pparams);


	}
}
 


bool EV_HLDM_FireBullets_FnDetour_t::install()
{
	initialize("EV_HLDM_FireBullets", L"client.dll");
	return detour_using_bytepattern((uintptr_t*)EV_HLDM_FireBullets);
}

void EV_HLDM_FireBullets_FnDetour_t::EV_HLDM_FireBullets(int idx, float* forward, float* right, float* up, int cShots, float* vecSrc, 
													  float* vecDirShooting, float* vecSpread, float flDistance, int iBulletType, 
													  int iTracerFreq, int* tracerCount, int iPenetration)
{
	CBulletTrace::the().on_bullet_fired(idx, forward, right, up, vecSrc, vecDirShooting, vecSpread, flDistance);

	CMemoryFnDetourMgr::the().EV_HLDM_FireBullets().call(idx, forward, right, up, cShots, vecSrc, vecDirShooting, vecSpread, flDistance,
														 iBulletType, iTracerFreq, tracerCount, iPenetration);


}


bool HUD_Redraw_FnDetour_t::install()
{
	initialize("HUD_Redraw", L"client.dll");
	return detour_using_memory_address((uintptr_t*)HUD_Redraw, (uintptr_t*)CMemoryHookMgr::the().cl_funcs()->pfnHUD_Redraw);
}

int HUD_Redraw_FnDetour_t::HUD_Redraw(float time, int intermission)
{
	OX_PROFILE_SCOPE("clientdll_hud_redraw");


	CRemovals::the().remove_hud_modifier();

	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		CSpriteMgr::the().update();
	}

	return CMemoryFnDetourMgr::the().HUD_Redraw().call(time, intermission);
}


bool R_GLStudioDrawPoints_FnDetour_t::install()
{
	initialize("R_GLStudioDrawPoints", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)R_GLStudioDrawPoints);
}

void R_GLStudioDrawPoints_FnDetour_t::R_GLStudioDrawPoints()
{
	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{

		OX_PROFILE_SCOPE("studio_drawpoints");

		CTriggerbot::the().capture_head_hitboxes();
		CModelChams::the().render_playerhead_hitbox();

		if (CModelChams::the().studio_draw_skeleton())
		{
			return;
		}

		CModelChams::the().update_studio_drawpoints_pre();
	}

	CMemoryFnDetourMgr::the().R_GLStudioDrawPoints().call();

	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		CModelChams::the().update_studio_drawpoints_post();
	}
}


bool V_FadeAlpha_FnDetour_t::install()
{
	initialize("V_FadeAlpha", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)V_FadeAlpha);
}

int V_FadeAlpha_FnDetour_t::V_FadeAlpha()
{


	int alpha = CMemoryFnDetourMgr::the().V_FadeAlpha().call();

	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		CFlashbangFadeModifier::the().update(alpha);
	}

	return alpha;
}


bool V_ApplyShake_FnDetour_t::install()
{
	initialize("V_ApplyShake", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)V_ApplyShake);
}

void V_ApplyShake_FnDetour_t::V_ApplyShake(float* origin, float* angles, float factor)
{
	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		if (CRemovals::the().remove_screenshake())
		{
			return;
		}
	}

	CMemoryFnDetourMgr::the().V_ApplyShake().call(origin, angles, factor);
}


bool S_StartDynamicSound_FnDetour_t::install()
{
	initialize("S_StartDynamicSound", L"hw.dll");
	if (nitro_compat::Compatibility::get().enabled())
		return install_nitro(nitro_compat::FunctionId::DynamicSound, S_StartDynamicSound);
	return detour_using_bytepattern((uintptr_t*)S_StartDynamicSound);
}

void S_StartDynamicSound_FnDetour_t::S_StartDynamicSound(int entnum, int entchannel, hl::sfx_t* sfx, hl::vec_t* origin, 
														 float fvol, float attenuation, int flags, int pitch)
{
	if (sfx)
	{
		if (strstr(sfx->name, "step"))
		{
			CSoundEsp::the().register_player_step(origin, entnum);
		}
	}

	CMemoryFnDetourMgr::the().S_StartDynamicSound().call(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch);
}


bool R_DrawViewModel_FnDetour_t::install()
{
	initialize("R_DrawViewModel", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)R_DrawViewModel);
}

void R_DrawViewModel_FnDetour_t::R_DrawViewModel()
{
	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		if (CRemovals::the().remove_viewmodel())
		{
			return;
		}

		CViewmodelOffset::the().update();

		CModelChams::the().force_default_models();
	}
	
	CMemoryFnDetourMgr::the().R_DrawViewModel().call();
}


bool CPartSmokeGrenade__Create_FnDetour_t::install()
{
	initialize("CPartSmokeGrenade::Create", L"client.dll");
	return detour_using_bytepattern((uintptr_t*)CPartSmokeGrenade__Create);
}

hl::CPartSmokeGrenade* __cdecl CPartSmokeGrenade__Create_FnDetour_t::CPartSmokeGrenade__Create(
	hl::CPartSmokeGrenade* _this, Vector org, Vector normal, hl::model_t* sprite, 
	float size, float brightness, const char* classname)
{


	auto part = CMemoryFnDetourMgr::the().CPartSmokeGrenade__Create().call(_this, org, normal, sprite, size/* * 0.5*/, brightness, classname);

	CSmokeVisuals::the().on_smoke_particleeffect_create(part);

	return part;
}


bool CreateGasSmoke_FnDetour_t::install()
{
	initialize("CreateGasSmoke", L"client.dll");
	return detour_using_bytepattern((uintptr_t*)CreateGasSmoke);
}

void CreateGasSmoke_FnDetour_t::CreateGasSmoke(Vector origin, Vector vVelocity, bool bInsideSmoke, bool bSpawnInside, bool bBlowable)
{
	CMemoryFnDetourMgr::the().CreateGasSmoke().call(origin, vVelocity, bInsideSmoke, bSpawnInside, bBlowable);

	CSmokeVisuals::the().post_gas_smoke_create();
}


bool CEngine__Unload_FnDetour_t::install()
{
	initialize("CEngine::Unload", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CEngine_Unload_hook);
}

extern "C" void __declspec(naked) __fastcall CEngine_Unload_hook(hl::CEngine* ecx, void* edx)
{
	__asm
	{
		push ecx
		call CEngine_Unload_impl
		add esp, 4
		test eax, eax
		jz no_native_reference


		pop edx
		push eax
		push edx
		jmp FreeLibrary
	no_native_reference:
		ret
	}
}

extern "C" HMODULE __cdecl CEngine_Unload_impl(hl::CEngine* ecx)
{
	HMODULE native_reference = nullptr;
	if (COxWare::the().get_injection_technique() == INJECT_LOADLIBRARY)
		GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			reinterpret_cast<LPCWSTR>(&CEngine_Unload_hook), &native_reference);
	CConsole::the().info("Game is closing, shutting down cheat...");


	CMemoryFnDetourMgr::the().toggle_unloading_from_CEngine__Unload();

	bool engine_restarting = ecx->m_nQuitting == hl::QUIT_RESTART;
	


	CConsole::the().info("Requesting the end of cheat execution...");
	COxWare::the().end_cheat_execution(true);

	while (!COxWare::the().is_shutted_down())
	{
		nitro_compat::pump_pending_operation();

		std::this_thread::sleep_for(0.1s);
	}

	CMemoryFnDetourMgr::the().CEngine__Unload().call(ecx);
	CMemoryFnDetourMgr::the().CEngine__Unload().uninstall();

	if (engine_restarting)
	{


		CInjectedDllIPCLayerClient::the().write_code(C2I_Restarting);
	}
	else
	{

		CInjectedDllIPCLayerClient::the().write_code(C2I_Unloading);
	}
	return native_reference;
}


bool SCR_CalcRefdef_FnDetour_t::install()
{
	initialize("SCR_CalcRefdef", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)SCR_CalcRefdef);
}

void SCR_CalcRefdef_FnDetour_t::SCR_CalcRefdef()
{


	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		CFieldOfViewChanger::the().scale_fov();
	}

	CMemoryFnDetourMgr::the().SCR_CalcRefdef().call();
}


bool SCR_UpdateScreen_FnDetour_t::install()
{
	initialize("SCR_UpdateScreen", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)SCR_UpdateScreen);
}

void SCR_UpdateScreen_FnDetour_t::SCR_UpdateScreen()
{
	CAntiScreen::the().update();

	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{

		if (frame_skip_method.get_value() == 0 && CFrameSkipper::the().skip_current_frame())
		{
			return;
		}

		CEntityMgr::the().update_screen();
	}

	CMemoryFnDetourMgr::the().SCR_UpdateScreen().call();
}


bool SPR_Set_FnDetour_t::install()
{
	initialize("SPR_Set", L"hw.dll");
	if (nitro_compat::Compatibility::get().enabled())
		return install_nitro(nitro_compat::FunctionId::SpriteSet, SPR_Set);
	return detour_using_memory_address((uintptr_t*)SPR_Set, (uintptr_t*)CMemoryHookMgr::the().cl_enginefuncs()->pfnSPR_Set);
}

void SPR_Set_FnDetour_t::SPR_Set(hl::HSPRITE_t hSprite, int r, int g, int b)
{

	


	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		CSpriteMgr::the().handle_color_change(hSprite, r, g, b);
	}

	CMemoryFnDetourMgr::the().SPR_Set().call(hSprite, r, g, b);
}


bool CGame__AppActivate_FnDetour_t::install()
{
	initialize("CGame::AppActivate", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CGame__AppActivate);
}

void __thiscall CGame__AppActivate_FnDetour_t::CGame__AppActivate(void* ecx, bool fActive)
{


	COxWare::the().set_at_least_once_focused();


	if (COxWareUI::the().should_disable_ingame_input())
	{
		return;
	}

	CMemoryFnDetourMgr::the().CGame__AppActivate().call(ecx, fActive);
}


bool CHudAmmo__DrawCrosshair_FnDetour_t::install()
{
	initialize("CHudAmmo::DrawCrosshair", L"client.dll");
	if (nitro_compat::Compatibility::get().enabled())
	{
		auto fn = nitro_compat::Compatibility::get().crosshair_function();
		if (!fn.chain) return false;
		m_address = reinterpret_cast<uintptr_t*>(fn.hook->GetOriginalFunctionAddress());
		m_detoured = true;
		m_using_nitro = true;
		if (!m_binding.attach(fn, nitro_callback))
		{
			m_address = nullptr;
			m_detoured = false;
			m_using_nitro = false;
			return false;
		}
		CConsole::the().info("Installed '{}' through the Nitro client handler chain.", m_name);
		return true;
	}
	return detour_using_bytepattern((uintptr_t*)CHudAmmo__DrawCrosshair);
}

void CHudAmmo__DrawCrosshair_FnDetour_t::uninstall()
{
	if (!m_using_nitro) { GenericMemoryFnDetour_thiscall<int, void*, float, int>::uninstall(); return; }
	while (!m_binding.detach()) Sleep(100);
	m_address = nullptr;
	m_detoured = false;
}

void CHudAmmo__DrawCrosshair_FnDetour_t::drain_nitro()
{
	if (m_using_nitro) { m_binding.drain(); m_using_nitro = false; }
}

int __thiscall CHudAmmo__DrawCrosshair_FnDetour_t::call(void* ecx, float flTime, int weaponid) const
{
	if (m_using_nitro) return m_binding.call_next(ecx, flTime, weaponid);
	return GenericMemoryFnDetour_thiscall<int, void*, float, int>::call(ecx, flTime, weaponid);
}

int __cdecl CHudAmmo__DrawCrosshair_FnDetour_t::nitro_callback(void* ecx, float flTime, int weaponid)
{
	return CHudAmmo__DrawCrosshair(ecx, flTime, weaponid);
}

int __thiscall CHudAmmo__DrawCrosshair_FnDetour_t::CHudAmmo__DrawCrosshair(void* ecx, float flTime, int weaponid)
{
	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		if (crosshair_enable.get_value())
		{
			CVanillaCrosshair::the().draw();
			return 1;
		}
	}

	return CMemoryFnDetourMgr::the().CHudAmmo__DrawCrosshair().call(ecx, flTime, weaponid);
}


bool R_StudioDrawPlayer_FnDetour_t::install()
{
	initialize("R_StudioDrawPlayer", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)R_StudioDrawPlayer, (uintptr_t*)(*CMemoryHookMgr::the().pStudioAPI().get())->StudioDrawPlayer);
}

int R_StudioDrawPlayer_FnDetour_t::R_StudioDrawPlayer(int flags, hl::entity_state_t* pplayer)
{
	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		if (CRemovals::the().remove_player(pplayer->number))
		{
			return 0;
		}

		if (mdlchams_player_skeleton.get_value())
		{


			pplayer->weaponmodel = 0;
		}

		if (mdlchams_disable_animations.get_value())
		{
			pplayer->gaitsequence = 1;
		}


		if (mdlchams_render_real_playermodel.get_value())
		{
			static hl::cvar_t* cl_minmodels = nullptr;
			if (!cl_minmodels)
			{
				cl_minmodels = CGoldSrcCommandMgr::the().get_cvar("cl_minmodels");
			}

			cl_minmodels->value = 1;

			CModelChams::the().toggle_rendering_real_playermodel();

			CMemoryFnDetourMgr::the().R_StudioDrawPlayer().call(flags, pplayer);

			CModelChams::the().toggle_rendering_real_playermodel();

			cl_minmodels->value = 0;
		}
	}

	return CMemoryFnDetourMgr::the().R_StudioDrawPlayer().call(flags, pplayer);
}


bool CL_SendConsistencyInfo_FnDetour_t::install()
{
	initialize("CL_SendConsistencyInfo", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CL_SendConsistencyInfo);
}

void CL_SendConsistencyInfo_FnDetour_t::CL_SendConsistencyInfo(hl::sizebuf_t* msg)
{
	if (!CResourceConsistencyBypass::the().detour_consistency_info(msg))
	{
		CMemoryFnDetourMgr::the().CL_SendConsistencyInfo().call(msg);
	}
}


bool SCR_DrawFPS_FnDetour_t::install()
{
	initialize("SCR_DrawFPS", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)SCR_DrawFPS);
}

void SCR_DrawFPS_FnDetour_t::SCR_DrawFPS()
{


	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		CEngineRendering::the().repaint();
	}

	CMemoryFnDetourMgr::the().SCR_DrawFPS().call();
}


bool Cmd_AddMallocCommand_FnDetour_t::install()
{
	initialize("Cmd_AddMallocCommand", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)Cmd_AddMallocCommand);
}

void Cmd_AddMallocCommand_FnDetour_t::Cmd_AddMallocCommand(char* cmd_name, hl::xcommand_t function, int flag)
{


	CMemoryFnDetourMgr::the().Cmd_AddMallocCommand().call(cmd_name, function, flag);


	auto cmd = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetFirstCmdFunctionHandle();
	CGoldSrcCommandMgr::the().register_cmd(cmd->name, cmd);
}


bool hudRegisterVariable_FnDetour_t::install()
{
	initialize("hudRegisterVariable", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)hudRegisterVariable, (uintptr_t*)CMemoryHookMgr::the().cl_enginefuncs()->pfnRegisterVariable);
}

hl::cvar_t* hudRegisterVariable_FnDetour_t::hudRegisterVariable(char* szName, char* szValue, int flags)
{
	auto cvar = CMemoryFnDetourMgr::the().hudRegisterVariable().call(szName, szValue, flags);


	if (cvar)
	{
		CGoldSrcCommandMgr::the().register_cvar(cvar->name, cvar);
	}

	return cvar;
}


bool MSG_WriteUsercmd_FnDetour_t::install()
{
	initialize("MSG_WriteUsercmd", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)MSG_WriteUsercmd);
}

void MSG_WriteUsercmd_FnDetour_t::MSG_WriteUsercmd(hl::sizebuf_t* buf, hl::usercmd_t* to, hl::usercmd_t* from)
{


	CClientMovementPacket::the().update_msg_writeusercmd(to);

	CMemoryFnDetourMgr::the().MSG_WriteUsercmd().call(buf, to, from);
}


bool CHudSniperScope__Draw_FnDetour_t::install()
{
	initialize("CHudSniperScope::Draw", L"client.dll");
	return detour_using_bytepattern((uintptr_t*)CHudSniperScope__Draw);
}

int __thiscall CHudSniperScope__Draw_FnDetour_t::CHudSniperScope__Draw(void* ecx, float flTime)
{
	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		if (CRemovals::the().remove_sniper_scope())
		{
			return 1;
		}
	}

	return CMemoryFnDetourMgr::the().CHudSniperScope__Draw().call(ecx, flTime);
}


bool CL_IsThirdPerson_FnDetour_t::install()
{
	initialize("CL_IsThirdPerson", L"client.dll");
	return detour_using_memory_address((uintptr_t*)CL_IsThirdPerson, (uintptr_t*)CMemoryHookMgr::the().cl_funcs()->pfnHUD_CL_IsThirdperson);
}

int CL_IsThirdPerson_FnDetour_t::CL_IsThirdPerson()
{
	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		return CThirdPerson::the().thirdperson.is_active() && thirdperson_dist.get_value() != 0;
	}

	return CMemoryFnDetourMgr::the().CL_IsThirdPerson().call();
}


bool CL_ProcessEntityUpdate_FnDetour_t::install()
{
	initialize("CL_ProcessEntityUpdate", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)CL_ProcessEntityUpdate);
}

void CL_ProcessEntityUpdate_FnDetour_t::CL_ProcessEntityUpdate(hl::cl_entity_t* ent)
{


	CMemoryFnDetourMgr::the().CL_ProcessEntityUpdate().call(ent);

	CEntityMgr::the().entity_state_update(ent);
}


bool HUD_PostRunCmd_FnDetour_t::install()
{
	initialize("HUD_PostRunCmd", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)HUD_PostRunCmd, (uintptr_t*)CMemoryHookMgr::the().cl_funcs()->pfnHUD_PostRunCmd);
}

void HUD_PostRunCmd_FnDetour_t::HUD_PostRunCmd(hl::local_state_t* from, hl::local_state_t* to, hl::usercmd_t* cmd, int runfuncs, 
											   double time, unsigned int random_seed)
{
	CMemoryFnDetourMgr::the().HUD_PostRunCmd().call(from, to, cmd, runfuncs, time, random_seed);

	CWeapons::the().update(to, cmd);
}


bool HUD_CreateEntities_FnDetour_t::install()
{
	initialize("HUD_CreateEntities", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)HUD_CreateEntities, (uintptr_t*)CMemoryHookMgr::the().cl_funcs()->pfnHUD_CreateEntities);
}

void HUD_CreateEntities_FnDetour_t::HUD_CreateEntities()
{


	CMemoryFnDetourMgr::the().HUD_CreateEntities().call();

	auto cl = CMemoryHookMgr::the().cl().get();
	for (int i = 0; i < cl->maxclients; i++)
	{
		CEntityMgr::the().player_info_update(i);
	}
}


bool HUD_DrawTransparentTriangles_FnDetour_t::install()
{
	initialize("HUD_DrawTransparentTriangles", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)HUD_DrawTransparentTriangles, (uintptr_t*)CMemoryHookMgr::the().cl_funcs()->pfnHUD_DrawTransTris);
}

void HUD_DrawTransparentTriangles_FnDetour_t::HUD_DrawTransparentTriangles()
{
	CMemoryFnDetourMgr::the().HUD_DrawTransparentTriangles().call();

	auto iparticleman = CHLInterfaceHook::the().IParticleMan();
	if (iparticleman)
	{
		CEnvironmentalEffects::the().render();
	}
}


bool MakeSkyVec_FnDetour_t::install()
{
	initialize("MakeSkyVec", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)MakeSkyVec);
}

void MakeSkyVec_FnDetour_t::MakeSkyVec(float s, float t, int axis)
{


	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		if (world_visuals_dimlight_sky.get_value() && CGameUtil::the().is_fully_connected())
		{
			CWorldVisuals::the().dim();
		}
	}

	CMemoryFnDetourMgr::the().MakeSkyVec().call(s, t, axis);
}


bool HUD_Frame_FnDetour_t::install()
{
	initialize("HUD_Frame", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)HUD_Frame, (uintptr_t*)CMemoryHookMgr::the().cl_funcs()->pfnHUD_Frame);
}

void HUD_Frame_FnDetour_t::HUD_Frame()
{


	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		CWorldVisuals::the().render_fog();
	}

	CMemoryFnDetourMgr::the().HUD_Frame().call();
}


bool R_DrawEntitiesOnList_FnDetour_t::install()
{
	initialize("R_DrawEntitiesOnList", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)R_DrawEntitiesOnList);
}

void R_DrawEntitiesOnList_FnDetour_t::R_DrawEntitiesOnList()
{


	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		CBacktrack::the().update();
	}
	
	CMemoryFnDetourMgr::the().R_DrawEntitiesOnList().call();
}


bool R_StudioSetupLighting_FnDetour_t::install()
{
	initialize("R_StudioSetupLighting", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)R_StudioSetupLighting, (uintptr_t*)CMemoryHookMgr::the().engine_studio_api().get()->StudioSetupLighting);
}

void R_StudioSetupLighting_FnDetour_t::R_StudioSetupLighting(hl::alight_t* plighting)
{


	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		CModelChams::the().update_studio_lighting(plighting);
	}

	CMemoryFnDetourMgr::the().R_StudioSetupLighting().call(plighting);
}


bool VGui_ViewportPaintBackground_FnDetour_t::install()
{
	initialize("VGui_ViewportPaintBackground", L"hw.dll");
	return detour_using_memory_address((uintptr_t*)VGui_ViewportPaintBackground, (uintptr_t*)CMemoryHookMgr::the().cl_enginefuncs()->pfnVGui_ViewportPaintBackground);
}

void VGui_ViewportPaintBackground_FnDetour_t::VGui_ViewportPaintBackground(int* extents)
{


	if (!CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{

		if (frame_skip_method.get_value() == 1 && CFrameSkipper::the().skip_current_frame())
		{
			return;
		}
	}

	CMemoryFnDetourMgr::the().VGui_ViewportPaintBackground().call(extents);
}


#ifdef INTERCEPT_STEAM_LOGGING
bool CLogInstance__log_FnDetour_t::install()
{
	initialize("CLogInstance::log", L"steamclient.dll");

	auto [base, end] = g_libloader_i->get_loaded_dll_address_space(m_module_name.c_str(), SPACE_CODE);
	if (!base || !end)
	{
		CConsole::the().error("Can't install CLogInstance::log() hook. Dll probably not loaded in address space.");
		return false;
	}

	CBytePattern p = CBytePattern("\x55\x8B\xEC\xA1\xCC\xCC\xCC\xCC\x83\xEC\x3C\x83\x38\x00\x75\x04\x33\xC0\xEB\x14\x8D\x45\xFC\x50\x6A\x0D\x68\xCC\xCC\xCC\xCC\xFF\x15\xCC\xCC\xCC\xCC\x83\xC4\x0C\x81\x65\xC8\xFF\xFF\x0F\xFF\xB9\x04\x00\x00\x00\x89\x45\xFC\x33\xC0");
	if (p.empty())
	{
		CConsole::the().error("Failed to find CLogInstance::log() inside steamclient.dll");
		return false;
	}

	uintptr_t* address = p.search_in_loaded_address_space(base, end);

	return detour_using_memory_address((uintptr_t*)CLogInstance__log, address);
}

void CLogInstance__log_FnDetour_t::CLogInstance__log(int a1, int a2, int a3, int a4, int a5, int a6, 
													 int * a7, int * a8, int * a9, char * message, int a11)
{
	char string[0x4000];
	vsnprintf(string, sizeof(string), message, (va_list)a11);

	CConsole::the().info("[STEAM]{}", string);

	CMemoryFnDetourMgr::the().CLogInstance__log().call(a1, a2, a3, a4, a5, a6, a7, a8, a9, message, a11);
}
#endif


bool Steam_GSInitiateGameConnection_FnDetour_t::install()
{
	initialize("Steam_GSInitiateGameConnection", L"hw.dll");
	return detour_using_bytepattern((uintptr_t*)Steam_GSInitiateGameConnection);
}

int Steam_GSInitiateGameConnection_FnDetour_t::Steam_GSInitiateGameConnection(
	void* pData, int cbMaxData, hl::uint64 steamID, hl::uint32 unIPServer, 
	hl::uint16 usPortServer, hl::qboolean bSecure)
{
	auto spoof = CSIDSpoofer::the().spoof(pData, cbMaxData);

	int ret = 0;
	if (spoof)
	{
		ret = spoof.value();
	}
	else
	{
		ret = CMemoryFnDetourMgr::the().Steam_GSInitiateGameConnection().call(pData, cbMaxData, steamID, unIPServer, usPortServer, bSecure);
	}

#if 0
	uint32_t* pTicket = (uint32_t*)pData;
	for (int i = 0; i < cbMaxData / 4; i++)
	{
		uint8_t bytes[] =
		{
			*(((uint8_t*)(&pTicket[i])) + 0),
			*(((uint8_t*)(&pTicket[i])) + 1),
			*(((uint8_t*)(&pTicket[i])) + 2),
			*(((uint8_t*)(&pTicket[i])) + 3),
		};
		CConsole::the().info("{:<2}: \\x{:08X} ({:<16}) '{}'", i, pTicket[i], pTicket[i], std::format("{}{}{}{}",
																									  (char)bytes[3], (char)bytes[2],
																									  (char)bytes[1], (char)bytes[0]));
	}
#endif

	return ret;
}
