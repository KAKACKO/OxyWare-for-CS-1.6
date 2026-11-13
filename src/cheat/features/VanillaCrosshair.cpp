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

VarBoolean crosshair_enable("crosshair_enable", "Enables vanilla-line crosshair rendering", false);
VarBoolean crosshair_dynamic("crosshair_dynamic", "Dynamically sized crosshair", false);
VarBoolean crosshair_translucent("crosshair_translucent", "Dynamically translucent crosshair (transparent)", false);
VarBoolean crosshair_static("crosshair_static", "Makes the crosshair static while shooting", false);
VarInteger crosshair_size("crosshair_size", "Size of the crosshair", 10, 0, 50);
VarInteger crosshair_gap("crosshair_gap", "Size of the gap between bars of the crosshair", 10, 0, 50);
VarInteger crosshair_thickness("crosshair_thickness", "Thickness of the line that represents crosshair", 1, 1, 5);
VarInteger crosshair_type("crosshair_type", "Changes crosshair type (regular, T-shaped, circle)", 0, 0, 2);
VarColor crosshair_color("crosshair_color", "Color of the crosshair", CColor(0, 255, 0));

constexpr static int g_gap_size_by_weapon_index[MAX_WEAPONS][2] =
{
	{ 4, 3 },
	{ 8, 3 },
	{ 4, 3 },
	{ 8, 3 },
	{ 8, 3 },
	{ 9, 4 },
	{ 6, 3 },
	{ 9, 3 },
	{ 3, 3 },
	{ 8, 3 },
	{ 4, 3 },
	{ 8, 3 },
	{ 6, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 8, 3 },
	{ 8, 3 },
	{ 8, 3 },
	{ 6, 2 },
	{ 6, 3 },
	{ 8, 6 },
	{ 4, 3 },
	{ 7, 3 },
	{ 6, 4 },
	{ 8, 3 },
	{ 8, 3 },
	{ 5, 3 },
	{ 4, 4 },
	{ 7, 3 },
	{ 7, 3 },
};

float CVanillaCrosshair::get_weapon_max_speed_for_crosshair(int weapon_id)
{
	switch (weapon_id)
	{
		case WEAPON_AUG:
		case WEAPON_M4A1:
		case WEAPON_FAMAS:
		case WEAPON_SG550:
		case WEAPON_GALIL:
		case WEAPON_AK47:
		case WEAPON_M249:
		case WEAPON_SG552:
		{
			return 140.0f;
		}
		case WEAPON_P90:
		{
			return 170.0f;
		}
	}

	return 0;
}

#define SCALEDBASE_SMALL	1024
#define SCALEDBASE_MEDIUM	800
#define SCALEDBASE_LARGE	640

int CVanillaCrosshair::scaled_base(int screen_width)
{
	char* crosshair_size = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetCvarString((char*)"cl_crosshair_size");

	if (crosshair_size != NULL)
	{
		if (!stricmp(crosshair_size, "auto"))
		{
			if (screen_width < 640)
			{
				return SCALEDBASE_SMALL;
			}
			else if (screen_width < 1024)
			{
				return SCALEDBASE_MEDIUM;
			}
		}
		else if (!stricmp(crosshair_size, "small"))
		{
			return SCALEDBASE_SMALL;
		}
		else if (!stricmp(crosshair_size, "medium"))
		{
			return SCALEDBASE_MEDIUM;
		}
		else if (!stricmp(crosshair_size, "large"))
		{
			return SCALEDBASE_LARGE;
		}
	}

	return SCALEDBASE_LARGE;
}

