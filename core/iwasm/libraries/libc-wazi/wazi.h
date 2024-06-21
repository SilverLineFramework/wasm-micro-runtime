#ifndef _WAZI_H_
#define _WAZI_H_

#include <zephyr/sys/libc-hooks.h>
#include <stdio.h>

#include "wasm_export.h"
#include "bh_platform.h"
#include "wazi_defs.h"

#define VB(fmt, ...) LOG_VERBOSE("WAZI: " fmt, gettid(), ## __VA_ARGS__)
#define WARN(fmt, ...)  LOG_WARNING("WAZI: " fmt, ## __VA_ARGS__)
#define ERR(fmt, ...) LOG_ERROR("WAZI: " fmt, ## __VA_ARGS__)

/** Memory defines/translations **/
typedef uint8_t* Addr;
typedef uint32_t FuncPtr_t;

#define BASE_ADDR() ({  \
  (Addr) wasm_runtime_addr_app_to_native(get_module_inst(exec_env), 0); \
})

#define MADDR(wasm_addr) ({  \
  Addr addr = wasm_addr ? (Addr) wasm_runtime_addr_app_to_native(get_module_inst(exec_env), wasm_addr) : NULL;  \
  if (addr == NULL) { }  \
  addr; \
})

#define WADDR(mem_addr) ({  \
  wasm_runtime_addr_native_to_app(get_module_inst(exec_env), mem_addr); \
})

#define ERROR_PTR NULL
#define ERROR_FN_PTR NULL
/** **/


