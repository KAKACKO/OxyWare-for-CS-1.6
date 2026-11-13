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


#include "interface/IDeveloperConsole.h"
#include "interface/gui/IGUIWidgets.h"
#include <interface/gui/IGUIFontManager.h>
#include <interface/gui/IGUIThemeManager.h>
#include <interface/gui/IGUIWindowRendering.h>
#include <interface/IAppdataManager.h>
#include <interface/IVariableManager.h>

#include "tier/StringTools.h"
#include "tier/MessageBox.h"
#include "tier/GenericUtil.h"

#include "BaseVariable.h"

#include <unordered_set>
#include <deque>
#include <map>

VarInteger num_logfiles_to_keep("num_logfiles_to_keep", "Amount of logfiles to keep. Older logfiles will be automatically removed.", 10, 1, 30);

IDeveloperConsole* g_devconsole_i = nullptr;

class CDeveloperConsole : public IDeveloperConsole
{
public: 
	CDeveloperConsole();
	~CDeveloperConsole();

	void initialize(const std::string& module_name);
	void shutdown();
	void render();

	void print(EOutputModule module, EOutputCategory category, const std::string& what, bool printed_already = false, std::chrono::system_clock::time_point time = std::chrono::system_clock::now());

	void flush_screen();

	void register_module(EOutputModule which);
	void unregister_module(EOutputModule which);

	std::filesystem::path get_logfile_path();
	std::filesystem::path get_logfile_path_fancy();

	void enable_tooltip(bool enable)
	{
		m_tooltip_enabled = enable;
	}

private:
	std::string generate_logfilename();

	void prepare_log_directory();

	static int text_edit_callback_stub(ImGuiInputTextCallbackData* data)
	{
		CDeveloperConsole* _this = (CDeveloperConsole*)data->UserData;
		return _this->text_edit_callback(data);
	}

	int text_edit_callback(ImGuiInputTextCallbackData* data);

private:

	inline void print_locally(EOutputCategory category, const std::string& what)
	{
		print(EOutputModule::GUI, category, what);
	}

	inline void print_console(EOutputCategory category, const std::string& what)
	{
		print(EOutputModule::CONSOLE, category, what);
	}

	struct output_line_t
	{
		EOutputCategory		category;
		EOutputModule		module;
		std::string			contents;
		std::chrono::system_clock::time_point time;
	};
	std::string line_preamble(const output_line_t& line);

	void log(const output_line_t& line);

	std::vector<output_line_t> m_contents;
	std::mutex m_output_lock;


	std::unordered_set<EOutputModule> m_module_list;

	bool m_need_to_scroll_to_the_bottom = false;

	std::ofstream m_logfile_output_stream;


	std::string m_module_name;

	std::string m_logfile_path_current;

	bool m_reclaim_focus = false;
	char m_input_buffer[256];


	std::deque<std::string> m_entered_commands;
	int m_history_pos = 0;


	int m_selected_candidate = -1;
	std::vector<std::string> m_tooltip_candidates;
	void rebuild_tooltip_canditate_list(const char* input_buffer, int length);
	void render_candidate_tooltip();

	bool m_tooltip_enabled = false;

	inline static constexpr int k_max_tooltip_candidates = 999;
};

BaseCommand clear(
	"clear", "Clears the console",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		g_devconsole_i->flush_screen();
	}
);

BaseCommand print(
	"print", "<message>", "Prints string to the console",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		if (args.count() <= 1)
		{
			cmd->print_usage();
			return;
		}

		g_devconsole_i->print(EOutputModule::CONSOLE, EOutputCategory::INFO, args.get(1));
	}
);

CDeveloperConsole g_dev_console;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CDeveloperConsole, IDeveloperConsole, IDEVELOPERCONSOLE_INTERFACEID, g_dev_console);


CDeveloperConsole::CDeveloperConsole()
{
	g_devconsole_i = this;


#ifdef M_CHEATER
	initialize(MODULE_CHEAT);
#endif
}

CDeveloperConsole::~CDeveloperConsole()
{
	g_devconsole_i = nullptr;
}

static std::string remove_extension(const std::string& filename) {
	size_t lastdot = filename.find_last_of(".");
	if (lastdot == std::string::npos) return filename;
	return filename.substr(0, lastdot);
}