void CVanillaCrosshair::draw()
{
	auto current_weapon = CWeapons::the().get_current_weapon();
	if (!current_weapon)
	{
		return;
	}


	static uint32_t prev_cross_time = GetTickCount();


	bool flip_every_N_ms = false;
	if (GetTickCount() - prev_cross_time > 10)
	{
		flip_every_N_ms = true;
		prev_cross_time = GetTickCount();
	}

	auto screen_info = CGameUtil::the().get_engine_screen_info();

	int gap = 0, delta_gap = 0;

	gap = g_gap_size_by_weapon_index[current_weapon->m_weapon_id][0];
	delta_gap = g_gap_size_by_weapon_index[current_weapon->m_weapon_id][1];

	bool use_vanilla_gap = (crosshair_gap.get_value() == 0);


	gap = use_vanilla_gap ? gap : crosshair_gap.get_value();

	EWeaponAccuracyFlags accuracy_flags = current_weapon->calc_wpn_accuracy_flags();


	if (accuracy_flags != WPNACCFLAG_NONE && crosshair_dynamic.get_value() != 0)
	{
		int player_flags = CLocalState::the().get_player_flags();

		if ((player_flags & FL_ONGROUND) || !FBitSet(accuracy_flags, WPNACCFLAG_AIR))
		{
			if ((player_flags & FL_DUCKING) && FBitSet(accuracy_flags, WPNACCFLAG_DUCK))
			{
				gap = (int)((float)gap * 0.5f);
			}
			else if (CLocalState::the().get_local_velocity_2d() > get_weapon_max_speed_for_crosshair(current_weapon->m_weapon_id) && FBitSet(accuracy_flags, WPNACCFLAG_SPEED))
			{
				gap = (int)((float)gap * 1.5f);
			}
		}
		else
		{
			gap = (int)((float)gap * 2.0f);
		}

		if (FBitSet(accuracy_flags, WPNACCFLAG_MULTIPLY_BY_14))
		{
			gap = (int)((float)gap * 1.4f);
		}

		if (FBitSet(accuracy_flags, WPNACCFLAG_MULTIPLY_BY_14_2))
		{
			gap = (int)((float)gap * 1.4f);
		}
	}

	auto shots_fired_ptr = CMemoryHookMgr::the().g_iShotsFired().get();

	if (m_ammo_last_check >= *shots_fired_ptr)
	{
		if (flip_every_N_ms)
		{
			m_crosshair_gap -= (0.013f * m_crosshair_gap + 0.1f);
		}
		m_alpha += 2;
	}
	else
	{
		if (!crosshair_static.get_value())
		{
			m_crosshair_gap += delta_gap;
			m_crosshair_gap =
				std::min(m_crosshair_gap,
						 use_vanilla_gap ? 15.0f : (crosshair_gap.get_value() * 3.0f));
		}

		m_alpha -= 40;
		m_alpha = std::max(m_alpha, 120);
	}


	if (*shots_fired_ptr > 600)
	{
		*shots_fired_ptr = 1;
	}
	m_ammo_last_check = *shots_fired_ptr;


	if (m_crosshair_gap <= (float)gap)
	{
		m_crosshair_gap = (float)gap;
	}
	if (m_alpha > 255)
	{
		m_alpha = 255;
	}

	int bar_size = (int)((crosshair_size.get_value() == 0) ? ((m_crosshair_gap - (float)gap) / 2.0f) + 5.0f : crosshair_size.get_value());
	float crosshair_dist = m_crosshair_gap;

	CColor cross_color = crosshair_color.get_value();

	int scaled = scaled_base(screen_info.iWidth);
	if (screen_info.iWidth != scaled)
	{
		bar_size = screen_info.iWidth * bar_size / scaled;
		crosshair_dist = screen_info.iWidth * crosshair_dist / scaled;
	}

	int r = (int)(cross_color.r * 255.0f);
	int g = (int)(cross_color.g * 255.0f);
	int b = (int)(cross_color.b * 255.0f);
	int a = m_alpha;

	int w = screen_info.iWidth;
	int h = screen_info.iHeight;

	int type = crosshair_type.get_value();

	auto fillrgba = crosshair_translucent.get_value() ? CMemoryHookMgr::the().cl_enginefuncs()->pfnFillRGBA : CMemoryHookMgr::the().cl_enginefuncs()->pfnFillRGBABlend;

	int t = crosshair_thickness.get_value();
	int t_c = t > 1 ? (t / 2) : 0;

	if (type == 0)
	{
		fillrgba(w / 2 - (int)crosshair_dist - bar_size + 1, h / 2 - t_c, bar_size, t, r, g, b, a);
		fillrgba(w / 2 + (int)crosshair_dist, h / 2 - t_c, bar_size, t, r, g, b, a);
		fillrgba(w / 2 - t_c, h / 2 - (int)crosshair_dist - bar_size + 1, t, bar_size, r, g, b, a);
		fillrgba(w / 2 - t_c, h / 2 + (int)crosshair_dist, t, bar_size, r, g, b, a);
	}
	else if (type == 1)
	{
		fillrgba(w / 2 - (int)crosshair_dist - bar_size + 1, h / 2 - (t / 2), bar_size, t, r, g, b, a);
		fillrgba(w / 2 + (int)crosshair_dist, h / 2 - (t / 2), bar_size, t, r, g, b, a);

		fillrgba(w / 2 - (t / 2), h / 2 + (int)crosshair_dist, t, bar_size, r, g, b, a);
	}
	else if (type == 2)
	{
#if 0
		float step = (std::numbers::pi * 2) / 100;

		for (float angle = 0.0f; angle < std::numbers::pi * 2; angle += step)
		{
			float x = (w / 2) + (10 * cosf(angle));
			float y = (h / 2) + (10 * sinf(angle));

			fillrgba(x, y, 1, 1, r, g, b, a);
		}
#else

		int segments = std::clamp((int)(crosshair_dist / 3.0f), 16, 64);
		CEngineRendering::the().render_circle_opengl((float)(w / 2), (float)(h / 2), crosshair_dist, segments, (float)t, !crosshair_translucent.get_value(), r, g, b, a);
#endif
	}
#if 0
	else if (type == 3)
	{
		auto render_spinner_line = [&](int delta_angle)
		{
			int segments = std::clamp((int)(crosshair_dist / 3), 16, 64);
			float rad1 = 10.0f, rad2 = 20.0f;


			static int ii = 0, ms = 0;
			if (GetTickCount() > ms + 100)
			{
				if (++ii > segments)
					ii = 0;

				ms = GetTickCount();
			}
			float theta = 2.0f * std::numbers::pi * float(ii) / float(segments);

			float x1 = w / 2 + rad1 * cosf(theta + delta_angle);
			float y1 = h / 2 + rad1 * sinf(theta + delta_angle);

			float x2 = w / 2 + rad2 * cosf(theta + delta_angle);
			float y2 = h / 2 + rad2 * sinf(theta + delta_angle);

			CGameUtil::the().render_line_opengl({ x1, y1 }, { x2, y2 }, t, !crosshair_translucent.get_value(), r, g, b, a);
		};

		render_spinner_line(90 * 0);
		render_spinner_line(90 * 1);
		render_spinner_line(90 * 2);
	}
#endif
}
