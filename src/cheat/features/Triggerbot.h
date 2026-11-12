#ifndef TRIGGERBOT_H
#define TRIGGERBOT_H
#pragma once

#include <random>
#include <array>

extern VarInteger triggerbot_delay_min_ms;
extern VarInteger triggerbot_delay_max_ms;
extern VarBoolean triggerbot_head_only;
extern VarBoolean triggerbot_enemy_only;
extern VarBoolean triggerbot_toggle_bind;

class CTriggerbot
{
public:
	DECL_BASIC_CLASS(CTriggerbot);

	static InCommand activation;

	void update(hl::usercmd_t* cmd);
	void capture_head_hitboxes();

private:
	struct CrosshairTrace
	{
		int player_index = 0;
		int hitgroup = 0;
		Vector origin;
		Vector direction;
		float visible_distance = 0.0f;
	};

	struct HeadHitbox
	{
		float transform[3][4];
		Vector min;
		Vector max;
	};

	struct HeadHitboxCache
	{
		std::array<HeadHitbox, 8> boxes;
		int count = 0;
		uint32_t updated_at_ms = 0;
	};

	void reset_target();
	CrosshairTrace trace_player_under_crosshair(const hl::usercmd_t* cmd);
	bool ray_hits_head_hitbox(const CrosshairTrace& trace) const;

	int m_target_index = 0;
	int m_weapon_id = 0;
	uint32_t m_acquired_at_ms = 0;
	uint32_t m_delay_ms = 0;
	bool m_pistol_release_next = false;
	std::mt19937 m_rng{ std::random_device{}() };
	std::array<HeadHitboxCache, 65> m_head_hitboxes;
};

#endif
