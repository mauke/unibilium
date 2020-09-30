#ifndef GUARD_UNIBILIUM_H_
#define GUARD_UNIBILIUM_H_

/*

Copyright 2008, 2010-2013, 2015 Lukas Mai.

This file is part of unibilium.

Unibilium is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Unibilium is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with unibilium.  If not, see <http://www.gnu.org/licenses/>.

*/

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

enum unibi_boolean {
    unibi_boolean_begin_,
    unibi_auto_left_margin,
    unibi_auto_right_margin,
    unibi_no_esc_ctlc,
    unibi_ceol_standout_glitch,
    unibi_eat_newline_glitch,
    unibi_erase_overstrike,
    unibi_generic_type,
    unibi_hard_copy,
    unibi_has_meta_key,
    unibi_has_status_line,
    unibi_insert_null_glitch,
    unibi_memory_above,
    unibi_memory_below,
    unibi_move_insert_mode,
    unibi_move_standout_mode,
    unibi_over_strike,
    unibi_status_line_esc_ok,
    unibi_dest_tabs_magic_smso,
    unibi_tilde_glitch,
    unibi_transparent_underline,
    unibi_xon_xoff,
    unibi_needs_xon_xoff,
    unibi_prtr_silent,
    unibi_hard_cursor,
    unibi_non_rev_rmcup,
    unibi_no_pad_char,
    unibi_non_dest_scroll_region,
    unibi_can_change,
    unibi_back_color_erase,
    unibi_hue_lightness_saturation,
    unibi_col_addr_glitch,
    unibi_cr_cancels_micro_mode,
    unibi_has_print_wheel,
    unibi_row_addr_glitch,
    unibi_semi_auto_right_margin,
    unibi_cpi_changes_res,
    unibi_lpi_changes_res,
    unibi_backspaces_with_bs,
    unibi_crt_no_scrolling,
    unibi_no_correctly_working_cr,
    unibi_gnu_has_meta_key,
    unibi_linefeed_is_newline,
    unibi_has_hardware_tabs,
    unibi_return_does_clr_eol,
    unibi_boolean_end_
};

enum unibi_numeric {
    unibi_numeric_begin_ = unibi_boolean_end_,
    unibi_columns,
    unibi_init_tabs,
    unibi_lines,
    unibi_lines_of_memory,
    unibi_magic_cookie_glitch,
    unibi_padding_baud_rate,
    unibi_virtual_terminal,
    unibi_width_status_line,
    unibi_num_labels,
    unibi_label_height,
    unibi_label_width,
    unibi_max_attributes,
    unibi_maximum_windows,
    unibi_max_colors,
    unibi_max_pairs,
    unibi_no_color_video,
    unibi_buffer_capacity,
    unibi_dot_vert_spacing,
    unibi_dot_horz_spacing,
    unibi_max_micro_address,
    unibi_max_micro_jump,
    unibi_micro_col_size,
    unibi_micro_line_size,
    unibi_number_of_pins,
    unibi_output_res_char,
    unibi_output_res_line,
    unibi_output_res_horz_inch,
    unibi_output_res_vert_inch,
    unibi_print_rate,
    unibi_wide_char_size,
    unibi_buttons,
    unibi_bit_image_entwining,
    unibi_bit_image_type,
    unibi_magic_cookie_glitch_ul,
    unibi_carriage_return_delay,
    unibi_new_line_delay,
    unibi_backspace_delay,
    unibi_horizontal_tab_delay,
    unibi_number_of_function_keys,
    unibi_numeric_end_
};