void CDeveloperConsole::initialize(const std::string& module_name)
{

	m_module_name = remove_extension(module_name);

	prepare_log_directory();

	m_logfile_output_stream = std::ofstream(get_logfile_path(), std::ios_base::app);
	if (m_logfile_output_stream.bad())
	{
		print_locally(EOutputCategory::ERROR, std::format("Failed to open logfile. The path is '{}'", get_logfile_path()));
	}
	else
	{
		m_logfile_output_stream << "// OxyWare console log file\n";
		m_logfile_output_stream << "// \n";
		m_logfile_output_stream << "// version     : " OXVER_STRING "\n";
		m_logfile_output_stream << "// build       : " OX_BUILD "\n";
		m_logfile_output_stream << "// compiled at : " OX_COMPILE_TIMESTAMP "\n";
		m_logfile_output_stream << "// generated by: " << module_name << "\n";
		m_logfile_output_stream << "// \n";
		m_logfile_output_stream << "\n";
		std::flush(m_logfile_output_stream);

		print_locally(EOutputCategory::INFO, std::format("Logging to {}", get_logfile_path()));
	}


	m_entered_commands.push_back("");

	print_locally(EOutputCategory::INFO, "Console Initialized");
}

void CDeveloperConsole::shutdown()
{
	print_locally(EOutputCategory::INFO, "Shutting down console...");

	if (m_logfile_output_stream.good())
	{
		m_logfile_output_stream.close();
	}
}

void CDeveloperConsole::render()
{
	const float footer_height_to_reserve = -32.0f;

	g_gui_widgets_i->add_text("Console", TEXTPROP_None, g_gui_fontmgr_i->get_font(FID_SegoeUI, FSZ_30px, FDC_Regular));

	auto window_size = g_gui_widgets_i->get_current_window_size();
	auto window_pos = g_gui_widgets_i->get_current_window_pos();

	float spacing = 25.0f;
	float offset_from_the_right = 80.0f;
	float y_offset = 15.0f;

	auto info_font = g_gui_fontmgr_i->get_font(FID_ProggyClean, FSZ_13px, FDC_Regular);


	auto label = std::format("{} lines", m_contents.size());
	auto label_size = g_gui_fontmgr_i->calc_font_text_size(info_font, label.c_str());
	g_gui_window_rendering_i->render_text(
		g_gui_window_rendering_i->get_current_drawlist(),
		info_font,
		{ window_pos.x + window_size.x - offset_from_the_right - label_size.x, window_pos.y + y_offset },
		g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_TextBlack>(),
		label);


	auto label1 = std::format("logging to \"{}\"", get_logfile_path_fancy());
	auto label1_size = g_gui_fontmgr_i->calc_font_text_size(info_font, label1.c_str());
	g_gui_window_rendering_i->render_text(
		g_gui_window_rendering_i->get_current_drawlist(),
		info_font,
		{ window_pos.x + window_size.x - offset_from_the_right - label_size.x - spacing - label1_size.x, window_pos.y + y_offset },
		g_gui_thememgr_i->get_current_theme()->get_color<GUICLR_TextBlack>(),
		label1);

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 4.0f, 4.0f });

	g_gui_widgets_i->add_child(
		"console", { 0.0f, footer_height_to_reserve }, true, ImGuiWindowFlags_HorizontalScrollbar, [this]()
		{
			g_gui_widgets_i->render_clipped_contents(
				m_contents.size(),
				[&](int line_no)
				{
					auto& line = m_contents.at(line_no);


					g_gui_widgets_i->push_stylevar(ImGuiStyleVar_ItemSpacing, { 0.0f, 1.0f });
					g_gui_widgets_i->push_stylevar(ImGuiStyleVar_IndentSpacing, 0.0f);
					g_gui_widgets_i->push_font(g_gui_fontmgr_i->get_font(FID_ProggyClean, FSZ_13px, FDC_Regular));

					g_gui_widgets_i->add_text(line_preamble(line), TEXTPROP_ColorRegular);
					g_gui_widgets_i->sameline();
					g_gui_widgets_i->add_text("[", TEXTPROP_ColorRegular);
					g_gui_widgets_i->sameline();
					g_gui_widgets_i->add_colored_text(s_module_color_table[(int)line.module].color, outputmodule_as_string(line.module));
					g_gui_widgets_i->sameline();
					g_gui_widgets_i->add_text("] ", TEXTPROP_ColorRegular);
					g_gui_widgets_i->sameline();


					const char* which[] = { "", "error: ", "warning: " };

					if (line.category != EOutputCategory::INFO)
					{
						g_gui_widgets_i->add_colored_text(s_category_color_table[(int)line.category].color, which[(int)line.category]);
						g_gui_widgets_i->sameline();
					}


					if (line.category != EOutputCategory::INFO)
					{
						g_gui_widgets_i->add_colored_text(s_category_color_table[(int)line.category].color, line.contents);
					}
					else
					{
						g_gui_widgets_i->add_text(line.contents);
					}

					g_gui_widgets_i->pop_font();

					g_gui_widgets_i->pop_stylevar(2);
				});


			if (m_need_to_scroll_to_the_bottom)
			{
				g_gui_widgets_i->set_scroll_here_y(1.0f);
				m_need_to_scroll_to_the_bottom = false;
			}
		});

	g_gui_widgets_i->pop_stylevar(1);

	auto button_size = Vector2D(25, 25);
	auto cursor_pos = g_gui_widgets_i->get_cursor_pos();
	if (g_gui_widgets_i->add_floating_button(";", cursor_pos, { window_size.x - 25.0f - button_size.x, 5.0f }, button_size, false, BUTTONFLAG_CenterLabel))
	{
		CGenericUtil::the().copy_to_clipboard(";");
		print_console(EOutputCategory::INFO, "Copied ';' to clipboard!");
	}

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 4.0f, 4.0f });

	static const ImGuiInputTextFlags input_text_flags = 
		ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | 
		ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackEdit;
	if (g_gui_widgets_i->add_text_input("Enter commands", m_input_buffer, sizeof(m_input_buffer), input_text_flags, true, &text_edit_callback_stub, (void*)this))
	{
		if (m_selected_candidate != -1)
		{

			strcpy(m_input_buffer, m_tooltip_candidates[m_selected_candidate].c_str());
			rebuild_tooltip_canditate_list(m_input_buffer, strlen(m_input_buffer));
		}
		else
		{
			if (m_input_buffer[0])
			{

				g_variablemgr_i->execute_command(m_input_buffer);

				if (m_entered_commands.size() > 20)
				{
					m_entered_commands.pop_front();
				}


				if (std::find(m_entered_commands.begin(), m_entered_commands.end(), m_input_buffer) == m_entered_commands.end())
				{
					m_entered_commands.push_back(m_input_buffer);
				}


				m_history_pos = 0;

				strcpy(m_input_buffer, "");
			}
			else
			{
				print_console(EOutputCategory::INFO, "> ");
			}
		}

		m_reclaim_focus = true;
	}


	g_gui_widgets_i->set_item_default_focus();
	if (m_reclaim_focus)
	{
		g_gui_widgets_i->set_keyboard_focus_here(-1);
		m_reclaim_focus = false;
	}

	render_candidate_tooltip();

	g_gui_widgets_i->pop_stylevar(1);
}