/** Syscalls **/
// [0] : adc_channel_setup 
int wazi_syscall_adc_channel_setup (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [1] : adc_read 
int wazi_syscall_adc_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [2] : adc_read_async 
int wazi_syscall_adc_read_async (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [3] : atomic_add 
int32_t wazi_syscall_atomic_add (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [4] : atomic_and 
int32_t wazi_syscall_atomic_and (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [5] : atomic_cas 
bool wazi_syscall_atomic_cas (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [6] : atomic_nand 
int32_t wazi_syscall_atomic_nand (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [7] : atomic_or 
int32_t wazi_syscall_atomic_or (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [8] : atomic_ptr_cas 
bool wazi_syscall_atomic_ptr_cas (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [9] : atomic_ptr_set 
int32_t wazi_syscall_atomic_ptr_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [10] : atomic_set 
int32_t wazi_syscall_atomic_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [11] : atomic_sub 
int32_t wazi_syscall_atomic_sub (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [12] : atomic_xor 
int32_t wazi_syscall_atomic_xor (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [13] : auxdisplay_backlight_get 
int wazi_syscall_auxdisplay_backlight_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [14] : auxdisplay_backlight_set 
int wazi_syscall_auxdisplay_backlight_set (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2);
// [15] : auxdisplay_brightness_get 
int wazi_syscall_auxdisplay_brightness_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [16] : auxdisplay_brightness_set 
int wazi_syscall_auxdisplay_brightness_set (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2);
// [17] : auxdisplay_capabilities_get 
int wazi_syscall_auxdisplay_capabilities_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [18] : auxdisplay_clear 
int wazi_syscall_auxdisplay_clear (wasm_exec_env_t exec_env, int32_t sca1);
// [19] : auxdisplay_cursor_position_get 
int wazi_syscall_auxdisplay_cursor_position_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [20] : auxdisplay_cursor_position_set 
int wazi_syscall_auxdisplay_cursor_position_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int16_t sca3, int16_t sca4);
// [21] : auxdisplay_cursor_set_enabled 
int wazi_syscall_auxdisplay_cursor_set_enabled (wasm_exec_env_t exec_env, int32_t sca1, bool sca2);
// [22] : auxdisplay_cursor_shift_set 
int wazi_syscall_auxdisplay_cursor_shift_set (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, bool sca3);
// [23] : auxdisplay_custom_character_set 
int wazi_syscall_auxdisplay_custom_character_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [24] : auxdisplay_custom_command 
int wazi_syscall_auxdisplay_custom_command (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [25] : auxdisplay_display_off 
int wazi_syscall_auxdisplay_display_off (wasm_exec_env_t exec_env, int32_t sca1);
// [26] : auxdisplay_display_on 
int wazi_syscall_auxdisplay_display_on (wasm_exec_env_t exec_env, int32_t sca1);
// [27] : auxdisplay_display_position_get 
int wazi_syscall_auxdisplay_display_position_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [28] : auxdisplay_display_position_set 
int wazi_syscall_auxdisplay_display_position_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int16_t sca3, int16_t sca4);
// [29] : auxdisplay_is_busy 
int wazi_syscall_auxdisplay_is_busy (wasm_exec_env_t exec_env, int32_t sca1);
// [30] : auxdisplay_position_blinking_set_enabled 
int wazi_syscall_auxdisplay_position_blinking_set_enabled (wasm_exec_env_t exec_env, int32_t sca1, bool sca2);
// [31] : auxdisplay_write 
int wazi_syscall_auxdisplay_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint16_t sca3);
// [32] : bbram_check_invalid 
int wazi_syscall_bbram_check_invalid (wasm_exec_env_t exec_env, int32_t sca1);
// [33] : bbram_check_power 
int wazi_syscall_bbram_check_power (wasm_exec_env_t exec_env, int32_t sca1);
// [34] : bbram_check_standby_power 
int wazi_syscall_bbram_check_standby_power (wasm_exec_env_t exec_env, int32_t sca1);
// [35] : bbram_get_size 
int wazi_syscall_bbram_get_size (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [36] : bbram_read 
int wazi_syscall_bbram_read (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, int32_t sca4);
// [37] : bbram_write 
int wazi_syscall_bbram_write (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, int32_t sca4);
// [38] : bc12_set_result_cb 
int wazi_syscall_bc12_set_result_cb (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [39] : bc12_set_role 
int wazi_syscall_bc12_set_role (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [40] : can_add_rx_filter_msgq 
int wazi_syscall_can_add_rx_filter_msgq (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [41] : can_calc_timing 
int wazi_syscall_can_calc_timing (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, uint16_t sca4);
// [42] : can_calc_timing_data 
int wazi_syscall_can_calc_timing_data (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, uint16_t sca4);
// [43] : can_get_capabilities 
int wazi_syscall_can_get_capabilities (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [44] : can_get_core_clock 
int wazi_syscall_can_get_core_clock (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [45] : can_get_max_bitrate 
int wazi_syscall_can_get_max_bitrate (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [46] : can_get_max_filters 
int wazi_syscall_can_get_max_filters (wasm_exec_env_t exec_env, int32_t sca1, bool sca2);
// [47] : can_get_mode 
uint32_t wazi_syscall_can_get_mode (wasm_exec_env_t exec_env, int32_t sca1);
// [48] : can_get_state 
int wazi_syscall_can_get_state (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [49] : can_get_timing_data_max 
int32_t wazi_syscall_can_get_timing_data_max (wasm_exec_env_t exec_env, int32_t sca1);
// [50] : can_get_timing_data_min 
int32_t wazi_syscall_can_get_timing_data_min (wasm_exec_env_t exec_env, int32_t sca1);
// [51] : can_get_timing_max 
int32_t wazi_syscall_can_get_timing_max (wasm_exec_env_t exec_env, int32_t sca1);
// [52] : can_get_timing_min 
int32_t wazi_syscall_can_get_timing_min (wasm_exec_env_t exec_env, int32_t sca1);
// [53] : can_get_transceiver 
int32_t wazi_syscall_can_get_transceiver (wasm_exec_env_t exec_env, int32_t sca1);
// [54] : can_recover 
int wazi_syscall_can_recover (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [55] : can_remove_rx_filter 
void wazi_syscall_can_remove_rx_filter (wasm_exec_env_t exec_env, int32_t sca1, int sca2);
// [56] : can_send 
int wazi_syscall_can_send (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4, int32_t sca5);
// [57] : can_set_bitrate 
int wazi_syscall_can_set_bitrate (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [58] : can_set_bitrate_data 
int wazi_syscall_can_set_bitrate_data (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [59] : can_set_mode 
int wazi_syscall_can_set_mode (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [60] : can_set_timing 
int wazi_syscall_can_set_timing (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [61] : can_set_timing_data 
int wazi_syscall_can_set_timing_data (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [62] : can_start 
int wazi_syscall_can_start (wasm_exec_env_t exec_env, int32_t sca1);
// [63] : can_stats_get_ack_errors 
uint32_t wazi_syscall_can_stats_get_ack_errors (wasm_exec_env_t exec_env, int32_t sca1);
// [64] : can_stats_get_bit0_errors 
uint32_t wazi_syscall_can_stats_get_bit0_errors (wasm_exec_env_t exec_env, int32_t sca1);
// [65] : can_stats_get_bit1_errors 
uint32_t wazi_syscall_can_stats_get_bit1_errors (wasm_exec_env_t exec_env, int32_t sca1);
// [66] : can_stats_get_bit_errors 
uint32_t wazi_syscall_can_stats_get_bit_errors (wasm_exec_env_t exec_env, int32_t sca1);
// [67] : can_stats_get_crc_errors 
uint32_t wazi_syscall_can_stats_get_crc_errors (wasm_exec_env_t exec_env, int32_t sca1);
// [68] : can_stats_get_form_errors 
uint32_t wazi_syscall_can_stats_get_form_errors (wasm_exec_env_t exec_env, int32_t sca1);
// [69] : can_stats_get_rx_overruns 
uint32_t wazi_syscall_can_stats_get_rx_overruns (wasm_exec_env_t exec_env, int32_t sca1);
// [70] : can_stats_get_stuff_errors 
uint32_t wazi_syscall_can_stats_get_stuff_errors (wasm_exec_env_t exec_env, int32_t sca1);
// [71] : can_stop 
int wazi_syscall_can_stop (wasm_exec_env_t exec_env, int32_t sca1);
// [72] : charger_charge_enable 
int wazi_syscall_charger_charge_enable (wasm_exec_env_t exec_env, int32_t sca1, bool sca2);
// [73] : charger_get_prop 
int wazi_syscall_charger_get_prop (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3);
// [74] : charger_set_prop 
int wazi_syscall_charger_set_prop (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3);
// [75] : counter_cancel_channel_alarm 
int wazi_syscall_counter_cancel_channel_alarm (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2);
// [76] : counter_get_frequency 
uint32_t wazi_syscall_counter_get_frequency (wasm_exec_env_t exec_env, int32_t sca1);
// [77] : counter_get_guard_period 
uint32_t wazi_syscall_counter_get_guard_period (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [78] : counter_get_max_top_value 
uint32_t wazi_syscall_counter_get_max_top_value (wasm_exec_env_t exec_env, int32_t sca1);
// [79] : counter_get_num_of_channels 
uint8_t wazi_syscall_counter_get_num_of_channels (wasm_exec_env_t exec_env, int32_t sca1);
// [80] : counter_get_pending_int 
int wazi_syscall_counter_get_pending_int (wasm_exec_env_t exec_env, int32_t sca1);
// [81] : counter_get_top_value 
uint32_t wazi_syscall_counter_get_top_value (wasm_exec_env_t exec_env, int32_t sca1);
// [82] : counter_get_value 
int wazi_syscall_counter_get_value (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [83] : counter_get_value_64 
int wazi_syscall_counter_get_value_64 (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [84] : counter_is_counting_up 
bool wazi_syscall_counter_is_counting_up (wasm_exec_env_t exec_env, int32_t sca1);
// [85] : counter_set_channel_alarm 
int wazi_syscall_counter_set_channel_alarm (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, int32_t sca3);
// [86] : counter_set_guard_period 
int wazi_syscall_counter_set_guard_period (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3);
// [87] : counter_set_top_value 
int wazi_syscall_counter_set_top_value (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [88] : counter_start 
int wazi_syscall_counter_start (wasm_exec_env_t exec_env, int32_t sca1);
// [89] : counter_stop 
int wazi_syscall_counter_stop (wasm_exec_env_t exec_env, int32_t sca1);
// [90] : counter_ticks_to_us 
uint64_t wazi_syscall_counter_ticks_to_us (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [91] : counter_us_to_ticks 
uint32_t wazi_syscall_counter_us_to_ticks (wasm_exec_env_t exec_env, int32_t sca1, uint64_t sca2);
// [92] : dac_channel_setup 
int wazi_syscall_dac_channel_setup (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [93] : dac_write_value 
int wazi_syscall_dac_write_value (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint32_t sca3);
// [94] : device_get_binding 
int32_t wazi_syscall_device_get_binding (wasm_exec_env_t exec_env, int32_t sca1);
// [95] : device_is_ready 
bool wazi_syscall_device_is_ready (wasm_exec_env_t exec_env, int32_t sca1);
// [96] : devmux_select_get 
uint32_t wazi_syscall_devmux_select_get (wasm_exec_env_t exec_env, int32_t sca1);
// [97] : devmux_select_set 
int wazi_syscall_devmux_select_set (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [98] : dma_chan_filter 
int wazi_syscall_dma_chan_filter (wasm_exec_env_t exec_env, int32_t sca1, int sca2, int32_t sca3);
// [99] : dma_release_channel 
void wazi_syscall_dma_release_channel (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [100] : dma_request_channel 
int wazi_syscall_dma_request_channel (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [101] : dma_resume 
int wazi_syscall_dma_resume (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [102] : dma_start 
int wazi_syscall_dma_start (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [103] : dma_stop 
int wazi_syscall_dma_stop (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [104] : dma_suspend 
int wazi_syscall_dma_suspend (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [105] : eeprom_get_size 
uint32_t wazi_syscall_eeprom_get_size (wasm_exec_env_t exec_env, int32_t sca1);
// [106] : eeprom_read 
int wazi_syscall_eeprom_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [107] : eeprom_write 
int wazi_syscall_eeprom_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [108] : emul_fuel_gauge_is_battery_cutoff 
int wazi_syscall_emul_fuel_gauge_is_battery_cutoff (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [109] : emul_fuel_gauge_set_battery_charging 
int wazi_syscall_emul_fuel_gauge_set_battery_charging (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int sca3);
// [110] : entropy_get_entropy 
int wazi_syscall_entropy_get_entropy (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint16_t sca3);
// [111] : espi_config 
int wazi_syscall_espi_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [112] : espi_flash_erase 
int wazi_syscall_espi_flash_erase (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [113] : espi_get_channel_status 
bool wazi_syscall_espi_get_channel_status (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [114] : espi_read_flash 
int wazi_syscall_espi_read_flash (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [115] : espi_read_lpc_request 
int wazi_syscall_espi_read_lpc_request (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [116] : espi_read_request 
int wazi_syscall_espi_read_request (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [117] : espi_receive_oob 
int wazi_syscall_espi_receive_oob (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [118] : espi_receive_vwire 
int wazi_syscall_espi_receive_vwire (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [119] : espi_saf_activate 
int wazi_syscall_espi_saf_activate (wasm_exec_env_t exec_env, int32_t sca1);
// [120] : espi_saf_config 
int wazi_syscall_espi_saf_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [121] : espi_saf_flash_erase 
int wazi_syscall_espi_saf_flash_erase (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [122] : espi_saf_flash_read 
int wazi_syscall_espi_saf_flash_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [123] : espi_saf_flash_unsuccess 
int wazi_syscall_espi_saf_flash_unsuccess (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [124] : espi_saf_flash_write 
int wazi_syscall_espi_saf_flash_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [125] : espi_saf_get_channel_status 
bool wazi_syscall_espi_saf_get_channel_status (wasm_exec_env_t exec_env, int32_t sca1);
// [126] : espi_saf_set_protection_regions 
int wazi_syscall_espi_saf_set_protection_regions (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [127] : espi_send_oob 
int wazi_syscall_espi_send_oob (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [128] : espi_send_vwire 
int wazi_syscall_espi_send_vwire (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint8_t sca3);
// [129] : espi_write_flash 
int wazi_syscall_espi_write_flash (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [130] : espi_write_lpc_request 
int wazi_syscall_espi_write_lpc_request (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [131] : espi_write_request 
int wazi_syscall_espi_write_request (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [132] : flash_erase 
int wazi_syscall_flash_erase (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [133] : flash_ex_op 
int wazi_syscall_flash_ex_op (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint32_t sca3, int32_t sca4);
// [134] : flash_get_page_count 
uint32_t wazi_syscall_flash_get_page_count (wasm_exec_env_t exec_env, int32_t sca1);
// [135] : flash_get_page_info_by_idx 
int wazi_syscall_flash_get_page_info_by_idx (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3);
// [136] : flash_get_page_info_by_offs 
int wazi_syscall_flash_get_page_info_by_offs (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [137] : flash_get_parameters 
int32_t wazi_syscall_flash_get_parameters (wasm_exec_env_t exec_env, int32_t sca1);
// [138] : flash_get_write_block_size 
uint32_t wazi_syscall_flash_get_write_block_size (wasm_exec_env_t exec_env, int32_t sca1);
// [139] : flash_read 
int wazi_syscall_flash_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [140] : flash_read_jedec_id 
int wazi_syscall_flash_read_jedec_id (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [141] : flash_sfdp_read 
int wazi_syscall_flash_sfdp_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [142] : flash_simulator_get_memory 
int32_t wazi_syscall_flash_simulator_get_memory (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [143] : flash_write 
int wazi_syscall_flash_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [144] : fuel_gauge_battery_cutoff 
int wazi_syscall_fuel_gauge_battery_cutoff (wasm_exec_env_t exec_env, int32_t sca1);
// [145] : fuel_gauge_get_buffer_prop 
int wazi_syscall_fuel_gauge_get_buffer_prop (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3, uint32_t sca4);
// [146] : fuel_gauge_get_prop 
int wazi_syscall_fuel_gauge_get_prop (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3);
// [147] : fuel_gauge_get_props 
int wazi_syscall_fuel_gauge_get_props (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [148] : fuel_gauge_set_prop 
int wazi_syscall_fuel_gauge_set_prop (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3);
// [149] : fuel_gauge_set_props 
int wazi_syscall_fuel_gauge_set_props (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [150] : gnss_get_enabled_systems 
int wazi_syscall_gnss_get_enabled_systems (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [151] : gnss_get_fix_rate 
int wazi_syscall_gnss_get_fix_rate (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [152] : gnss_get_navigation_mode 
int wazi_syscall_gnss_get_navigation_mode (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [153] : gnss_get_periodic_config 
int wazi_syscall_gnss_get_periodic_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [154] : gnss_get_supported_systems 
int wazi_syscall_gnss_get_supported_systems (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [155] : gnss_set_enabled_systems 
int wazi_syscall_gnss_set_enabled_systems (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [156] : gnss_set_fix_rate 
int wazi_syscall_gnss_set_fix_rate (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [157] : gnss_set_navigation_mode 
int wazi_syscall_gnss_set_navigation_mode (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [158] : gnss_set_periodic_config 
int wazi_syscall_gnss_set_periodic_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [159] : gpio_get_pending_int 
int wazi_syscall_gpio_get_pending_int (wasm_exec_env_t exec_env, int32_t sca1);
// [160] : gpio_pin_configure 
int wazi_syscall_gpio_pin_configure (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint32_t sca3);
// [161] : gpio_pin_get_config 
int wazi_syscall_gpio_pin_get_config (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, int32_t sca3);
// [162] : gpio_pin_interrupt_configure 
int wazi_syscall_gpio_pin_interrupt_configure (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint32_t sca3);
// [163] : gpio_port_clear_bits_raw 
int wazi_syscall_gpio_port_clear_bits_raw (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [164] : gpio_port_get_direction 
int wazi_syscall_gpio_port_get_direction (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3, int32_t sca4);
// [165] : gpio_port_get_raw 
int wazi_syscall_gpio_port_get_raw (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [166] : gpio_port_set_bits_raw 
int wazi_syscall_gpio_port_set_bits_raw (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [167] : gpio_port_set_masked_raw 
int wazi_syscall_gpio_port_set_masked_raw (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3);
// [168] : gpio_port_toggle_bits 
int wazi_syscall_gpio_port_toggle_bits (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [169] : hwinfo_clear_reset_cause 
int wazi_syscall_hwinfo_clear_reset_cause (wasm_exec_env_t exec_env);
// [170] : hwinfo_get_device_id 
uint32_t wazi_syscall_hwinfo_get_device_id (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [171] : hwinfo_get_reset_cause 
int wazi_syscall_hwinfo_get_reset_cause (wasm_exec_env_t exec_env, int32_t sca1);
// [172] : hwinfo_get_supported_reset_cause 
int wazi_syscall_hwinfo_get_supported_reset_cause (wasm_exec_env_t exec_env, int32_t sca1);
// [173] : hwspinlock_get_max_id 
uint32_t wazi_syscall_hwspinlock_get_max_id (wasm_exec_env_t exec_env, int32_t sca1);
// [174] : hwspinlock_lock 
void wazi_syscall_hwspinlock_lock (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [175] : hwspinlock_trylock 
int wazi_syscall_hwspinlock_trylock (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [176] : hwspinlock_unlock 
void wazi_syscall_hwspinlock_unlock (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [177] : i2c_configure 
int wazi_syscall_i2c_configure (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [178] : i2c_get_config 
int wazi_syscall_i2c_get_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [179] : i2c_recover_bus 
int wazi_syscall_i2c_recover_bus (wasm_exec_env_t exec_env, int32_t sca1);
// [180] : i2c_target_driver_register 
int wazi_syscall_i2c_target_driver_register (wasm_exec_env_t exec_env, int32_t sca1);
// [181] : i2c_target_driver_unregister 
int wazi_syscall_i2c_target_driver_unregister (wasm_exec_env_t exec_env, int32_t sca1);
// [182] : i2c_transfer 
int wazi_syscall_i2c_transfer (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint8_t sca3, uint16_t sca4);
// [183] : i2s_buf_read 
int wazi_syscall_i2s_buf_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [184] : i2s_buf_write 
int wazi_syscall_i2s_buf_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [185] : i2s_configure 
int wazi_syscall_i2s_configure (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [186] : i2s_trigger 
int wazi_syscall_i2s_trigger (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [187] : i3c_do_ccc 
int wazi_syscall_i3c_do_ccc (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [188] : i3c_transfer 
int wazi_syscall_i3c_transfer (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint8_t sca3);
// [189] : ipm_complete 
void wazi_syscall_ipm_complete (wasm_exec_env_t exec_env, int32_t sca1);
// [190] : ipm_max_data_size_get 
int wazi_syscall_ipm_max_data_size_get (wasm_exec_env_t exec_env, int32_t sca1);
// [191] : ipm_max_id_val_get 
uint32_t wazi_syscall_ipm_max_id_val_get (wasm_exec_env_t exec_env, int32_t sca1);
// [192] : ipm_send 
int wazi_syscall_ipm_send (wasm_exec_env_t exec_env, int32_t sca1, int sca2, uint32_t sca3, int32_t sca4, int sca5);
// [193] : ipm_set_enabled 
int wazi_syscall_ipm_set_enabled (wasm_exec_env_t exec_env, int32_t sca1, int sca2);
// [194] : ivshmem_enable_interrupts 
int wazi_syscall_ivshmem_enable_interrupts (wasm_exec_env_t exec_env, int32_t sca1, bool sca2);
// [195] : ivshmem_get_id 
uint32_t wazi_syscall_ivshmem_get_id (wasm_exec_env_t exec_env, int32_t sca1);
// [196] : ivshmem_get_max_peers 
uint32_t wazi_syscall_ivshmem_get_max_peers (wasm_exec_env_t exec_env, int32_t sca1);
// [197] : ivshmem_get_mem 
uint32_t wazi_syscall_ivshmem_get_mem (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [198] : ivshmem_get_output_mem_section 
uint32_t wazi_syscall_ivshmem_get_output_mem_section (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3);
// [199] : ivshmem_get_protocol 
uint16_t wazi_syscall_ivshmem_get_protocol (wasm_exec_env_t exec_env, int32_t sca1);
// [200] : ivshmem_get_rw_mem_section 
uint32_t wazi_syscall_ivshmem_get_rw_mem_section (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [201] : ivshmem_get_state 
uint32_t wazi_syscall_ivshmem_get_state (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [202] : ivshmem_get_vectors 
uint16_t wazi_syscall_ivshmem_get_vectors (wasm_exec_env_t exec_env, int32_t sca1);
// [203] : ivshmem_int_peer 
int wazi_syscall_ivshmem_int_peer (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint16_t sca3);
// [204] : ivshmem_register_handler 
int wazi_syscall_ivshmem_register_handler (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint16_t sca3);
// [205] : ivshmem_set_state 
int wazi_syscall_ivshmem_set_state (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [206] : kscan_config 
int wazi_syscall_kscan_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [207] : kscan_disable_callback 
int wazi_syscall_kscan_disable_callback (wasm_exec_env_t exec_env, int32_t sca1);
// [208] : kscan_enable_callback 
int wazi_syscall_kscan_enable_callback (wasm_exec_env_t exec_env, int32_t sca1);
// [209] : k_busy_wait 
void wazi_syscall_k_busy_wait (wasm_exec_env_t exec_env, uint32_t sca1);
// [210] : k_condvar_broadcast 
int wazi_syscall_k_condvar_broadcast (wasm_exec_env_t exec_env, int32_t sca1);
// [211] : k_condvar_init 
int wazi_syscall_k_condvar_init (wasm_exec_env_t exec_env, int32_t sca1);
// [212] : k_condvar_signal 
int wazi_syscall_k_condvar_signal (wasm_exec_env_t exec_env, int32_t sca1);
// [213] : k_condvar_wait 
int wazi_syscall_k_condvar_wait (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [214] : k_event_clear 
uint32_t wazi_syscall_k_event_clear (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [215] : k_event_init 
void wazi_syscall_k_event_init (wasm_exec_env_t exec_env, int32_t sca1);
// [216] : k_event_post 
uint32_t wazi_syscall_k_event_post (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [217] : k_event_set 
uint32_t wazi_syscall_k_event_set (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [218] : k_event_set_masked 
uint32_t wazi_syscall_k_event_set_masked (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3);
// [219] : k_event_wait 
uint32_t wazi_syscall_k_event_wait (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, bool sca3, uint32_t sca4);
// [220] : k_event_wait_all 
uint32_t wazi_syscall_k_event_wait_all (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, bool sca3, uint32_t sca4);
// [221] : k_float_disable 
int wazi_syscall_k_float_disable (wasm_exec_env_t exec_env, int32_t sca1);
// [222] : k_float_enable 
int wazi_syscall_k_float_enable (wasm_exec_env_t exec_env, int32_t sca1, unsigned int sca2);
// [223] : k_futex_wait 
int wazi_syscall_k_futex_wait (wasm_exec_env_t exec_env, int32_t sca1, int sca2, uint32_t sca3);
// [224] : k_futex_wake 
int wazi_syscall_k_futex_wake (wasm_exec_env_t exec_env, int32_t sca1, bool sca2);
// [225] : k_is_preempt_thread 
int wazi_syscall_k_is_preempt_thread (wasm_exec_env_t exec_env);
// [226] : k_mem_paging_histogram_backing_store_page_in_get 
void wazi_syscall_k_mem_paging_histogram_backing_store_page_in_get (wasm_exec_env_t exec_env, int32_t sca1);
// [227] : k_mem_paging_histogram_backing_store_page_out_get 
void wazi_syscall_k_mem_paging_histogram_backing_store_page_out_get (wasm_exec_env_t exec_env, int32_t sca1);
// [228] : k_mem_paging_histogram_eviction_get 
void wazi_syscall_k_mem_paging_histogram_eviction_get (wasm_exec_env_t exec_env, int32_t sca1);
// [229] : k_mem_paging_stats_get 
void wazi_syscall_k_mem_paging_stats_get (wasm_exec_env_t exec_env, int32_t sca1);
// [230] : k_mem_paging_thread_stats_get 
void wazi_syscall_k_mem_paging_thread_stats_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [231] : k_msgq_alloc_init 
int wazi_syscall_k_msgq_alloc_init (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3);
// [232] : k_msgq_get 
int wazi_syscall_k_msgq_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [233] : k_msgq_get_attrs 
void wazi_syscall_k_msgq_get_attrs (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [234] : k_msgq_num_free_get 
uint32_t wazi_syscall_k_msgq_num_free_get (wasm_exec_env_t exec_env, int32_t sca1);
// [235] : k_msgq_num_used_get 
uint32_t wazi_syscall_k_msgq_num_used_get (wasm_exec_env_t exec_env, int32_t sca1);
// [236] : k_msgq_peek 
int wazi_syscall_k_msgq_peek (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [237] : k_msgq_peek_at 
int wazi_syscall_k_msgq_peek_at (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [238] : k_msgq_purge 
void wazi_syscall_k_msgq_purge (wasm_exec_env_t exec_env, int32_t sca1);
// [239] : k_msgq_put 
int wazi_syscall_k_msgq_put (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [240] : k_mutex_init 
int wazi_syscall_k_mutex_init (wasm_exec_env_t exec_env, int32_t sca1);
// [241] : k_mutex_lock 
int wazi_syscall_k_mutex_lock (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [242] : k_mutex_unlock 
int wazi_syscall_k_mutex_unlock (wasm_exec_env_t exec_env, int32_t sca1);
// [243] : k_object_access_grant 
void wazi_syscall_k_object_access_grant (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [244] : k_object_alloc 
int32_t wazi_syscall_k_object_alloc (wasm_exec_env_t exec_env, int32_t sca1);
// [245] : k_object_alloc_size 
int32_t wazi_syscall_k_object_alloc_size (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [246] : k_object_release 
void wazi_syscall_k_object_release (wasm_exec_env_t exec_env, int32_t sca1);
// [247] : k_pipe_alloc_init 
int wazi_syscall_k_pipe_alloc_init (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [248] : k_pipe_buffer_flush 
void wazi_syscall_k_pipe_buffer_flush (wasm_exec_env_t exec_env, int32_t sca1);
// [249] : k_pipe_flush 
void wazi_syscall_k_pipe_flush (wasm_exec_env_t exec_env, int32_t sca1);
// [250] : k_pipe_get 
int wazi_syscall_k_pipe_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4, uint32_t sca5, uint32_t sca6);
// [251] : k_pipe_put 
int wazi_syscall_k_pipe_put (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4, uint32_t sca5, uint32_t sca6);
// [252] : k_pipe_read_avail 
uint32_t wazi_syscall_k_pipe_read_avail (wasm_exec_env_t exec_env, int32_t sca1);
// [253] : k_pipe_write_avail 
uint32_t wazi_syscall_k_pipe_write_avail (wasm_exec_env_t exec_env, int32_t sca1);
// [254] : k_poll 
int wazi_syscall_k_poll (wasm_exec_env_t exec_env, int32_t sca1, int sca2, uint32_t sca3);
// [255] : k_poll_signal_check 
void wazi_syscall_k_poll_signal_check (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [256] : k_poll_signal_init 
void wazi_syscall_k_poll_signal_init (wasm_exec_env_t exec_env, int32_t sca1);
// [257] : k_poll_signal_raise 
int wazi_syscall_k_poll_signal_raise (wasm_exec_env_t exec_env, int32_t sca1, int sca2);
// [258] : k_poll_signal_reset 
void wazi_syscall_k_poll_signal_reset (wasm_exec_env_t exec_env, int32_t sca1);
// [259] : k_queue_alloc_append 
int32_t wazi_syscall_k_queue_alloc_append (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [260] : k_queue_alloc_prepend 
int32_t wazi_syscall_k_queue_alloc_prepend (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [261] : k_queue_cancel_wait 
void wazi_syscall_k_queue_cancel_wait (wasm_exec_env_t exec_env, int32_t sca1);
// [262] : k_queue_get 
int32_t wazi_syscall_k_queue_get (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [263] : k_queue_init 
void wazi_syscall_k_queue_init (wasm_exec_env_t exec_env, int32_t sca1);
// [264] : k_queue_is_empty 
int wazi_syscall_k_queue_is_empty (wasm_exec_env_t exec_env, int32_t sca1);
// [265] : k_queue_peek_head 
int32_t wazi_syscall_k_queue_peek_head (wasm_exec_env_t exec_env, int32_t sca1);
// [266] : k_queue_peek_tail 
int32_t wazi_syscall_k_queue_peek_tail (wasm_exec_env_t exec_env, int32_t sca1);
// [267] : k_sched_current_thread_query 
int32_t wazi_syscall_k_sched_current_thread_query (wasm_exec_env_t exec_env);
// [268] : k_sem_count_get 
unsigned int wazi_syscall_k_sem_count_get (wasm_exec_env_t exec_env, int32_t sca1);
// [269] : k_sem_give 
void wazi_syscall_k_sem_give (wasm_exec_env_t exec_env, int32_t sca1);
// [270] : k_sem_init 
int wazi_syscall_k_sem_init (wasm_exec_env_t exec_env, int32_t sca1, unsigned int sca2, unsigned int sca3);
// [271] : k_sem_reset 
void wazi_syscall_k_sem_reset (wasm_exec_env_t exec_env, int32_t sca1);
// [272] : k_sem_take 
int wazi_syscall_k_sem_take (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [273] : k_sleep 
int32_t wazi_syscall_k_sleep (wasm_exec_env_t exec_env, uint32_t sca1);
// [274] : k_stack_alloc_init 
int32_t wazi_syscall_k_stack_alloc_init (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [275] : k_stack_pop 
int wazi_syscall_k_stack_pop (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [276] : k_stack_push 
int wazi_syscall_k_stack_push (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [277] : k_str_out 
void wazi_syscall_k_str_out (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [278] : k_thread_abort 
void wazi_syscall_k_thread_abort (wasm_exec_env_t exec_env, int32_t sca1);
// [279] : k_thread_create 
int32_t wazi_syscall_k_thread_create (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4, int32_t sca5, int32_t sca6, int32_t sca7, int sca8, uint32_t sca9, uint32_t sca10);
// [280] : k_thread_custom_data_get 
int32_t wazi_syscall_k_thread_custom_data_get (wasm_exec_env_t exec_env);
// [281] : k_thread_custom_data_set 
void wazi_syscall_k_thread_custom_data_set (wasm_exec_env_t exec_env, int32_t sca1);
// [282] : k_thread_deadline_set 
void wazi_syscall_k_thread_deadline_set (wasm_exec_env_t exec_env, int32_t sca1, int sca2);
// [283] : k_thread_join 
int wazi_syscall_k_thread_join (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [284] : k_thread_name_copy 
int wazi_syscall_k_thread_name_copy (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [285] : k_thread_name_set 
int wazi_syscall_k_thread_name_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [286] : k_thread_priority_get 
int wazi_syscall_k_thread_priority_get (wasm_exec_env_t exec_env, int32_t sca1);
// [287] : k_thread_priority_set 
void wazi_syscall_k_thread_priority_set (wasm_exec_env_t exec_env, int32_t sca1, int sca2);
// [288] : k_thread_resume 
void wazi_syscall_k_thread_resume (wasm_exec_env_t exec_env, int32_t sca1);
// [289] : k_thread_stack_alloc 
int32_t wazi_syscall_k_thread_stack_alloc (wasm_exec_env_t exec_env, uint32_t sca1, int sca2);
// [290] : k_thread_stack_free 
int wazi_syscall_k_thread_stack_free (wasm_exec_env_t exec_env, int32_t sca1);
// [291] : k_thread_stack_space_get 
int wazi_syscall_k_thread_stack_space_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [292] : k_thread_start 
void wazi_syscall_k_thread_start (wasm_exec_env_t exec_env, int32_t sca1);
// [293] : k_thread_suspend 
void wazi_syscall_k_thread_suspend (wasm_exec_env_t exec_env, int32_t sca1);
// [294] : k_thread_timeout_expires_ticks 
uint32_t wazi_syscall_k_thread_timeout_expires_ticks (wasm_exec_env_t exec_env, int32_t sca1);
// [295] : k_thread_timeout_remaining_ticks 
uint32_t wazi_syscall_k_thread_timeout_remaining_ticks (wasm_exec_env_t exec_env, int32_t sca1);
// [296] : k_timer_expires_ticks 
uint32_t wazi_syscall_k_timer_expires_ticks (wasm_exec_env_t exec_env, int32_t sca1);
// [297] : k_timer_remaining_ticks 
uint32_t wazi_syscall_k_timer_remaining_ticks (wasm_exec_env_t exec_env, int32_t sca1);
// [298] : k_timer_start 
void wazi_syscall_k_timer_start (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3);
// [299] : k_timer_status_get 
uint32_t wazi_syscall_k_timer_status_get (wasm_exec_env_t exec_env, int32_t sca1);
// [300] : k_timer_status_sync 
uint32_t wazi_syscall_k_timer_status_sync (wasm_exec_env_t exec_env, int32_t sca1);
// [301] : k_timer_stop 
void wazi_syscall_k_timer_stop (wasm_exec_env_t exec_env, int32_t sca1);
// [302] : k_timer_user_data_get 
int32_t wazi_syscall_k_timer_user_data_get (wasm_exec_env_t exec_env, int32_t sca1);
// [303] : k_timer_user_data_set 
void wazi_syscall_k_timer_user_data_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [304] : k_uptime_ticks 
int64_t wazi_syscall_k_uptime_ticks (wasm_exec_env_t exec_env);
// [305] : k_usleep 
int32_t wazi_syscall_k_usleep (wasm_exec_env_t exec_env, int32_t sca1);
// [306] : k_wakeup 
void wazi_syscall_k_wakeup (wasm_exec_env_t exec_env, int32_t sca1);
// [307] : k_yield 
void wazi_syscall_k_yield (wasm_exec_env_t exec_env);
// [308] : led_blink 
int wazi_syscall_led_blink (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, uint32_t sca4);
// [309] : led_get_info 
int wazi_syscall_led_get_info (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3);
// [310] : led_off 
int wazi_syscall_led_off (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [311] : led_on 
int wazi_syscall_led_on (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [312] : led_set_brightness 
int wazi_syscall_led_set_brightness (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint8_t sca3);
// [313] : led_set_channel 
int wazi_syscall_led_set_channel (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint8_t sca3);
// [314] : led_set_color 
int wazi_syscall_led_set_color (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint8_t sca3, int32_t sca4);
// [315] : led_write_channels 
int wazi_syscall_led_write_channels (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, int32_t sca4);
// [316] : log_buffered_cnt 
uint32_t wazi_syscall_log_buffered_cnt (wasm_exec_env_t exec_env);
// [317] : log_filter_set 
uint32_t wazi_syscall_log_filter_set (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int16_t sca3, uint32_t sca4);
// [318] : log_frontend_filter_set 
uint32_t wazi_syscall_log_frontend_filter_set (wasm_exec_env_t exec_env, int16_t sca1, uint32_t sca2);
// [319] : log_panic 
void wazi_syscall_log_panic (wasm_exec_env_t exec_env);
// [320] : log_process 
bool wazi_syscall_log_process (wasm_exec_env_t exec_env);
// [321] : maxim_ds3231_get_syncpoint 
int wazi_syscall_maxim_ds3231_get_syncpoint (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [322] : maxim_ds3231_req_syncpoint 
int wazi_syscall_maxim_ds3231_req_syncpoint (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [323] : mbox_max_channels_get 
uint32_t wazi_syscall_mbox_max_channels_get (wasm_exec_env_t exec_env, int32_t sca1);
// [324] : mbox_mtu_get 
int wazi_syscall_mbox_mtu_get (wasm_exec_env_t exec_env, int32_t sca1);
// [325] : mbox_send 
int wazi_syscall_mbox_send (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [326] : mbox_set_enabled 
int wazi_syscall_mbox_set_enabled (wasm_exec_env_t exec_env, int32_t sca1, bool sca2);
// [327] : mdio_bus_disable 
void wazi_syscall_mdio_bus_disable (wasm_exec_env_t exec_env, int32_t sca1);
// [328] : mdio_bus_enable 
void wazi_syscall_mdio_bus_enable (wasm_exec_env_t exec_env, int32_t sca1);
// [329] : mdio_read 
int wazi_syscall_mdio_read (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint8_t sca3, int32_t sca4);
// [330] : mdio_read_c45 
int wazi_syscall_mdio_read_c45 (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint8_t sca3, uint16_t sca4, int32_t sca5);
// [331] : mdio_write 
int wazi_syscall_mdio_write (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint8_t sca3, uint16_t sca4);
// [332] : mdio_write_c45 
int wazi_syscall_mdio_write_c45 (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint8_t sca3, uint16_t sca4, uint16_t sca5);
// [333] : net_addr_ntop 
int32_t wazi_syscall_net_addr_ntop (wasm_exec_env_t exec_env, uint16_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [334] : net_addr_pton 
int wazi_syscall_net_addr_pton (wasm_exec_env_t exec_env, uint16_t sca1, int32_t sca2, int32_t sca3);
// [335] : net_eth_get_ptp_clock_by_index 
int32_t wazi_syscall_net_eth_get_ptp_clock_by_index (wasm_exec_env_t exec_env, int sca1);
// [336] : net_if_get_by_index 
int32_t wazi_syscall_net_if_get_by_index (wasm_exec_env_t exec_env, int sca1);
// [337] : net_if_ipv4_addr_add_by_index 
bool wazi_syscall_net_if_ipv4_addr_add_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [338] : net_if_ipv4_addr_lookup_by_index 
int wazi_syscall_net_if_ipv4_addr_lookup_by_index (wasm_exec_env_t exec_env, int32_t sca1);
// [339] : net_if_ipv4_addr_rm_by_index 
bool wazi_syscall_net_if_ipv4_addr_rm_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2);
// [340] : net_if_ipv4_set_gw_by_index 
bool wazi_syscall_net_if_ipv4_set_gw_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2);
// [341] : net_if_ipv4_set_netmask_by_index 
bool wazi_syscall_net_if_ipv4_set_netmask_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2);
// [342] : net_if_ipv6_addr_add_by_index 
bool wazi_syscall_net_if_ipv6_addr_add_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [343] : net_if_ipv6_addr_lookup_by_index 
int wazi_syscall_net_if_ipv6_addr_lookup_by_index (wasm_exec_env_t exec_env, int32_t sca1);
// [344] : net_if_ipv6_addr_rm_by_index 
bool wazi_syscall_net_if_ipv6_addr_rm_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2);
// [345] : net_socket_service_register 
int wazi_syscall_net_socket_service_register (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int sca3, int32_t sca4);
// [346] : nrf_qspi_nor_xip_enable 
void wazi_syscall_nrf_qspi_nor_xip_enable (wasm_exec_env_t exec_env, int32_t sca1, bool sca2);
// [347] : peci_config 
int wazi_syscall_peci_config (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [348] : peci_disable 
int wazi_syscall_peci_disable (wasm_exec_env_t exec_env, int32_t sca1);
// [349] : peci_enable 
int wazi_syscall_peci_enable (wasm_exec_env_t exec_env, int32_t sca1);
// [350] : peci_transfer 
int wazi_syscall_peci_transfer (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [351] : ps2_config 
int wazi_syscall_ps2_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [352] : ps2_disable_callback 
int wazi_syscall_ps2_disable_callback (wasm_exec_env_t exec_env, int32_t sca1);
// [353] : ps2_enable_callback 
int wazi_syscall_ps2_enable_callback (wasm_exec_env_t exec_env, int32_t sca1);
// [354] : ps2_read 
int wazi_syscall_ps2_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [355] : ps2_write 
int wazi_syscall_ps2_write (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2);
// [356] : ptp_clock_get 
int wazi_syscall_ptp_clock_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [357] : pwm_capture_cycles 
int wazi_syscall_pwm_capture_cycles (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint16_t sca3, int32_t sca4, int32_t sca5, uint32_t sca6);
// [358] : pwm_disable_capture 
int wazi_syscall_pwm_disable_capture (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [359] : pwm_enable_capture 
int wazi_syscall_pwm_enable_capture (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [360] : pwm_get_cycles_per_sec 
int wazi_syscall_pwm_get_cycles_per_sec (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3);
// [361] : pwm_set_cycles 
int wazi_syscall_pwm_set_cycles (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, uint32_t sca4, uint16_t sca5);
// [362] : reset_line_assert 
int wazi_syscall_reset_line_assert (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [363] : reset_line_deassert 
int wazi_syscall_reset_line_deassert (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [364] : reset_line_toggle 
int wazi_syscall_reset_line_toggle (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [365] : reset_status 
int wazi_syscall_reset_status (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3);
// [366] : retained_mem_clear 
int wazi_syscall_retained_mem_clear (wasm_exec_env_t exec_env, int32_t sca1);
// [367] : retained_mem_read 
int wazi_syscall_retained_mem_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [368] : retained_mem_size 
uint32_t wazi_syscall_retained_mem_size (wasm_exec_env_t exec_env, int32_t sca1);
// [369] : retained_mem_write 
int wazi_syscall_retained_mem_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [370] : rtc_alarm_get_supported_fields 
int wazi_syscall_rtc_alarm_get_supported_fields (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3);
// [371] : rtc_alarm_get_time 
int wazi_syscall_rtc_alarm_get_time (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3, int32_t sca4);
// [372] : rtc_alarm_is_pending 
int wazi_syscall_rtc_alarm_is_pending (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2);
// [373] : rtc_alarm_set_callback 
int wazi_syscall_rtc_alarm_set_callback (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3, int32_t sca4);
// [374] : rtc_alarm_set_time 
int wazi_syscall_rtc_alarm_set_time (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint16_t sca3, int32_t sca4);
// [375] : rtc_get_calibration 
int wazi_syscall_rtc_get_calibration (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [376] : rtc_get_time 
int wazi_syscall_rtc_get_time (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [377] : rtc_set_calibration 
int wazi_syscall_rtc_set_calibration (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [378] : rtc_set_time 
int wazi_syscall_rtc_set_time (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [379] : rtc_update_set_callback 
int wazi_syscall_rtc_update_set_callback (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [380] : rtio_cqe_copy_out 
int wazi_syscall_rtio_cqe_copy_out (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, uint32_t sca4);
// [381] : rtio_cqe_get_mempool_buffer 
int wazi_syscall_rtio_cqe_get_mempool_buffer (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4);
// [382] : rtio_release_buffer 
void wazi_syscall_rtio_release_buffer (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [383] : rtio_sqe_cancel 
int wazi_syscall_rtio_sqe_cancel (wasm_exec_env_t exec_env, int32_t sca1);
// [384] : rtio_sqe_copy_in_get_handles 
int wazi_syscall_rtio_sqe_copy_in_get_handles (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [385] : rtio_submit 
int wazi_syscall_rtio_submit (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [386] : sdhc_card_busy 
int wazi_syscall_sdhc_card_busy (wasm_exec_env_t exec_env, int32_t sca1);
// [387] : sdhc_card_present 
int wazi_syscall_sdhc_card_present (wasm_exec_env_t exec_env, int32_t sca1);
// [388] : sdhc_disable_interrupt 
int wazi_syscall_sdhc_disable_interrupt (wasm_exec_env_t exec_env, int32_t sca1, int sca2);
// [389] : sdhc_enable_interrupt 
int wazi_syscall_sdhc_enable_interrupt (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int sca3, int32_t sca4);
// [390] : sdhc_execute_tuning 
int wazi_syscall_sdhc_execute_tuning (wasm_exec_env_t exec_env, int32_t sca1);
// [391] : sdhc_get_host_props 
int wazi_syscall_sdhc_get_host_props (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [392] : sdhc_hw_reset 
int wazi_syscall_sdhc_hw_reset (wasm_exec_env_t exec_env, int32_t sca1);
// [393] : sdhc_request 
int wazi_syscall_sdhc_request (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [394] : sdhc_set_io 
int wazi_syscall_sdhc_set_io (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [395] : sensor_attr_get 
int wazi_syscall_sensor_attr_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4);
// [396] : sensor_attr_set 
int wazi_syscall_sensor_attr_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4);
// [397] : sensor_channel_get 
int wazi_syscall_sensor_channel_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3);
// [398] : sensor_get_decoder 
int wazi_syscall_sensor_get_decoder (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [399] : sensor_reconfigure_read_iodev 
int wazi_syscall_sensor_reconfigure_read_iodev (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4);
// [400] : sensor_sample_fetch 
int wazi_syscall_sensor_sample_fetch (wasm_exec_env_t exec_env, int32_t sca1);
// [401] : sensor_sample_fetch_chan 
int wazi_syscall_sensor_sample_fetch_chan (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [402] : sip_supervisory_call 
void wazi_syscall_sip_supervisory_call (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, uint32_t sca4, uint32_t sca5, uint32_t sca6, uint32_t sca7, uint32_t sca8, uint32_t sca9, int32_t sca10);
// [403] : sip_svc_plat_async_res_req 
int wazi_syscall_sip_svc_plat_async_res_req (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4, int32_t sca5, int32_t sca6, int32_t sca7, int32_t sca8, int32_t sca9, int32_t sca10, uint32_t sca11);
// [404] : sip_svc_plat_async_res_res 
int wazi_syscall_sip_svc_plat_async_res_res (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4, int32_t sca5);
// [405] : sip_svc_plat_format_trans_id 
uint32_t wazi_syscall_sip_svc_plat_format_trans_id (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3);
// [406] : sip_svc_plat_free_async_memory 
void wazi_syscall_sip_svc_plat_free_async_memory (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [407] : sip_svc_plat_func_id_valid 
bool wazi_syscall_sip_svc_plat_func_id_valid (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3);
// [408] : sip_svc_plat_get_error_code 
uint32_t wazi_syscall_sip_svc_plat_get_error_code (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [409] : sip_svc_plat_get_trans_idx 
uint32_t wazi_syscall_sip_svc_plat_get_trans_idx (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [410] : sip_svc_plat_update_trans_id 
void wazi_syscall_sip_svc_plat_update_trans_id (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [411] : smbus_block_pcall 
int wazi_syscall_smbus_block_pcall (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, uint8_t sca4, int32_t sca5, int32_t sca6, int32_t sca7);
// [412] : smbus_block_read 
int wazi_syscall_smbus_block_read (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, int32_t sca4, int32_t sca5);
// [413] : smbus_block_write 
int wazi_syscall_smbus_block_write (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, uint8_t sca4, int32_t sca5);
// [414] : smbus_byte_data_read 
int wazi_syscall_smbus_byte_data_read (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, int32_t sca4);
// [415] : smbus_byte_data_write 
int wazi_syscall_smbus_byte_data_write (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, uint8_t sca4);
// [416] : smbus_byte_read 
int wazi_syscall_smbus_byte_read (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3);
// [417] : smbus_byte_write 
int wazi_syscall_smbus_byte_write (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3);
// [418] : smbus_configure 
int wazi_syscall_smbus_configure (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [419] : smbus_get_config 
int wazi_syscall_smbus_get_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [420] : smbus_host_notify_remove_cb 
int wazi_syscall_smbus_host_notify_remove_cb (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [421] : smbus_pcall 
int wazi_syscall_smbus_pcall (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, uint16_t sca4, int32_t sca5);
// [422] : smbus_quick 
int wazi_syscall_smbus_quick (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3);
// [423] : smbus_smbalert_remove_cb 
int wazi_syscall_smbus_smbalert_remove_cb (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [424] : smbus_word_data_read 
int wazi_syscall_smbus_word_data_read (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, int32_t sca4);
// [425] : smbus_word_data_write 
int wazi_syscall_smbus_word_data_write (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, uint16_t sca4);
// [426] : spi_release 
int wazi_syscall_spi_release (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [427] : spi_transceive 
int wazi_syscall_spi_transceive (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4);
// [428] : syscon_get_base 
int wazi_syscall_syscon_get_base (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [429] : syscon_get_size 
int wazi_syscall_syscon_get_size (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [430] : syscon_read_reg 
int wazi_syscall_syscon_read_reg (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3);
// [431] : syscon_write_reg 
int wazi_syscall_syscon_write_reg (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint32_t sca3);
// [432] : sys_cache_data_flush_and_invd_range 
int wazi_syscall_sys_cache_data_flush_and_invd_range (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [433] : sys_cache_data_flush_range 
int wazi_syscall_sys_cache_data_flush_range (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [434] : sys_cache_data_invd_range 
int wazi_syscall_sys_cache_data_invd_range (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [435] : sys_clock_hw_cycles_per_sec_runtime_get 
int wazi_syscall_sys_clock_hw_cycles_per_sec_runtime_get (wasm_exec_env_t exec_env);
// [436] : sys_csrand_get 
int wazi_syscall_sys_csrand_get (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [437] : sys_rand32_get 
uint32_t wazi_syscall_sys_rand32_get (wasm_exec_env_t exec_env);
// [438] : sys_rand_get 
void wazi_syscall_sys_rand_get (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [439] : tgpio_pin_config_ext_timestamp 
int wazi_syscall_tgpio_pin_config_ext_timestamp (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3);
// [440] : tgpio_pin_disable 
int wazi_syscall_tgpio_pin_disable (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [441] : tgpio_pin_periodic_output 
int wazi_syscall_tgpio_pin_periodic_output (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint64_t sca3, uint64_t sca4, bool sca5);
// [442] : tgpio_pin_read_ts_ec 
int wazi_syscall_tgpio_pin_read_ts_ec (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3, int32_t sca4);
// [443] : tgpio_port_get_cycles_per_second 
int wazi_syscall_tgpio_port_get_cycles_per_second (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [444] : tgpio_port_get_time 
int wazi_syscall_tgpio_port_get_time (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [445] : uart_configure 
int wazi_syscall_uart_configure (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [446] : uart_config_get 
int wazi_syscall_uart_config_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [447] : uart_drv_cmd 
int wazi_syscall_uart_drv_cmd (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3);
// [448] : uart_err_check 
int wazi_syscall_uart_err_check (wasm_exec_env_t exec_env, int32_t sca1);
// [449] : uart_irq_err_disable 
void wazi_syscall_uart_irq_err_disable (wasm_exec_env_t exec_env, int32_t sca1);
// [450] : uart_irq_err_enable 
void wazi_syscall_uart_irq_err_enable (wasm_exec_env_t exec_env, int32_t sca1);
// [451] : uart_irq_is_pending 
int wazi_syscall_uart_irq_is_pending (wasm_exec_env_t exec_env, int32_t sca1);
// [452] : uart_irq_rx_disable 
void wazi_syscall_uart_irq_rx_disable (wasm_exec_env_t exec_env, int32_t sca1);
// [453] : uart_irq_rx_enable 
void wazi_syscall_uart_irq_rx_enable (wasm_exec_env_t exec_env, int32_t sca1);
// [454] : uart_irq_tx_disable 
void wazi_syscall_uart_irq_tx_disable (wasm_exec_env_t exec_env, int32_t sca1);
// [455] : uart_irq_tx_enable 
void wazi_syscall_uart_irq_tx_enable (wasm_exec_env_t exec_env, int32_t sca1);
// [456] : uart_irq_update 
int wazi_syscall_uart_irq_update (wasm_exec_env_t exec_env, int32_t sca1);
// [457] : uart_line_ctrl_get 
int wazi_syscall_uart_line_ctrl_get (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3);
// [458] : uart_line_ctrl_set 
int wazi_syscall_uart_line_ctrl_set (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3);
// [459] : uart_mux_find 
int32_t wazi_syscall_uart_mux_find (wasm_exec_env_t exec_env, int sca1);
// [460] : uart_poll_in 
int wazi_syscall_uart_poll_in (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [461] : uart_poll_in_u16 
int wazi_syscall_uart_poll_in_u16 (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2);
// [462] : uart_poll_out 
void wazi_syscall_uart_poll_out (wasm_exec_env_t exec_env, int32_t sca1, unsigned char sca2);
// [463] : uart_poll_out_u16 
void wazi_syscall_uart_poll_out_u16 (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2);
// [464] : uart_rx_disable 
int wazi_syscall_uart_rx_disable (wasm_exec_env_t exec_env, int32_t sca1);
// [465] : uart_rx_enable 
int wazi_syscall_uart_rx_enable (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4);
// [466] : uart_rx_enable_u16 
int wazi_syscall_uart_rx_enable_u16 (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4);
// [467] : uart_tx 
int wazi_syscall_uart_tx (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4);
// [468] : uart_tx_abort 
int wazi_syscall_uart_tx_abort (wasm_exec_env_t exec_env, int32_t sca1);
// [469] : uart_tx_u16 
int wazi_syscall_uart_tx_u16 (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4);
// [470] : updatehub_autohandler 
void wazi_syscall_updatehub_autohandler (wasm_exec_env_t exec_env);
// [471] : updatehub_confirm 
int wazi_syscall_updatehub_confirm (wasm_exec_env_t exec_env);
// [472] : updatehub_probe 
int32_t wazi_syscall_updatehub_probe (wasm_exec_env_t exec_env);
// [473] : updatehub_reboot 
int wazi_syscall_updatehub_reboot (wasm_exec_env_t exec_env);
// [474] : updatehub_update 
int32_t wazi_syscall_updatehub_update (wasm_exec_env_t exec_env);
// [475] : user_fault 
void wazi_syscall_user_fault (wasm_exec_env_t exec_env, unsigned int sca1);
// [476] : w1_change_bus_lock 
int wazi_syscall_w1_change_bus_lock (wasm_exec_env_t exec_env, int32_t sca1, bool sca2);
// [477] : w1_configure 
int wazi_syscall_w1_configure (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [478] : w1_get_slave_count 
uint32_t wazi_syscall_w1_get_slave_count (wasm_exec_env_t exec_env, int32_t sca1);
// [479] : w1_read_bit 
int wazi_syscall_w1_read_bit (wasm_exec_env_t exec_env, int32_t sca1);
// [480] : w1_read_block 
int wazi_syscall_w1_read_block (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [481] : w1_read_byte 
int wazi_syscall_w1_read_byte (wasm_exec_env_t exec_env, int32_t sca1);
// [482] : w1_reset_bus 
int wazi_syscall_w1_reset_bus (wasm_exec_env_t exec_env, int32_t sca1);
// [483] : w1_search_bus 
int wazi_syscall_w1_search_bus (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint8_t sca3, int32_t sca4, int32_t sca5);
// [484] : w1_write_bit 
int wazi_syscall_w1_write_bit (wasm_exec_env_t exec_env, int32_t sca1, bool sca2);
// [485] : w1_write_block 
int wazi_syscall_w1_write_block (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3);
// [486] : w1_write_byte 
int wazi_syscall_w1_write_byte (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2);
// [487] : wdt_disable 
int wazi_syscall_wdt_disable (wasm_exec_env_t exec_env, int32_t sca1);
// [488] : wdt_feed 
int wazi_syscall_wdt_feed (wasm_exec_env_t exec_env, int32_t sca1, int sca2);
// [489] : wdt_setup 
int wazi_syscall_wdt_setup (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2);
// [490] : xtensa_user_fault 
void wazi_syscall_xtensa_user_fault (wasm_exec_env_t exec_env, unsigned int sca1);
// [491] : zephyr_fputc 
int wazi_syscall_zephyr_fputc (wasm_exec_env_t exec_env, int sca1, int32_t sca2);
// [492] : zephyr_fwrite 
uint32_t wazi_syscall_zephyr_fwrite (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, int32_t sca4);
// [493] : zephyr_read_stdin 
int wazi_syscall_zephyr_read_stdin (wasm_exec_env_t exec_env, int32_t sca1, int sca2);
// [494] : zephyr_write_stdout 
int wazi_syscall_zephyr_write_stdout (wasm_exec_env_t exec_env, int32_t sca1, int sca2);
// [495] : zsock_accept 
int wazi_syscall_zsock_accept (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3);
// [496] : zsock_bind 
int wazi_syscall_zsock_bind (wasm_exec_env_t exec_env, int sca1, int32_t sca2, uint32_t sca3);
// [497] : zsock_close 
int wazi_syscall_zsock_close (wasm_exec_env_t exec_env, int sca1);
// [498] : zsock_connect 
int wazi_syscall_zsock_connect (wasm_exec_env_t exec_env, int sca1, int32_t sca2, uint32_t sca3);
// [499] : zsock_fcntl 
int wazi_syscall_zsock_fcntl (wasm_exec_env_t exec_env, int sca1, int sca2, int sca3);
// [500] : zsock_gethostname 
int wazi_syscall_zsock_gethostname (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [501] : zsock_getpeername 
int wazi_syscall_zsock_getpeername (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3);
// [502] : zsock_getsockname 
int wazi_syscall_zsock_getsockname (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3);
// [503] : zsock_getsockopt 
int wazi_syscall_zsock_getsockopt (wasm_exec_env_t exec_env, int sca1, int sca2, int sca3, int32_t sca4, int32_t sca5);
// [504] : zsock_get_context_object 
int32_t wazi_syscall_zsock_get_context_object (wasm_exec_env_t exec_env, int sca1);
// [505] : zsock_inet_pton 
int wazi_syscall_zsock_inet_pton (wasm_exec_env_t exec_env, uint16_t sca1, int32_t sca2, int32_t sca3);
// [506] : zsock_ioctl 
int wazi_syscall_zsock_ioctl (wasm_exec_env_t exec_env, int sca1, uint32_t sca2, int32_t sca3);
// [507] : zsock_listen 
int wazi_syscall_zsock_listen (wasm_exec_env_t exec_env, int sca1, int sca2);
// [508] : zsock_poll 
int wazi_syscall_zsock_poll (wasm_exec_env_t exec_env, int32_t sca1, int sca2, int sca3);
// [509] : zsock_recvfrom 
uint32_t wazi_syscall_zsock_recvfrom (wasm_exec_env_t exec_env, int sca1, int32_t sca2, uint32_t sca3, int sca4, int32_t sca5, int32_t sca6);
// [510] : zsock_recvmsg 
uint32_t wazi_syscall_zsock_recvmsg (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int sca3);
// [511] : zsock_select 
int wazi_syscall_zsock_select (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3, int32_t sca4, int32_t sca5);
// [512] : zsock_sendmsg 
uint32_t wazi_syscall_zsock_sendmsg (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int sca3);
// [513] : zsock_sendto 
uint32_t wazi_syscall_zsock_sendto (wasm_exec_env_t exec_env, int sca1, int32_t sca2, uint32_t sca3, int sca4, int32_t sca5, uint32_t sca6);
// [514] : zsock_setsockopt 
int wazi_syscall_zsock_setsockopt (wasm_exec_env_t exec_env, int sca1, int sca2, int sca3, int32_t sca4, uint32_t sca5);
// [515] : zsock_shutdown 
int wazi_syscall_zsock_shutdown (wasm_exec_env_t exec_env, int sca1, int sca2);
// [516] : zsock_socket 
int wazi_syscall_zsock_socket (wasm_exec_env_t exec_env, int sca1, int sca2, int sca3);
// [517] : zsock_socketpair 
int wazi_syscall_zsock_socketpair (wasm_exec_env_t exec_env, int sca1, int sca2, int sca3, int32_t sca4);
// [518] : z_errno 
int32_t wazi_syscall_z_errno (wasm_exec_env_t exec_env);
// [519] : z_log_msg_simple_create_0 
void wazi_syscall_z_log_msg_simple_create_0 (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3);
// [520] : z_log_msg_simple_create_1 
void wazi_syscall_z_log_msg_simple_create_1 (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3, uint32_t sca4);
// [521] : z_log_msg_simple_create_2 
void wazi_syscall_z_log_msg_simple_create_2 (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3, uint32_t sca4, uint32_t sca5);
// [522] : z_log_msg_static_create 
void wazi_syscall_z_log_msg_static_create (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3, int32_t sca4);
// [523] : z_sys_mutex_kernel_lock 
int wazi_syscall_z_sys_mutex_kernel_lock (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2);
// [524] : z_sys_mutex_kernel_unlock 
int wazi_syscall_z_sys_mutex_kernel_unlock (wasm_exec_env_t exec_env, int32_t sca1);
// [525] : z_zsock_getaddrinfo_internal 
int wazi_syscall_z_zsock_getaddrinfo_internal (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4);
// [526] : __posix_clock_get_base 
int wazi_syscall___posix_clock_get_base (wasm_exec_env_t exec_env, uint32_t sca1, int32_t sca2);

#endif