enum unibi_string {
    unibi_string_begin_ = unibi_numeric_end_,
    unibi_back_tab,
    unibi_bell,
    unibi_carriage_return,
    unibi_change_scroll_region,
    unibi_clear_all_tabs,
    unibi_clear_screen,
    unibi_clr_eol,
    unibi_clr_eos,
    unibi_column_address,
    unibi_command_character,
    unibi_cursor_address,
    unibi_cursor_down,
    unibi_cursor_home,
    unibi_cursor_invisible,
    unibi_cursor_left,
    unibi_cursor_mem_address,
    unibi_cursor_normal,
    unibi_cursor_right,
    unibi_cursor_to_ll,
    unibi_cursor_up,
    unibi_cursor_visible,
    unibi_delete_character,
    unibi_delete_line,
    unibi_dis_status_line,
    unibi_down_half_line,
    unibi_enter_alt_charset_mode,
    unibi_enter_blink_mode,
    unibi_enter_bold_mode,
    unibi_enter_ca_mode,
    unibi_enter_delete_mode,
    unibi_enter_dim_mode,
    unibi_enter_insert_mode,
    unibi_enter_secure_mode,
    unibi_enter_protected_mode,
    unibi_enter_reverse_mode,
    unibi_enter_standout_mode,
    unibi_enter_underline_mode,
    unibi_erase_chars,
    unibi_exit_alt_charset_mode,
    unibi_exit_attribute_mode,
    unibi_exit_ca_mode,
    unibi_exit_delete_mode,
    unibi_exit_insert_mode,
    unibi_exit_standout_mode,
    unibi_exit_underline_mode,
    unibi_flash_screen,
    unibi_form_feed,
    unibi_from_status_line,
    unibi_init_1string,
    unibi_init_2string,
    unibi_init_3string,
    unibi_init_file,
    unibi_insert_character,
    unibi_insert_line,
    unibi_insert_padding,
    unibi_key_backspace,
    unibi_key_catab,
    unibi_key_clear,
    unibi_key_ctab,
    unibi_key_dc,
    unibi_key_dl,
    unibi_key_down,
    unibi_key_eic,
    unibi_key_eol,
    unibi_key_eos,
    unibi_key_f0,
    unibi_key_f1,
    unibi_key_f10,
    unibi_key_f2,
    unibi_key_f3,
    unibi_key_f4,
    unibi_key_f5,
    unibi_key_f6,
    unibi_key_f7,
    unibi_key_f8,
    unibi_key_f9,
    unibi_key_home,
    unibi_key_ic,
    unibi_key_il,
    unibi_key_left,
    unibi_key_ll,
    unibi_key_npage,
    unibi_key_ppage,
    unibi_key_right,
    unibi_key_sf,
    unibi_key_sr,
    unibi_key_stab,
    unibi_key_up,
    unibi_keypad_local,
    unibi_keypad_xmit,
    unibi_lab_f0,
    unibi_lab_f1,
    unibi_lab_f10,
    unibi_lab_f2,
    unibi_lab_f3,
    unibi_lab_f4,
    unibi_lab_f5,
    unibi_lab_f6,
    unibi_lab_f7,
    unibi_lab_f8,
    unibi_lab_f9,
    unibi_meta_off,
    unibi_meta_on,
    unibi_newline,
    unibi_pad_char,
    unibi_parm_dch,
    unibi_parm_delete_line,
    unibi_parm_down_cursor,
    unibi_parm_ich,
    unibi_parm_index,
    unibi_parm_insert_line,
    unibi_parm_left_cursor,
    unibi_parm_right_cursor,
    unibi_parm_rindex,
    unibi_parm_up_cursor,
    unibi_pkey_key,
    unibi_pkey_local,
    unibi_pkey_xmit,
    unibi_print_screen,
    unibi_prtr_off,
    unibi_prtr_on,
    unibi_repeat_char,
    unibi_reset_1string,
    unibi_reset_2string,
    unibi_reset_3string,
    unibi_reset_file,
    unibi_restore_cursor,
    unibi_row_address,
    unibi_save_cursor,
    unibi_scroll_forward,
    unibi_scroll_reverse,
    unibi_set_attributes,
    unibi_set_tab,
    unibi_set_window,
    unibi_tab,
    unibi_to_status_line,
    unibi_underline_char,
    unibi_up_half_line,
    unibi_init_prog,
    unibi_key_a1,
    unibi_key_a3,
    unibi_key_b2,
    unibi_key_c1,
    unibi_key_c3,
    unibi_prtr_non,
    unibi_char_padding,
    unibi_acs_chars,
    unibi_plab_norm,
    unibi_key_btab,
    unibi_enter_xon_mode,
    unibi_exit_xon_mode,
    unibi_enter_am_mode,
    unibi_exit_am_mode,
    unibi_xon_character,
    unibi_xoff_character,
    unibi_ena_acs,
    unibi_label_on,
    unibi_label_off,
    unibi_key_beg,
    unibi_key_cancel,
    unibi_key_close,
    unibi_key_command,
    unibi_key_copy,
    unibi_key_create,
    unibi_key_end,
    unibi_key_enter,
    unibi_key_exit,
    unibi_key_find,
    unibi_key_help,
    unibi_key_mark,
    unibi_key_message,
    unibi_key_move,
    unibi_key_next,
    unibi_key_open,
    unibi_key_options,
    unibi_key_previous,
    unibi_key_print,
    unibi_key_redo,
    unibi_key_reference,
    unibi_key_refresh,
    unibi_key_replace,
    unibi_key_restart,
    unibi_key_resume,
    unibi_key_save,
    unibi_key_suspend,
    unibi_key_undo,
    unibi_key_sbeg,
    unibi_key_scancel,
    unibi_key_scommand,
    unibi_key_scopy,
    unibi_key_screate,
    unibi_key_sdc,
    unibi_key_sdl,
    unibi_key_select,
    unibi_key_send,
    unibi_key_seol,
    unibi_key_sexit,
    unibi_key_sfind,
    unibi_key_shelp,
    unibi_key_shome,
    unibi_key_sic,
    unibi_key_sleft,
    unibi_key_smessage,
    unibi_key_smove,
    unibi_key_snext,
    unibi_key_soptions,
    unibi_key_sprevious,
    unibi_key_sprint,
    unibi_key_sredo,
    unibi_key_sreplace,
    unibi_key_sright,
    unibi_key_srsume,
    unibi_key_ssave,
    unibi_key_ssuspend,
    unibi_key_sundo,
    unibi_req_for_input,
    unibi_key_f11,
    unibi_key_f12,
    unibi_key_f13,
    unibi_key_f14,
    unibi_key_f15,
    unibi_key_f16,
    unibi_key_f17,
    unibi_key_f18,
    unibi_key_f19,
    unibi_key_f20,
    unibi_key_f21,
    unibi_key_f22,
    unibi_key_f23,
    unibi_key_f24,
    unibi_key_f25,
    unibi_key_f26,
    unibi_key_f27,
    unibi_key_f28,
    unibi_key_f29,
    unibi_key_f30,
    unibi_key_f31,
    unibi_key_f32,
    unibi_key_f33,
    unibi_key_f34,
    unibi_key_f35,
    unibi_key_f36,
    unibi_key_f37,
    unibi_key_f38,
    unibi_key_f39,
    unibi_key_f40,
    unibi_key_f41,
    unibi_key_f42,
    unibi_key_f43,
    unibi_key_f44,
    unibi_key_f45,
    unibi_key_f46,
    unibi_key_f47,
    unibi_key_f48,
    unibi_key_f49,
    unibi_key_f50,
    unibi_key_f51,
    unibi_key_f52,
    unibi_key_f53,
    unibi_key_f54,
    unibi_key_f55,
    unibi_key_f56,
    unibi_key_f57,
    unibi_key_f58,
    unibi_key_f59,
    unibi_key_f60,
    unibi_key_f61,
    unibi_key_f62,
    unibi_key_f63,
    unibi_clr_bol,
    unibi_clear_margins,
    unibi_set_left_margin,
    unibi_set_right_margin,
    unibi_label_format,
    unibi_set_clock,
    unibi_display_clock,
    unibi_remove_clock,
    unibi_create_window,
    unibi_goto_window,
    unibi_hangup,
    unibi_dial_phone,
    unibi_quick_dial,
    unibi_tone,
    unibi_pulse,
    unibi_flash_hook,
    unibi_fixed_pause,
    unibi_wait_tone,
    unibi_user0,
    unibi_user1,
    unibi_user2,
    unibi_user3,
    unibi_user4,
    unibi_user5,
    unibi_user6,
    unibi_user7,
    unibi_user8,
    unibi_user9,
    unibi_orig_pair,
    unibi_orig_colors,
    unibi_initialize_color,
    unibi_initialize_pair,
    unibi_set_color_pair,
    unibi_set_foreground,
    unibi_set_background,
    unibi_change_char_pitch,
    unibi_change_line_pitch,
    unibi_change_res_horz,
    unibi_change_res_vert,
    unibi_define_char,
    unibi_enter_doublewide_mode,
    unibi_enter_draft_quality,
    unibi_enter_italics_mode,
    unibi_enter_leftward_mode,
    unibi_enter_micro_mode,
    unibi_enter_near_letter_quality,
    unibi_enter_normal_quality,
    unibi_enter_shadow_mode,
    unibi_enter_subscript_mode,
    unibi_enter_superscript_mode,
    unibi_enter_upward_mode,
    unibi_exit_doublewide_mode,
    unibi_exit_italics_mode,
    unibi_exit_leftward_mode,
    unibi_exit_micro_mode,
    unibi_exit_shadow_mode,
    unibi_exit_subscript_mode,
    unibi_exit_superscript_mode,
    unibi_exit_upward_mode,
    unibi_micro_column_address,
    unibi_micro_down,
    unibi_micro_left,
    unibi_micro_right,
    unibi_micro_row_address,
    unibi_micro_up,
    unibi_order_of_pins,
    unibi_parm_down_micro,
    unibi_parm_left_micro,
    unibi_parm_right_micro,
    unibi_parm_up_micro,
    unibi_select_char_set,
    unibi_set_bottom_margin,
    unibi_set_bottom_margin_parm,
    unibi_set_left_margin_parm,
    unibi_set_right_margin_parm,
    unibi_set_top_margin,
    unibi_set_top_margin_parm,
    unibi_start_bit_image,
    unibi_start_char_set_def,
    unibi_stop_bit_image,
    unibi_stop_char_set_def,
    unibi_subscript_characters,
    unibi_superscript_characters,
    unibi_these_cause_cr,
    unibi_zero_motion,
    unibi_char_set_names,
    unibi_key_mouse,
    unibi_mouse_info,
    unibi_req_mouse_pos,
    unibi_get_mouse,
    unibi_set_a_foreground,
    unibi_set_a_background,
    unibi_pkey_plab,
    unibi_device_type,
    unibi_code_set_init,
    unibi_set0_des_seq,
    unibi_set1_des_seq,
    unibi_set2_des_seq,
    unibi_set3_des_seq,
    unibi_set_lr_margin,
    unibi_set_tb_margin,
    unibi_bit_image_repeat,
    unibi_bit_image_newline,
    unibi_bit_image_carriage_return,
    unibi_color_names,
    unibi_define_bit_image_region,
    unibi_end_bit_image_region,
    unibi_set_color_band,
    unibi_set_page_length,
    unibi_display_pc_char,
    unibi_enter_pc_charset_mode,
    unibi_exit_pc_charset_mode,
    unibi_enter_scancode_mode,
    unibi_exit_scancode_mode,
    unibi_pc_term_options,
    unibi_scancode_escape,
    unibi_alt_scancode_esc,
    unibi_enter_horizontal_hl_mode,
    unibi_enter_left_hl_mode,
    unibi_enter_low_hl_mode,
    unibi_enter_right_hl_mode,
    unibi_enter_top_hl_mode,
    unibi_enter_vertical_hl_mode,
    unibi_set_a_attributes,
    unibi_set_pglen_inch,
    unibi_termcap_init2,
    unibi_termcap_reset,
    unibi_linefeed_if_not_lf,
    unibi_backspace_if_not_bs,
    unibi_other_non_function_keys,
    unibi_arrow_key_map,
    unibi_acs_ulcorner,
    unibi_acs_llcorner,
    unibi_acs_urcorner,
    unibi_acs_lrcorner,
    unibi_acs_ltee,
    unibi_acs_rtee,
    unibi_acs_btee,
    unibi_acs_ttee,
    unibi_acs_hline,
    unibi_acs_vline,
    unibi_acs_plus,
    unibi_memory_lock,
    unibi_memory_unlock,
    unibi_box_chars_1,
    unibi_string_end_
};

