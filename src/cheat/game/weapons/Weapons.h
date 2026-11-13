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

#ifndef _WEAPONS_H
#define _WEAPONS_H
#pragma once

struct wpn_name_t
{
	const char* name, *model_name;
};

static constexpr wpn_name_t g_weapon_names[MAX_WEAPONS] =
{
	{ "none",			"none" },
	{ "p228",			"models/v_p228.mdl" },
	{ "glock",			"models/v_glock18.mdl" },
	{ "scout",			"models/v_scout.mdl" },
	{ "hegrenade",		"models/v_hegrenade.mdl" },
	{ "xm1014",			"models/v_xm1014.mdl" },
	{ "c4",				"models/v_c4.mdl" },
	{ "mac10",			"models/v_mac10.mdl" },
	{ "aug",			"models/v_aug.mdl" },
	{ "smokegrenade",	"models/v_smokegrenade.mdl" },
	{ "elite", 			"models/v_elite.mdl" },
	{ "fiveseven",		"models/v_fiveseven.mdl" },
	{ "ump45", 			"models/v_ump45.mdl" },
	{ "sg550", 			"models/v_sg550.mdl" },
	{ "galil",			"models/v_galil.mdl" },
	{ "famas", 			"models/v_famas.mdl" },
	{ "usp", 			"models/v_usp.mdl" },
	{ "glock18",		"models/v_glock18.mdl" },
	{ "awp", 			"models/v_awp.mdl" },
	{ "mp5n", 			"models/v_mp5.mdl" },
	{ "m249", 			"models/v_m249.mdl" },
	{ "m3",				"models/v_m3.mdl" },
	{ "m4a1", 			"models/v_m4a1.mdl" },
	{ "tmp", 			"models/v_tmp.mdl" },
	{ "g3sg1",			"models/v_g3sg1.mdl" },
	{ "flashbang", 		"models/v_flashbang.mdl" },
	{ "deagle", 		"models/v_deagle.mdl" },
	{ "sg552",			"models/v_sg552.mdl" },
	{ "ak47", 			"models/v_ak47.mdl" },
	{ "knife", 			"models/v_knife.mdl" },
	{ "p90",			"models/v_p90.mdl" }
};

class CCSWeapon
{
public:
	CCSWeapon(WeaponIdType weapon_id, EWeaponClassType weapon_class, EAmmoType ammo, EBulletType bullet_type,
			  int clip_size, int weight, int price,
			  int primary_damage, int secondary_damage,
			  float max_speed, float reload_time, EWallPenetrationStrength wall_pen_strength,
			  float max_distance, float range_modifier, 
			  

			  bool is_nonattack, bool is_sniper, bool is_pistol, bool is_shotgun, bool is_machinegun,
			  bool is_submachinegun, bool is_rifle, bool is_knife, bool is_gun, bool is_nade, bool is_c4);

	CCSWeapon() = default;

	void update(hl::local_state_t* state, hl::usercmd_t* cmd);


	inline bool is_automatic_fire() const { return !is_class_type(WEAPONCLASS_PISTOL); }

	inline bool is_class_type(EWeaponClassType t) const { return m_weapon_class == t; }

	bool is_silenced() const;
	bool is_in_burst() const;
	bool can_attack() const;
	bool has_secondary_attack() const;


	inline bool state_present(EWeaponState st) const
	{
		return m_weapon_data ? FBitSet(m_weapon_data->m_iWeaponState, st) : false;
	}

	inline int get_current_clip() const
	{
		return m_weapon_data ? m_weapon_data->m_iClip : 0;
	}

	inline bool is_out_of_ammo() const
	{
		return get_current_clip() < 1;
	}

	inline int get_damage() const
	{
		if (is_silenced() || is_in_burst())
		{
			return m_secondary_damage;
		}

		return m_primary_damage;
	}


	inline bool is_nonattack() const { return m_is_nonattack; }
	inline bool is_rifle() const { return m_is_rifle; }
	inline bool is_sniper() const { return m_is_sniper; }
	inline bool is_pistol() const { return m_is_pistol; }
	inline bool is_shotgun() const { return m_is_shotgun; }
	inline bool is_machinegun() const { return m_is_machinegun; }
	inline bool is_submachinegun() const { return m_is_submachinegun; }
	inline bool is_knife() const { return m_is_knife; }
	inline bool is_gun() const { return m_is_gun; }
	inline bool is_nade() const { return m_is_nade; }
	inline bool is_c4() const { return m_is_c4; }

	inline bool is_reloading() const { return m_weapon_data ? m_weapon_data->m_fInReload : false; }
	inline float next_primary_attack_delay() const { return m_weapon_data ? m_weapon_data->m_flNextPrimaryAttack : 0.0f; }
	inline float next_secondary_attack_delay() const { return m_weapon_data ? m_weapon_data->m_flNextSecondaryAttack : 0.0f; }

	inline float get_accuracy() const { return m_cs_weapon ? m_cs_weapon->m_flAccuracy : 0.0f; }
	inline int get_shots_fired() const { return m_cs_weapon ? m_cs_weapon->m_iShotsFired : 0; }

	float time_since_last_fired() const;


	bool is_useable() const;

	EWeaponAccuracyFlags calc_wpn_accuracy_flags() const;

private:
	void calc_spread();
	
public:
	WeaponIdType m_weapon_id;


	EWeaponClassType m_weapon_class;


	EAmmoType m_ammo_type;


	EBulletType m_bullet_type;


	std::string m_name, m_model_name;


	int m_clip_size;
	int m_weight;
	int m_price;


	int m_primary_damage;
	int m_secondary_damage;


	float m_max_speed;


	float m_reload_time;


	EWallPenetrationStrength m_wall_penetration_strength;


	float m_max_distance;


	float m_range_modifier;


	float m_last_time_fired, m_decrease_shots_fired_time;


	float m_spread;


	hl::local_state_t* m_local_state;
	hl::weapon_data_t* m_weapon_data;
	hl::usercmd_t* m_cmd;
	hl::CBasePlayerWeapon* m_cs_weapon;

private:

	bool m_is_nonattack;
	bool m_is_rifle;
	bool m_is_sniper;
	bool m_is_pistol;
	bool m_is_shotgun;
	bool m_is_machinegun;
	bool m_is_submachinegun;
	bool m_is_knife;
	bool m_is_gun;
	bool m_is_nade;
	bool m_is_c4;
};


class CWeapons
{
public:
	DECL_BASIC_CLASS(CWeapons);

public:
	void update(hl::local_state_t* state, hl::usercmd_t* cmd);

	CCSWeapon* get_current_weapon()
	{
		return &s_weapons[m_current_weapon_id];
	}
private:
	static std::array<CCSWeapon, MAX_WEAPONS> s_weapons;

	bool m_initialized = false;

	WeaponIdType m_current_weapon_id;
};


#endif