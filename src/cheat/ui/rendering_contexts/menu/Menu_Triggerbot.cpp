#include "precompiled.h"

void MenuChilden::Triggerbot::Settings::contents()
{
	CUIMenuWidgets::the().feature_enabled_section_incommands(
		&CTriggerbot::activation,
		[]()
		{
			CUIMenuWidgets::the().add_description_text("Click the button beside Enable to bind a key.");
			auto* always_active = CTriggerbot::activation.get_always_enabled_var();
			if (CUIMenuWidgets::the().add_checkbox("Always active", always_active,
				"Fire whenever a player is under the crosshair."))
			{
				CTriggerbot::activation.set_is_active(false);
				if (always_active->get_value())
				{
					triggerbot_toggle_bind.set_value(false);
				}
			}
			if (CUIMenuWidgets::the().add_checkbox("Toggle key", &triggerbot_toggle_bind,
				"Press once to turn Triggerbot on, and again to turn it off. Leave unchecked to hold the key."))
			{
				CTriggerbot::activation.set_is_active(false);
				if (triggerbot_toggle_bind.get_value())
				{
					always_active->set_value(false);
				}
			}
			CUIMenuWidgets::the().add_checkbox("Enemy only", &triggerbot_enemy_only);
			CUIMenuWidgets::the().add_checkbox("Head only", &triggerbot_head_only,
				"Uses the player model's head hitbox.");

			g_gui_widgets_i->add_separtor_with_text("Reaction delay");
			CUIMenuWidgets::the().add_slider("Minimum", "%0.0f ms", &triggerbot_delay_min_ms);
			CUIMenuWidgets::the().add_slider("Maximum", "%0.0f ms", &triggerbot_delay_max_ms);
			CUIMenuWidgets::the().add_description_text(
				"A new delay is chosen uniformly between these values each time the crosshair acquires a player. Reversed values are handled automatically.");
		});
}