void CDeveloperConsole::print(EOutputModule module, EOutputCategory category, const std::string& what, bool printed_already, std::chrono::system_clock::time_point time)
{
	m_output_lock.lock();

	std::string fmt = what;

	std::string fmt_final;
	for (size_t i = 0; i < fmt.length(); i++)
	{
		unsigned char c = fmt[i];
		if (!std::isprint(c))
		{
			fmt_final += std::format("\\x{:02X}", c);
		}
		else
		{
			fmt_final.push_back(c);
		}
	}

	fmt_final.push_back('\n');

	output_line_t new_line =
	{
		category,
		module,
		fmt_final,
		time
	};

	if (!printed_already)
	{
		OutputDebugStringA(("[ox]" + line_preamble(new_line) + fmt_final).c_str());
	}

	m_contents.push_back(new_line);

	log(new_line);

	m_need_to_scroll_to_the_bottom = true;

	m_output_lock.unlock();
}

void CDeveloperConsole::flush_screen()
{
	size_t size = m_contents.size();
	m_contents.clear();

	print_console(EOutputCategory::INFO, std::format("Cleared {} lines", size));
}

void CDeveloperConsole::register_module(EOutputModule which)
{
	auto [iter, did_insert] = m_module_list.insert(which);
	assert(did_insert);

	print_locally(EOutputCategory::INFO, std::format("DeveloperConsole: Registered '{}' module", outputmodule_as_string(which)));
}

void CDeveloperConsole::unregister_module(EOutputModule which)
{
	auto num_erased = m_module_list.erase(which);
	assert(num_erased == 1);

	print_locally(EOutputCategory::INFO, std::format("DeveloperConsole: Unregistered '{}' module", outputmodule_as_string(which)));
}

#include <shlobj_core.h>

#ifdef small
#undef small
#endif


static std::filesystem::path get_appdata_dir()
{
	PWSTR pwstr_appdata_directory;
	HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &pwstr_appdata_directory);
	assert(SUCCEEDED(result));
	std::filesystem::path ret = pwstr_appdata_directory;
	CoTaskMemFree(pwstr_appdata_directory);
	return ret / "oxware";
}

std::filesystem::path CDeveloperConsole::get_logfile_path()
{
	auto logdir = get_appdata_dir() / m_logfile_path_current;
	auto logfile = logdir / generate_logfilename();
	return logfile;
}