typedef struct unibi_term unibi_term;

unibi_term *unibi_dummy(void);
unibi_term *unibi_from_mem(const char *, size_t);
void unibi_destroy(unibi_term *);

size_t unibi_dump(const unibi_term *, char *, size_t);

const char *unibi_get_name(const unibi_term *);
void        unibi_set_name(unibi_term *, const char *);

const char **unibi_get_aliases(const unibi_term *);
void         unibi_set_aliases(unibi_term *, const char **);

int  unibi_get_bool(const unibi_term *, enum unibi_boolean);
void unibi_set_bool(unibi_term *, enum unibi_boolean, int);

int  unibi_get_num(const unibi_term *, enum unibi_numeric);
void unibi_set_num(unibi_term *, enum unibi_numeric, int);

const char *unibi_get_str(const unibi_term *, enum unibi_string);
void        unibi_set_str(unibi_term *, enum unibi_string, const char *);

unibi_term *unibi_from_fp(FILE *);
unibi_term *unibi_from_fd(int);
unibi_term *unibi_from_file(const char *);
unibi_term *unibi_from_term(const char *);
unibi_term *unibi_from_env(void);

extern const char *const unibi_terminfo_dirs;

const char *unibi_name_bool(enum unibi_boolean);
const char *unibi_short_name_bool(enum unibi_boolean);
const char *unibi_name_num(enum unibi_numeric);
const char *unibi_short_name_num(enum unibi_numeric);
const char *unibi_name_str(enum unibi_string);
const char *unibi_short_name_str(enum unibi_string);


