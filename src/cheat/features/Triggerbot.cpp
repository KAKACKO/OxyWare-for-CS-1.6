#include "precompiled.h"

VarBoolean triggerbot_toggle_bind("triggerbot_toggle_bind", "Press the bound key to toggle Triggerbot on or off", false);

InCommand CTriggerbot::activation = InCommand("triggerbot", NULL, false, IN_ACTCOND_Alive | IN_ACTCOND_NotInMessageMode,
	[](BaseInCommand* command)
	{
		if (triggerbot_toggle_bind.get_value())
		{
			command->toggle_active();
		}
		else
		{
			command->set_is_active(true);
		}
	},
	[](BaseInCommand* command)
	{
		if (!triggerbot_toggle_bind.get_value())
		{
			command->set_is_active(false);
		}
	});

VarInteger triggerbot_delay_min_ms("triggerbot_delay_min_ms", "Minimum delay after acquiring a player under the crosshair", 80, 0, 1000);
VarInteger triggerbot_delay_max_ms("triggerbot_delay_max_ms", "Maximum delay after acquiring a player under the crosshair", 140, 0, 1000);
VarBoolean triggerbot_head_only("triggerbot_head_only", "Only fire when the crosshair intersects the head hitbox", false);
VarBoolean triggerbot_enemy_only("triggerbot_enemy_only", "Only fire at players on the enemy team", true);

void CTriggerbot::reset_target()
{
	m_target_index = 0;
	m_weapon_id = 0;
	m_acquired_at_ms = 0;
	m_delay_ms = 0;
	m_pistol_release_next = false;
}

void CTriggerbot::capture_head_hitboxes()
{
	if (!triggerbot_head_only.get_value())
	{
		return;
	}

	auto* studio = CMemoryHookMgr::the().engine_studio_api().get();
	if (!studio)
	{
		return;
	}

	auto* entity = studio->GetCurrentEntity();
	if (!entity || !entity->player || entity->index <= 0 || entity->index >= m_head_hitboxes.size())
	{
		return;
	}

	auto* header = CModelChams::the().get_currently_rendered_model_header();
	if (!header || header->numhitboxes <= 0 || header->numhitboxes > 256)
	{
		return;
	}

	auto* hitboxes = reinterpret_cast<hl::mstudiobbox_t*>(reinterpret_cast<byte*>(header) + header->hitboxindex);
	auto* transforms = reinterpret_cast<float(*)[MAXSTUDIOBONES][3][4]>(studio->StudioGetBoneTransform());
	if (!transforms)
	{
		return;
	}

	auto& cache = m_head_hitboxes[entity->index];
	int count = 0;
	for (int i = 0; i < header->numhitboxes && count < cache.boxes.size(); ++i)
	{
		if (hitboxes[i].group != hl::HITGROUP_HEAD || hitboxes[i].bone < 0 || hitboxes[i].bone >= header->numbones || hitboxes[i].bone >= MAXSTUDIOBONES)
		{
			continue;
		}

		auto& box = cache.boxes[count++];
		memcpy(box.transform, (*transforms)[hitboxes[i].bone], sizeof(box.transform));
		box.min = hitboxes[i].bbmin;
		box.max = hitboxes[i].bbmax;
	}

	if (count > 0)
	{
		cache.count = count;
		cache.updated_at_ms = GetTickCount();
	}
}

bool CTriggerbot::ray_hits_head_hitbox(const CrosshairTrace& trace) const
{
	if (trace.player_index <= 0 || trace.player_index >= m_head_hitboxes.size())
	{
		return false;
	}

	const auto& cache = m_head_hitboxes[trace.player_index];
	if (!cache.count || GetTickCount() - cache.updated_at_ms > 250)
	{
		return false;
	}

	for (int box_index = 0; box_index < cache.count; ++box_index)
	{
		const auto& box = cache.boxes[box_index];
		Vector offset = trace.origin - Vector(box.transform[0][3], box.transform[1][3], box.transform[2][3]);
		float near_distance = 0.0f;
		float far_distance = trace.visible_distance;
		bool intersects = true;

		for (int axis = 0; axis < 3; ++axis)
		{
			float start = offset.x * box.transform[0][axis] + offset.y * box.transform[1][axis] + offset.z * box.transform[2][axis];
			float direction = trace.direction.x * box.transform[0][axis] + trace.direction.y * box.transform[1][axis] + trace.direction.z * box.transform[2][axis];
			if (std::abs(direction) < 0.000001f)
			{
				if (start < box.min[axis] || start > box.max[axis])
				{
					intersects = false;
					break;
				}
				continue;
			}

			float t1 = (box.min[axis] - start) / direction;
			float t2 = (box.max[axis] - start) / direction;
			if (t1 > t2)
			{
				std::swap(t1, t2);
			}
			near_distance = std::max(near_distance, t1);
			far_distance = std::min(far_distance, t2);
			if (near_distance > far_distance)
			{
				intersects = false;
				break;
			}
		}

		if (intersects)
		{
			return true;
		}
	}

	return false;
}

