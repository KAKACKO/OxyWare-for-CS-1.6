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

VarInteger movement_bhop_mode("movement_bhop_mode", "Bunnyhop mode - legit or rage", BHOPMODE_Legit, BHOPMODE_Legit, BHOPMODE_Rage);
VarBoolean movement_bhop_jump_on_ladder("movement_bhop_jump_on_ladder", "Jump if also on a ladder", true);
VarBoolean movement_bhop_jump_in_water("movement_bhop_jump_in_water", "Jump if in water", true);
VarInteger movement_bhop_repeat_ms("movement_bhop_repeat_ms", "Interval which determines when we can jump again after we jumped", 0, 0, 1000);
VarBoolean movement_bhop_mode_noslowdown("movement_bhop_mode_noslowdown", "Enables bhop noslowdown", false);
VarInteger movement_bhop_mode_noslowdown_method("movement_bhop_mode_noslowdown_method", "Noslowdown method", BHOPNSDN_EngineSpeed, BHOPNSDN_ServerSpeed, BHOPNSDN_EngineSpeed);
VarInteger movement_bhop_mode_noslowdown_factor("movement_bhop_mode_noslowdown_factor", "How much to no-slowdown", 1, 1, 10);
VarInteger movement_bhop_legit_ground_dist_min("movement_bhop_legit_ground_dist_min", "Minimal ground distance where to start jumping", 10, 5, 50);
VarInteger movement_bhop_legit_ground_dist_max("movement_bhop_legit_ground_dist_max", "Maximal ground distance where to start jumping", 25, 5, 50);
VarInteger movement_bhop_legit_efficiency("movement_bhop_legit_efficiency", "Efficiency settings of the bunnyhop", 0, 0, 2);
VarInteger movement_bhop_legit_scroll_density("movement_bhop_legit_scroll_density", "How dense will the resulting scroll pattern be", 2, 1, 5);

void CMovementBunnyHop::update(float frametime)
{
	int bhop_mode = movement_bhop_mode.get_value();

	bool is_surfing = CLocalState::the().is_surfing();
	if (is_surfing)
	{
		return;
	}

	if (movement_bhop_jump_in_water.get_value() && CLocalState::the().get_waterlevel() >= 2)
	{
		if (timer_allows_jump())
		{
			CClientMovementPacket::the().set_button_bit(IN_JUMP, true);
			reset_jump_time();
		}
		return;
	}
	
	if (movement_bhop_jump_on_ladder.get_value() && CLocalState::the().is_on_ladder())
	{
		if (timer_allows_jump())
		{
			CClientMovementPacket::the().set_button_bit(IN_JUMP, true);
			reset_jump_time();
		}
		return;
	}

	if (bhop_mode == BHOPMODE_Legit)
	{

		legit_bhop(frametime);
	}
	else if (bhop_mode == BHOPMODE_Rage)
	{ 

		rage_bhop(frametime);
	}
}

void CMovementBunnyHop::rage_bhop(float frametime)
{
	if (!timer_allows_jump())
	{
		return;
	}


	bool jump = perfect_condition_for_jump();

	if (jump)
	{
		reset_jump_time();
	}

	CClientMovementPacket::the().set_button_bit(IN_JUMP, jump);
}

void CMovementBunnyHop::legit_bhop(float frametime)
{
	float gnd_dist = CLocalState::the().get_ground_dist();
	bool is_onground = perfect_condition_for_jump();
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	determine_random_gnddist_for_jump_simul(is_onground);

	auto bhop_eff = (EBhopEfficiency)movement_bhop_legit_efficiency.get_value();

	if (bhop_eff == BHOPEFF_Helper && hop_helper(frametime))
	{
		return;
	}


	bool perform_jump = (is_onground || gnd_dist < m_simul_start_gnddist);


	bool block_jump = false;
	if (randomize_jump_pattern(is_onground))
	{
		block_jump = true;
	}

	if (perform_jump && !block_jump)
	{
		if (bhop_eff == BHOPEFF_RandomFOGBased)
		{

			static int cnt = 0;
			if (++cnt % cl_enginefuncs->pfnRandomLong(1, 10) == 0)
			{
				return;
			}
		}

		if (timer_allows_jump())
		{
			if (is_onground)
			{
				nsdn_speedhack();
			}

			simulate_jump(frametime);

			reset_jump_time();
		}


		m_fog_counter++;
	}
	else
	{
		m_fog_counter = false;
		if (m_remained_in_jump)
		{
			if (!CClientMovementPacket::the().set_button_bit_atomic(IN_JUMP))
			{
				m_remained_in_jump = false;
			}
		}
	}
}

