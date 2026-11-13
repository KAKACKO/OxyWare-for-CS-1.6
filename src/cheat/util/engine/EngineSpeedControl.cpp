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

VarBoolean speed_control_enable("speed_control_enable", "Enables in-game speed controlling", false);

VarFloat speed_control_amount_engine("speed_control_amount_engine", "Speed intensity of engine speedhack", 1.0f, 0.1f, 50.0f);
VarInteger speed_control_amount_server("speed_control_amount_server", "Speed intensity of server speedhack", 1, 0, 50);
VarFloat speed_control_amount_net("speed_control_amount_net", "Speed intensity of net speedhack", 0.0f, 0.01f, 50.0f);

VarBoolean speed_control_invert("speed_control_invert", "Applies 1/value to every speehack type", false);
VarBoolean speed_control_negative("speed_control_negative", "Makes every speehack value negative", false);

void CEngineSpeedControl::update()
{
	if (!speed_control_enable.get_value())
	{
		reset();
		return;
	}

	float engine_value = speed_control_amount_engine.get_value();
	int server_value = speed_control_amount_server.get_value();
	float net_value = speed_control_amount_net.get_value();

	bool invert = speed_control_invert.get_value();
	bool negative = speed_control_negative.get_value();


	if (engine_value != speed_control_amount_engine.get_min())
	{
		if (invert) engine_value = 1.0f / engine_value;
		if (negative) engine_value *= -1.0f;
		classic_cs16_cheating_scene_speedhack(engine_value);
	}
	else
	{
		reset();
	}

	if (server_value != speed_control_amount_server.get_min())
	{
		if (negative) engine_value *= -1;
		server_speedhack(server_value);
	}

	if (net_value != speed_control_amount_net.get_min())
	{
		if (invert) engine_value = 1.0f / engine_value;
		if (negative) engine_value *= -1.0f;
		realtime_speedhack(net_value);
	}
}

void CEngineSpeedControl::server_speedhack(uint8_t msec, bool override_previous)
{
	CClientMovementPacket::the().svside_movement_speed_factor(msec, override_previous);
}

void CEngineSpeedControl::classic_cs16_cheating_scene_speedhack(double new_speed)
{


	static double prev_random_thing_xd = 1.0;
	if (prev_random_thing_xd != new_speed)
	{
		double* totally_random_stuff_ptr = CMemoryHookMgr::the().random_1k_speedhack_modifier_constant().get();

		CGenericUtil::the().push_page_protection((uintptr_t)totally_random_stuff_ptr, sizeof(double), PAGE_EXECUTE_READWRITE);
		*totally_random_stuff_ptr = new_speed * 1000.0;
		CGenericUtil::the().pop_page_protection();

		prev_random_thing_xd = new_speed;
	}
}


void CEngineSpeedControl::realtime_speedhack(double new_speed)
{
	double* realtime = CMemoryHookMgr::the().realtime().get();

	double old = *realtime;
	*realtime += new_speed / 1000.0;

	CConsole::the().dinfo("realtime: {} -> {}", old, *realtime);
}

void CEngineSpeedControl::reset()
{


	CEngineSpeedControl::the().classic_cs16_cheating_scene_speedhack(1.0);
}