CTriggerbot::CrosshairTrace CTriggerbot::trace_player_under_crosshair(const hl::usercmd_t* cmd)
{
	CrosshairTrace result;
	auto* cl = CMemoryHookMgr::the().cl().get();
	auto* engine = CMemoryHookMgr::the().cl_enginefuncs();
	if (!cl || !engine || !engine->pEventAPI)
	{
		return result;
	}

	auto* event_api = engine->pEventAPI;
	CMath::the().angle_vectors(cmd->viewangles, &result.direction, nullptr, nullptr);

	result.origin = CLocalState::the().get_origin() + cl->viewheight;
	Vector end = result.origin + result.direction * 8192.0f;
	hl::pmtrace_t trace = {};
	hl::pmtrace_t world_trace = {};

	event_api->EV_SetUpPlayerPrediction(false, true);
	event_api->EV_PushPMStates();
	event_api->EV_SetSolidPlayers(cl->playernum);
	event_api->EV_SetTraceHull(2);
	event_api->EV_PlayerTrace(result.origin, end, PM_NORMAL, -1, &trace);
	event_api->EV_PlayerTrace(result.origin, end, PM_WORLD_ONLY, -1, &world_trace);
	result.player_index = trace.fraction < 1.0f ? event_api->EV_IndexFromTrace(&trace) : 0;
	event_api->EV_PopPMStates();

	result.hitgroup = trace.hitgroup;
	result.visible_distance = world_trace.fraction * 8192.0f;
	return result;
}

void CTriggerbot::update(hl::usercmd_t* cmd)
{
	if (triggerbot_toggle_bind.get_value() && activation.get_always_enabled_var()->get_value())
	{
		activation.get_always_enabled_var()->set_value(false);
	}

	if (!cmd || !activation.is_active() || !CGameUtil::the().is_fully_connected() ||
		!CLocalState::the().local_player() || !CLocalState::the().player_can_shoot() ||
		COxWareUI::the().should_disable_ingame_input() || CEngineInput::the().is_gameui_rendering() ||
		GetForegroundWindow() != COxWareUI::the().get_window_handle() ||
		CClientMovementPacket::the().is_in(IN_ATTACK))
	{
		reset_target();
		return;
	}

	auto* weapon = CWeapons::the().get_current_weapon();
	if (!weapon || !weapon->is_gun() || weapon->is_reloading() || weapon->is_out_of_ammo())
	{
		reset_target();
		return;
	}

	auto trace = trace_player_under_crosshair(cmd);
	int player_index = trace.player_index;
	if (!CGameUtil::the().is_player_index(player_index) ||
		(triggerbot_head_only.get_value() && trace.hitgroup != hl::HITGROUP_HEAD && !ray_hits_head_hitbox(trace)))
	{
		reset_target();
		return;
	}

	auto player = CEntityMgr::the().get_player_by_id(player_index);
	if (!player || !(*player)->is_valid() || !(*player)->is_alive() ||
		(*player)->is_local_player() || (*player)->is_out_of_update_for(1.0f) ||
		(triggerbot_enemy_only.get_value() && CGameUtil::the().is_player_on_enemy_team(player_index) != 1))
	{
		reset_target();
		return;
	}

	uint32_t now = GetTickCount();
	if (m_target_index != player_index || m_weapon_id != weapon->m_weapon_id)
	{
		m_target_index = player_index;
		m_weapon_id = weapon->m_weapon_id;
		m_acquired_at_ms = now;
		m_pistol_release_next = false;

		int low = std::min(triggerbot_delay_min_ms.get_value(), triggerbot_delay_max_ms.get_value());
		int high = std::max(triggerbot_delay_min_ms.get_value(), triggerbot_delay_max_ms.get_value());
		m_delay_ms = std::uniform_int_distribution<int>(low, high)(m_rng);
	}

	if (now - m_acquired_at_ms < m_delay_ms)
	{
		return;
	}

	if (weapon->is_pistol() && m_pistol_release_next)
	{
		m_pistol_release_next = false;
		return;
	}

	if (!weapon->can_attack())
	{
		return;
	}

	CClientMovementPacket::the().set_button_bit(IN_ATTACK, true);
	if (weapon->is_pistol())
	{
		m_pistol_release_next = true;
	}
}