std::filesystem::path CDeveloperConsole::get_logfile_path_fancy()
{
	auto logdir = std::filesystem::path("%appdata%") / m_logfile_path_current;
	auto logfile = logdir / generate_logfilename();
	return logfile;
}

std::string CDeveloperConsole::generate_logfilename()
{
	static std::string generated_filename = "";
	if (!generated_filename.empty())
	{

		return generated_filename;
	}

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, std::format("{}_%d%m%Y_%H%M%S.log", m_module_name).c_str());
	generated_filename = oss.str();
	return generated_filename;
}

void CDeveloperConsole::prepare_log_directory()
{
	print_locally(EOutputCategory::INFO, "Preparing log directories...");

	m_logfile_path_current = "log\\current\\";

	auto base_directory = get_appdata_dir();
	auto current_logdir = get_appdata_dir() / m_logfile_path_current;
	auto logdir = get_appdata_dir() / "log\\";

	std::error_code code;


	if (!std::filesystem::exists(base_directory, code))
	{
		if (!std::filesystem::create_directory(base_directory, code))
		{
			CMessageBox::display_warning("Couldn't create the base cheat directory '{}'.\n\nError message: {}", base_directory, code.message());
			m_logfile_path_current = get_appdata_dir().string();
			return;
		}
	}


	if (!std::filesystem::exists(logdir, code))
	{
		if (!std::filesystem::create_directory(logdir, code))
		{
			CMessageBox::display_warning("Couldn't create directory for logs. The directory was '{}'.\n\nError message: {}", logdir, code.message());
			m_logfile_path_current = get_appdata_dir().string();
			return;
		}
	}


	if (!std::filesystem::exists(current_logdir, code))
	{
		if (!std::filesystem::create_directory(current_logdir, code))
		{
			CMessageBox::display_warning("Couldn't create directory for logs. The directory was '{}'.\n\nError message: {}", current_logdir, code.message());
			m_logfile_path_current = "log\\";
			return;
		}
	}


	for (const auto& iter : std::filesystem::directory_iterator(current_logdir, code))
	{
		std::filesystem::rename(iter.path(), logdir / iter.path().filename(), code);
		
		print_locally(EOutputCategory::INFO, std::format("Moved {} to {}.", iter.path(), logdir));
	}

	print_locally(EOutputCategory::INFO, std::format("Maximum amount of logfiles that will be kept: {}", num_logfiles_to_keep.get_value()));


	std::multimap<std::filesystem::file_time_type, std::filesystem::path> files_to_remove;
	int n = 0;
	for (const auto& iter : std::filesystem::directory_iterator(logdir, code))
	{
		if (n >= num_logfiles_to_keep.get_value()
			&& std::filesystem::is_regular_file(iter.path(), code))
		{
			files_to_remove.insert({ std::filesystem::last_write_time(iter.path(), code), iter.path() });
		}

		n++;
	}


	for (const auto& [time, log_file] : files_to_remove)
	{
		if (!std::filesystem::remove(log_file, code))
		{
			print_locally(EOutputCategory::ERROR, std::format("Failed to remove old log file! '{}'", log_file));
			print_locally(EOutputCategory::ERROR, std::format("Error message: {}", code.message()));
		}
		else
		{
			print_locally(EOutputCategory::INFO, std::format("Removed old log file '{}'", log_file.filename()));
		}
	}

	print_locally(EOutputCategory::INFO, "Done.");
}

int CDeveloperConsole::text_edit_callback(ImGuiInputTextCallbackData* data)
{
	bool history = false;
	
	switch (data->EventFlag)
	{
		case ImGuiInputTextFlags_CallbackCompletion:
		{

			if (m_tooltip_candidates.empty())
			{
				int max_size = (int)(m_entered_commands.size() - 1);
				if (++m_history_pos > max_size)
				{
					m_history_pos = 0;
				}

				history = true;
			}
			else
			{
				int max_size = (int)(m_tooltip_candidates.size() - 1);
				if (++m_selected_candidate > max_size)
				{
					m_selected_candidate = 0;
				}

			}
			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			switch (data->EventKey)
			{
				case ImGuiKey_DownArrow:
				{
					if (m_tooltip_candidates.empty())
					{
						if (--m_history_pos < 0)
						{
							m_history_pos = 0;
						}

						history = true;
					}
					else
					{
						int max_size = (int)(m_tooltip_candidates.size() - 1);
						if (++m_selected_candidate > max_size)
						{
							m_selected_candidate = m_tooltip_candidates.empty() ? 0 : max_size;
						}

					}
					break;
				}
				case ImGuiKey_UpArrow:
				{
					if (m_tooltip_candidates.empty())
					{
						int max_size = (int)(m_entered_commands.size() - 1);
						if (++m_history_pos > max_size)
						{
							m_history_pos = m_entered_commands.empty() ? 0 : max_size;
						}

						history = true;
					}
					else
					{
						if (--m_selected_candidate < 0)
						{
							m_selected_candidate = 0;
						}

					}
					break;
				}
			}

			break;
		}
		case ImGuiInputTextFlags_CallbackEdit:
		{
			rebuild_tooltip_canditate_list(data->Buf, data->BufTextLen);

			m_history_pos = 0;


			break;
		}
	}


	if (history && !m_entered_commands.empty())
	{
		auto& item = m_entered_commands.at(m_history_pos);

		g_gui_widgets_i->delete_chars_on_textinput_buffer(data, 0, data->BufTextLen);
		g_gui_widgets_i->insert_chars_to_textinput_buffer(data, 0, (item + " ").c_str());
	}

	return 0;
}