void CMovementBunnyHop::simulate_jump(float frametime)
{
	m_remained_in_jump = CClientMovementPacket::the().set_button_bit_atomic(IN_JUMP);
}

void CMovementBunnyHop::determine_random_gnddist_for_jump_simul(bool on_ground)
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();
	static bool was_on_ground = false;
	if (on_ground != was_on_ground)
	{
		int min = movement_bhop_legit_ground_dist_min.get_value();
		int max = movement_bhop_legit_ground_dist_max.get_value();


		if (on_ground)
		{
			m_simul_start_gnddist = (float)cl_enginefuncs->pfnRandomLong(min, max);
		}
		else
		{
			m_simul_start_gnddist = (float)cl_enginefuncs->pfnRandomLong(min, max);
		}

		was_on_ground = on_ground;
	}
}

bool CMovementBunnyHop::hop_helper(float frametime)
{

	if (predicted_nextframe_on_ground(frametime))
	{


		CClientMovementPacket::the().set_button_bit(IN_JUMP, false);
		return true;
	}
	
	return false;
}

bool CMovementBunnyHop::perfect_condition_for_jump()
{
	return CLocalState::the().is_on_ground_safe();
}

bool CMovementBunnyHop::randomize_jump_pattern(bool is_onground)
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();
	static unsigned cnt = 0;
	int dens = movement_bhop_legit_scroll_density.get_value();
	if (dens == 5)
	{
		return false;
	}

	if (++cnt % cl_enginefuncs->pfnRandomLong(1, 5 - dens) != 0 && !is_onground)
	{
		return true;
	}

	return false;
}

bool CMovementBunnyHop::predicted_nextframe_on_ground(float frametime)
{
	float gnd_dist = CLocalState::the().get_ground_dist();
	float fall_vel = CLocalState::the().get_fall_velocity();
	bool is_onground = perfect_condition_for_jump();

	if (is_onground)
	{

		return false;
	}

	if (gnd_dist == 0.0f || fall_vel == 0.0f)
	{
		return false;
	}


	return fall_vel > 0.0f && ((fall_vel * frametime) * 2.0f > gnd_dist);
}

void CMovementBunnyHop::render_debug()
{
	CEngineFontRendering::the().render_debug("--- Bhop ---");

	CEngineFontRendering::the().render_debug("Perfect condition for jump: {}", perfect_condition_for_jump());
	CEngineFontRendering::the().render_debug("m_simul_start_gnddist: {}", m_simul_start_gnddist);
}

void CMovementBunnyHop::nsdn_speedhack()
{
	if (!movement_bhop_mode_noslowdown.get_value())
	{
		return;
	}

	auto method = (EBhopNoSlowDownMethod)movement_bhop_mode_noslowdown_method.get_value();
	int factor = movement_bhop_mode_noslowdown_factor.get_value();

	switch (method)
	{
		case BHOPNSDN_ServerSpeed:
		{
			int speed = std::max((10 - (factor - 1)), 0);
			CEngineSpeedControl::the().server_speedhack(speed);
			break;
		}
		case BHOPNSDN_EngineSpeed:
		{
			CEngineSpeedControl::the().classic_cs16_cheating_scene_speedhack(((double)(10 - (factor - 1)) / 4.0) / 10.0);
			break;
		}
	}
}

bool CMovementBunnyHop::timer_allows_jump()
{
	int repeat_ms = movement_bhop_repeat_ms.get_value();
	if (repeat_ms == 0)
	{
		return true;
	}

	if (GetTickCount() - m_jump_timer_ms <= repeat_ms)
	{
		return false;
	}

	return true;
}

void CMovementBunnyHop::reset_jump_time()
{
	int repeat_ms = movement_bhop_repeat_ms.get_value();
	if (repeat_ms == 0)
	{
		return;
	}

	m_jump_timer_ms = GetTickCount();
}