size_t unibi_count_ext_bool(const unibi_term *);
size_t unibi_count_ext_num(const unibi_term *);
size_t unibi_count_ext_str(const unibi_term *);

int unibi_get_ext_bool(const unibi_term *, size_t);
int unibi_get_ext_num(const unibi_term *, size_t);
const char *unibi_get_ext_str(const unibi_term *, size_t);

void unibi_set_ext_bool(unibi_term *, size_t, int);
void unibi_set_ext_num(unibi_term *, size_t, int);
void unibi_set_ext_str(unibi_term *, size_t, const char *);

const char *unibi_get_ext_bool_name(const unibi_term *, size_t);
const char *unibi_get_ext_num_name(const unibi_term *, size_t);
const char *unibi_get_ext_str_name(const unibi_term *, size_t);

void unibi_set_ext_bool_name(unibi_term *, size_t, const char *);
void unibi_set_ext_num_name(unibi_term *, size_t, const char *);
void unibi_set_ext_str_name(unibi_term *, size_t, const char *);

size_t unibi_add_ext_bool(unibi_term *, const char *, int);
size_t unibi_add_ext_num(unibi_term *, const char *, int);
size_t unibi_add_ext_str(unibi_term *, const char *, const char *);

void unibi_del_ext_bool(unibi_term *, size_t);
void unibi_del_ext_num(unibi_term *, size_t);
void unibi_del_ext_str(unibi_term *, size_t);


typedef struct {
    int i_;
    char *p_;
} unibi_var_t;

unibi_var_t unibi_var_from_num(int);
unibi_var_t unibi_var_from_str(char *);
int         unibi_num_from_var(unibi_var_t);
const char *unibi_str_from_var(unibi_var_t);

void unibi_format(
    unibi_var_t [26],
    unibi_var_t [26],
    const char *,
    unibi_var_t [9],
    void (*)(void *, const char *, size_t),
    void *,
    void (*)(void *, size_t, int, int),
    void *
);

size_t unibi_run(const char *, unibi_var_t [9], char *, size_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GUARD_UNIBILIUM_H_ */