std::string CDeveloperConsole::line_preamble(const output_line_t& line)
{
	return std::format("[{}]", CStringTools::the().format_timestamp_log(line.time));
}

void CDeveloperConsole::log(const output_line_t& line)
{
	if (m_logfile_output_stream.bad())
	{
		return;
	}

	m_logfile_output_stream << line_preamble(line) << "[" << outputmodule_as_string(line.module) << "]";

	const char* which[] = { "", "error: ", "warning: " };

	if (line.category != EOutputCategory::INFO)
	{
		m_logfile_output_stream << which[(int)line.category];
	}

	m_logfile_output_stream << line.contents;

	std::flush(m_logfile_output_stream);
}

void CDeveloperConsole::rebuild_tooltip_canditate_list(const char* input_buffer, int length)
{
	m_tooltip_candidates.clear();

	g_variablemgr_i->for_each_command(
		[&](BaseCommand* cmd)
		{
			bool match = true;

			for (int k = 0; k < length; k++)
			{
				if (input_buffer[k] != cmd->get_name()[k])
				{
					match = false;
					break;
				}
			}

			if (m_tooltip_candidates.size() <= k_max_tooltip_candidates && match)
			{
				m_tooltip_candidates.push_back(cmd->get_name());
			}
		});

	g_variablemgr_i->for_each_variable(
		[&](BaseVariable* var)
		{
			bool match = true;

			for (int k = 0; k < length; k++)
			{
				if (input_buffer[k] != var->get_name()[k])
				{
					match = false;
					break;
				}
			}

			if (m_tooltip_candidates.size() <= k_max_tooltip_candidates && match)
			{
				m_tooltip_candidates.push_back(std::format("{} {}", var->get_name(), var->get_value_string()));
			}
		});
}

void CDeveloperConsole::render_candidate_tooltip()
{
	if (!m_tooltip_enabled)
	{
		return;
	}

	if (m_input_buffer[0] == 0 || m_tooltip_candidates.empty())
	{
		if (!m_tooltip_candidates.empty())
		{
			m_tooltip_candidates.clear();


		}

		m_selected_candidate = -1;
		return;
	}

	if (m_tooltip_candidates.size() == 1 && m_tooltip_candidates[0] == m_input_buffer)
	{

		m_tooltip_candidates.clear();
	}

	g_gui_widgets_i->set_next_window_pos(g_gui_widgets_i->get_cursor_screen_pos());
	g_gui_widgets_i->set_next_window_size(Vector2D(320, (m_tooltip_candidates.size() * 20.0f) > 150 ? 150 : m_tooltip_candidates.size() * 20.0f));
	g_gui_widgets_i->set_next_window_rounding(5.0f, ImDrawFlags_RoundCornersBottom);

	g_gui_widgets_i->push_font(g_gui_fontmgr_i->get_font(FID_ProggyClean, FSZ_13px, FDC_Regular));
	g_gui_widgets_i->create_new_window(
		"Console tooltip",
		ImGuiWindowFlags_Tooltip |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_HorizontalScrollbar,
		[&]()
		{
			int n = 0;
			for (const auto& c : m_tooltip_candidates)
			{
				if (g_gui_widgets_i->add_selectable(c.empty() ? "##selectable" : c, n == m_selected_candidate))
				{
					strcpy(m_input_buffer, c.c_str());


					m_selected_candidate = -1;
					m_reclaim_focus = true;
					rebuild_tooltip_canditate_list(m_input_buffer, strlen(m_input_buffer));
				}

				n++;
			}
		});
	g_gui_widgets_i->pop_font();
}
