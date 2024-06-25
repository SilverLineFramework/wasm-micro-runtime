/*
  MIT License

  Copyright (c) [2023] [Arjun Ramesh]

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "wazi.h"
#include "copy.h"
#include "wazi_conversions.h"

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#ifdef CONFIG_MINIMAL_LIBC
#include <zephyr/posix/time.h>
#endif
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/auxdisplay.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/usb/usb_bc12.h>
#include <zephyr/drivers/charger.h>
#include <zephyr/drivers/espi.h>
#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/drivers/gnss.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/sdhc.h>
#include <zephyr/drivers/smbus.h>
#include <zephyr/drivers/w1.h>
#include <zephyr/drivers/kscan.h>
#include <zephyr/drivers/ps2.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/bbram.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/drivers/dac.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/espi.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/drivers/hwspinlock.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/drivers/i3c.h>
#include <zephyr/drivers/ipm.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/virtualization/ivshmem.h>
#include <zephyr/drivers/misc/devmux/devmux.h>

#include <zephyr/net/socket_select.h>
#include <zephyr/net/socket.h>


#define SC(nr, f)   LOG_VERBOSE("WAZI: SC | " # f);
#define RETURN(v)   return v;
#define RETURN_VOID(v) v; return;

#define ERRSC(f,...) { \
  LOG_ERROR("WAZI: SC \"" # f "\" not implemented correctly yet! " __VA_ARGS__);  \
}
#define FATALSC(f,...) { \
  LOG_FATAL("WAZI: SC \"" # f "\" fatal error! " __VA_ARGS__);  \
}
#define MISSC(f,...) { \
  LOG_FATAL("WAZI: SC \"" # f "\" fatal error! No such syscall on platform");  \
}

/***** WAZI Methods *******/
// 0 TODO
int wazi_syscall_adc_channel_setup (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(0, adc_channel_setup);
	ERRSC(adc_channel_setup);
	struct device *dev = (struct device*) MADDR(sca1);
	struct adc_channel_cfg *channel_cfg = (struct adc_channel_cfg*) MADDR(sca2);
	RETURN((int) adc_channel_setup(dev,channel_cfg));
}

// 1 TODO
int wazi_syscall_adc_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(1, adc_read);
	ERRSC(adc_read);
	struct device *dev = (struct device*) MADDR(sca1);
	struct adc_sequence *sequence = (struct adc_sequence*) MADDR(sca2);
	RETURN((int) adc_read(dev,sequence));
}

// 2 TODO
int wazi_syscall_adc_read_async (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(2, adc_read_async);
	ERRSC(adc_read_async);
	struct device *dev = (struct device*) MADDR(sca1);
	struct adc_sequence *sequence = (struct adc_sequence*) MADDR(sca2);
	struct k_poll_signal *async = (struct k_poll_signal*) MADDR(sca3);
	RETURN((int) adc_read_async(dev,sequence,async));
}

// 3 TODO
int32_t wazi_syscall_atomic_add (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(3, atomic_add);
	ERRSC(atomic_add);
	atomic_t *target = (atomic_t*) MADDR(sca1);
	atomic_val_t value = sca2;
	RETURN((int32_t) atomic_add(target,value));
}

// 4 TODO
int32_t wazi_syscall_atomic_and (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(4, atomic_and);
	ERRSC(atomic_and);
	atomic_t *target = (atomic_t*) MADDR(sca1);
	atomic_val_t value = sca2;
	RETURN((int32_t) atomic_and(target,value));
}

// 5 TODO
bool wazi_syscall_atomic_cas (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(5, atomic_cas);
	ERRSC(atomic_cas);
	atomic_t *target = (atomic_t*) MADDR(sca1);
	atomic_val_t old_value = sca2;
	atomic_val_t new_value = sca3;
	RETURN((bool) atomic_cas(target,old_value,new_value));
}

// 6 TODO
int32_t wazi_syscall_atomic_nand (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(6, atomic_nand);
	ERRSC(atomic_nand);
	atomic_t *target = (atomic_t*) MADDR(sca1);
	atomic_val_t value = sca2;
	RETURN((int32_t) atomic_nand(target,value));
}

// 7 TODO
int32_t wazi_syscall_atomic_or (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(7, atomic_or);
	ERRSC(atomic_or);
	atomic_t *target = (atomic_t*) MADDR(sca1);
	atomic_val_t value = sca2;
	RETURN((int32_t) atomic_or(target,value));
}

// 8 TODO
bool wazi_syscall_atomic_ptr_cas (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(8, atomic_ptr_cas);
	ERRSC(atomic_ptr_cas);
	atomic_ptr_t *target = (atomic_ptr_t*) MADDR(sca1);
	atomic_ptr_val_t old_value = (atomic_ptr_val_t) MADDR(sca2);
	atomic_ptr_val_t new_value = (atomic_ptr_val_t) MADDR(sca3);
	RETURN((bool) atomic_ptr_cas(target,old_value,new_value));
}

// 9 TODO
int32_t wazi_syscall_atomic_ptr_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(9, atomic_ptr_set);
	ERRSC(atomic_ptr_set);
	atomic_ptr_t *target = (atomic_ptr_t*) MADDR(sca1);
	atomic_ptr_val_t value = (atomic_ptr_val_t) MADDR(sca2);
	RETURN((int32_t) atomic_ptr_set(target,value));
}

// 10 TODO
int32_t wazi_syscall_atomic_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(10, atomic_set);
	ERRSC(atomic_set);
	atomic_t *target = (atomic_t*) MADDR(sca1);
	atomic_val_t value = sca2;
	RETURN((int32_t) atomic_set(target,value));
}

// 11 TODO
int32_t wazi_syscall_atomic_sub (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(11, atomic_sub);
	ERRSC(atomic_sub);
	atomic_t *target = (atomic_t*) MADDR(sca1);
	atomic_val_t value = sca2;
	RETURN((int32_t) atomic_sub(target,value));
}

// 12 TODO
int32_t wazi_syscall_atomic_xor (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(12, atomic_xor);
	ERRSC(atomic_xor);
	atomic_t *target = (atomic_t*) MADDR(sca1);
	atomic_val_t value = sca2;
	RETURN((int32_t) atomic_xor(target,value));
}

// 13 TODO
int wazi_syscall_auxdisplay_backlight_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(13, auxdisplay_backlight_get);
	ERRSC(auxdisplay_backlight_get);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t *backlight = (uint8_t*) MADDR(sca2);
	RETURN((int) auxdisplay_backlight_get(dev,backlight));
}

// 14 TODO
int wazi_syscall_auxdisplay_backlight_set (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2) {
	SC(14, auxdisplay_backlight_set);
	ERRSC(auxdisplay_backlight_set);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t backlight = sca2;
	RETURN((int) auxdisplay_backlight_set(dev,backlight));
}

// 15 TODO
int wazi_syscall_auxdisplay_brightness_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(15, auxdisplay_brightness_get);
	ERRSC(auxdisplay_brightness_get);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t *brightness = (uint8_t*) MADDR(sca2);
	RETURN((int) auxdisplay_brightness_get(dev,brightness));
}

// 16 TODO
int wazi_syscall_auxdisplay_brightness_set (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2) {
	SC(16, auxdisplay_brightness_set);
	ERRSC(auxdisplay_brightness_set);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t brightness = sca2;
	RETURN((int) auxdisplay_brightness_set(dev,brightness));
}

// 17 TODO
int wazi_syscall_auxdisplay_capabilities_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(17, auxdisplay_capabilities_get);
	ERRSC(auxdisplay_capabilities_get);
	struct device *dev = (struct device*) MADDR(sca1);
	struct auxdisplay_capabilities *capabilities = (struct auxdisplay_capabilities*) MADDR(sca2);
	RETURN((int) auxdisplay_capabilities_get(dev,capabilities));
}

// 18 TODO
int wazi_syscall_auxdisplay_clear (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(18, auxdisplay_clear);
	ERRSC(auxdisplay_clear);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) auxdisplay_clear(dev));
}

// 19 TODO
int wazi_syscall_auxdisplay_cursor_position_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(19, auxdisplay_cursor_position_get);
	ERRSC(auxdisplay_cursor_position_get);
	struct device *dev = (struct device*) MADDR(sca1);
	int16_t *x = (int16_t*) MADDR(sca2);
	int16_t *y = (int16_t*) MADDR(sca3);
	RETURN((int) auxdisplay_cursor_position_get(dev,x,y));
}

// 20 TODO
int wazi_syscall_auxdisplay_cursor_position_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int16_t sca3, int16_t sca4) {
	SC(20, auxdisplay_cursor_position_set);
	ERRSC(auxdisplay_cursor_position_set);
	struct device *dev = (struct device*) MADDR(sca1);
	enum auxdisplay_position type = sca2;
	int16_t x = sca3;
	int16_t y = sca4;
	RETURN((int) auxdisplay_cursor_position_set(dev,type,x,y));
}

// 21 TODO
int wazi_syscall_auxdisplay_cursor_set_enabled (wasm_exec_env_t exec_env, int32_t sca1, bool sca2) {
	SC(21, auxdisplay_cursor_set_enabled);
	ERRSC(auxdisplay_cursor_set_enabled);
	struct device *dev = (struct device*) MADDR(sca1);
	bool enabled = sca2;
	RETURN((int) auxdisplay_cursor_set_enabled(dev,enabled));
}

// 22 TODO
int wazi_syscall_auxdisplay_cursor_shift_set (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, bool sca3) {
	SC(22, auxdisplay_cursor_shift_set);
	ERRSC(auxdisplay_cursor_shift_set);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t direction = sca2;
	bool display_shift = sca3;
	RETURN((int) auxdisplay_cursor_shift_set(dev,direction,display_shift));
}

// 23 TODO
int wazi_syscall_auxdisplay_custom_character_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(23, auxdisplay_custom_character_set);
	ERRSC(auxdisplay_custom_character_set);
	struct device *dev = (struct device*) MADDR(sca1);
	struct auxdisplay_character *character = (struct auxdisplay_character*) MADDR(sca2);
	RETURN((int) auxdisplay_custom_character_set(dev,character));
}

// 24 TODO
int wazi_syscall_auxdisplay_custom_command (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(24, auxdisplay_custom_command);
	ERRSC(auxdisplay_custom_command);
	struct device *dev = (struct device*) MADDR(sca1);
	struct auxdisplay_custom_data *data = (struct auxdisplay_custom_data*) MADDR(sca2);
	RETURN((int) auxdisplay_custom_command(dev,data));
}

// 25 TODO
int wazi_syscall_auxdisplay_display_off (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(25, auxdisplay_display_off);
	ERRSC(auxdisplay_display_off);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) auxdisplay_display_off(dev));
}

// 26 TODO
int wazi_syscall_auxdisplay_display_on (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(26, auxdisplay_display_on);
	ERRSC(auxdisplay_display_on);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) auxdisplay_display_on(dev));
}

// 27 TODO
int wazi_syscall_auxdisplay_display_position_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(27, auxdisplay_display_position_get);
	ERRSC(auxdisplay_display_position_get);
	struct device *dev = (struct device*) MADDR(sca1);
	int16_t *x = (int16_t*) MADDR(sca2);
	int16_t *y = (int16_t*) MADDR(sca3);
	RETURN((int) auxdisplay_display_position_get(dev,x,y));
}

// 28 TODO
int wazi_syscall_auxdisplay_display_position_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int16_t sca3, int16_t sca4) {
	SC(28, auxdisplay_display_position_set);
	ERRSC(auxdisplay_display_position_set);
	struct device *dev = (struct device*) MADDR(sca1);
	enum auxdisplay_position type = sca2;
	int16_t x = sca3;
	int16_t y = sca4;
	RETURN((int) auxdisplay_display_position_set(dev,type,x,y));
}

// 29 TODO
int wazi_syscall_auxdisplay_is_busy (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(29, auxdisplay_is_busy);
	ERRSC(auxdisplay_is_busy);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) auxdisplay_is_busy(dev));
}

// 30 TODO
int wazi_syscall_auxdisplay_position_blinking_set_enabled (wasm_exec_env_t exec_env, int32_t sca1, bool sca2) {
	SC(30, auxdisplay_position_blinking_set_enabled);
	ERRSC(auxdisplay_position_blinking_set_enabled);
	struct device *dev = (struct device*) MADDR(sca1);
	bool enabled = sca2;
	RETURN((int) auxdisplay_position_blinking_set_enabled(dev,enabled));
}

// 31 TODO
int wazi_syscall_auxdisplay_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint16_t sca3) {
	SC(31, auxdisplay_write);
	ERRSC(auxdisplay_write);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t *data = (uint8_t*) MADDR(sca2);
	uint16_t len = sca3;
	RETURN((int) auxdisplay_write(dev,data,len));
}

// 32 TODO
int wazi_syscall_bbram_check_invalid (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(32, bbram_check_invalid);
	ERRSC(bbram_check_invalid);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) bbram_check_invalid(dev));
}

// 33 TODO
int wazi_syscall_bbram_check_power (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(33, bbram_check_power);
	ERRSC(bbram_check_power);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) bbram_check_power(dev));
}

// 34 TODO
int wazi_syscall_bbram_check_standby_power (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(34, bbram_check_standby_power);
	ERRSC(bbram_check_standby_power);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) bbram_check_standby_power(dev));
}

// 35 TODO
int wazi_syscall_bbram_get_size (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(35, bbram_get_size);
	ERRSC(bbram_get_size);
	struct device *dev = (struct device*) MADDR(sca1);
	size_t *size = (size_t*) MADDR(sca2);
	RETURN((int) bbram_get_size(dev,size));
}

// 36 TODO
int wazi_syscall_bbram_read (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, int32_t sca4) {
	SC(36, bbram_read);
	ERRSC(bbram_read);
	struct device *dev = (struct device*) MADDR(sca1);
	size_t offset = sca2;
	size_t size = sca3;
	uint8_t *data = (uint8_t*) MADDR(sca4);
	RETURN((int) bbram_read(dev,offset,size,data));
}

// 37 TODO
int wazi_syscall_bbram_write (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, int32_t sca4) {
	SC(37, bbram_write);
	ERRSC(bbram_write);
	struct device *dev = (struct device*) MADDR(sca1);
	size_t offset = sca2;
	size_t size = sca3;
	uint8_t *data = (uint8_t*) MADDR(sca4);
	RETURN((int) bbram_write(dev,offset,size,data));
}

// 38 TODO
int wazi_syscall_bc12_set_result_cb (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(38, bc12_set_result_cb);
	ERRSC(bc12_set_result_cb);
	struct device *dev = (struct device*) MADDR(sca1);
	bc12_callback_t cb = ERROR_FN_PTR;
	void *user_data = (void*) MADDR(sca3);
	RETURN((int) bc12_set_result_cb(dev,cb,user_data));
}

// 39 TODO
int wazi_syscall_bc12_set_role (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(39, bc12_set_role);
	ERRSC(bc12_set_role);
	struct device *dev = (struct device*) MADDR(sca1);
	enum bc12_role role = sca2;
	RETURN((int) bc12_set_role(dev,role));
}

// 40 TODO
int wazi_syscall_can_add_rx_filter_msgq (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(40, can_add_rx_filter_msgq);
	ERRSC(can_add_rx_filter_msgq);
	struct device *dev = (struct device*) MADDR(sca1);
	struct k_msgq *msgq = (struct k_msgq*) MADDR(sca2);
	struct can_filter *filter = (struct can_filter*) MADDR(sca3);
	RETURN((int) can_add_rx_filter_msgq(dev,msgq,filter));
}

// 41 TODO
int wazi_syscall_can_calc_timing (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, uint16_t sca4) {
	SC(41, can_calc_timing);
	ERRSC(can_calc_timing);
	struct device *dev = (struct device*) MADDR(sca1);
	struct can_timing *res = (struct can_timing*) MADDR(sca2);
	uint32_t bitrate = sca3;
	uint16_t sample_pnt = sca4;
	RETURN((int) can_calc_timing(dev,res,bitrate,sample_pnt));
}

// 42 TODO
int wazi_syscall_can_calc_timing_data (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, uint16_t sca4) {
	SC(42, can_calc_timing_data);
	ERRSC(can_calc_timing_data);
	struct device *dev = (struct device*) MADDR(sca1);
	struct can_timing *res = (struct can_timing*) MADDR(sca2);
	uint32_t bitrate = sca3;
	uint16_t sample_pnt = sca4;
	RETURN((int) can_calc_timing_data(dev,res,bitrate,sample_pnt));
}

// 43 TODO
int wazi_syscall_can_get_capabilities (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(43, can_get_capabilities);
	ERRSC(can_get_capabilities);
	struct device *dev = (struct device*) MADDR(sca1);
	can_mode_t *cap = (can_mode_t*) MADDR(sca2);
	RETURN((int) can_get_capabilities(dev,cap));
}

// 44 TODO
int wazi_syscall_can_get_core_clock (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(44, can_get_core_clock);
	ERRSC(can_get_core_clock);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t *rate = (uint32_t*) MADDR(sca2);
	RETURN((int) can_get_core_clock(dev,rate));
}

// 45 TODO
int wazi_syscall_can_get_max_bitrate (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(45, can_get_max_bitrate);
	ERRSC(can_get_max_bitrate);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t *max_bitrate = (uint32_t*) MADDR(sca2);
	RETURN((int) can_get_max_bitrate(dev,max_bitrate));
}

// 46 TODO
int wazi_syscall_can_get_max_filters (wasm_exec_env_t exec_env, int32_t sca1, bool sca2) {
	SC(46, can_get_max_filters);
	ERRSC(can_get_max_filters);
	struct device *dev = (struct device*) MADDR(sca1);
	bool ide = sca2;
	RETURN((int) can_get_max_filters(dev,ide));
}

// 47 TODO
uint32_t wazi_syscall_can_get_mode (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(47, can_get_mode);
	ERRSC(can_get_mode);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) can_get_mode(dev));
}

// 48 TODO
int wazi_syscall_can_get_state (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(48, can_get_state);
	ERRSC(can_get_state);
	struct device *dev = (struct device*) MADDR(sca1);
	enum can_state *state = (enum can_state*) MADDR(sca2);
	struct can_bus_err_cnt *err_cnt = (struct can_bus_err_cnt*) MADDR(sca3);
	RETURN((int) can_get_state(dev,state,err_cnt));
}

// 49 TODO
int32_t wazi_syscall_can_get_timing_data_max (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(49, can_get_timing_data_max);
	ERRSC(can_get_timing_data_max);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int32_t) can_get_timing_data_max(dev));
}

// 50 TODO
int32_t wazi_syscall_can_get_timing_data_min (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(50, can_get_timing_data_min);
	ERRSC(can_get_timing_data_min);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int32_t) can_get_timing_data_min(dev));
}

// 51 TODO
int32_t wazi_syscall_can_get_timing_max (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(51, can_get_timing_max);
	ERRSC(can_get_timing_max);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int32_t) can_get_timing_max(dev));
}

// 52 TODO
int32_t wazi_syscall_can_get_timing_min (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(52, can_get_timing_min);
	ERRSC(can_get_timing_min);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int32_t) can_get_timing_min(dev));
}

// 53 TODO
int32_t wazi_syscall_can_get_transceiver (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(53, can_get_transceiver);
	ERRSC(can_get_transceiver);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int32_t) can_get_transceiver(dev));
}

// 54 TODO
int wazi_syscall_can_recover (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(54, can_recover);
	ERRSC(can_recover);
	struct device *dev = (struct device*) MADDR(sca1);
	k_timeout_t timeout = { .ticks = sca2 };
	RETURN((int) can_recover(dev,timeout));
}

// 55 TODO
void wazi_syscall_can_remove_rx_filter (wasm_exec_env_t exec_env, int32_t sca1, int sca2) {
	SC(55, can_remove_rx_filter);
	ERRSC(can_remove_rx_filter);
	struct device *dev = (struct device*) MADDR(sca1);
	int filter_id = sca2;
	RETURN_VOID(can_remove_rx_filter(dev,filter_id));
}

// 56 TODO
int wazi_syscall_can_send (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4, int32_t sca5) {
	SC(56, can_send);
	ERRSC(can_send);
	struct device *dev = (struct device*) MADDR(sca1);
	struct can_frame *frame = (struct can_frame*) MADDR(sca2);
	k_timeout_t timeout = { .ticks = sca3 };
	can_tx_callback_t callback = ERROR_FN_PTR;
	void *user_data = (void*) MADDR(sca5);
	RETURN((int) can_send(dev,frame,timeout,callback,user_data));
}

// 57 TODO
int wazi_syscall_can_set_bitrate (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(57, can_set_bitrate);
	ERRSC(can_set_bitrate);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t bitrate = sca2;
	RETURN((int) can_set_bitrate(dev,bitrate));
}

// 58 TODO
int wazi_syscall_can_set_bitrate_data (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(58, can_set_bitrate_data);
	ERRSC(can_set_bitrate_data);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t bitrate_data = sca2;
	RETURN((int) can_set_bitrate_data(dev,bitrate_data));
}

// 59 TODO
int wazi_syscall_can_set_mode (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(59, can_set_mode);
	ERRSC(can_set_mode);
	struct device *dev = (struct device*) MADDR(sca1);
	can_mode_t mode = sca2;
	RETURN((int) can_set_mode(dev,mode));
}

// 60 TODO
int wazi_syscall_can_set_timing (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(60, can_set_timing);
	ERRSC(can_set_timing);
	struct device *dev = (struct device*) MADDR(sca1);
	struct can_timing *timing = (struct can_timing*) MADDR(sca2);
	RETURN((int) can_set_timing(dev,timing));
}

// 61 TODO
int wazi_syscall_can_set_timing_data (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(61, can_set_timing_data);
	ERRSC(can_set_timing_data);
	struct device *dev = (struct device*) MADDR(sca1);
	struct can_timing *timing_data = (struct can_timing*) MADDR(sca2);
	RETURN((int) can_set_timing_data(dev,timing_data));
}

// 62 TODO
int wazi_syscall_can_start (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(62, can_start);
	ERRSC(can_start);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) can_start(dev));
}

// 63 TODO
uint32_t wazi_syscall_can_stats_get_ack_errors (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(63, can_stats_get_ack_errors);
	ERRSC(can_stats_get_ack_errors);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) can_stats_get_ack_errors(dev));
}

// 64 TODO
uint32_t wazi_syscall_can_stats_get_bit0_errors (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(64, can_stats_get_bit0_errors);
	ERRSC(can_stats_get_bit0_errors);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) can_stats_get_bit0_errors(dev));
}

// 65 TODO
uint32_t wazi_syscall_can_stats_get_bit1_errors (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(65, can_stats_get_bit1_errors);
	ERRSC(can_stats_get_bit1_errors);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) can_stats_get_bit1_errors(dev));
}

// 66 TODO
uint32_t wazi_syscall_can_stats_get_bit_errors (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(66, can_stats_get_bit_errors);
	ERRSC(can_stats_get_bit_errors);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) can_stats_get_bit_errors(dev));
}

// 67 TODO
uint32_t wazi_syscall_can_stats_get_crc_errors (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(67, can_stats_get_crc_errors);
	ERRSC(can_stats_get_crc_errors);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) can_stats_get_crc_errors(dev));
}

// 68 TODO
uint32_t wazi_syscall_can_stats_get_form_errors (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(68, can_stats_get_form_errors);
	ERRSC(can_stats_get_form_errors);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) can_stats_get_form_errors(dev));
}

// 69 TODO
uint32_t wazi_syscall_can_stats_get_rx_overruns (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(69, can_stats_get_rx_overruns);
	ERRSC(can_stats_get_rx_overruns);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) can_stats_get_rx_overruns(dev));
}

// 70 TODO
uint32_t wazi_syscall_can_stats_get_stuff_errors (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(70, can_stats_get_stuff_errors);
	ERRSC(can_stats_get_stuff_errors);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) can_stats_get_stuff_errors(dev));
}

// 71 TODO
int wazi_syscall_can_stop (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(71, can_stop);
	ERRSC(can_stop);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) can_stop(dev));
}

// 72 TODO
int wazi_syscall_charger_charge_enable (wasm_exec_env_t exec_env, int32_t sca1, bool sca2) {
	SC(72, charger_charge_enable);
	ERRSC(charger_charge_enable);
	struct device *dev = (struct device*) MADDR(sca1);
	bool enable = sca2;
	RETURN((int) charger_charge_enable(dev,enable));
}

// 73 TODO
int wazi_syscall_charger_get_prop (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3) {
	SC(73, charger_get_prop);
	ERRSC(charger_get_prop);
	struct device *dev = (struct device*) MADDR(sca1);
	charger_prop_t prop = sca2;
	union charger_propval *val = (union charger_propval*) MADDR(sca3);
	RETURN((int) charger_get_prop(dev,prop,val));
}

// 74 TODO
int wazi_syscall_charger_set_prop (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3) {
	SC(74, charger_set_prop);
	ERRSC(charger_set_prop);
	struct device *dev = (struct device*) MADDR(sca1);
	charger_prop_t prop = sca2;
	union charger_propval *val = (union charger_propval*) MADDR(sca3);
	RETURN((int) charger_set_prop(dev,prop,val));
}

// 75 TODO
int wazi_syscall_counter_cancel_channel_alarm (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2) {
	SC(75, counter_cancel_channel_alarm);
	ERRSC(counter_cancel_channel_alarm);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t chan_id = sca2;
	RETURN((int) counter_cancel_channel_alarm(dev,chan_id));
}

// 76 TODO
uint32_t wazi_syscall_counter_get_frequency (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(76, counter_get_frequency);
	ERRSC(counter_get_frequency);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) counter_get_frequency(dev));
}

// 77 TODO
uint32_t wazi_syscall_counter_get_guard_period (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(77, counter_get_guard_period);
	ERRSC(counter_get_guard_period);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t flags = sca2;
	RETURN((uint32_t) counter_get_guard_period(dev,flags));
}

// 78 TODO
uint32_t wazi_syscall_counter_get_max_top_value (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(78, counter_get_max_top_value);
	ERRSC(counter_get_max_top_value);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) counter_get_max_top_value(dev));
}

// 79 TODO
uint8_t wazi_syscall_counter_get_num_of_channels (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(79, counter_get_num_of_channels);
	ERRSC(counter_get_num_of_channels);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint8_t) counter_get_num_of_channels(dev));
}

// 80 TODO
int wazi_syscall_counter_get_pending_int (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(80, counter_get_pending_int);
	ERRSC(counter_get_pending_int);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) counter_get_pending_int(dev));
}

// 81 TODO
uint32_t wazi_syscall_counter_get_top_value (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(81, counter_get_top_value);
	ERRSC(counter_get_top_value);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) counter_get_top_value(dev));
}

// 82 TODO
int wazi_syscall_counter_get_value (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(82, counter_get_value);
	ERRSC(counter_get_value);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t *ticks = (uint32_t*) MADDR(sca2);
	RETURN((int) counter_get_value(dev,ticks));
}

// 83 TODO
int wazi_syscall_counter_get_value_64 (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(83, counter_get_value_64);
	ERRSC(counter_get_value_64);
	struct device *dev = (struct device*) MADDR(sca1);
	uint64_t *ticks = (uint64_t*) MADDR(sca2);
	RETURN((int) counter_get_value_64(dev,ticks));
}

// 84 TODO
bool wazi_syscall_counter_is_counting_up (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(84, counter_is_counting_up);
	ERRSC(counter_is_counting_up);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((bool) counter_is_counting_up(dev));
}

// 85 TODO
int wazi_syscall_counter_set_channel_alarm (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, int32_t sca3) {
	SC(85, counter_set_channel_alarm);
	ERRSC(counter_set_channel_alarm);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t chan_id = sca2;
	struct counter_alarm_cfg *alarm_cfg = (struct counter_alarm_cfg*) MADDR(sca3);
	RETURN((int) counter_set_channel_alarm(dev,chan_id,alarm_cfg));
}

// 86 TODO
int wazi_syscall_counter_set_guard_period (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3) {
	SC(86, counter_set_guard_period);
	ERRSC(counter_set_guard_period);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t ticks = sca2;
	uint32_t flags = sca3;
	RETURN((int) counter_set_guard_period(dev,ticks,flags));
}

// 87 TODO
int wazi_syscall_counter_set_top_value (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(87, counter_set_top_value);
	ERRSC(counter_set_top_value);
	struct device *dev = (struct device*) MADDR(sca1);
	struct counter_top_cfg *cfg = (struct counter_top_cfg*) MADDR(sca2);
	RETURN((int) counter_set_top_value(dev,cfg));
}

// 88 TODO
int wazi_syscall_counter_start (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(88, counter_start);
	ERRSC(counter_start);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) counter_start(dev));
}

// 89 TODO
int wazi_syscall_counter_stop (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(89, counter_stop);
	ERRSC(counter_stop);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) counter_stop(dev));
}

// 90 TODO
uint64_t wazi_syscall_counter_ticks_to_us (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(90, counter_ticks_to_us);
	ERRSC(counter_ticks_to_us);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t ticks = sca2;
	RETURN((uint64_t) counter_ticks_to_us(dev,ticks));
}

// 91 TODO
uint32_t wazi_syscall_counter_us_to_ticks (wasm_exec_env_t exec_env, int32_t sca1, uint64_t sca2) {
	SC(91, counter_us_to_ticks);
	ERRSC(counter_us_to_ticks);
	struct device *dev = (struct device*) MADDR(sca1);
	uint64_t us = sca2;
	RETURN((uint32_t) counter_us_to_ticks(dev,us));
}

// 92 TODO
int wazi_syscall_dac_channel_setup (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(92, dac_channel_setup);
	ERRSC(dac_channel_setup);
	struct device *dev = (struct device*) MADDR(sca1);
	struct dac_channel_cfg *channel_cfg = (struct dac_channel_cfg*) MADDR(sca2);
	RETURN((int) dac_channel_setup(dev,channel_cfg));
}

// 93 TODO
int wazi_syscall_dac_write_value (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint32_t sca3) {
	SC(93, dac_write_value);
	ERRSC(dac_write_value);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t channel = sca2;
	uint32_t value = sca3;
	RETURN((int) dac_write_value(dev,channel,value));
}

// 94 TODO
int32_t wazi_syscall_device_get_binding (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(94, device_get_binding);
	ERRSC(device_get_binding);
	char *name = (char*) MADDR(sca1);
	RETURN((int32_t) device_get_binding(name));
}

// 95 TODO
bool wazi_syscall_device_is_ready (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(95, device_is_ready);
	ERRSC(device_is_ready);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((bool) device_is_ready(dev));
}

// 96 TODO
uint32_t wazi_syscall_devmux_select_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(96, devmux_select_get);
	ERRSC(devmux_select_get);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) devmux_select_get(dev));
}

// 97 TODO
int wazi_syscall_devmux_select_set (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(97, devmux_select_set);
	ERRSC(devmux_select_set);
	struct device *dev = (struct device*) MADDR(sca1);
	size_t index = sca2;
	RETURN((int) devmux_select_set(dev,index));
}

// 98 TODO
int wazi_syscall_dma_chan_filter (wasm_exec_env_t exec_env, int32_t sca1, int sca2, int32_t sca3) {
	SC(98, dma_chan_filter);
	ERRSC(dma_chan_filter);
	struct device *dev = (struct device*) MADDR(sca1);
	int channel = sca2;
	void *filter_param = (void*) MADDR(sca3);
	RETURN((int) dma_chan_filter(dev,channel,filter_param));
}

// 99 TODO
void wazi_syscall_dma_release_channel (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(99, dma_release_channel);
	ERRSC(dma_release_channel);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	RETURN_VOID(dma_release_channel(dev,channel));
}

// 100 TODO
int wazi_syscall_dma_request_channel (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(100, dma_request_channel);
	ERRSC(dma_request_channel);
	struct device *dev = (struct device*) MADDR(sca1);
	void *filter_param = (void*) MADDR(sca2);
	RETURN((int) dma_request_channel(dev,filter_param));
}

// 101 TODO
int wazi_syscall_dma_resume (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(101, dma_resume);
	ERRSC(dma_resume);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	RETURN((int) dma_resume(dev,channel));
}

// 102 TODO
int wazi_syscall_dma_start (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(102, dma_start);
	ERRSC(dma_start);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	RETURN((int) dma_start(dev,channel));
}

// 103 TODO
int wazi_syscall_dma_stop (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(103, dma_stop);
	ERRSC(dma_stop);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	RETURN((int) dma_stop(dev,channel));
}

// 104 TODO
int wazi_syscall_dma_suspend (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(104, dma_suspend);
	ERRSC(dma_suspend);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	RETURN((int) dma_suspend(dev,channel));
}

// 105 TODO
uint32_t wazi_syscall_eeprom_get_size (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(105, eeprom_get_size);
	ERRSC(eeprom_get_size);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) eeprom_get_size(dev));
}

// 106 TODO
int wazi_syscall_eeprom_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(106, eeprom_read);
	ERRSC(eeprom_read);
	struct device *dev = (struct device*) MADDR(sca1);
	off_t offset = (off_t) MADDR(sca2);
	void *data = (void*) MADDR(sca3);
	size_t len = sca4;
	RETURN((int) eeprom_read(dev,offset,data,len));
}

// 107 TODO
int wazi_syscall_eeprom_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(107, eeprom_write);
	ERRSC(eeprom_write);
	struct device *dev = (struct device*) MADDR(sca1);
	off_t offset = (off_t) MADDR(sca2);
	void *data = (void*) MADDR(sca3);
	size_t len = sca4;
	RETURN((int) eeprom_write(dev,offset,data,len));
}

// 108 TODO
int wazi_syscall_emul_fuel_gauge_is_battery_cutoff (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(108, emul_fuel_gauge_is_battery_cutoff);
	ERRSC(emul_fuel_gauge_is_battery_cutoff);
	struct emul *target = (struct emul*) MADDR(sca1);
	bool *cutoff = (bool*) MADDR(sca2);
	RETURN((int) emul_fuel_gauge_is_battery_cutoff(target,cutoff));
}

// 109 TODO
int wazi_syscall_emul_fuel_gauge_set_battery_charging (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int sca3) {
	SC(109, emul_fuel_gauge_set_battery_charging);
	ERRSC(emul_fuel_gauge_set_battery_charging);
	struct emul *target = (struct emul*) MADDR(sca1);
	uint32_t uV = sca2;
	int uA = sca3;
	RETURN((int) emul_fuel_gauge_set_battery_charging(target,uV,uA));
}

// 110 TODO
int wazi_syscall_entropy_get_entropy (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint16_t sca3) {
	SC(110, entropy_get_entropy);
	ERRSC(entropy_get_entropy);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t *buffer = (uint8_t*) MADDR(sca2);
	uint16_t length = sca3;
	RETURN((int) entropy_get_entropy(dev,buffer,length));
}

// 111 TODO
int wazi_syscall_espi_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(111, espi_config);
	ERRSC(espi_config);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_cfg *cfg = (struct espi_cfg*) MADDR(sca2);
	RETURN((int) espi_config(dev,cfg));
}

// 112 TODO
int wazi_syscall_espi_flash_erase (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(112, espi_flash_erase);
	ERRSC(espi_flash_erase);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_flash_packet *pckt = (struct espi_flash_packet*) MADDR(sca2);
	RETURN((int) espi_flash_erase(dev,pckt));
}

// 113 TODO
bool wazi_syscall_espi_get_channel_status (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(113, espi_get_channel_status);
	ERRSC(espi_get_channel_status);
	struct device *dev = (struct device*) MADDR(sca1);
	enum espi_channel ch = sca2;
	RETURN((bool) espi_get_channel_status(dev,ch));
}

// 114 TODO
int wazi_syscall_espi_read_flash (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(114, espi_read_flash);
	ERRSC(espi_read_flash);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_flash_packet *pckt = (struct espi_flash_packet*) MADDR(sca2);
	RETURN((int) espi_read_flash(dev,pckt));
}

// 115 TODO
int wazi_syscall_espi_read_lpc_request (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(115, espi_read_lpc_request);
	ERRSC(espi_read_lpc_request);
	struct device *dev = (struct device*) MADDR(sca1);
	enum lpc_peripheral_opcode op = sca2;
	uint32_t *data = (uint32_t*) MADDR(sca3);
	RETURN((int) espi_read_lpc_request(dev,op,data));
}

// 116 TODO
int wazi_syscall_espi_read_request (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(116, espi_read_request);
	ERRSC(espi_read_request);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_request_packet *req = (struct espi_request_packet*) MADDR(sca2);
	RETURN((int) espi_read_request(dev,req));
}

// 117 TODO
int wazi_syscall_espi_receive_oob (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(117, espi_receive_oob);
	ERRSC(espi_receive_oob);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_oob_packet *pckt = (struct espi_oob_packet*) MADDR(sca2);
	RETURN((int) espi_receive_oob(dev,pckt));
}

// 118 TODO
int wazi_syscall_espi_receive_vwire (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(118, espi_receive_vwire);
	ERRSC(espi_receive_vwire);
	struct device *dev = (struct device*) MADDR(sca1);
	enum espi_vwire_signal signal = sca2;
	uint8_t *level = (uint8_t*) MADDR(sca3);
	RETURN((int) espi_receive_vwire(dev,signal,level));
}

// 119 TODO
int wazi_syscall_espi_saf_activate (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(119, espi_saf_activate);
	ERRSC(espi_saf_activate);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) espi_saf_activate(dev));
}

// 120 TODO
int wazi_syscall_espi_saf_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(120, espi_saf_config);
	ERRSC(espi_saf_config);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_saf_cfg *cfg = (struct espi_saf_cfg*) MADDR(sca2);
	RETURN((int) espi_saf_config(dev,cfg));
}

// 121 TODO
int wazi_syscall_espi_saf_flash_erase (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(121, espi_saf_flash_erase);
	ERRSC(espi_saf_flash_erase);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_saf_packet *pckt = (struct espi_saf_packet*) MADDR(sca2);
	RETURN((int) espi_saf_flash_erase(dev,pckt));
}

// 122 TODO
int wazi_syscall_espi_saf_flash_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(122, espi_saf_flash_read);
	ERRSC(espi_saf_flash_read);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_saf_packet *pckt = (struct espi_saf_packet*) MADDR(sca2);
	RETURN((int) espi_saf_flash_read(dev,pckt));
}

// 123 TODO
int wazi_syscall_espi_saf_flash_unsuccess (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(123, espi_saf_flash_unsuccess);
	ERRSC(espi_saf_flash_unsuccess);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_saf_packet *pckt = (struct espi_saf_packet*) MADDR(sca2);
	RETURN((int) espi_saf_flash_unsuccess(dev,pckt));
}

// 124 TODO
int wazi_syscall_espi_saf_flash_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(124, espi_saf_flash_write);
	ERRSC(espi_saf_flash_write);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_saf_packet *pckt = (struct espi_saf_packet*) MADDR(sca2);
	RETURN((int) espi_saf_flash_write(dev,pckt));
}

// 125 TODO
bool wazi_syscall_espi_saf_get_channel_status (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(125, espi_saf_get_channel_status);
	ERRSC(espi_saf_get_channel_status);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((bool) espi_saf_get_channel_status(dev));
}

// 126 TODO
int wazi_syscall_espi_saf_set_protection_regions (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(126, espi_saf_set_protection_regions);
	ERRSC(espi_saf_set_protection_regions);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_saf_protection *pr = (struct espi_saf_protection*) MADDR(sca2);
	RETURN((int) espi_saf_set_protection_regions(dev,pr));
}

// 127 TODO
int wazi_syscall_espi_send_oob (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(127, espi_send_oob);
	ERRSC(espi_send_oob);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_oob_packet *pckt = (struct espi_oob_packet*) MADDR(sca2);
	RETURN((int) espi_send_oob(dev,pckt));
}

// 128 TODO
int wazi_syscall_espi_send_vwire (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint8_t sca3) {
	SC(128, espi_send_vwire);
	ERRSC(espi_send_vwire);
	struct device *dev = (struct device*) MADDR(sca1);
	enum espi_vwire_signal signal = sca2;
	uint8_t level = sca3;
	RETURN((int) espi_send_vwire(dev,signal,level));
}

// 129 TODO
int wazi_syscall_espi_write_flash (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(129, espi_write_flash);
	ERRSC(espi_write_flash);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_flash_packet *pckt = (struct espi_flash_packet*) MADDR(sca2);
	RETURN((int) espi_write_flash(dev,pckt));
}

// 130 TODO
int wazi_syscall_espi_write_lpc_request (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(130, espi_write_lpc_request);
	ERRSC(espi_write_lpc_request);
	struct device *dev = (struct device*) MADDR(sca1);
	enum lpc_peripheral_opcode op = sca2;
	uint32_t *data = (uint32_t*) MADDR(sca3);
	RETURN((int) espi_write_lpc_request(dev,op,data));
}

// 131 TODO
int wazi_syscall_espi_write_request (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(131, espi_write_request);
	ERRSC(espi_write_request);
	struct device *dev = (struct device*) MADDR(sca1);
	struct espi_request_packet *req = (struct espi_request_packet*) MADDR(sca2);
	RETURN((int) espi_write_request(dev,req));
}

// 132 TODO
int wazi_syscall_flash_erase (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(132, flash_erase);
	ERRSC(flash_erase);
	struct device *dev = (struct device*) MADDR(sca1);
	off_t offset = (off_t) MADDR(sca2);
	size_t size = sca3;
	RETURN((int) flash_erase(dev,offset,size));
}

// 133 TODO
int wazi_syscall_flash_ex_op (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint32_t sca3, int32_t sca4) {
	SC(133, flash_ex_op);
	ERRSC(flash_ex_op);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t code = sca2;
	uintptr_t in = (uintptr_t) MADDR(sca3);
	void *out = (void*) MADDR(sca4);
	RETURN((int) flash_ex_op(dev,code,in,out));
}

// 134 TODO
uint32_t wazi_syscall_flash_get_page_count (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(134, flash_get_page_count);
	ERRSC(flash_get_page_count);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) flash_get_page_count(dev));
}

// 135 TODO
int wazi_syscall_flash_get_page_info_by_idx (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3) {
	SC(135, flash_get_page_info_by_idx);
	ERRSC(flash_get_page_info_by_idx);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t page_index = sca2;
	struct flash_pages_info *info = (struct flash_pages_info*) MADDR(sca3);
	RETURN((int) flash_get_page_info_by_idx(dev,page_index,info));
}

// 136 TODO
int wazi_syscall_flash_get_page_info_by_offs (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(136, flash_get_page_info_by_offs);
	ERRSC(flash_get_page_info_by_offs);
	struct device *dev = (struct device*) MADDR(sca1);
	off_t offset = (off_t) MADDR(sca2);
	struct flash_pages_info *info = (struct flash_pages_info*) MADDR(sca3);
	RETURN((int) flash_get_page_info_by_offs(dev,offset,info));
}

// 137 TODO
int32_t wazi_syscall_flash_get_parameters (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(137, flash_get_parameters);
	ERRSC(flash_get_parameters);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int32_t) flash_get_parameters(dev));
}

// 138 TODO
uint32_t wazi_syscall_flash_get_write_block_size (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(138, flash_get_write_block_size);
	ERRSC(flash_get_write_block_size);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) flash_get_write_block_size(dev));
}

// 139 TODO
int wazi_syscall_flash_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(139, flash_read);
	ERRSC(flash_read);
	struct device *dev = (struct device*) MADDR(sca1);
	off_t offset = (off_t) MADDR(sca2);
	void *data = (void*) MADDR(sca3);
	size_t len = sca4;
	RETURN((int) flash_read(dev,offset,data,len));
}

// 140 TODO
int wazi_syscall_flash_read_jedec_id (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(140, flash_read_jedec_id);
	ERRSC(flash_read_jedec_id);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t *id = (uint8_t*) MADDR(sca2);
	RETURN((int) flash_read_jedec_id(dev,id));
}

// 141 TODO
int wazi_syscall_flash_sfdp_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(141, flash_sfdp_read);
	ERRSC(flash_sfdp_read);
	struct device *dev = (struct device*) MADDR(sca1);
	off_t offset = (off_t) MADDR(sca2);
	void *data = (void*) MADDR(sca3);
	size_t len = sca4;
	RETURN((int) flash_sfdp_read(dev,offset,data,len));
}

// 142 TODO
int32_t wazi_syscall_flash_simulator_get_memory (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(142, flash_simulator_get_memory);
	ERRSC(flash_simulator_get_memory);
	struct device *dev = (struct device*) MADDR(sca1);
	size_t *mock_size = (size_t*) MADDR(sca2);
	RETURN((int32_t) flash_simulator_get_memory(dev,mock_size));
}

// 143 TODO
int wazi_syscall_flash_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(143, flash_write);
	ERRSC(flash_write);
	struct device *dev = (struct device*) MADDR(sca1);
	off_t offset = (off_t) MADDR(sca2);
	void *data = (void*) MADDR(sca3);
	size_t len = sca4;
	RETURN((int) flash_write(dev,offset,data,len));
}

// 144 TODO
int wazi_syscall_fuel_gauge_battery_cutoff (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(144, fuel_gauge_battery_cutoff);
	ERRSC(fuel_gauge_battery_cutoff);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) fuel_gauge_battery_cutoff(dev));
}

// 145 TODO
int wazi_syscall_fuel_gauge_get_buffer_prop (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3, uint32_t sca4) {
	SC(145, fuel_gauge_get_buffer_prop);
	ERRSC(fuel_gauge_get_buffer_prop);
	struct device *dev = (struct device*) MADDR(sca1);
	fuel_gauge_prop_t prop_type = sca2;
	void *dst = (void*) MADDR(sca3);
	size_t dst_len = sca4;
	RETURN((int) fuel_gauge_get_buffer_prop(dev,prop_type,dst,dst_len));
}

// 146 TODO
int wazi_syscall_fuel_gauge_get_prop (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3) {
	SC(146, fuel_gauge_get_prop);
	ERRSC(fuel_gauge_get_prop);
	struct device *dev = (struct device*) MADDR(sca1);
	fuel_gauge_prop_t prop = sca2;
	union fuel_gauge_prop_val *val = (union fuel_gauge_prop_val*) MADDR(sca3);
	RETURN((int) fuel_gauge_get_prop(dev,prop,val));
}

// 147 TODO
int wazi_syscall_fuel_gauge_get_props (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(147, fuel_gauge_get_props);
	ERRSC(fuel_gauge_get_props);
	struct device *dev = (struct device*) MADDR(sca1);
	fuel_gauge_prop_t *props = (fuel_gauge_prop_t*) MADDR(sca2);
	union fuel_gauge_prop_val *vals = (union fuel_gauge_prop_val*) MADDR(sca3);
	size_t len = sca4;
	RETURN((int) fuel_gauge_get_props(dev,props,vals,len));
}

// 148 TODO
int wazi_syscall_fuel_gauge_set_prop (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3) {
	SC(148, fuel_gauge_set_prop);
	ERRSC(fuel_gauge_set_prop);
	struct device *dev = (struct device*) MADDR(sca1);
	fuel_gauge_prop_t prop = sca2;
	union fuel_gauge_prop_val val = { .chg_current = sca3 };
	RETURN((int) fuel_gauge_set_prop(dev,prop,val));
}

// 149 TODO
int wazi_syscall_fuel_gauge_set_props (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(149, fuel_gauge_set_props);
	ERRSC(fuel_gauge_set_props);
	struct device *dev = (struct device*) MADDR(sca1);
	fuel_gauge_prop_t *props = (fuel_gauge_prop_t*) MADDR(sca2);
	union fuel_gauge_prop_val *vals = (union fuel_gauge_prop_val*) MADDR(sca3);
	size_t len = sca4;
	RETURN((int) fuel_gauge_set_props(dev,props,vals,len));
}

// 150 TODO
int wazi_syscall_gnss_get_enabled_systems (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(150, gnss_get_enabled_systems);
	ERRSC(gnss_get_enabled_systems);
	struct device *dev = (struct device*) MADDR(sca1);
	gnss_systems_t *systems = (gnss_systems_t*) MADDR(sca2);
	RETURN((int) gnss_get_enabled_systems(dev,systems));
}

// 151 TODO
int wazi_syscall_gnss_get_fix_rate (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(151, gnss_get_fix_rate);
	ERRSC(gnss_get_fix_rate);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t *fix_interval_ms = (uint32_t*) MADDR(sca2);
	RETURN((int) gnss_get_fix_rate(dev,fix_interval_ms));
}

// 152 TODO
int wazi_syscall_gnss_get_navigation_mode (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(152, gnss_get_navigation_mode);
	ERRSC(gnss_get_navigation_mode);
	struct device *dev = (struct device*) MADDR(sca1);
	enum gnss_navigation_mode *mode = (enum gnss_navigation_mode*) MADDR(sca2);
	RETURN((int) gnss_get_navigation_mode(dev,mode));
}

// 153 TODO
int wazi_syscall_gnss_get_periodic_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(153, gnss_get_periodic_config);
	ERRSC(gnss_get_periodic_config);
	struct device *dev = (struct device*) MADDR(sca1);
	struct gnss_periodic_config *config = (struct gnss_periodic_config*) MADDR(sca2);
	RETURN((int) gnss_get_periodic_config(dev,config));
}

// 154 TODO
int wazi_syscall_gnss_get_supported_systems (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(154, gnss_get_supported_systems);
	ERRSC(gnss_get_supported_systems);
	struct device *dev = (struct device*) MADDR(sca1);
	gnss_systems_t *systems = (gnss_systems_t*) MADDR(sca2);
	RETURN((int) gnss_get_supported_systems(dev,systems));
}

// 155 TODO
int wazi_syscall_gnss_set_enabled_systems (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(155, gnss_set_enabled_systems);
	ERRSC(gnss_set_enabled_systems);
	struct device *dev = (struct device*) MADDR(sca1);
	gnss_systems_t systems = sca2;
	RETURN((int) gnss_set_enabled_systems(dev,systems));
}

// 156 TODO
int wazi_syscall_gnss_set_fix_rate (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(156, gnss_set_fix_rate);
	ERRSC(gnss_set_fix_rate);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t fix_interval_ms = sca2;
	RETURN((int) gnss_set_fix_rate(dev,fix_interval_ms));
}

// 157 TODO
int wazi_syscall_gnss_set_navigation_mode (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(157, gnss_set_navigation_mode);
	ERRSC(gnss_set_navigation_mode);
	struct device *dev = (struct device*) MADDR(sca1);
	enum gnss_navigation_mode mode = sca2;
	RETURN((int) gnss_set_navigation_mode(dev,mode));
}

// 158 TODO
int wazi_syscall_gnss_set_periodic_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(158, gnss_set_periodic_config);
	ERRSC(gnss_set_periodic_config);
	struct device *dev = (struct device*) MADDR(sca1);
	struct gnss_periodic_config *config = (struct gnss_periodic_config*) MADDR(sca2);
	RETURN((int) gnss_set_periodic_config(dev,config));
}

// 159 TODO
int wazi_syscall_gpio_get_pending_int (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(159, gpio_get_pending_int);
	ERRSC(gpio_get_pending_int);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) gpio_get_pending_int(dev));
}

// 160 TODO
int wazi_syscall_gpio_pin_configure (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint32_t sca3) {
	SC(160, gpio_pin_configure);
	ERRSC(gpio_pin_configure);
	struct device *port = (struct device*) MADDR(sca1);
	gpio_pin_t pin = sca2;
	gpio_flags_t flags = sca3;
	RETURN((int) gpio_pin_configure(port,pin,flags));
}

// 161 TODO
int wazi_syscall_gpio_pin_get_config (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, int32_t sca3) {
	SC(161, gpio_pin_get_config);
	ERRSC(gpio_pin_get_config);
	struct device *port = (struct device*) MADDR(sca1);
	gpio_pin_t pin = sca2;
	gpio_flags_t *flags = (gpio_flags_t*) MADDR(sca3);
	RETURN((int) gpio_pin_get_config(port,pin,flags));
}

// 162 TODO
int wazi_syscall_gpio_pin_interrupt_configure (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint32_t sca3) {
	SC(162, gpio_pin_interrupt_configure);
	ERRSC(gpio_pin_interrupt_configure);
	struct device *port = (struct device*) MADDR(sca1);
	gpio_pin_t pin = sca2;
	gpio_flags_t flags = sca3;
	RETURN((int) gpio_pin_interrupt_configure(port,pin,flags));
}

// 163 TODO
int wazi_syscall_gpio_port_clear_bits_raw (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(163, gpio_port_clear_bits_raw);
	ERRSC(gpio_port_clear_bits_raw);
	struct device *port = (struct device*) MADDR(sca1);
	gpio_port_pins_t pins = sca2;
	RETURN((int) gpio_port_clear_bits_raw(port,pins));
}

// 164 TODO
int wazi_syscall_gpio_port_get_direction (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3, int32_t sca4) {
	SC(164, gpio_port_get_direction);
	ERRSC(gpio_port_get_direction);
	struct device *port = (struct device*) MADDR(sca1);
	gpio_port_pins_t map = sca2;
	gpio_port_pins_t *inputs = (gpio_port_pins_t*) MADDR(sca3);
	gpio_port_pins_t *outputs = (gpio_port_pins_t*) MADDR(sca4);
	RETURN((int) gpio_port_get_direction(port,map,inputs,outputs));
}

// 165 TODO
int wazi_syscall_gpio_port_get_raw (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(165, gpio_port_get_raw);
	ERRSC(gpio_port_get_raw);
	struct device *port = (struct device*) MADDR(sca1);
	gpio_port_value_t *value = (gpio_port_value_t*) MADDR(sca2);
	RETURN((int) gpio_port_get_raw(port,value));
}

// 166 TODO
int wazi_syscall_gpio_port_set_bits_raw (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(166, gpio_port_set_bits_raw);
	ERRSC(gpio_port_set_bits_raw);
	struct device *port = (struct device*) MADDR(sca1);
	gpio_port_pins_t pins = sca2;
	RETURN((int) gpio_port_set_bits_raw(port,pins));
}

// 167 TODO
int wazi_syscall_gpio_port_set_masked_raw (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3) {
	SC(167, gpio_port_set_masked_raw);
	ERRSC(gpio_port_set_masked_raw);
	struct device *port = (struct device*) MADDR(sca1);
	gpio_port_pins_t mask = sca2;
	gpio_port_value_t value = sca3;
	RETURN((int) gpio_port_set_masked_raw(port,mask,value));
}

// 168 TODO
int wazi_syscall_gpio_port_toggle_bits (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(168, gpio_port_toggle_bits);
	ERRSC(gpio_port_toggle_bits);
	struct device *port = (struct device*) MADDR(sca1);
	gpio_port_pins_t pins = sca2;
	RETURN((int) gpio_port_toggle_bits(port,pins));
}

// 169 TODO
int wazi_syscall_hwinfo_clear_reset_cause (wasm_exec_env_t exec_env) {
	SC(169, hwinfo_clear_reset_cause);
	ERRSC(hwinfo_clear_reset_cause);
	RETURN((int) hwinfo_clear_reset_cause());
}

// 170 TODO
uint32_t wazi_syscall_hwinfo_get_device_id (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(170, hwinfo_get_device_id);
	ERRSC(hwinfo_get_device_id);
	uint8_t *buffer = (uint8_t*) MADDR(sca1);
	size_t length = sca2;
	RETURN((uint32_t) hwinfo_get_device_id(buffer,length));
}

// 171 TODO
int wazi_syscall_hwinfo_get_reset_cause (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(171, hwinfo_get_reset_cause);
	ERRSC(hwinfo_get_reset_cause);
	uint32_t *cause = (uint32_t*) MADDR(sca1);
	RETURN((int) hwinfo_get_reset_cause(cause));
}

// 172 TODO
int wazi_syscall_hwinfo_get_supported_reset_cause (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(172, hwinfo_get_supported_reset_cause);
	ERRSC(hwinfo_get_supported_reset_cause);
	uint32_t *supported = (uint32_t*) MADDR(sca1);
	RETURN((int) hwinfo_get_supported_reset_cause(supported));
}

// 173 TODO
uint32_t wazi_syscall_hwspinlock_get_max_id (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(173, hwspinlock_get_max_id);
	ERRSC(hwspinlock_get_max_id);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) hwspinlock_get_max_id(dev));
}

// 174 TODO
void wazi_syscall_hwspinlock_lock (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(174, hwspinlock_lock);
	ERRSC(hwspinlock_lock);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t id = sca2;
	RETURN_VOID(hwspinlock_lock(dev,id));
}

// 175 TODO
int wazi_syscall_hwspinlock_trylock (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(175, hwspinlock_trylock);
	ERRSC(hwspinlock_trylock);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t id = sca2;
	RETURN((int) hwspinlock_trylock(dev,id));
}

// 176 TODO
void wazi_syscall_hwspinlock_unlock (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(176, hwspinlock_unlock);
	ERRSC(hwspinlock_unlock);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t id = sca2;
	RETURN_VOID(hwspinlock_unlock(dev,id));
}

// 177 TODO
int wazi_syscall_i2c_configure (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(177, i2c_configure);
	ERRSC(i2c_configure);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t dev_config = sca2;
	RETURN((int) i2c_configure(dev,dev_config));
}

// 178 TODO
int wazi_syscall_i2c_get_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(178, i2c_get_config);
	ERRSC(i2c_get_config);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t *dev_config = (uint32_t*) MADDR(sca2);
	RETURN((int) i2c_get_config(dev,dev_config));
}

// 179 TODO
int wazi_syscall_i2c_recover_bus (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(179, i2c_recover_bus);
	ERRSC(i2c_recover_bus);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) i2c_recover_bus(dev));
}

// 180 TODO
int wazi_syscall_i2c_target_driver_register (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(180, i2c_target_driver_register);
	ERRSC(i2c_target_driver_register);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) i2c_target_driver_register(dev));
}

// 181 TODO
int wazi_syscall_i2c_target_driver_unregister (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(181, i2c_target_driver_unregister);
	ERRSC(i2c_target_driver_unregister);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) i2c_target_driver_unregister(dev));
}

// 182 TODO
int wazi_syscall_i2c_transfer (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint8_t sca3, uint16_t sca4) {
	SC(182, i2c_transfer);
	ERRSC(i2c_transfer);
	struct device *dev = (struct device*) MADDR(sca1);
	struct i2c_msg *msgs = (struct i2c_msg*) MADDR(sca2);
	uint8_t num_msgs = sca3;
	uint16_t addr = sca4;
	RETURN((int) i2c_transfer(dev,msgs,num_msgs,addr));
}

// 183 TODO
int wazi_syscall_i2s_buf_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(183, i2s_buf_read);
	ERRSC(i2s_buf_read);
	struct device *dev = (struct device*) MADDR(sca1);
	void *buf = (void*) MADDR(sca2);
	size_t *size = (size_t*) MADDR(sca3);
	RETURN((int) i2s_buf_read(dev,buf,size));
}

// 184 TODO
int wazi_syscall_i2s_buf_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(184, i2s_buf_write);
	ERRSC(i2s_buf_write);
	struct device *dev = (struct device*) MADDR(sca1);
	void *buf = (void*) MADDR(sca2);
	size_t size = sca3;
	RETURN((int) i2s_buf_write(dev,buf,size));
}

// 185 TODO
int wazi_syscall_i2s_configure (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(185, i2s_configure);
	ERRSC(i2s_configure);
	struct device *dev = (struct device*) MADDR(sca1);
	enum i2s_dir dir = sca2;
	struct i2s_config *cfg = (struct i2s_config*) MADDR(sca3);
	RETURN((int) i2s_configure(dev,dir,cfg));
}

// 186 TODO
int wazi_syscall_i2s_trigger (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(186, i2s_trigger);
	ERRSC(i2s_trigger);
	struct device *dev = (struct device*) MADDR(sca1);
	enum i2s_dir dir = sca2;
	enum i2s_trigger_cmd cmd = sca3;
	RETURN((int) i2s_trigger(dev,dir,cmd));
}

// 187 TODO
int wazi_syscall_i3c_do_ccc (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(187, i3c_do_ccc);
	ERRSC(i3c_do_ccc);
	struct device *dev = (struct device*) MADDR(sca1);
	struct i3c_ccc_payload *payload = (struct i3c_ccc_payload*) MADDR(sca2);
	RETURN((int) i3c_do_ccc(dev,payload));
}

// 188 TODO
int wazi_syscall_i3c_transfer (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint8_t sca3) {
	SC(188, i3c_transfer);
	ERRSC(i3c_transfer);
	struct i3c_device_desc *target = (struct i3c_device_desc*) MADDR(sca1);
	struct i3c_msg *msgs = (struct i3c_msg*) MADDR(sca2);
	uint8_t num_msgs = sca3;
	RETURN((int) i3c_transfer(target,msgs,num_msgs));
}

// 189 TODO
void wazi_syscall_ipm_complete (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(189, ipm_complete);
	ERRSC(ipm_complete);
	struct device *ipmdev = (struct device*) MADDR(sca1);
	RETURN_VOID(ipm_complete(ipmdev));
}

// 190 TODO
int wazi_syscall_ipm_max_data_size_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(190, ipm_max_data_size_get);
	ERRSC(ipm_max_data_size_get);
	struct device *ipmdev = (struct device*) MADDR(sca1);
	RETURN((int) ipm_max_data_size_get(ipmdev));
}

// 191 TODO
uint32_t wazi_syscall_ipm_max_id_val_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(191, ipm_max_id_val_get);
	ERRSC(ipm_max_id_val_get);
	struct device *ipmdev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) ipm_max_id_val_get(ipmdev));
}

// 192 TODO
int wazi_syscall_ipm_send (wasm_exec_env_t exec_env, int32_t sca1, int sca2, uint32_t sca3, int32_t sca4, int sca5) {
	SC(192, ipm_send);
	ERRSC(ipm_send);
	struct device *ipmdev = (struct device*) MADDR(sca1);
	int wait = sca2;
	uint32_t id = sca3;
	void *data = (void*) MADDR(sca4);
	int size = sca5;
	RETURN((int) ipm_send(ipmdev,wait,id,data,size));
}

// 193 TODO
int wazi_syscall_ipm_set_enabled (wasm_exec_env_t exec_env, int32_t sca1, int sca2) {
	SC(193, ipm_set_enabled);
	ERRSC(ipm_set_enabled);
	struct device *ipmdev = (struct device*) MADDR(sca1);
	int enable = sca2;
	RETURN((int) ipm_set_enabled(ipmdev,enable));
}

// 194 TODO
int wazi_syscall_ivshmem_enable_interrupts (wasm_exec_env_t exec_env, int32_t sca1, bool sca2) {
	SC(194, ivshmem_enable_interrupts);
	ERRSC(ivshmem_enable_interrupts);
	struct device *dev = (struct device*) MADDR(sca1);
	bool enable = sca2;
	RETURN((int) ivshmem_enable_interrupts(dev,enable));
}

// 195 TODO
uint32_t wazi_syscall_ivshmem_get_id (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(195, ivshmem_get_id);
	ERRSC(ivshmem_get_id);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) ivshmem_get_id(dev));
}

// 196 TODO
uint32_t wazi_syscall_ivshmem_get_max_peers (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(196, ivshmem_get_max_peers);
	ERRSC(ivshmem_get_max_peers);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) ivshmem_get_max_peers(dev));
}

// 197 TODO
uint32_t wazi_syscall_ivshmem_get_mem (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(197, ivshmem_get_mem);
	ERRSC(ivshmem_get_mem);
	struct device *dev = (struct device*) MADDR(sca1);
	uintptr_t *memmap = (uintptr_t*) MADDR(sca2);
	RETURN((uint32_t) ivshmem_get_mem(dev,memmap));
}

// 198 TODO
uint32_t wazi_syscall_ivshmem_get_output_mem_section (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3) {
	SC(198, ivshmem_get_output_mem_section);
	ERRSC(ivshmem_get_output_mem_section);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t peer_id = sca2;
	uintptr_t *memmap = (uintptr_t*) MADDR(sca3);
	RETURN((uint32_t) ivshmem_get_output_mem_section(dev,peer_id,memmap));
}

// 199 TODO
uint16_t wazi_syscall_ivshmem_get_protocol (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(199, ivshmem_get_protocol);
	ERRSC(ivshmem_get_protocol);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint16_t) ivshmem_get_protocol(dev));
}

// 200 TODO
uint32_t wazi_syscall_ivshmem_get_rw_mem_section (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(200, ivshmem_get_rw_mem_section);
	ERRSC(ivshmem_get_rw_mem_section);
	struct device *dev = (struct device*) MADDR(sca1);
	uintptr_t *memmap = (uintptr_t*) MADDR(sca2);
	RETURN((uint32_t) ivshmem_get_rw_mem_section(dev,memmap));
}

// 201 TODO
uint32_t wazi_syscall_ivshmem_get_state (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(201, ivshmem_get_state);
	ERRSC(ivshmem_get_state);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t peer_id = sca2;
	RETURN((uint32_t) ivshmem_get_state(dev,peer_id));
}

// 202 TODO
uint16_t wazi_syscall_ivshmem_get_vectors (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(202, ivshmem_get_vectors);
	ERRSC(ivshmem_get_vectors);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint16_t) ivshmem_get_vectors(dev));
}

// 203 TODO
int wazi_syscall_ivshmem_int_peer (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint16_t sca3) {
	SC(203, ivshmem_int_peer);
	ERRSC(ivshmem_int_peer);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t peer_id = sca2;
	uint16_t vector = sca3;
	RETURN((int) ivshmem_int_peer(dev,peer_id,vector));
}

// 204 TODO
int wazi_syscall_ivshmem_register_handler (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint16_t sca3) {
	SC(204, ivshmem_register_handler);
	ERRSC(ivshmem_register_handler);
	struct device *dev = (struct device*) MADDR(sca1);
	struct k_poll_signal *signal = (struct k_poll_signal*) MADDR(sca2);
	uint16_t vector = sca3;
	RETURN((int) ivshmem_register_handler(dev,signal,vector));
}

// 205 TODO
int wazi_syscall_ivshmem_set_state (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(205, ivshmem_set_state);
	ERRSC(ivshmem_set_state);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t state = sca2;
	RETURN((int) ivshmem_set_state(dev,state));
}

// 206 TODO
int wazi_syscall_kscan_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(206, kscan_config);
	ERRSC(kscan_config);
	struct device *dev = (struct device*) MADDR(sca1);
	kscan_callback_t callback = ERROR_FN_PTR;
	RETURN((int) kscan_config(dev,callback));
}

// 207 TODO
int wazi_syscall_kscan_disable_callback (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(207, kscan_disable_callback);
	ERRSC(kscan_disable_callback);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) kscan_disable_callback(dev));
}

// 208 TODO
int wazi_syscall_kscan_enable_callback (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(208, kscan_enable_callback);
	ERRSC(kscan_enable_callback);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) kscan_enable_callback(dev));
}

// 209 TODO
void wazi_syscall_k_busy_wait (wasm_exec_env_t exec_env, uint32_t sca1) {
	SC(209, k_busy_wait);
	ERRSC(k_busy_wait);
	uint32_t usec_to_wait = sca1;
	RETURN_VOID(k_busy_wait(usec_to_wait));
}

// 210 TODO
int wazi_syscall_k_condvar_broadcast (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(210, k_condvar_broadcast);
	ERRSC(k_condvar_broadcast);
	struct k_condvar *condvar = (struct k_condvar*) MADDR(sca1);
	RETURN((int) k_condvar_broadcast(condvar));
}

// 211 TODO
int wazi_syscall_k_condvar_init (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(211, k_condvar_init);
	ERRSC(k_condvar_init);
	struct k_condvar *condvar = (struct k_condvar*) MADDR(sca1);
	RETURN((int) k_condvar_init(condvar));
}

// 212 TODO
int wazi_syscall_k_condvar_signal (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(212, k_condvar_signal);
	ERRSC(k_condvar_signal);
	struct k_condvar *condvar = (struct k_condvar*) MADDR(sca1);
	RETURN((int) k_condvar_signal(condvar));
}

// 213 TODO
int wazi_syscall_k_condvar_wait (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(213, k_condvar_wait);
	ERRSC(k_condvar_wait);
	struct k_condvar *condvar = (struct k_condvar*) MADDR(sca1);
	struct k_mutex *mutex = (struct k_mutex*) MADDR(sca2);
	k_timeout_t timeout = { .ticks = sca3 };
	RETURN((int) k_condvar_wait(condvar,mutex,timeout));
}

// 214 TODO
uint32_t wazi_syscall_k_event_clear (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(214, k_event_clear);
	ERRSC(k_event_clear);
	struct k_event *event = (struct k_event*) MADDR(sca1);
	uint32_t events = sca2;
	RETURN((uint32_t) k_event_clear(event,events));
}

// 215 TODO
void wazi_syscall_k_event_init (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(215, k_event_init);
	ERRSC(k_event_init);
	struct k_event *event = (struct k_event*) MADDR(sca1);
	RETURN_VOID(k_event_init(event));
}

// 216 TODO
uint32_t wazi_syscall_k_event_post (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(216, k_event_post);
	ERRSC(k_event_post);
	struct k_event *event = (struct k_event*) MADDR(sca1);
	uint32_t events = sca2;
	RETURN((uint32_t) k_event_post(event,events));
}

// 217 TODO
uint32_t wazi_syscall_k_event_set (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(217, k_event_set);
	ERRSC(k_event_set);
	struct k_event *event = (struct k_event*) MADDR(sca1);
	uint32_t events = sca2;
	RETURN((uint32_t) k_event_set(event,events));
}

// 218 TODO
uint32_t wazi_syscall_k_event_set_masked (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3) {
	SC(218, k_event_set_masked);
	ERRSC(k_event_set_masked);
	struct k_event *event = (struct k_event*) MADDR(sca1);
	uint32_t events = sca2;
	uint32_t events_mask = sca3;
	RETURN((uint32_t) k_event_set_masked(event,events,events_mask));
}

// 219 TODO
uint32_t wazi_syscall_k_event_wait (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, bool sca3, uint32_t sca4) {
	SC(219, k_event_wait);
	ERRSC(k_event_wait);
	struct k_event *event = (struct k_event*) MADDR(sca1);
	uint32_t events = sca2;
	bool reset = sca3;
	k_timeout_t timeout = { .ticks = sca4 };
	RETURN((uint32_t) k_event_wait(event,events,reset,timeout));
}

// 220 TODO
uint32_t wazi_syscall_k_event_wait_all (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, bool sca3, uint32_t sca4) {
	SC(220, k_event_wait_all);
	ERRSC(k_event_wait_all);
	struct k_event *event = (struct k_event*) MADDR(sca1);
	uint32_t events = sca2;
	bool reset = sca3;
	k_timeout_t timeout = { .ticks = sca4 };
	RETURN((uint32_t) k_event_wait_all(event,events,reset,timeout));
}

// 221 TODO
int wazi_syscall_k_float_disable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(221, k_float_disable);
	ERRSC(k_float_disable);
	struct k_thread *thread = (struct k_thread*) MADDR(sca1);
	RETURN((int) k_float_disable(thread));
}

// 222 TODO
int wazi_syscall_k_float_enable (wasm_exec_env_t exec_env, int32_t sca1, unsigned int sca2) {
	SC(222, k_float_enable);
	ERRSC(k_float_enable);
	struct k_thread *thread = (struct k_thread*) MADDR(sca1);
	unsigned int options = sca2;
	RETURN((int) k_float_enable(thread,options));
}

// 223 TODO
int wazi_syscall_k_futex_wait (wasm_exec_env_t exec_env, int32_t sca1, int sca2, uint32_t sca3) {
	SC(223, k_futex_wait);
	ERRSC(k_futex_wait);
	struct k_futex *futex = (struct k_futex*) MADDR(sca1);
	int expected = sca2;
	k_timeout_t timeout = { .ticks = sca3 };
	RETURN((int) k_futex_wait(futex,expected,timeout));
}

// 224 TODO
int wazi_syscall_k_futex_wake (wasm_exec_env_t exec_env, int32_t sca1, bool sca2) {
	SC(224, k_futex_wake);
	ERRSC(k_futex_wake);
	struct k_futex *futex = (struct k_futex*) MADDR(sca1);
	bool wake_all = sca2;
	RETURN((int) k_futex_wake(futex,wake_all));
}

// 225 TODO
int wazi_syscall_k_is_preempt_thread (wasm_exec_env_t exec_env) {
	SC(225, k_is_preempt_thread);
	ERRSC(k_is_preempt_thread);
	RETURN((int) k_is_preempt_thread());
}

// 226 TODO
void wazi_syscall_k_mem_paging_histogram_backing_store_page_in_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(226, k_mem_paging_histogram_backing_store_page_in_get);
	ERRSC(k_mem_paging_histogram_backing_store_page_in_get);
	struct k_mem_paging_histogram_t *hist = (struct k_mem_paging_histogram_t*) MADDR(sca1);
	RETURN_VOID(k_mem_paging_histogram_backing_store_page_in_get(hist));
}

// 227 TODO
void wazi_syscall_k_mem_paging_histogram_backing_store_page_out_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(227, k_mem_paging_histogram_backing_store_page_out_get);
	ERRSC(k_mem_paging_histogram_backing_store_page_out_get);
	struct k_mem_paging_histogram_t *hist = (struct k_mem_paging_histogram_t*) MADDR(sca1);
	RETURN_VOID(k_mem_paging_histogram_backing_store_page_out_get(hist));
}

// 228 TODO
void wazi_syscall_k_mem_paging_histogram_eviction_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(228, k_mem_paging_histogram_eviction_get);
	ERRSC(k_mem_paging_histogram_eviction_get);
	struct k_mem_paging_histogram_t *hist = (struct k_mem_paging_histogram_t*) MADDR(sca1);
	RETURN_VOID(k_mem_paging_histogram_eviction_get(hist));
}

// 229 TODO
void wazi_syscall_k_mem_paging_stats_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(229, k_mem_paging_stats_get);
	ERRSC(k_mem_paging_stats_get);
	struct k_mem_paging_stats_t *stats = (struct k_mem_paging_stats_t*) MADDR(sca1);
	RETURN_VOID(k_mem_paging_stats_get(stats));
}

// 230 TODO
void wazi_syscall_k_mem_paging_thread_stats_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(230, k_mem_paging_thread_stats_get);
	ERRSC(k_mem_paging_thread_stats_get);
	struct k_thread *thread = (struct k_thread*) MADDR(sca1);
	struct k_mem_paging_stats_t *stats = (struct k_mem_paging_stats_t*) MADDR(sca2);
	RETURN_VOID(k_mem_paging_thread_stats_get(thread,stats));
}

// 231 TODO
int wazi_syscall_k_msgq_alloc_init (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3) {
	SC(231, k_msgq_alloc_init);
	ERRSC(k_msgq_alloc_init);
	struct k_msgq *msgq = (struct k_msgq*) MADDR(sca1);
	size_t msg_size = sca2;
	uint32_t max_msgs = sca3;
	RETURN((int) k_msgq_alloc_init(msgq,msg_size,max_msgs));
}

// 232 TODO
int wazi_syscall_k_msgq_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(232, k_msgq_get);
	ERRSC(k_msgq_get);
	struct k_msgq *msgq = (struct k_msgq*) MADDR(sca1);
	void *data = (void*) MADDR(sca2);
	k_timeout_t timeout = { .ticks = sca3 };
	RETURN((int) k_msgq_get(msgq,data,timeout));
}

// 233 TODO
void wazi_syscall_k_msgq_get_attrs (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(233, k_msgq_get_attrs);
	ERRSC(k_msgq_get_attrs);
	struct k_msgq *msgq = (struct k_msgq*) MADDR(sca1);
	struct k_msgq_attrs *attrs = (struct k_msgq_attrs*) MADDR(sca2);
	RETURN_VOID(k_msgq_get_attrs(msgq,attrs));
}

// 234 TODO
uint32_t wazi_syscall_k_msgq_num_free_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(234, k_msgq_num_free_get);
	ERRSC(k_msgq_num_free_get);
	struct k_msgq *msgq = (struct k_msgq*) MADDR(sca1);
	RETURN((uint32_t) k_msgq_num_free_get(msgq));
}

// 235 TODO
uint32_t wazi_syscall_k_msgq_num_used_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(235, k_msgq_num_used_get);
	ERRSC(k_msgq_num_used_get);
	struct k_msgq *msgq = (struct k_msgq*) MADDR(sca1);
	RETURN((uint32_t) k_msgq_num_used_get(msgq));
}

// 236 TODO
int wazi_syscall_k_msgq_peek (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(236, k_msgq_peek);
	ERRSC(k_msgq_peek);
	struct k_msgq *msgq = (struct k_msgq*) MADDR(sca1);
	void *data = (void*) MADDR(sca2);
	RETURN((int) k_msgq_peek(msgq,data));
}

// 237 TODO
int wazi_syscall_k_msgq_peek_at (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(237, k_msgq_peek_at);
	ERRSC(k_msgq_peek_at);
	struct k_msgq *msgq = (struct k_msgq*) MADDR(sca1);
	void *data = (void*) MADDR(sca2);
	uint32_t idx = sca3;
	RETURN((int) k_msgq_peek_at(msgq,data,idx));
}

// 238 TODO
void wazi_syscall_k_msgq_purge (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(238, k_msgq_purge);
	ERRSC(k_msgq_purge);
	struct k_msgq *msgq = (struct k_msgq*) MADDR(sca1);
	RETURN_VOID(k_msgq_purge(msgq));
}

// 239 TODO
int wazi_syscall_k_msgq_put (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(239, k_msgq_put);
	ERRSC(k_msgq_put);
	struct k_msgq *msgq = (struct k_msgq*) MADDR(sca1);
	void *data = (void*) MADDR(sca2);
	k_timeout_t timeout = { .ticks = sca3 };
	RETURN((int) k_msgq_put(msgq,data,timeout));
}

// 240 TODO
int wazi_syscall_k_mutex_init (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(240, k_mutex_init);
	ERRSC(k_mutex_init);
	struct k_mutex *mutex = (struct k_mutex*) MADDR(sca1);
	RETURN((int) k_mutex_init(mutex));
}

// 241 TODO
int wazi_syscall_k_mutex_lock (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(241, k_mutex_lock);
	ERRSC(k_mutex_lock);
	struct k_mutex *mutex = (struct k_mutex*) MADDR(sca1);
	k_timeout_t timeout = { .ticks = sca2 };
	RETURN((int) k_mutex_lock(mutex,timeout));
}

// 242 TODO
int wazi_syscall_k_mutex_unlock (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(242, k_mutex_unlock);
	ERRSC(k_mutex_unlock);
	struct k_mutex *mutex = (struct k_mutex*) MADDR(sca1);
	RETURN((int) k_mutex_unlock(mutex));
}

// 243 TODO
void wazi_syscall_k_object_access_grant (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(243, k_object_access_grant);
	ERRSC(k_object_access_grant);
	void *object = (void*) MADDR(sca1);
	struct k_thread *thread = (struct k_thread*) MADDR(sca2);
	RETURN_VOID(k_object_access_grant(object,thread));
}

// 244 TODO
int32_t wazi_syscall_k_object_alloc (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(244, k_object_alloc);
	ERRSC(k_object_alloc);
	enum k_objects otype = sca1;
	RETURN((int32_t) k_object_alloc(otype));
}

// 245 TODO
int32_t wazi_syscall_k_object_alloc_size (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(245, k_object_alloc_size);
	ERRSC(k_object_alloc_size);
	enum k_objects otype = sca1;
	size_t size = sca2;
	RETURN((int32_t) k_object_alloc_size(otype,size));
}

// 246 TODO
void wazi_syscall_k_object_release (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(246, k_object_release);
	ERRSC(k_object_release);
	void *object = (void*) MADDR(sca1);
	RETURN_VOID(k_object_release(object));
}

// 247 TODO
int wazi_syscall_k_pipe_alloc_init (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(247, k_pipe_alloc_init);
	ERRSC(k_pipe_alloc_init);
	struct k_pipe *pipe = (struct k_pipe*) MADDR(sca1);
	size_t size = sca2;
	RETURN((int) k_pipe_alloc_init(pipe,size));
}

// 248 TODO
void wazi_syscall_k_pipe_buffer_flush (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(248, k_pipe_buffer_flush);
	ERRSC(k_pipe_buffer_flush);
	struct k_pipe *pipe = (struct k_pipe*) MADDR(sca1);
	RETURN_VOID(k_pipe_buffer_flush(pipe));
}

// 249 TODO
void wazi_syscall_k_pipe_flush (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(249, k_pipe_flush);
	ERRSC(k_pipe_flush);
	struct k_pipe *pipe = (struct k_pipe*) MADDR(sca1);
	RETURN_VOID(k_pipe_flush(pipe));
}

// 250 TODO
int wazi_syscall_k_pipe_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4, uint32_t sca5, uint32_t sca6) {
	SC(250, k_pipe_get);
	ERRSC(k_pipe_get);
	struct k_pipe *pipe = (struct k_pipe*) MADDR(sca1);
	void *data = (void*) MADDR(sca2);
	size_t bytes_to_read = sca3;
	size_t *bytes_read = (size_t*) MADDR(sca4);
	size_t min_xfer = sca5;
	k_timeout_t timeout = { .ticks = sca6 };
	RETURN((int) k_pipe_get(pipe,data,bytes_to_read,bytes_read,min_xfer,timeout));
}

// 251 TODO
int wazi_syscall_k_pipe_put (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4, uint32_t sca5, uint32_t sca6) {
	SC(251, k_pipe_put);
	ERRSC(k_pipe_put);
	struct k_pipe *pipe = (struct k_pipe*) MADDR(sca1);
	void *data = (void*) MADDR(sca2);
	size_t bytes_to_write = sca3;
	size_t *bytes_written = (size_t*) MADDR(sca4);
	size_t min_xfer = sca5;
	k_timeout_t timeout = { .ticks = sca6 };
	RETURN((int) k_pipe_put(pipe,data,bytes_to_write,bytes_written,min_xfer,timeout));
}

// 252 TODO
uint32_t wazi_syscall_k_pipe_read_avail (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(252, k_pipe_read_avail);
	ERRSC(k_pipe_read_avail);
	struct k_pipe *pipe = (struct k_pipe*) MADDR(sca1);
	RETURN((uint32_t) k_pipe_read_avail(pipe));
}

// 253 TODO
uint32_t wazi_syscall_k_pipe_write_avail (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(253, k_pipe_write_avail);
	ERRSC(k_pipe_write_avail);
	struct k_pipe *pipe = (struct k_pipe*) MADDR(sca1);
	RETURN((uint32_t) k_pipe_write_avail(pipe));
}

// 254 TODO
int wazi_syscall_k_poll (wasm_exec_env_t exec_env, int32_t sca1, int sca2, uint32_t sca3) {
	SC(254, k_poll);
	ERRSC(k_poll);
	struct k_poll_event *events = (struct k_poll_event*) MADDR(sca1);
	int num_events = sca2;
	k_timeout_t timeout = { .ticks = sca3 };
	RETURN((int) k_poll(events,num_events,timeout));
}

// 255 TODO
void wazi_syscall_k_poll_signal_check (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(255, k_poll_signal_check);
	ERRSC(k_poll_signal_check);
	struct k_poll_signal *sig = (struct k_poll_signal*) MADDR(sca1);
	unsigned int *signaled = (unsigned int*) MADDR(sca2);
	int *result = (int*) MADDR(sca3);
	RETURN_VOID(k_poll_signal_check(sig,signaled,result));
}

// 256 TODO
void wazi_syscall_k_poll_signal_init (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(256, k_poll_signal_init);
	ERRSC(k_poll_signal_init);
	struct k_poll_signal *sig = (struct k_poll_signal*) MADDR(sca1);
	RETURN_VOID(k_poll_signal_init(sig));
}

// 257 TODO
int wazi_syscall_k_poll_signal_raise (wasm_exec_env_t exec_env, int32_t sca1, int sca2) {
	SC(257, k_poll_signal_raise);
	ERRSC(k_poll_signal_raise);
	struct k_poll_signal *sig = (struct k_poll_signal*) MADDR(sca1);
	int result = sca2;
	RETURN((int) k_poll_signal_raise(sig,result));
}

// 258 TODO
void wazi_syscall_k_poll_signal_reset (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(258, k_poll_signal_reset);
	ERRSC(k_poll_signal_reset);
	struct k_poll_signal *sig = (struct k_poll_signal*) MADDR(sca1);
	RETURN_VOID(k_poll_signal_reset(sig));
}

// 259 TODO
int32_t wazi_syscall_k_queue_alloc_append (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(259, k_queue_alloc_append);
	ERRSC(k_queue_alloc_append);
	struct k_queue *queue = (struct k_queue*) MADDR(sca1);
	void *data = (void*) MADDR(sca2);
	RETURN((int32_t) k_queue_alloc_append(queue,data));
}

// 260 TODO
int32_t wazi_syscall_k_queue_alloc_prepend (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(260, k_queue_alloc_prepend);
	ERRSC(k_queue_alloc_prepend);
	struct k_queue *queue = (struct k_queue*) MADDR(sca1);
	void *data = (void*) MADDR(sca2);
	RETURN((int32_t) k_queue_alloc_prepend(queue,data));
}

// 261 TODO
void wazi_syscall_k_queue_cancel_wait (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(261, k_queue_cancel_wait);
	ERRSC(k_queue_cancel_wait);
	struct k_queue *queue = (struct k_queue*) MADDR(sca1);
	RETURN_VOID(k_queue_cancel_wait(queue));
}

// 262 TODO
int32_t wazi_syscall_k_queue_get (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(262, k_queue_get);
	ERRSC(k_queue_get);
	struct k_queue *queue = (struct k_queue*) MADDR(sca1);
	k_timeout_t timeout = { .ticks = sca2 };
	RETURN((int32_t) k_queue_get(queue,timeout));
}

// 263 TODO
void wazi_syscall_k_queue_init (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(263, k_queue_init);
	ERRSC(k_queue_init);
	struct k_queue *queue = (struct k_queue*) MADDR(sca1);
	RETURN_VOID(k_queue_init(queue));
}

// 264 TODO
int wazi_syscall_k_queue_is_empty (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(264, k_queue_is_empty);
	ERRSC(k_queue_is_empty);
	struct k_queue *queue = (struct k_queue*) MADDR(sca1);
	RETURN((int) k_queue_is_empty(queue));
}

// 265 TODO
int32_t wazi_syscall_k_queue_peek_head (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(265, k_queue_peek_head);
	ERRSC(k_queue_peek_head);
	struct k_queue *queue = (struct k_queue*) MADDR(sca1);
	RETURN((int32_t) k_queue_peek_head(queue));
}

// 266 TODO
int32_t wazi_syscall_k_queue_peek_tail (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(266, k_queue_peek_tail);
	ERRSC(k_queue_peek_tail);
	struct k_queue *queue = (struct k_queue*) MADDR(sca1);
	RETURN((int32_t) k_queue_peek_tail(queue));
}

// 267 TODO
int32_t wazi_syscall_k_sched_current_thread_query (wasm_exec_env_t exec_env) {
	SC(267, k_sched_current_thread_query);
	ERRSC(k_sched_current_thread_query);
	RETURN((int32_t) k_sched_current_thread_query());
}

// 268 TODO
unsigned int wazi_syscall_k_sem_count_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(268, k_sem_count_get);
	ERRSC(k_sem_count_get);
	struct k_sem *sem = (struct k_sem*) MADDR(sca1);
	RETURN((unsigned int) k_sem_count_get(sem));
}

// 269 TODO
void wazi_syscall_k_sem_give (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(269, k_sem_give);
	ERRSC(k_sem_give);
	struct k_sem *sem = (struct k_sem*) MADDR(sca1);
	RETURN_VOID(k_sem_give(sem));
}

// 270 TODO
int wazi_syscall_k_sem_init (wasm_exec_env_t exec_env, int32_t sca1, unsigned int sca2, unsigned int sca3) {
	SC(270, k_sem_init);
	ERRSC(k_sem_init);
	struct k_sem *sem = (struct k_sem*) MADDR(sca1);
	unsigned int initial_count = sca2;
	unsigned int limit = sca3;
	RETURN((int) k_sem_init(sem,initial_count,limit));
}

// 271 TODO
void wazi_syscall_k_sem_reset (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(271, k_sem_reset);
	ERRSC(k_sem_reset);
	struct k_sem *sem = (struct k_sem*) MADDR(sca1);
	RETURN_VOID(k_sem_reset(sem));
}

// 272 TODO
int wazi_syscall_k_sem_take (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(272, k_sem_take);
	ERRSC(k_sem_take);
	struct k_sem *sem = (struct k_sem*) MADDR(sca1);
	k_timeout_t timeout = { .ticks = sca2 };
	RETURN((int) k_sem_take(sem,timeout));
}

// 273 TODO
int32_t wazi_syscall_k_sleep (wasm_exec_env_t exec_env, uint32_t sca1) {
	SC(273, k_sleep);
	ERRSC(k_sleep);
	k_timeout_t timeout = { .ticks = sca1 };
	RETURN((int32_t) k_sleep(timeout));
}

// 274 TODO
int32_t wazi_syscall_k_stack_alloc_init (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(274, k_stack_alloc_init);
	ERRSC(k_stack_alloc_init);
	struct k_stack *stack = (struct k_stack*) MADDR(sca1);
	uint32_t num_entries = sca2;
	RETURN((int32_t) k_stack_alloc_init(stack,num_entries));
}

// 275 TODO
int wazi_syscall_k_stack_pop (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(275, k_stack_pop);
	ERRSC(k_stack_pop);
	struct k_stack *stack = (struct k_stack*) MADDR(sca1);
	stack_data_t *data = (stack_data_t*) MADDR(sca2);
	k_timeout_t timeout = { .ticks = sca3 };
	RETURN((int) k_stack_pop(stack,data,timeout));
}

// 276 TODO
int wazi_syscall_k_stack_push (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(276, k_stack_push);
	ERRSC(k_stack_push);
	struct k_stack *stack = (struct k_stack*) MADDR(sca1);
	stack_data_t data = (stack_data_t) MADDR(sca2);
	RETURN((int) k_stack_push(stack,data));
}

// 277 TODO
void wazi_syscall_k_str_out (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(277, k_str_out);
	ERRSC(k_str_out);
	char *c = (char*) MADDR(sca1);
	size_t n = sca2;
	RETURN_VOID(k_str_out(c,n));
}

// 278 TODO
void wazi_syscall_k_thread_abort (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(278, k_thread_abort);
	ERRSC(k_thread_abort);
	k_tid_t thread = (k_tid_t) MADDR(sca1);
	RETURN_VOID(k_thread_abort(thread));
}

// 279 TODO
int32_t wazi_syscall_k_thread_create (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4, int32_t sca5, int32_t sca6, int32_t sca7, int sca8, uint32_t sca9, uint32_t sca10) {
	SC(279, k_thread_create);
	ERRSC(k_thread_create);
	struct k_thread *new_thread = (struct k_thread*) MADDR(sca1);
	k_thread_stack_t *stack = (k_thread_stack_t*) MADDR(sca2);
	size_t stack_size = sca3;
	k_thread_entry_t entry = ERROR_FN_PTR;
	void *p1 = (void*) MADDR(sca5);
	void *p2 = (void*) MADDR(sca6);
	void *p3 = (void*) MADDR(sca7);
	int prio = sca8;
	uint32_t options = sca9;
	k_timeout_t delay = { .ticks = sca10 };
	RETURN((int32_t) k_thread_create(new_thread,stack,stack_size,entry,p1,p2,p3,prio,options,delay));
}

// 280 TODO
int32_t wazi_syscall_k_thread_custom_data_get (wasm_exec_env_t exec_env) {
	SC(280, k_thread_custom_data_get);
	ERRSC(k_thread_custom_data_get);
	RETURN((int32_t) k_thread_custom_data_get());
}

// 281 TODO
void wazi_syscall_k_thread_custom_data_set (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(281, k_thread_custom_data_set);
	ERRSC(k_thread_custom_data_set);
	void *value = (void*) MADDR(sca1);
	RETURN_VOID(k_thread_custom_data_set(value));
}

// 282 TODO
void wazi_syscall_k_thread_deadline_set (wasm_exec_env_t exec_env, int32_t sca1, int sca2) {
	SC(282, k_thread_deadline_set);
	ERRSC(k_thread_deadline_set);
	k_tid_t thread = (k_tid_t) MADDR(sca1);
	int deadline = sca2;
	RETURN_VOID(k_thread_deadline_set(thread,deadline));
}

// 283 TODO
int wazi_syscall_k_thread_join (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(283, k_thread_join);
	ERRSC(k_thread_join);
	struct k_thread *thread = (struct k_thread*) MADDR(sca1);
	k_timeout_t timeout = { .ticks = sca2 };
	RETURN((int) k_thread_join(thread,timeout));
}

// 284 TODO
int wazi_syscall_k_thread_name_copy (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(284, k_thread_name_copy);
	ERRSC(k_thread_name_copy);
	k_tid_t thread = (k_tid_t) MADDR(sca1);
	char *buf = (char*) MADDR(sca2);
	size_t size = sca3;
	RETURN((int) k_thread_name_copy(thread,buf,size));
}

// 285 TODO
int wazi_syscall_k_thread_name_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(285, k_thread_name_set);
	ERRSC(k_thread_name_set);
	k_tid_t thread = (k_tid_t) MADDR(sca1);
	char *str = (char*) MADDR(sca2);
	RETURN((int) k_thread_name_set(thread,str));
}

// 286 TODO
int wazi_syscall_k_thread_priority_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(286, k_thread_priority_get);
	ERRSC(k_thread_priority_get);
	k_tid_t thread = (k_tid_t) MADDR(sca1);
	RETURN((int) k_thread_priority_get(thread));
}

// 287 TODO
void wazi_syscall_k_thread_priority_set (wasm_exec_env_t exec_env, int32_t sca1, int sca2) {
	SC(287, k_thread_priority_set);
	ERRSC(k_thread_priority_set);
	k_tid_t thread = (k_tid_t) MADDR(sca1);
	int prio = sca2;
	RETURN_VOID(k_thread_priority_set(thread,prio));
}

// 288 TODO
void wazi_syscall_k_thread_resume (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(288, k_thread_resume);
	ERRSC(k_thread_resume);
	k_tid_t thread = (k_tid_t) MADDR(sca1);
	RETURN_VOID(k_thread_resume(thread));
}

// 289 TODO
int32_t wazi_syscall_k_thread_stack_alloc (wasm_exec_env_t exec_env, uint32_t sca1, int sca2) {
	SC(289, k_thread_stack_alloc);
	ERRSC(k_thread_stack_alloc);
	size_t size = sca1;
	int flags = sca2;
	RETURN((int32_t) k_thread_stack_alloc(size,flags));
}

// 290 TODO
int wazi_syscall_k_thread_stack_free (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(290, k_thread_stack_free);
	ERRSC(k_thread_stack_free);
	k_thread_stack_t *stack = (k_thread_stack_t*) MADDR(sca1);
	RETURN((int) k_thread_stack_free(stack));
}

// 291 TODO
int wazi_syscall_k_thread_stack_space_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(291, k_thread_stack_space_get);
	ERRSC(k_thread_stack_space_get);
	struct k_thread *thread = (struct k_thread*) MADDR(sca1);
	size_t *unused_ptr = (size_t*) MADDR(sca2);
	RETURN((int) k_thread_stack_space_get(thread,unused_ptr));
}

// 292 TODO
void wazi_syscall_k_thread_start (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(292, k_thread_start);
	ERRSC(k_thread_start);
	k_tid_t thread = (k_tid_t) MADDR(sca1);
	RETURN_VOID(k_thread_start(thread));
}

// 293 TODO
void wazi_syscall_k_thread_suspend (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(293, k_thread_suspend);
	ERRSC(k_thread_suspend);
	k_tid_t thread = (k_tid_t) MADDR(sca1);
	RETURN_VOID(k_thread_suspend(thread));
}

// 294 TODO
uint32_t wazi_syscall_k_thread_timeout_expires_ticks (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(294, k_thread_timeout_expires_ticks);
	ERRSC(k_thread_timeout_expires_ticks);
	struct k_thread *t = (struct k_thread*) MADDR(sca1);
	RETURN((uint32_t) k_thread_timeout_expires_ticks(t));
}

// 295 TODO
uint32_t wazi_syscall_k_thread_timeout_remaining_ticks (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(295, k_thread_timeout_remaining_ticks);
	ERRSC(k_thread_timeout_remaining_ticks);
	struct k_thread *t = (struct k_thread*) MADDR(sca1);
	RETURN((uint32_t) k_thread_timeout_remaining_ticks(t));
}

// 296 TODO
uint32_t wazi_syscall_k_timer_expires_ticks (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(296, k_timer_expires_ticks);
	ERRSC(k_timer_expires_ticks);
	struct k_timer *timer = (struct k_timer*) MADDR(sca1);
	RETURN((uint32_t) k_timer_expires_ticks(timer));
}

// 297 TODO
uint32_t wazi_syscall_k_timer_remaining_ticks (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(297, k_timer_remaining_ticks);
	ERRSC(k_timer_remaining_ticks);
	struct k_timer *timer = (struct k_timer*) MADDR(sca1);
	RETURN((uint32_t) k_timer_remaining_ticks(timer));
}

// 298 TODO
void wazi_syscall_k_timer_start (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3) {
	SC(298, k_timer_start);
	ERRSC(k_timer_start);
	struct k_timer *timer = (struct k_timer*) MADDR(sca1);
	k_timeout_t duration = { .ticks = sca2 };
	k_timeout_t period = { .ticks = sca3 };
	RETURN_VOID(k_timer_start(timer,duration,period));
}

// 299 TODO
uint32_t wazi_syscall_k_timer_status_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(299, k_timer_status_get);
	ERRSC(k_timer_status_get);
	struct k_timer *timer = (struct k_timer*) MADDR(sca1);
	RETURN((uint32_t) k_timer_status_get(timer));
}

// 300 TODO
uint32_t wazi_syscall_k_timer_status_sync (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(300, k_timer_status_sync);
	ERRSC(k_timer_status_sync);
	struct k_timer *timer = (struct k_timer*) MADDR(sca1);
	RETURN((uint32_t) k_timer_status_sync(timer));
}

// 301 TODO
void wazi_syscall_k_timer_stop (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(301, k_timer_stop);
	ERRSC(k_timer_stop);
	struct k_timer *timer = (struct k_timer*) MADDR(sca1);
	RETURN_VOID(k_timer_stop(timer));
}

// 302 TODO
int32_t wazi_syscall_k_timer_user_data_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(302, k_timer_user_data_get);
	ERRSC(k_timer_user_data_get);
	struct k_timer *timer = (struct k_timer*) MADDR(sca1);
	RETURN((int32_t) k_timer_user_data_get(timer));
}

// 303 TODO
void wazi_syscall_k_timer_user_data_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(303, k_timer_user_data_set);
	ERRSC(k_timer_user_data_set);
	struct k_timer *timer = (struct k_timer*) MADDR(sca1);
	void *user_data = (void*) MADDR(sca2);
	RETURN_VOID(k_timer_user_data_set(timer,user_data));
}

// 304 TODO
int64_t wazi_syscall_k_uptime_ticks (wasm_exec_env_t exec_env) {
	SC(304, k_uptime_ticks);
	ERRSC(k_uptime_ticks);
	RETURN((int64_t) k_uptime_ticks());
}

// 305 TODO
int32_t wazi_syscall_k_usleep (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(305, k_usleep);
	ERRSC(k_usleep);
	int32_t us = sca1;
	RETURN((int32_t) k_usleep(us));
}

// 306 TODO
void wazi_syscall_k_wakeup (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(306, k_wakeup);
	ERRSC(k_wakeup);
	k_tid_t thread = (k_tid_t) MADDR(sca1);
	RETURN_VOID(k_wakeup(thread));
}

// 307 TODO
void wazi_syscall_k_yield (wasm_exec_env_t exec_env) {
	SC(307, k_yield);
	ERRSC(k_yield);
	RETURN_VOID(k_yield());
}

// 308 TODO
int wazi_syscall_led_blink (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, uint32_t sca4) {
	SC(308, led_blink);
	ERRSC(led_blink);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t led = sca2;
	uint32_t delay_on = sca3;
	uint32_t delay_off = sca4;
	RETURN((int) led_blink(dev,led,delay_on,delay_off));
}

// 309 TODO
int wazi_syscall_led_get_info (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3) {
	SC(309, led_get_info);
	ERRSC(led_get_info);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t led = sca2;
	struct led_info **info = ERROR_PTR;
	RETURN((int) led_get_info(dev,led,info));
}

// 310 TODO
int wazi_syscall_led_off (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(310, led_off);
	ERRSC(led_off);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t led = sca2;
	RETURN((int) led_off(dev,led));
}

// 311 TODO
int wazi_syscall_led_on (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(311, led_on);
	ERRSC(led_on);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t led = sca2;
	RETURN((int) led_on(dev,led));
}

// 312 TODO
int wazi_syscall_led_set_brightness (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint8_t sca3) {
	SC(312, led_set_brightness);
	ERRSC(led_set_brightness);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t led = sca2;
	uint8_t value = sca3;
	RETURN((int) led_set_brightness(dev,led,value));
}

// 313 TODO
int wazi_syscall_led_set_channel (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint8_t sca3) {
	SC(313, led_set_channel);
	ERRSC(led_set_channel);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	uint8_t value = sca3;
	RETURN((int) led_set_channel(dev,channel,value));
}

// 314 TODO
int wazi_syscall_led_set_color (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint8_t sca3, int32_t sca4) {
	SC(314, led_set_color);
	ERRSC(led_set_color);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t led = sca2;
	uint8_t num_colors = sca3;
	uint8_t *color = (uint8_t*) MADDR(sca4);
	RETURN((int) led_set_color(dev,led,num_colors,color));
}

// 315 TODO
int wazi_syscall_led_write_channels (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, int32_t sca4) {
	SC(315, led_write_channels);
	ERRSC(led_write_channels);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t start_channel = sca2;
	uint32_t num_channels = sca3;
	uint8_t *buf = (uint8_t*) MADDR(sca4);
	RETURN((int) led_write_channels(dev,start_channel,num_channels,buf));
}

// 316 TODO
uint32_t wazi_syscall_log_buffered_cnt (wasm_exec_env_t exec_env) {
	SC(316, log_buffered_cnt);
	ERRSC(log_buffered_cnt);
	RETURN((uint32_t) log_buffered_cnt());
}

// 317 TODO
uint32_t wazi_syscall_log_filter_set (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int16_t sca3, uint32_t sca4) {
	SC(317, log_filter_set);
	ERRSC(log_filter_set);
	struct log_backend *backend = (struct log_backend*) MADDR(sca1);
	uint32_t domain_id = sca2;
	int16_t source_id = sca3;
	uint32_t level = sca4;
	RETURN((uint32_t) log_filter_set(backend,domain_id,source_id,level));
}

// 318 TODO
uint32_t wazi_syscall_log_frontend_filter_set (wasm_exec_env_t exec_env, int16_t sca1, uint32_t sca2) {
	SC(318, log_frontend_filter_set);
	ERRSC(log_frontend_filter_set);
	int16_t source_id = sca1;
	uint32_t level = sca2;
	RETURN((uint32_t) log_frontend_filter_set(source_id,level));
}

// 319 TODO
void wazi_syscall_log_panic (wasm_exec_env_t exec_env) {
	SC(319, log_panic);
	ERRSC(log_panic);
	RETURN_VOID(log_panic());
}

// 320 TODO
bool wazi_syscall_log_process (wasm_exec_env_t exec_env) {
	SC(320, log_process);
	ERRSC(log_process);
	RETURN((bool) log_process());
}

// 321 TODO
int wazi_syscall_maxim_ds3231_get_syncpoint (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(321, maxim_ds3231_get_syncpoint);
	ERRSC(maxim_ds3231_get_syncpoint);
	struct device *dev = (struct device*) MADDR(sca1);
	struct maxim_ds3231_syncpoint *syncpoint = (struct maxim_ds3231_syncpoint*) MADDR(sca2);
	RETURN((int) maxim_ds3231_get_syncpoint(dev,syncpoint));
}

// 322 TODO
int wazi_syscall_maxim_ds3231_req_syncpoint (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(322, maxim_ds3231_req_syncpoint);
	ERRSC(maxim_ds3231_req_syncpoint);
	struct device *dev = (struct device*) MADDR(sca1);
	struct k_poll_signal *signal = (struct k_poll_signal*) MADDR(sca2);
	RETURN((int) maxim_ds3231_req_syncpoint(dev,signal));
}

// 323 TODO
uint32_t wazi_syscall_mbox_max_channels_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(323, mbox_max_channels_get);
	ERRSC(mbox_max_channels_get);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) mbox_max_channels_get(dev));
}

// 324 TODO
int wazi_syscall_mbox_mtu_get (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(324, mbox_mtu_get);
	ERRSC(mbox_mtu_get);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) mbox_mtu_get(dev));
}

// 325 TODO
int wazi_syscall_mbox_send (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(325, mbox_send);
	ERRSC(mbox_send);
	struct mbox_channel *channel = (struct mbox_channel*) MADDR(sca1);
	struct mbox_msg *msg = (struct mbox_msg*) MADDR(sca2);
	RETURN((int) mbox_send(channel,msg));
}

// 326 TODO
int wazi_syscall_mbox_set_enabled (wasm_exec_env_t exec_env, int32_t sca1, bool sca2) {
	SC(326, mbox_set_enabled);
	ERRSC(mbox_set_enabled);
	struct mbox_channel *channel = (struct mbox_channel*) MADDR(sca1);
	bool enable = sca2;
	RETURN((int) mbox_set_enabled(channel,enable));
}

// 327 TODO
void wazi_syscall_mdio_bus_disable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(327, mdio_bus_disable);
	ERRSC(mdio_bus_disable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN_VOID(mdio_bus_disable(dev));
}

// 328 TODO
void wazi_syscall_mdio_bus_enable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(328, mdio_bus_enable);
	ERRSC(mdio_bus_enable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN_VOID(mdio_bus_enable(dev));
}

// 329 TODO
int wazi_syscall_mdio_read (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint8_t sca3, int32_t sca4) {
	SC(329, mdio_read);
	ERRSC(mdio_read);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t prtad = sca2;
	uint8_t regad = sca3;
	uint16_t *data = (uint16_t*) MADDR(sca4);
	RETURN((int) mdio_read(dev,prtad,regad,data));
}

// 330 TODO
int wazi_syscall_mdio_read_c45 (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint8_t sca3, uint16_t sca4, int32_t sca5) {
	SC(330, mdio_read_c45);
	ERRSC(mdio_read_c45);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t prtad = sca2;
	uint8_t devad = sca3;
	uint16_t regad = sca4;
	uint16_t *data = (uint16_t*) MADDR(sca5);
	RETURN((int) mdio_read_c45(dev,prtad,devad,regad,data));
}

// 331 TODO
int wazi_syscall_mdio_write (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint8_t sca3, uint16_t sca4) {
	SC(331, mdio_write);
	ERRSC(mdio_write);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t prtad = sca2;
	uint8_t regad = sca3;
	uint16_t data = sca4;
	RETURN((int) mdio_write(dev,prtad,regad,data));
}

// 332 TODO
int wazi_syscall_mdio_write_c45 (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint8_t sca3, uint16_t sca4, uint16_t sca5) {
	SC(332, mdio_write_c45);
	ERRSC(mdio_write_c45);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t prtad = sca2;
	uint8_t devad = sca3;
	uint16_t regad = sca4;
	uint16_t data = sca5;
	RETURN((int) mdio_write_c45(dev,prtad,devad,regad,data));
}

// 333 TODO
int32_t wazi_syscall_net_addr_ntop (wasm_exec_env_t exec_env, uint16_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(333, net_addr_ntop);
	ERRSC(net_addr_ntop);
	sa_family_t family = sca1;
	void *src = (void*) MADDR(sca2);
	char *dst = (char*) MADDR(sca3);
	size_t size = sca4;
	RETURN((int32_t) net_addr_ntop(family,src,dst,size));
}

// 334 TODO
int wazi_syscall_net_addr_pton (wasm_exec_env_t exec_env, uint16_t sca1, int32_t sca2, int32_t sca3) {
	SC(334, net_addr_pton);
	ERRSC(net_addr_pton);
	sa_family_t family = sca1;
	char *src = (char*) MADDR(sca2);
	void *dst = (void*) MADDR(sca3);
	RETURN((int) net_addr_pton(family,src,dst));
}

// 335 TODO
int32_t wazi_syscall_net_eth_get_ptp_clock_by_index (wasm_exec_env_t exec_env, int sca1) {
	SC(335, net_eth_get_ptp_clock_by_index);
	ERRSC(net_eth_get_ptp_clock_by_index);
	int index = sca1;
	RETURN((int32_t) net_eth_get_ptp_clock_by_index(index));
}

// 336 TODO
int32_t wazi_syscall_net_if_get_by_index (wasm_exec_env_t exec_env, int sca1) {
	SC(336, net_if_get_by_index);
	ERRSC(net_if_get_by_index);
	int index = sca1;
	RETURN((int32_t) net_if_get_by_index(index));
}

// 337 TODO
bool wazi_syscall_net_if_ipv4_addr_add_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(337, net_if_ipv4_addr_add_by_index);
	ERRSC(net_if_ipv4_addr_add_by_index);
	int index = sca1;
	struct in_addr *addr = (struct in_addr*) MADDR(sca2);
	enum net_addr_type addr_type = sca3;
	uint32_t vlifetime = sca4;
	RETURN((bool) net_if_ipv4_addr_add_by_index(index,addr,addr_type,vlifetime));
}

// 338 TODO
int wazi_syscall_net_if_ipv4_addr_lookup_by_index (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(338, net_if_ipv4_addr_lookup_by_index);
	ERRSC(net_if_ipv4_addr_lookup_by_index);
	struct in_addr *addr = (struct in_addr*) MADDR(sca1);
	RETURN((int) net_if_ipv4_addr_lookup_by_index(addr));
}

// 339 TODO
bool wazi_syscall_net_if_ipv4_addr_rm_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2) {
	SC(339, net_if_ipv4_addr_rm_by_index);
	ERRSC(net_if_ipv4_addr_rm_by_index);
	int index = sca1;
	struct in_addr *addr = (struct in_addr*) MADDR(sca2);
	RETURN((bool) net_if_ipv4_addr_rm_by_index(index,addr));
}

// 340 TODO
bool wazi_syscall_net_if_ipv4_set_gw_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2) {
	SC(340, net_if_ipv4_set_gw_by_index);
	ERRSC(net_if_ipv4_set_gw_by_index);
	int index = sca1;
	struct in_addr *gw = (struct in_addr*) MADDR(sca2);
	RETURN((bool) net_if_ipv4_set_gw_by_index(index,gw));
}

// 341 TODO
bool wazi_syscall_net_if_ipv4_set_netmask_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2) {
	SC(341, net_if_ipv4_set_netmask_by_index);
	ERRSC(net_if_ipv4_set_netmask_by_index);
	int index = sca1;
	struct in_addr *netmask = (struct in_addr*) MADDR(sca2);
	RETURN((bool) net_if_ipv4_set_netmask_by_index(index,netmask));
}

// 342 TODO
bool wazi_syscall_net_if_ipv6_addr_add_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(342, net_if_ipv6_addr_add_by_index);
	ERRSC(net_if_ipv6_addr_add_by_index);
	int index = sca1;
	struct in6_addr *addr = (struct in6_addr*) MADDR(sca2);
	enum net_addr_type addr_type = sca3;
	uint32_t vlifetime = sca4;
	RETURN((bool) net_if_ipv6_addr_add_by_index(index,addr,addr_type,vlifetime));
}

// 343 TODO
int wazi_syscall_net_if_ipv6_addr_lookup_by_index (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(343, net_if_ipv6_addr_lookup_by_index);
	ERRSC(net_if_ipv6_addr_lookup_by_index);
	struct in6_addr *addr = (struct in6_addr*) MADDR(sca1);
	RETURN((int) net_if_ipv6_addr_lookup_by_index(addr));
}

// 344 TODO
bool wazi_syscall_net_if_ipv6_addr_rm_by_index (wasm_exec_env_t exec_env, int sca1, int32_t sca2) {
	SC(344, net_if_ipv6_addr_rm_by_index);
	ERRSC(net_if_ipv6_addr_rm_by_index);
	int index = sca1;
	struct in6_addr *addr = (struct in6_addr*) MADDR(sca2);
	RETURN((bool) net_if_ipv6_addr_rm_by_index(index,addr));
}

// 345 TODO
int wazi_syscall_net_socket_service_register (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int sca3, int32_t sca4) {
	SC(345, net_socket_service_register);
	ERRSC(net_socket_service_register);
	struct net_socket_service_desc *service = (struct net_socket_service_desc*) MADDR(sca1);
	struct zsock_pollfd *fds = (struct zsock_pollfd*) MADDR(sca2);
	int len = sca3;
	void *user_data = (void*) MADDR(sca4);
	RETURN((int) net_socket_service_register(service,fds,len,user_data));
}

// 346 TODO
void wazi_syscall_nrf_qspi_nor_xip_enable (wasm_exec_env_t exec_env, int32_t sca1, bool sca2) {
	SC(346, nrf_qspi_nor_xip_enable);
	ERRSC(nrf_qspi_nor_xip_enable);
	struct device *dev = (struct device*) MADDR(sca1);
	bool enable = sca2;
	RETURN_VOID(nrf_qspi_nor_xip_enable(dev,enable));
}

// 347 TODO
int wazi_syscall_peci_config (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(347, peci_config);
	ERRSC(peci_config);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t bitrate = sca2;
	RETURN((int) peci_config(dev,bitrate));
}

// 348 TODO
int wazi_syscall_peci_disable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(348, peci_disable);
	ERRSC(peci_disable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) peci_disable(dev));
}

// 349 TODO
int wazi_syscall_peci_enable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(349, peci_enable);
	ERRSC(peci_enable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) peci_enable(dev));
}

// 350 TODO
int wazi_syscall_peci_transfer (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(350, peci_transfer);
	ERRSC(peci_transfer);
	struct device *dev = (struct device*) MADDR(sca1);
	struct peci_msg *msg = (struct peci_msg*) MADDR(sca2);
	RETURN((int) peci_transfer(dev,msg));
}

// 351 TODO
int wazi_syscall_ps2_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(351, ps2_config);
	ERRSC(ps2_config);
	struct device *dev = (struct device*) MADDR(sca1);
	ps2_callback_t callback_isr = ERROR_FN_PTR;
	RETURN((int) ps2_config(dev,callback_isr));
}

// 352 TODO
int wazi_syscall_ps2_disable_callback (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(352, ps2_disable_callback);
	ERRSC(ps2_disable_callback);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) ps2_disable_callback(dev));
}

// 353 TODO
int wazi_syscall_ps2_enable_callback (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(353, ps2_enable_callback);
	ERRSC(ps2_enable_callback);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) ps2_enable_callback(dev));
}

// 354 TODO
int wazi_syscall_ps2_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(354, ps2_read);
	ERRSC(ps2_read);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t *value = (uint8_t*) MADDR(sca2);
	RETURN((int) ps2_read(dev,value));
}

// 355 TODO
int wazi_syscall_ps2_write (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2) {
	SC(355, ps2_write);
	ERRSC(ps2_write);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t value = sca2;
	RETURN((int) ps2_write(dev,value));
}

// 356 TODO
int wazi_syscall_ptp_clock_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(356, ptp_clock_get);
	ERRSC(ptp_clock_get);
	struct device *dev = (struct device*) MADDR(sca1);
	struct net_ptp_time *tm = (struct net_ptp_time*) MADDR(sca2);
	RETURN((int) ptp_clock_get(dev,tm));
}

// 357 TODO
int wazi_syscall_pwm_capture_cycles (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint16_t sca3, int32_t sca4, int32_t sca5, uint32_t sca6) {
	SC(357, pwm_capture_cycles);
	ERRSC(pwm_capture_cycles);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	pwm_flags_t flags = sca3;
	uint32_t *period = (uint32_t*) MADDR(sca4);
	uint32_t *pulse = (uint32_t*) MADDR(sca5);
	k_timeout_t timeout = { .ticks = sca6 };
	RETURN((int) pwm_capture_cycles(dev,channel,flags,period,pulse,timeout));
}

// 358 TODO
int wazi_syscall_pwm_disable_capture (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(358, pwm_disable_capture);
	ERRSC(pwm_disable_capture);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	RETURN((int) pwm_disable_capture(dev,channel));
}

// 359 TODO
int wazi_syscall_pwm_enable_capture (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(359, pwm_enable_capture);
	ERRSC(pwm_enable_capture);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	RETURN((int) pwm_enable_capture(dev,channel));
}

// 360 TODO
int wazi_syscall_pwm_get_cycles_per_sec (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3) {
	SC(360, pwm_get_cycles_per_sec);
	ERRSC(pwm_get_cycles_per_sec);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	uint64_t *cycles = (uint64_t*) MADDR(sca3);
	RETURN((int) pwm_get_cycles_per_sec(dev,channel,cycles));
}

// 361 TODO
int wazi_syscall_pwm_set_cycles (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, uint32_t sca4, uint16_t sca5) {
	SC(361, pwm_set_cycles);
	ERRSC(pwm_set_cycles);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t channel = sca2;
	uint32_t period = sca3;
	uint32_t pulse = sca4;
	pwm_flags_t flags = sca5;
	RETURN((int) pwm_set_cycles(dev,channel,period,pulse,flags));
}

// 362 TODO
int wazi_syscall_reset_line_assert (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(362, reset_line_assert);
	ERRSC(reset_line_assert);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t id = sca2;
	RETURN((int) reset_line_assert(dev,id));
}

// 363 TODO
int wazi_syscall_reset_line_deassert (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(363, reset_line_deassert);
	ERRSC(reset_line_deassert);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t id = sca2;
	RETURN((int) reset_line_deassert(dev,id));
}

// 364 TODO
int wazi_syscall_reset_line_toggle (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(364, reset_line_toggle);
	ERRSC(reset_line_toggle);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t id = sca2;
	RETURN((int) reset_line_toggle(dev,id));
}

// 365 TODO
int wazi_syscall_reset_status (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3) {
	SC(365, reset_status);
	ERRSC(reset_status);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t id = sca2;
	uint8_t *status = (uint8_t*) MADDR(sca3);
	RETURN((int) reset_status(dev,id,status));
}

// 366 TODO
int wazi_syscall_retained_mem_clear (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(366, retained_mem_clear);
	ERRSC(retained_mem_clear);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) retained_mem_clear(dev));
}

// 367 TODO
int wazi_syscall_retained_mem_read (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(367, retained_mem_read);
	ERRSC(retained_mem_read);
	struct device *dev = (struct device*) MADDR(sca1);
	off_t offset = (off_t) MADDR(sca2);
	uint8_t *buffer = (uint8_t*) MADDR(sca3);
	size_t size = sca4;
	RETURN((int) retained_mem_read(dev,offset,buffer,size));
}

// 368 TODO
uint32_t wazi_syscall_retained_mem_size (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(368, retained_mem_size);
	ERRSC(retained_mem_size);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) retained_mem_size(dev));
}

// 369 TODO
int wazi_syscall_retained_mem_write (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(369, retained_mem_write);
	ERRSC(retained_mem_write);
	struct device *dev = (struct device*) MADDR(sca1);
	off_t offset = (off_t) MADDR(sca2);
	uint8_t *buffer = (uint8_t*) MADDR(sca3);
	size_t size = sca4;
	RETURN((int) retained_mem_write(dev,offset,buffer,size));
}

// 370 TODO
int wazi_syscall_rtc_alarm_get_supported_fields (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3) {
	SC(370, rtc_alarm_get_supported_fields);
	ERRSC(rtc_alarm_get_supported_fields);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t id = sca2;
	uint16_t *mask = (uint16_t*) MADDR(sca3);
	RETURN((int) rtc_alarm_get_supported_fields(dev,id,mask));
}

// 371 TODO
int wazi_syscall_rtc_alarm_get_time (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3, int32_t sca4) {
	SC(371, rtc_alarm_get_time);
	ERRSC(rtc_alarm_get_time);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t id = sca2;
	uint16_t *mask = (uint16_t*) MADDR(sca3);
	struct rtc_time *timeptr = (struct rtc_time*) MADDR(sca4);
	RETURN((int) rtc_alarm_get_time(dev,id,mask,timeptr));
}

// 372 TODO
int wazi_syscall_rtc_alarm_is_pending (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2) {
	SC(372, rtc_alarm_is_pending);
	ERRSC(rtc_alarm_is_pending);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t id = sca2;
	RETURN((int) rtc_alarm_is_pending(dev,id));
}

// 373 TODO
int wazi_syscall_rtc_alarm_set_callback (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3, int32_t sca4) {
	SC(373, rtc_alarm_set_callback);
	ERRSC(rtc_alarm_set_callback);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t id = sca2;
	rtc_alarm_callback callback = ERROR_FN_PTR;
	void *user_data = (void*) MADDR(sca4);
	RETURN((int) rtc_alarm_set_callback(dev,id,callback,user_data));
}

// 374 TODO
int wazi_syscall_rtc_alarm_set_time (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint16_t sca3, int32_t sca4) {
	SC(374, rtc_alarm_set_time);
	ERRSC(rtc_alarm_set_time);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t id = sca2;
	uint16_t mask = sca3;
	struct rtc_time *timeptr = (struct rtc_time*) MADDR(sca4);
	RETURN((int) rtc_alarm_set_time(dev,id,mask,timeptr));
}

// 375 TODO
int wazi_syscall_rtc_get_calibration (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(375, rtc_get_calibration);
	ERRSC(rtc_get_calibration);
	struct device *dev = (struct device*) MADDR(sca1);
	int32_t *calibration = (int32_t*) MADDR(sca2);
	RETURN((int) rtc_get_calibration(dev,calibration));
}

// 376 TODO
int wazi_syscall_rtc_get_time (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(376, rtc_get_time);
	ERRSC(rtc_get_time);
	struct device *dev = (struct device*) MADDR(sca1);
	struct rtc_time *timeptr = (struct rtc_time*) MADDR(sca2);
	RETURN((int) rtc_get_time(dev,timeptr));
}

// 377 TODO
int wazi_syscall_rtc_set_calibration (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(377, rtc_set_calibration);
	ERRSC(rtc_set_calibration);
	struct device *dev = (struct device*) MADDR(sca1);
	int32_t calibration = sca2;
	RETURN((int) rtc_set_calibration(dev,calibration));
}

// 378 TODO
int wazi_syscall_rtc_set_time (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(378, rtc_set_time);
	ERRSC(rtc_set_time);
	struct device *dev = (struct device*) MADDR(sca1);
	struct rtc_time *timeptr = (struct rtc_time*) MADDR(sca2);
	RETURN((int) rtc_set_time(dev,timeptr));
}

// 379 TODO
int wazi_syscall_rtc_update_set_callback (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(379, rtc_update_set_callback);
	ERRSC(rtc_update_set_callback);
	struct device *dev = (struct device*) MADDR(sca1);
	rtc_update_callback callback = ERROR_FN_PTR;
	void *user_data = (void*) MADDR(sca3);
	RETURN((int) rtc_update_set_callback(dev,callback,user_data));
}

// 380 TODO
int wazi_syscall_rtio_cqe_copy_out (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, uint32_t sca4) {
	SC(380, rtio_cqe_copy_out);
	ERRSC(rtio_cqe_copy_out);
	struct rtio *r = (struct rtio*) MADDR(sca1);
	struct rtio_cqe *cqes = (struct rtio_cqe*) MADDR(sca2);
	size_t cqe_count = sca3;
	k_timeout_t timeout = { .ticks = sca4 };
	RETURN((int) rtio_cqe_copy_out(r,cqes,cqe_count,timeout));
}

// 381 TODO
int wazi_syscall_rtio_cqe_get_mempool_buffer (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4) {
	SC(381, rtio_cqe_get_mempool_buffer);
	ERRSC(rtio_cqe_get_mempool_buffer);
	struct rtio *r = (struct rtio*) MADDR(sca1);
	struct rtio_cqe *cqe = (struct rtio_cqe*) MADDR(sca2);
	uint8_t **buff = ERROR_PTR;
	uint32_t *buff_len = (uint32_t*) MADDR(sca4);
	RETURN((int) rtio_cqe_get_mempool_buffer(r,cqe,buff,buff_len));
}

// 382 TODO
void wazi_syscall_rtio_release_buffer (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(382, rtio_release_buffer);
	ERRSC(rtio_release_buffer);
	struct rtio *r = (struct rtio*) MADDR(sca1);
	void *buff = (void*) MADDR(sca2);
	uint32_t buff_len = sca3;
	RETURN_VOID(rtio_release_buffer(r,buff,buff_len));
}

// 383 TODO
int wazi_syscall_rtio_sqe_cancel (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(383, rtio_sqe_cancel);
	ERRSC(rtio_sqe_cancel);
	struct rtio_sqe *sqe = (struct rtio_sqe*) MADDR(sca1);
	RETURN((int) rtio_sqe_cancel(sqe));
}

// 384 TODO
int wazi_syscall_rtio_sqe_copy_in_get_handles (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(384, rtio_sqe_copy_in_get_handles);
	ERRSC(rtio_sqe_copy_in_get_handles);
	struct rtio *r = (struct rtio*) MADDR(sca1);
	struct rtio_sqe *sqes = (struct rtio_sqe*) MADDR(sca2);
	struct rtio_sqe **handle = ERROR_PTR;
	size_t sqe_count = sca4;
	RETURN((int) rtio_sqe_copy_in_get_handles(r,sqes,handle,sqe_count));
}

// 385 TODO
int wazi_syscall_rtio_submit (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(385, rtio_submit);
	ERRSC(rtio_submit);
	struct rtio *r = (struct rtio*) MADDR(sca1);
	uint32_t wait_count = sca2;
	RETURN((int) rtio_submit(r,wait_count));
}

// 386 TODO
int wazi_syscall_sdhc_card_busy (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(386, sdhc_card_busy);
	ERRSC(sdhc_card_busy);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) sdhc_card_busy(dev));
}

// 387 TODO
int wazi_syscall_sdhc_card_present (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(387, sdhc_card_present);
	ERRSC(sdhc_card_present);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) sdhc_card_present(dev));
}

// 388 TODO
int wazi_syscall_sdhc_disable_interrupt (wasm_exec_env_t exec_env, int32_t sca1, int sca2) {
	SC(388, sdhc_disable_interrupt);
	ERRSC(sdhc_disable_interrupt);
	struct device *dev = (struct device*) MADDR(sca1);
	int sources = sca2;
	RETURN((int) sdhc_disable_interrupt(dev,sources));
}

// 389 TODO
int wazi_syscall_sdhc_enable_interrupt (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int sca3, int32_t sca4) {
	SC(389, sdhc_enable_interrupt);
	ERRSC(sdhc_enable_interrupt);
	struct device *dev = (struct device*) MADDR(sca1);
	sdhc_interrupt_cb_t callback = ERROR_FN_PTR;
	int sources = sca3;
	void *user_data = (void*) MADDR(sca4);
	RETURN((int) sdhc_enable_interrupt(dev,callback,sources,user_data));
}

// 390 TODO
int wazi_syscall_sdhc_execute_tuning (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(390, sdhc_execute_tuning);
	ERRSC(sdhc_execute_tuning);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) sdhc_execute_tuning(dev));
}

// 391 TODO
int wazi_syscall_sdhc_get_host_props (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(391, sdhc_get_host_props);
	ERRSC(sdhc_get_host_props);
	struct device *dev = (struct device*) MADDR(sca1);
	struct sdhc_host_props *props = (struct sdhc_host_props*) MADDR(sca2);
	RETURN((int) sdhc_get_host_props(dev,props));
}

// 392 TODO
int wazi_syscall_sdhc_hw_reset (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(392, sdhc_hw_reset);
	ERRSC(sdhc_hw_reset);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) sdhc_hw_reset(dev));
}

// 393 TODO
int wazi_syscall_sdhc_request (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(393, sdhc_request);
	ERRSC(sdhc_request);
	struct device *dev = (struct device*) MADDR(sca1);
	struct sdhc_command *cmd = (struct sdhc_command*) MADDR(sca2);
	struct sdhc_data *data = (struct sdhc_data*) MADDR(sca3);
	RETURN((int) sdhc_request(dev,cmd,data));
}

// 394 TODO
int wazi_syscall_sdhc_set_io (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(394, sdhc_set_io);
	ERRSC(sdhc_set_io);
	struct device *dev = (struct device*) MADDR(sca1);
	struct sdhc_io *io = (struct sdhc_io*) MADDR(sca2);
	RETURN((int) sdhc_set_io(dev,io));
}

// 395 TODO
int wazi_syscall_sensor_attr_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4) {
	SC(395, sensor_attr_get);
	ERRSC(sensor_attr_get);
	struct device *dev = (struct device*) MADDR(sca1);
	enum sensor_channel chan = sca2;
	enum sensor_attribute attr = sca3;
	struct sensor_value *val = (struct sensor_value*) MADDR(sca4);
	RETURN((int) sensor_attr_get(dev,chan,attr,val));
}

// 396 TODO
int wazi_syscall_sensor_attr_set (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4) {
	SC(396, sensor_attr_set);
	ERRSC(sensor_attr_set);
	struct device *dev = (struct device*) MADDR(sca1);
	enum sensor_channel chan = sca2;
	enum sensor_attribute attr = sca3;
	struct sensor_value *val = (struct sensor_value*) MADDR(sca4);
	RETURN((int) sensor_attr_set(dev,chan,attr,val));
}

// 397 TODO
int wazi_syscall_sensor_channel_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3) {
	SC(397, sensor_channel_get);
	ERRSC(sensor_channel_get);
	struct device *dev = (struct device*) MADDR(sca1);
	enum sensor_channel chan = sca2;
	struct sensor_value *val = (struct sensor_value*) MADDR(sca3);
	RETURN((int) sensor_channel_get(dev,chan,val));
}

// 398 TODO
int wazi_syscall_sensor_get_decoder (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(398, sensor_get_decoder);
	ERRSC(sensor_get_decoder);
	struct device *dev = (struct device*) MADDR(sca1);
	struct sensor_decoder_api **decoder = ERROR_PTR;
	RETURN((int) sensor_get_decoder(dev,decoder));
}

// 399 TODO
int wazi_syscall_sensor_reconfigure_read_iodev (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(399, sensor_reconfigure_read_iodev);
	ERRSC(sensor_reconfigure_read_iodev);
	struct rtio_iodev *iodev = (struct rtio_iodev*) MADDR(sca1);
	struct device *sensor = (struct device*) MADDR(sca2);
	enum sensor_channel *channels = (enum sensor_channel*) MADDR(sca3);
	size_t num_channels = sca4;
	RETURN((int) sensor_reconfigure_read_iodev(iodev,sensor,channels,num_channels));
}

// 400 TODO
int wazi_syscall_sensor_sample_fetch (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(400, sensor_sample_fetch);
	ERRSC(sensor_sample_fetch);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) sensor_sample_fetch(dev));
}

// 401 TODO
int wazi_syscall_sensor_sample_fetch_chan (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(401, sensor_sample_fetch_chan);
	ERRSC(sensor_sample_fetch_chan);
	struct device *dev = (struct device*) MADDR(sca1);
	enum sensor_channel type = sca2;
	RETURN((int) sensor_sample_fetch_chan(dev,type));
}

// 402 TODO
void wazi_syscall_sip_supervisory_call (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, uint32_t sca4, uint32_t sca5, uint32_t sca6, uint32_t sca7, uint32_t sca8, uint32_t sca9, int32_t sca10) {
	SC(402, sip_supervisory_call);
	ERRSC(sip_supervisory_call);
	struct device *dev = (struct device*) MADDR(sca1);
	unsigned long function_id = sca2;
	unsigned long arg0 = sca3;
	unsigned long arg1 = sca4;
	unsigned long arg2 = sca5;
	unsigned long arg3 = sca6;
	unsigned long arg4 = sca7;
	unsigned long arg5 = sca8;
	unsigned long arg6 = sca9;
	struct arm_smccc_res *res = (struct arm_smccc_res*) MADDR(sca10);
	RETURN_VOID(sip_supervisory_call(dev,function_id,arg0,arg1,arg2,arg3,arg4,arg5,arg6,res));
}

// 403 TODO
int wazi_syscall_sip_svc_plat_async_res_req (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4, int32_t sca5, int32_t sca6, int32_t sca7, int32_t sca8, int32_t sca9, int32_t sca10, uint32_t sca11) {
	SC(403, sip_svc_plat_async_res_req);
	ERRSC(sip_svc_plat_async_res_req);
	struct device *dev = (struct device*) MADDR(sca1);
	unsigned long *a0 = (unsigned long*) MADDR(sca2);
	unsigned long *a1 = (unsigned long*) MADDR(sca3);
	unsigned long *a2 = (unsigned long*) MADDR(sca4);
	unsigned long *a3 = (unsigned long*) MADDR(sca5);
	unsigned long *a4 = (unsigned long*) MADDR(sca6);
	unsigned long *a5 = (unsigned long*) MADDR(sca7);
	unsigned long *a6 = (unsigned long*) MADDR(sca8);
	unsigned long *a7 = (unsigned long*) MADDR(sca9);
	char *buf = (char*) MADDR(sca10);
	size_t size = sca11;
	RETURN((int) sip_svc_plat_async_res_req(dev,a0,a1,a2,a3,a4,a5,a6,a7,buf,size));
}

// 404 TODO
int wazi_syscall_sip_svc_plat_async_res_res (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4, int32_t sca5) {
	SC(404, sip_svc_plat_async_res_res);
	ERRSC(sip_svc_plat_async_res_res);
	struct device *dev = (struct device*) MADDR(sca1);
	struct arm_smccc_res *res = (struct arm_smccc_res*) MADDR(sca2);
	char *buf = (char*) MADDR(sca3);
	size_t *size = (size_t*) MADDR(sca4);
	uint32_t *trans_id = (uint32_t*) MADDR(sca5);
	RETURN((int) sip_svc_plat_async_res_res(dev,res,buf,size,trans_id));
}

// 405 TODO
uint32_t wazi_syscall_sip_svc_plat_format_trans_id (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3) {
	SC(405, sip_svc_plat_format_trans_id);
	ERRSC(sip_svc_plat_format_trans_id);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t client_idx = sca2;
	uint32_t trans_idx = sca3;
	RETURN((uint32_t) sip_svc_plat_format_trans_id(dev,client_idx,trans_idx));
}

// 406 TODO
void wazi_syscall_sip_svc_plat_free_async_memory (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(406, sip_svc_plat_free_async_memory);
	ERRSC(sip_svc_plat_free_async_memory);
	struct device *dev = (struct device*) MADDR(sca1);
	struct sip_svc_request *request = (struct sip_svc_request*) MADDR(sca2);
	RETURN_VOID(sip_svc_plat_free_async_memory(dev,request));
}

// 407 TODO
bool wazi_syscall_sip_svc_plat_func_id_valid (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3) {
	SC(407, sip_svc_plat_func_id_valid);
	ERRSC(sip_svc_plat_func_id_valid);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t command = sca2;
	uint32_t func_id = sca3;
	RETURN((bool) sip_svc_plat_func_id_valid(dev,command,func_id));
}

// 408 TODO
uint32_t wazi_syscall_sip_svc_plat_get_error_code (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(408, sip_svc_plat_get_error_code);
	ERRSC(sip_svc_plat_get_error_code);
	struct device *dev = (struct device*) MADDR(sca1);
	struct arm_smccc_res *res = (struct arm_smccc_res*) MADDR(sca2);
	RETURN((uint32_t) sip_svc_plat_get_error_code(dev,res));
}

// 409 TODO
uint32_t wazi_syscall_sip_svc_plat_get_trans_idx (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(409, sip_svc_plat_get_trans_idx);
	ERRSC(sip_svc_plat_get_trans_idx);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t trans_id = sca2;
	RETURN((uint32_t) sip_svc_plat_get_trans_idx(dev,trans_id));
}

// 410 TODO
void wazi_syscall_sip_svc_plat_update_trans_id (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(410, sip_svc_plat_update_trans_id);
	ERRSC(sip_svc_plat_update_trans_id);
	struct device *dev = (struct device*) MADDR(sca1);
	struct sip_svc_request *request = (struct sip_svc_request*) MADDR(sca2);
	uint32_t trans_id = sca3;
	RETURN_VOID(sip_svc_plat_update_trans_id(dev,request,trans_id));
}

// 411 TODO
int wazi_syscall_smbus_block_pcall (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, uint8_t sca4, int32_t sca5, int32_t sca6, int32_t sca7) {
	SC(411, smbus_block_pcall);
	ERRSC(smbus_block_pcall);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	uint8_t cmd = sca3;
	uint8_t snd_count = sca4;
	uint8_t *snd_buf = (uint8_t*) MADDR(sca5);
	uint8_t *rcv_count = (uint8_t*) MADDR(sca6);
	uint8_t *rcv_buf = (uint8_t*) MADDR(sca7);
	RETURN((int) smbus_block_pcall(dev,addr,cmd,snd_count,snd_buf,rcv_count,rcv_buf));
}

// 412 TODO
int wazi_syscall_smbus_block_read (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, int32_t sca4, int32_t sca5) {
	SC(412, smbus_block_read);
	ERRSC(smbus_block_read);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	uint8_t cmd = sca3;
	uint8_t *count = (uint8_t*) MADDR(sca4);
	uint8_t *buf = (uint8_t*) MADDR(sca5);
	RETURN((int) smbus_block_read(dev,addr,cmd,count,buf));
}

// 413 TODO
int wazi_syscall_smbus_block_write (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, uint8_t sca4, int32_t sca5) {
	SC(413, smbus_block_write);
	ERRSC(smbus_block_write);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	uint8_t cmd = sca3;
	uint8_t count = sca4;
	uint8_t *buf = (uint8_t*) MADDR(sca5);
	RETURN((int) smbus_block_write(dev,addr,cmd,count,buf));
}

// 414 TODO
int wazi_syscall_smbus_byte_data_read (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, int32_t sca4) {
	SC(414, smbus_byte_data_read);
	ERRSC(smbus_byte_data_read);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	uint8_t cmd = sca3;
	uint8_t *byte = (uint8_t*) MADDR(sca4);
	RETURN((int) smbus_byte_data_read(dev,addr,cmd,byte));
}

// 415 TODO
int wazi_syscall_smbus_byte_data_write (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, uint8_t sca4) {
	SC(415, smbus_byte_data_write);
	ERRSC(smbus_byte_data_write);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	uint8_t cmd = sca3;
	uint8_t byte = sca4;
	RETURN((int) smbus_byte_data_write(dev,addr,cmd,byte));
}

// 416 TODO
int wazi_syscall_smbus_byte_read (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3) {
	SC(416, smbus_byte_read);
	ERRSC(smbus_byte_read);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	uint8_t *byte = (uint8_t*) MADDR(sca3);
	RETURN((int) smbus_byte_read(dev,addr,byte));
}

// 417 TODO
int wazi_syscall_smbus_byte_write (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3) {
	SC(417, smbus_byte_write);
	ERRSC(smbus_byte_write);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	uint8_t byte = sca3;
	RETURN((int) smbus_byte_write(dev,addr,byte));
}

// 418 TODO
int wazi_syscall_smbus_configure (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(418, smbus_configure);
	ERRSC(smbus_configure);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t dev_config = sca2;
	RETURN((int) smbus_configure(dev,dev_config));
}

// 419 TODO
int wazi_syscall_smbus_get_config (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(419, smbus_get_config);
	ERRSC(smbus_get_config);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t *dev_config = (uint32_t*) MADDR(sca2);
	RETURN((int) smbus_get_config(dev,dev_config));
}

// 420 TODO
int wazi_syscall_smbus_host_notify_remove_cb (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(420, smbus_host_notify_remove_cb);
	ERRSC(smbus_host_notify_remove_cb);
	struct device *dev = (struct device*) MADDR(sca1);
	struct smbus_callback *cb = (struct smbus_callback*) MADDR(sca2);
	RETURN((int) smbus_host_notify_remove_cb(dev,cb));
}

// 421 TODO
int wazi_syscall_smbus_pcall (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, uint16_t sca4, int32_t sca5) {
	SC(421, smbus_pcall);
	ERRSC(smbus_pcall);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	uint8_t cmd = sca3;
	uint16_t send_word = sca4;
	uint16_t *recv_word = (uint16_t*) MADDR(sca5);
	RETURN((int) smbus_pcall(dev,addr,cmd,send_word,recv_word));
}

// 422 TODO
int wazi_syscall_smbus_quick (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3) {
	SC(422, smbus_quick);
	ERRSC(smbus_quick);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	enum smbus_direction direction = sca3;
	RETURN((int) smbus_quick(dev,addr,direction));
}

// 423 TODO
int wazi_syscall_smbus_smbalert_remove_cb (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(423, smbus_smbalert_remove_cb);
	ERRSC(smbus_smbalert_remove_cb);
	struct device *dev = (struct device*) MADDR(sca1);
	struct smbus_callback *cb = (struct smbus_callback*) MADDR(sca2);
	RETURN((int) smbus_smbalert_remove_cb(dev,cb));
}

// 424 TODO
int wazi_syscall_smbus_word_data_read (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, int32_t sca4) {
	SC(424, smbus_word_data_read);
	ERRSC(smbus_word_data_read);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	uint8_t cmd = sca3;
	uint16_t *word = (uint16_t*) MADDR(sca4);
	RETURN((int) smbus_word_data_read(dev,addr,cmd,word));
}

// 425 TODO
int wazi_syscall_smbus_word_data_write (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint8_t sca3, uint16_t sca4) {
	SC(425, smbus_word_data_write);
	ERRSC(smbus_word_data_write);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t addr = sca2;
	uint8_t cmd = sca3;
	uint16_t word = sca4;
	RETURN((int) smbus_word_data_write(dev,addr,cmd,word));
}

// 426 TODO
int wazi_syscall_spi_release (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(426, spi_release);
	ERRSC(spi_release);
	struct device *dev = (struct device*) MADDR(sca1);
	struct spi_config *config = (struct spi_config*) MADDR(sca2);
	RETURN((int) spi_release(dev,config));
}

// 427 TODO
int wazi_syscall_spi_transceive (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4) {
	SC(427, spi_transceive);
	ERRSC(spi_transceive);
	struct device *dev = (struct device*) MADDR(sca1);
	struct spi_config *config = (struct spi_config*) MADDR(sca2);
	struct spi_buf_set *tx_bufs = (struct spi_buf_set*) MADDR(sca3);
	struct spi_buf_set *rx_bufs = (struct spi_buf_set*) MADDR(sca4);
	RETURN((int) spi_transceive(dev,config,tx_bufs,rx_bufs));
}

// 428 TODO
int wazi_syscall_syscon_get_base (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(428, syscon_get_base);
	ERRSC(syscon_get_base);
	struct device *dev = (struct device*) MADDR(sca1);
	uintptr_t *addr = (uintptr_t*) MADDR(sca2);
	RETURN((int) syscon_get_base(dev,addr));
}

// 429 TODO
int wazi_syscall_syscon_get_size (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(429, syscon_get_size);
	ERRSC(syscon_get_size);
	struct device *dev = (struct device*) MADDR(sca1);
	size_t *size = (size_t*) MADDR(sca2);
	RETURN((int) syscon_get_size(dev,size));
}

// 430 TODO
int wazi_syscall_syscon_read_reg (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, int32_t sca3) {
	SC(430, syscon_read_reg);
	ERRSC(syscon_read_reg);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t reg = sca2;
	uint32_t *val = (uint32_t*) MADDR(sca3);
	RETURN((int) syscon_read_reg(dev,reg,val));
}

// 431 TODO
int wazi_syscall_syscon_write_reg (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2, uint32_t sca3) {
	SC(431, syscon_write_reg);
	ERRSC(syscon_write_reg);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t reg = sca2;
	uint32_t val = sca3;
	RETURN((int) syscon_write_reg(dev,reg,val));
}

// 432 TODO
int wazi_syscall_sys_cache_data_flush_and_invd_range (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(432, sys_cache_data_flush_and_invd_range);
	ERRSC(sys_cache_data_flush_and_invd_range);
	void *addr = (void*) MADDR(sca1);
	size_t size = sca2;
	RETURN((int) sys_cache_data_flush_and_invd_range(addr,size));
}

// 433 TODO
int wazi_syscall_sys_cache_data_flush_range (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(433, sys_cache_data_flush_range);
	ERRSC(sys_cache_data_flush_range);
	void *addr = (void*) MADDR(sca1);
	size_t size = sca2;
	RETURN((int) sys_cache_data_flush_range(addr,size));
}

// 434 TODO
int wazi_syscall_sys_cache_data_invd_range (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(434, sys_cache_data_invd_range);
	ERRSC(sys_cache_data_invd_range);
	void *addr = (void*) MADDR(sca1);
	size_t size = sca2;
	RETURN((int) sys_cache_data_invd_range(addr,size));
}

// 435 TODO
int wazi_syscall_sys_clock_hw_cycles_per_sec_runtime_get (wasm_exec_env_t exec_env) {
	SC(435, sys_clock_hw_cycles_per_sec_runtime_get);
	ERRSC(sys_clock_hw_cycles_per_sec_runtime_get);
	RETURN((int) sys_clock_hw_cycles_per_sec_runtime_get());
}

// 436 TODO
int wazi_syscall_sys_csrand_get (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(436, sys_csrand_get);
	ERRSC(sys_csrand_get);
	void *dst = (void*) MADDR(sca1);
	size_t len = sca2;
	RETURN((int) sys_csrand_get(dst,len));
}

// 437 TODO
uint32_t wazi_syscall_sys_rand32_get (wasm_exec_env_t exec_env) {
	SC(437, sys_rand32_get);
	ERRSC(sys_rand32_get);
	RETURN((uint32_t) sys_rand32_get());
}

// 438 TODO
void wazi_syscall_sys_rand_get (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(438, sys_rand_get);
	ERRSC(sys_rand_get);
	void *dst = (void*) MADDR(sca1);
	size_t len = sca2;
	RETURN_VOID(sys_rand_get(dst,len));
}

// 439 TODO
int wazi_syscall_tgpio_pin_config_ext_timestamp (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3) {
	SC(439, tgpio_pin_config_ext_timestamp);
	ERRSC(tgpio_pin_config_ext_timestamp);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t pin = sca2;
	uint32_t event_polarity = sca3;
	RETURN((int) tgpio_pin_config_ext_timestamp(dev,pin,event_polarity));
}

// 440 TODO
int wazi_syscall_tgpio_pin_disable (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(440, tgpio_pin_disable);
	ERRSC(tgpio_pin_disable);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t pin = sca2;
	RETURN((int) tgpio_pin_disable(dev,pin));
}

// 441 TODO
int wazi_syscall_tgpio_pin_periodic_output (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint64_t sca3, uint64_t sca4, bool sca5) {
	SC(441, tgpio_pin_periodic_output);
	ERRSC(tgpio_pin_periodic_output);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t pin = sca2;
	uint64_t start_time = sca3;
	uint64_t repeat_interval = sca4;
	bool periodic_enable = sca5;
	RETURN((int) tgpio_pin_periodic_output(dev,pin,start_time,repeat_interval,periodic_enable));
}

// 442 TODO
int wazi_syscall_tgpio_pin_read_ts_ec (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3, int32_t sca4) {
	SC(442, tgpio_pin_read_ts_ec);
	ERRSC(tgpio_pin_read_ts_ec);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t pin = sca2;
	uint64_t *timestamp = (uint64_t*) MADDR(sca3);
	uint64_t *event_count = (uint64_t*) MADDR(sca4);
	RETURN((int) tgpio_pin_read_ts_ec(dev,pin,timestamp,event_count));
}

// 443 TODO
int wazi_syscall_tgpio_port_get_cycles_per_second (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(443, tgpio_port_get_cycles_per_second);
	ERRSC(tgpio_port_get_cycles_per_second);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t *cycles = (uint32_t*) MADDR(sca2);
	RETURN((int) tgpio_port_get_cycles_per_second(dev,cycles));
}

// 444 TODO
int wazi_syscall_tgpio_port_get_time (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(444, tgpio_port_get_time);
	ERRSC(tgpio_port_get_time);
	struct device *dev = (struct device*) MADDR(sca1);
	uint64_t *current_time = (uint64_t*) MADDR(sca2);
	RETURN((int) tgpio_port_get_time(dev,current_time));
}

// 445 TODO
int wazi_syscall_uart_configure (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(445, uart_configure);
	ERRSC(uart_configure);
	struct device *dev = (struct device*) MADDR(sca1);
	struct uart_config *cfg = (struct uart_config*) MADDR(sca2);
	RETURN((int) uart_configure(dev,cfg));
}

// 446 TODO
int wazi_syscall_uart_config_get (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(446, uart_config_get);
	ERRSC(uart_config_get);
	struct device *dev = (struct device*) MADDR(sca1);
	struct uart_config *cfg = (struct uart_config*) MADDR(sca2);
	RETURN((int) uart_config_get(dev,cfg));
}

// 447 TODO
int wazi_syscall_uart_drv_cmd (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3) {
	SC(447, uart_drv_cmd);
	ERRSC(uart_drv_cmd);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t cmd = sca2;
	uint32_t p = sca3;
	RETURN((int) uart_drv_cmd(dev,cmd,p));
}

// 448 TODO
int wazi_syscall_uart_err_check (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(448, uart_err_check);
	ERRSC(uart_err_check);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) uart_err_check(dev));
}

// 449 TODO
void wazi_syscall_uart_irq_err_disable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(449, uart_irq_err_disable);
	ERRSC(uart_irq_err_disable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN_VOID(uart_irq_err_disable(dev));
}

// 450 TODO
void wazi_syscall_uart_irq_err_enable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(450, uart_irq_err_enable);
	ERRSC(uart_irq_err_enable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN_VOID(uart_irq_err_enable(dev));
}

// 451 TODO
int wazi_syscall_uart_irq_is_pending (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(451, uart_irq_is_pending);
	ERRSC(uart_irq_is_pending);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) uart_irq_is_pending(dev));
}

// 452 TODO
void wazi_syscall_uart_irq_rx_disable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(452, uart_irq_rx_disable);
	ERRSC(uart_irq_rx_disable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN_VOID(uart_irq_rx_disable(dev));
}

// 453 TODO
void wazi_syscall_uart_irq_rx_enable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(453, uart_irq_rx_enable);
	ERRSC(uart_irq_rx_enable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN_VOID(uart_irq_rx_enable(dev));
}

// 454 TODO
void wazi_syscall_uart_irq_tx_disable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(454, uart_irq_tx_disable);
	ERRSC(uart_irq_tx_disable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN_VOID(uart_irq_tx_disable(dev));
}

// 455 TODO
void wazi_syscall_uart_irq_tx_enable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(455, uart_irq_tx_enable);
	ERRSC(uart_irq_tx_enable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN_VOID(uart_irq_tx_enable(dev));
}

// 456 TODO
int wazi_syscall_uart_irq_update (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(456, uart_irq_update);
	ERRSC(uart_irq_update);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) uart_irq_update(dev));
}

// 457 TODO
int wazi_syscall_uart_line_ctrl_get (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3) {
	SC(457, uart_line_ctrl_get);
	ERRSC(uart_line_ctrl_get);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t ctrl = sca2;
	uint32_t *val = (uint32_t*) MADDR(sca3);
	RETURN((int) uart_line_ctrl_get(dev,ctrl,val));
}

// 458 TODO
int wazi_syscall_uart_line_ctrl_set (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3) {
	SC(458, uart_line_ctrl_set);
	ERRSC(uart_line_ctrl_set);
	struct device *dev = (struct device*) MADDR(sca1);
	uint32_t ctrl = sca2;
	uint32_t val = sca3;
	RETURN((int) uart_line_ctrl_set(dev,ctrl,val));
}

// 459 TODO
int32_t wazi_syscall_uart_mux_find (wasm_exec_env_t exec_env, int sca1) {
	SC(459, uart_mux_find);
	ERRSC(uart_mux_find);
	int dlci_address = sca1;
	RETURN((int32_t) uart_mux_find(dlci_address));
}

// 460 TODO
int wazi_syscall_uart_poll_in (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(460, uart_poll_in);
	ERRSC(uart_poll_in);
	struct device *dev = (struct device*) MADDR(sca1);
	unsigned char *p_char = (unsigned char*) MADDR(sca2);
	RETURN((int) uart_poll_in(dev,p_char));
}

// 461 TODO
int wazi_syscall_uart_poll_in_u16 (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2) {
	SC(461, uart_poll_in_u16);
	ERRSC(uart_poll_in_u16);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t *p_u16 = (uint16_t*) MADDR(sca2);
	RETURN((int) uart_poll_in_u16(dev,p_u16));
}

// 462 TODO
void wazi_syscall_uart_poll_out (wasm_exec_env_t exec_env, int32_t sca1, unsigned char sca2) {
	SC(462, uart_poll_out);
	ERRSC(uart_poll_out);
	struct device *dev = (struct device*) MADDR(sca1);
	unsigned char out_char = sca2;
	RETURN_VOID(uart_poll_out(dev,out_char));
}

// 463 TODO
void wazi_syscall_uart_poll_out_u16 (wasm_exec_env_t exec_env, int32_t sca1, uint16_t sca2) {
	SC(463, uart_poll_out_u16);
	ERRSC(uart_poll_out_u16);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t out_u16 = sca2;
	RETURN_VOID(uart_poll_out_u16(dev,out_u16));
}

// 464 TODO
int wazi_syscall_uart_rx_disable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(464, uart_rx_disable);
	ERRSC(uart_rx_disable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) uart_rx_disable(dev));
}

// 465 TODO
int wazi_syscall_uart_rx_enable (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4) {
	SC(465, uart_rx_enable);
	ERRSC(uart_rx_enable);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t *buf = (uint8_t*) MADDR(sca2);
	size_t len = sca3;
	int32_t timeout = sca4;
	RETURN((int) uart_rx_enable(dev,buf,len,timeout));
}

// 466 TODO
int wazi_syscall_uart_rx_enable_u16 (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4) {
	SC(466, uart_rx_enable_u16);
	ERRSC(uart_rx_enable_u16);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t *buf = (uint16_t*) MADDR(sca2);
	size_t len = sca3;
	int32_t timeout = sca4;
	RETURN((int) uart_rx_enable_u16(dev,buf,len,timeout));
}

// 467 TODO
int wazi_syscall_uart_tx (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4) {
	SC(467, uart_tx);
	ERRSC(uart_tx);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t *buf = (uint8_t*) MADDR(sca2);
	size_t len = sca3;
	int32_t timeout = sca4;
	RETURN((int) uart_tx(dev,buf,len,timeout));
}

// 468 TODO
int wazi_syscall_uart_tx_abort (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(468, uart_tx_abort);
	ERRSC(uart_tx_abort);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) uart_tx_abort(dev));
}

// 469 TODO
int wazi_syscall_uart_tx_u16 (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3, int32_t sca4) {
	SC(469, uart_tx_u16);
	ERRSC(uart_tx_u16);
	struct device *dev = (struct device*) MADDR(sca1);
	uint16_t *buf = (uint16_t*) MADDR(sca2);
	size_t len = sca3;
	int32_t timeout = sca4;
	RETURN((int) uart_tx_u16(dev,buf,len,timeout));
}

// 470 TODO
void wazi_syscall_updatehub_autohandler (wasm_exec_env_t exec_env) {
	SC(470, updatehub_autohandler);
	ERRSC(updatehub_autohandler);
	RETURN_VOID(updatehub_autohandler());
}

// 471 TODO
int wazi_syscall_updatehub_confirm (wasm_exec_env_t exec_env) {
	SC(471, updatehub_confirm);
	ERRSC(updatehub_confirm);
	RETURN((int) updatehub_confirm());
}

// 472 TODO
int32_t wazi_syscall_updatehub_probe (wasm_exec_env_t exec_env) {
	SC(472, updatehub_probe);
	ERRSC(updatehub_probe);
	RETURN((int32_t) updatehub_probe());
}

// 473 TODO
int wazi_syscall_updatehub_reboot (wasm_exec_env_t exec_env) {
	SC(473, updatehub_reboot);
	ERRSC(updatehub_reboot);
	RETURN((int) updatehub_reboot());
}

// 474 TODO
int32_t wazi_syscall_updatehub_update (wasm_exec_env_t exec_env) {
	SC(474, updatehub_update);
	ERRSC(updatehub_update);
	RETURN((int32_t) updatehub_update());
}

// 475 TODO
void wazi_syscall_user_fault (wasm_exec_env_t exec_env, unsigned int sca1) {
	SC(475, user_fault);
	ERRSC(user_fault);
	unsigned int reason = sca1;
	RETURN_VOID(user_fault(reason));
}

// 476 TODO
int wazi_syscall_w1_change_bus_lock (wasm_exec_env_t exec_env, int32_t sca1, bool sca2) {
	SC(476, w1_change_bus_lock);
	ERRSC(w1_change_bus_lock);
	struct device *dev = (struct device*) MADDR(sca1);
	bool lock = sca2;
	RETURN((int) w1_change_bus_lock(dev,lock));
}

// 477 TODO
int wazi_syscall_w1_configure (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(477, w1_configure);
	ERRSC(w1_configure);
	struct device *dev = (struct device*) MADDR(sca1);
	enum w1_settings_type type = sca2;
	uint32_t value = sca3;
	RETURN((int) w1_configure(dev,type,value));
}

// 478 TODO
uint32_t wazi_syscall_w1_get_slave_count (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(478, w1_get_slave_count);
	ERRSC(w1_get_slave_count);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((uint32_t) w1_get_slave_count(dev));
}

// 479 TODO
int wazi_syscall_w1_read_bit (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(479, w1_read_bit);
	ERRSC(w1_read_bit);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) w1_read_bit(dev));
}

// 480 TODO
int wazi_syscall_w1_read_block (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(480, w1_read_block);
	ERRSC(w1_read_block);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t *buffer = (uint8_t*) MADDR(sca2);
	size_t len = sca3;
	RETURN((int) w1_read_block(dev,buffer,len));
}

// 481 TODO
int wazi_syscall_w1_read_byte (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(481, w1_read_byte);
	ERRSC(w1_read_byte);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) w1_read_byte(dev));
}

// 482 TODO
int wazi_syscall_w1_reset_bus (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(482, w1_reset_bus);
	ERRSC(w1_reset_bus);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) w1_reset_bus(dev));
}

// 483 TODO
int wazi_syscall_w1_search_bus (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2, uint8_t sca3, int32_t sca4, int32_t sca5) {
	SC(483, w1_search_bus);
	ERRSC(w1_search_bus);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t command = sca2;
	uint8_t family = sca3;
	w1_search_callback_t callback = ERROR_FN_PTR;
	void *user_data = (void*) MADDR(sca5);
	RETURN((int) w1_search_bus(dev,command,family,callback,user_data));
}

// 484 TODO
int wazi_syscall_w1_write_bit (wasm_exec_env_t exec_env, int32_t sca1, bool sca2) {
	SC(484, w1_write_bit);
	ERRSC(w1_write_bit);
	struct device *dev = (struct device*) MADDR(sca1);
	bool bit = sca2;
	RETURN((int) w1_write_bit(dev,bit));
}

// 485 TODO
int wazi_syscall_w1_write_block (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, uint32_t sca3) {
	SC(485, w1_write_block);
	ERRSC(w1_write_block);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t *buffer = (uint8_t*) MADDR(sca2);
	size_t len = sca3;
	RETURN((int) w1_write_block(dev,buffer,len));
}

// 486 TODO
int wazi_syscall_w1_write_byte (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2) {
	SC(486, w1_write_byte);
	ERRSC(w1_write_byte);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t byte = sca2;
	RETURN((int) w1_write_byte(dev,byte));
}

// 487 TODO
int wazi_syscall_wdt_disable (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(487, wdt_disable);
	ERRSC(wdt_disable);
	struct device *dev = (struct device*) MADDR(sca1);
	RETURN((int) wdt_disable(dev));
}

// 488 TODO
int wazi_syscall_wdt_feed (wasm_exec_env_t exec_env, int32_t sca1, int sca2) {
	SC(488, wdt_feed);
	ERRSC(wdt_feed);
	struct device *dev = (struct device*) MADDR(sca1);
	int channel_id = sca2;
	RETURN((int) wdt_feed(dev,channel_id));
}

// 489 TODO
int wazi_syscall_wdt_setup (wasm_exec_env_t exec_env, int32_t sca1, uint8_t sca2) {
	SC(489, wdt_setup);
	ERRSC(wdt_setup);
	struct device *dev = (struct device*) MADDR(sca1);
	uint8_t options = sca2;
	RETURN((int) wdt_setup(dev,options));
}

// 490 TODO
void wazi_syscall_xtensa_user_fault (wasm_exec_env_t exec_env, unsigned int sca1) {
	SC(490, xtensa_user_fault);
	ERRSC(xtensa_user_fault);
	unsigned int reason = sca1;
	RETURN_VOID(xtensa_user_fault(reason));
}

#if defined(CONFIG_MINIMAL_LIBC) || defined(CONFIG_PICOLIBC)
// 491 
int wazi_syscall_zephyr_fputc (wasm_exec_env_t exec_env, int sca1, int32_t sca2) {
	SC(491, zephyr_fputc);
	int c = sca1;
	FILE *stream = (FILE*) MADDR_FILE(sca2);
	RETURN((int) zephyr_fputc(c,stream));
}

// 492 TODO
uint32_t wazi_syscall_zephyr_fwrite (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, uint32_t sca3, int32_t sca4) {
	SC(492, zephyr_fwrite);
	ERRSC(zephyr_fwrite);
	void *ptr = (void*) MADDR(sca1);
	size_t size = sca2;
	size_t nitems = sca3;
	FILE *stream = (FILE*) MADDR_FILE(sca4);
	RETURN((uint32_t) zephyr_fwrite(ptr,size,nitems,stream));
}
#endif

#ifdef CONFIG_NEWLIB_LIBC
// 493 TODO
int wazi_syscall_zephyr_read_stdin (wasm_exec_env_t exec_env, int32_t sca1, int sca2) {
	SC(493, zephyr_read_stdin);
	ERRSC(zephyr_read_stdin);
	char *buf = (char*) MADDR(sca1);
	int nbytes = sca2;
	RETURN((int) zephyr_read_stdin(buf,nbytes));
}

// 494 TODO
int wazi_syscall_zephyr_write_stdout (wasm_exec_env_t exec_env, int32_t sca1, int sca2) {
	SC(494, zephyr_write_stdout);
	ERRSC(zephyr_write_stdout);
	void *buf = (void*) MADDR(sca1);
	int nbytes = sca2;
	RETURN((int) zephyr_write_stdout(buf,nbytes));
}
#endif

// 495 
int wazi_syscall_zsock_accept (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3) {
	SC(495, zsock_accept);
	int sock = sca1;
	struct sockaddr *addr = (struct sockaddr*) MADDR(sca2);
	socklen_t *addrlen = (socklen_t*) MADDR(sca3);
	RETURN((int) zsock_accept(sock,addr,addrlen));
}

// 496 
int wazi_syscall_zsock_bind (wasm_exec_env_t exec_env, int sca1, int32_t sca2, uint32_t sca3) {
	SC(496, zsock_bind);
	int sock = sca1;
	struct sockaddr *addr = (struct sockaddr*) MADDR(sca2);
	socklen_t addrlen = sca3;
	RETURN((int) zsock_bind(sock,addr,addrlen));
}

// 497 
int wazi_syscall_zsock_close (wasm_exec_env_t exec_env, int sca1) {
	SC(497, zsock_close);
	int sock = sca1;
	RETURN((int) zsock_close(sock));
}

// 498 
int wazi_syscall_zsock_connect (wasm_exec_env_t exec_env, int sca1, int32_t sca2, uint32_t sca3) {
	SC(498, zsock_connect);
	int sock = sca1;
	struct sockaddr *addr = (struct sockaddr*) MADDR(sca2);
	socklen_t addrlen = sca3;
	RETURN((int) zsock_connect(sock,addr,addrlen));
}

// 499 
int wazi_syscall_zsock_fcntl (wasm_exec_env_t exec_env, int sca1, int sca2, int sca3) {
	SC(499, zsock_fcntl);
	int sock = sca1;
	int cmd = sca2;
	int flags = sca3;
	RETURN((int) zsock_fcntl(sock,cmd,flags));
}

// 500 
int wazi_syscall_zsock_gethostname (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(500, zsock_gethostname);
	char *buf = (char*) MADDR(sca1);
	size_t len = sca2;
	RETURN((int) zsock_gethostname(buf,len));
}

// 501 
int wazi_syscall_zsock_getpeername (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3) {
	SC(501, zsock_getpeername);
	int sock = sca1;
	struct sockaddr *addr = (struct sockaddr*) MADDR(sca2);
	socklen_t *addrlen = (socklen_t*) MADDR(sca3);
	RETURN((int) zsock_getpeername(sock,addr,addrlen));
}

// 502 
int wazi_syscall_zsock_getsockname (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3) {
	SC(502, zsock_getsockname);
	int sock = sca1;
	struct sockaddr *addr = (struct sockaddr*) MADDR(sca2);
	socklen_t *addrlen = (socklen_t*) MADDR(sca3);
	RETURN((int) zsock_getsockname(sock,addr,addrlen));
}

// 503 
int wazi_syscall_zsock_getsockopt (wasm_exec_env_t exec_env, int sca1, int sca2, int sca3, int32_t sca4, int32_t sca5) {
	SC(503, zsock_getsockopt);
	int sock = sca1;
	int level = sca2;
	int optname = sca3;
	void *optval = (void*) MADDR(sca4);
	socklen_t *optlen = (socklen_t*) MADDR(sca5);
	RETURN((int) zsock_getsockopt(sock,level,optname,optval,optlen));
}

// 504 TODO
int32_t wazi_syscall_zsock_get_context_object (wasm_exec_env_t exec_env, int sca1) {
	SC(504, zsock_get_context_object);
	ERRSC(zsock_get_context_object);
	int sock = sca1;
	RETURN((int32_t) zsock_get_context_object(sock));
}

// 505 
int wazi_syscall_zsock_inet_pton (wasm_exec_env_t exec_env, uint16_t sca1, int32_t sca2, int32_t sca3) {
	SC(505, zsock_inet_pton);
	sa_family_t family = sca1;
	char *src = (char*) MADDR(sca2);
	void *dst = (void*) MADDR(sca3);
	RETURN((int) zsock_inet_pton(family,src,dst));
}

// 506 TODO
int wazi_syscall_zsock_ioctl (wasm_exec_env_t exec_env, int sca1, uint32_t sca2, int32_t sca3) {
	SC(506, zsock_ioctl);
	ERRSC(zsock_ioctl);
	int sock = sca1;
	unsigned long request = sca2;
	va_list ap = VA_LIST_INIT(sca3);
	RETURN((int) zsock_ioctl(sock,request,ap));
}

// 507 
int wazi_syscall_zsock_listen (wasm_exec_env_t exec_env, int sca1, int sca2) {
	SC(507, zsock_listen);
	int sock = sca1;
	int backlog = sca2;
	RETURN((int) zsock_listen(sock,backlog));
}

// 508 
int wazi_syscall_zsock_poll (wasm_exec_env_t exec_env, int32_t sca1, int sca2, int sca3) {
	SC(508, zsock_poll);
	struct zsock_pollfd *fds = (struct zsock_pollfd*) MADDR(sca1);
	int nfds = sca2;
	int timeout = sca3;
	RETURN((int) zsock_poll(fds,nfds,timeout));
}

// 509 
uint32_t wazi_syscall_zsock_recvfrom (wasm_exec_env_t exec_env, int sca1, int32_t sca2, uint32_t sca3, int sca4, int32_t sca5, int32_t sca6) {
	SC(509, zsock_recvfrom);
	int sock = sca1;
	void *buf = (void*) MADDR(sca2);
	size_t max_len = sca3;
	int flags = sca4;
	struct sockaddr *src_addr = (struct sockaddr*) MADDR(sca5);
	socklen_t *addrlen = (socklen_t*) MADDR(sca6);
	RETURN((uint32_t) zsock_recvfrom(sock,buf,max_len,flags,src_addr,addrlen));
}

// 510 
uint32_t wazi_syscall_zsock_recvmsg (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int sca3) {
	SC(510, zsock_recvmsg);
	int sock = sca1;
	struct msghdr *msg = (struct msghdr*) copy_msghdr(exec_env, MADDR(sca2));
	int flags = sca3;
  uint32_t retval = (uint32_t) zsock_recvmsg(sock,msg,flags);
  free(msg);
	RETURN(retval);
}

// 511 
int wazi_syscall_zsock_select (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int32_t sca3, int32_t sca4, int32_t sca5) {
	SC(511, zsock_select);
	int nfds = sca1;
	zsock_fd_set *readfds = (zsock_fd_set*) MADDR(sca2);
	zsock_fd_set *writefds = (zsock_fd_set*) MADDR(sca3);
	zsock_fd_set *exceptfds = (zsock_fd_set*) MADDR(sca4);
	struct zsock_timeval *timeout = (struct zsock_timeval*) MADDR(sca5);
	RETURN((int) zsock_select(nfds,readfds,writefds,exceptfds,timeout));
}

// 512 
uint32_t wazi_syscall_zsock_sendmsg (wasm_exec_env_t exec_env, int sca1, int32_t sca2, int sca3) {
	SC(512, zsock_sendmsg);
	int sock = sca1;
	struct msghdr *msg = (struct msghdr*) copy_msghdr(exec_env, MADDR(sca2));
	int flags = sca3;
  uint32_t retval = (uint32_t) zsock_sendmsg(sock,msg,flags);
  free(msg);
	RETURN(retval);
}

// 513 
uint32_t wazi_syscall_zsock_sendto (wasm_exec_env_t exec_env, int sca1, int32_t sca2, uint32_t sca3, int sca4, int32_t sca5, uint32_t sca6) {
	SC(513, zsock_sendto);
	int sock = sca1;
	void *buf = (void*) MADDR(sca2);
	size_t len = sca3;
	int flags = sca4;
	struct sockaddr *dest_addr = (struct sockaddr*) MADDR(sca5);
	socklen_t addrlen = sca6;
	RETURN((uint32_t) zsock_sendto(sock,buf,len,flags,dest_addr,addrlen));
}

// 514 
int wazi_syscall_zsock_setsockopt (wasm_exec_env_t exec_env, int sca1, int sca2, int sca3, int32_t sca4, uint32_t sca5) {
	SC(514, zsock_setsockopt);
	int sock = sca1;
	int level = sca2;
	int optname = sca3;
	void *optval = (void*) MADDR(sca4);
	socklen_t optlen = sca5;
	RETURN((int) zsock_setsockopt(sock,level,optname,optval,optlen));
}

// 515 
int wazi_syscall_zsock_shutdown (wasm_exec_env_t exec_env, int sca1, int sca2) {
	SC(515, zsock_shutdown);
	int sock = sca1;
	int how = sca2;
	RETURN((int) zsock_shutdown(sock,how));
}

// 516 
int wazi_syscall_zsock_socket (wasm_exec_env_t exec_env, int sca1, int sca2, int sca3) {
	SC(516, zsock_socket);
	int family = sca1;
	int type = sca2;
	int proto = sca3;
	RETURN((int) zsock_socket(family,type,proto));
}

// 517 
int wazi_syscall_zsock_socketpair (wasm_exec_env_t exec_env, int sca1, int sca2, int sca3, int32_t sca4) {
	SC(517, zsock_socketpair);
	int family = sca1;
	int type = sca2;
	int proto = sca3;
	int *sv = (int*) MADDR(sca4);
	RETURN((int) zsock_socketpair(family,type,proto,sv));
}

// 518 TODO
int32_t wazi_syscall_z_errno (wasm_exec_env_t exec_env) {
	SC(518, z_errno);
	ERRSC(z_errno);
	RETURN((int32_t) z_errno());
}

// 519 TODO
void wazi_syscall_z_log_msg_simple_create_0 (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3) {
	SC(519, z_log_msg_simple_create_0);
	ERRSC(z_log_msg_simple_create_0);
	void *source = (void*) MADDR(sca1);
	uint32_t level = sca2;
	char *fmt = (char*) MADDR(sca3);
	RETURN_VOID(z_log_msg_simple_create_0(source,level,fmt));
}

// 520 TODO
void wazi_syscall_z_log_msg_simple_create_1 (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3, uint32_t sca4) {
	SC(520, z_log_msg_simple_create_1);
	ERRSC(z_log_msg_simple_create_1);
	void *source = (void*) MADDR(sca1);
	uint32_t level = sca2;
	char *fmt = (char*) MADDR(sca3);
	uint32_t arg = sca4;
	RETURN_VOID(z_log_msg_simple_create_1(source,level,fmt,arg));
}

// 521 TODO
void wazi_syscall_z_log_msg_simple_create_2 (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3, uint32_t sca4, uint32_t sca5) {
	SC(521, z_log_msg_simple_create_2);
	ERRSC(z_log_msg_simple_create_2);
	void *source = (void*) MADDR(sca1);
	uint32_t level = sca2;
	char *fmt = (char*) MADDR(sca3);
	uint32_t arg0 = sca4;
	uint32_t arg1 = sca5;
	RETURN_VOID(z_log_msg_simple_create_2(source,level,fmt,arg0,arg1));
}

// 522 TODO
void wazi_syscall_z_log_msg_static_create (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2, int32_t sca3, int32_t sca4) {
	SC(522, z_log_msg_static_create);
	ERRSC(z_log_msg_static_create);
	void *source = (void*) MADDR(sca1);
	struct log_msg_desc desc = LOG_MSG_DESC_INIT(sca2);
	uint8_t *package = (uint8_t*) MADDR(sca3);
	void *data = (void*) MADDR(sca4);
	RETURN_VOID(z_log_msg_static_create(source,desc,package,data));
}

// 523 TODO
int wazi_syscall_z_sys_mutex_kernel_lock (wasm_exec_env_t exec_env, int32_t sca1, uint32_t sca2) {
	SC(523, z_sys_mutex_kernel_lock);
	ERRSC(z_sys_mutex_kernel_lock);
	struct sys_mutex *mutex = (struct sys_mutex*) MADDR(sca1);
	k_timeout_t timeout = { .ticks = sca2 };
	RETURN((int) z_sys_mutex_kernel_lock(mutex,timeout));
}

// 524 TODO
int wazi_syscall_z_sys_mutex_kernel_unlock (wasm_exec_env_t exec_env, int32_t sca1) {
	SC(524, z_sys_mutex_kernel_unlock);
	ERRSC(z_sys_mutex_kernel_unlock);
	struct sys_mutex *mutex = (struct sys_mutex*) MADDR(sca1);
	RETURN((int) z_sys_mutex_kernel_unlock(mutex));
}

// 525 TODO
int wazi_syscall_z_zsock_getaddrinfo_internal (wasm_exec_env_t exec_env, int32_t sca1, int32_t sca2, int32_t sca3, int32_t sca4) {
	SC(525, z_zsock_getaddrinfo_internal);
	ERRSC(z_zsock_getaddrinfo_internal);
	char *host = (char*) MADDR(sca1);
	char *service = (char*) MADDR(sca2);
	struct zsock_addrinfo *hints = (struct zsock_addrinfo*) MADDR(sca3);
	struct zsock_addrinfo *res = (struct zsock_addrinfo*) MADDR(sca4);
	RETURN((int) z_zsock_getaddrinfo_internal(host,service,hints,res));
}

// 526 TODO
int wazi_syscall___posix_clock_get_base (wasm_exec_env_t exec_env, uint32_t sca1, int32_t sca2) {
	SC(526, __posix_clock_get_base);
	ERRSC(__posix_clock_get_base);
	clockid_t clock_id = sca1;
	struct timespec *ts = (struct timespec*) MADDR(sca2);
	RETURN((int) __posix_clock_get_base(clock_id,ts));
}

// 491
//long wazi_syscall_zephyr_fputc(wasm_exec_env_t exec_env, int c, uintptr_t stream) {
//  SC(491,zephyr_fputc);
//  FILE *mstream = MADDR_FILE(stream);
//  RETURN(zephyr_fputc(c, mstream));
//}

void wazi_syscall_placeholder (wasm_exec_env_t exec_env) {
  return;
}

/* Native WAZI Symbols */
#define NSYMBOL(symbol, fn, sign) \
  { #symbol, (void*)fn, sign, NULL }

static NativeSymbol wazi_native_symbols[] = {
  // Syscalls
	//NSYMBOL (                    SYS_adc_channel_setup,                          wazi_syscall_adc_channel_setup,      "(ii)i" ),
	//NSYMBOL (                             SYS_adc_read,                                   wazi_syscall_adc_read,      "(ii)i" ),
	//NSYMBOL (                       SYS_adc_read_async,                             wazi_syscall_adc_read_async,     "(iii)i" ),
	//NSYMBOL (                           SYS_atomic_add,                                 wazi_syscall_atomic_add,      "(ii)i" ),
	//NSYMBOL (                           SYS_atomic_and,                                 wazi_syscall_atomic_and,      "(ii)i" ),
	//NSYMBOL (                           SYS_atomic_cas,                                 wazi_syscall_atomic_cas,     "(iii)i" ),
	//NSYMBOL (                          SYS_atomic_nand,                                wazi_syscall_atomic_nand,      "(ii)i" ),
	//NSYMBOL (                            SYS_atomic_or,                                  wazi_syscall_atomic_or,      "(ii)i" ),
	//NSYMBOL (                       SYS_atomic_ptr_cas,                             wazi_syscall_atomic_ptr_cas,     "(iii)i" ),
	//NSYMBOL (                       SYS_atomic_ptr_set,                             wazi_syscall_atomic_ptr_set,      "(ii)i" ),
	//NSYMBOL (                           SYS_atomic_set,                                 wazi_syscall_atomic_set,      "(ii)i" ),
	//NSYMBOL (                           SYS_atomic_sub,                                 wazi_syscall_atomic_sub,      "(ii)i" ),
	//NSYMBOL (                           SYS_atomic_xor,                                 wazi_syscall_atomic_xor,      "(ii)i" ),
	//NSYMBOL (             SYS_auxdisplay_backlight_get,                   wazi_syscall_auxdisplay_backlight_get,      "(ii)i" ),
	//NSYMBOL (             SYS_auxdisplay_backlight_set,                   wazi_syscall_auxdisplay_backlight_set,      "(ii)i" ),
	//NSYMBOL (            SYS_auxdisplay_brightness_get,                  wazi_syscall_auxdisplay_brightness_get,      "(ii)i" ),
	//NSYMBOL (            SYS_auxdisplay_brightness_set,                  wazi_syscall_auxdisplay_brightness_set,      "(ii)i" ),
	//NSYMBOL (          SYS_auxdisplay_capabilities_get,                wazi_syscall_auxdisplay_capabilities_get,      "(ii)i" ),
	//NSYMBOL (                     SYS_auxdisplay_clear,                           wazi_syscall_auxdisplay_clear,       "(i)i" ),
	//NSYMBOL (       SYS_auxdisplay_cursor_position_get,             wazi_syscall_auxdisplay_cursor_position_get,     "(iii)i" ),
	//NSYMBOL (       SYS_auxdisplay_cursor_position_set,             wazi_syscall_auxdisplay_cursor_position_set,    "(iiii)i" ),
	//NSYMBOL (        SYS_auxdisplay_cursor_set_enabled,              wazi_syscall_auxdisplay_cursor_set_enabled,      "(ii)i" ),
	//NSYMBOL (          SYS_auxdisplay_cursor_shift_set,                wazi_syscall_auxdisplay_cursor_shift_set,     "(iii)i" ),
	//NSYMBOL (      SYS_auxdisplay_custom_character_set,            wazi_syscall_auxdisplay_custom_character_set,      "(ii)i" ),
	//NSYMBOL (            SYS_auxdisplay_custom_command,                  wazi_syscall_auxdisplay_custom_command,      "(ii)i" ),
	//NSYMBOL (               SYS_auxdisplay_display_off,                     wazi_syscall_auxdisplay_display_off,       "(i)i" ),
	//NSYMBOL (                SYS_auxdisplay_display_on,                      wazi_syscall_auxdisplay_display_on,       "(i)i" ),
	//NSYMBOL (      SYS_auxdisplay_display_position_get,            wazi_syscall_auxdisplay_display_position_get,     "(iii)i" ),
	//NSYMBOL (      SYS_auxdisplay_display_position_set,            wazi_syscall_auxdisplay_display_position_set,    "(iiii)i" ),
	//NSYMBOL (                   SYS_auxdisplay_is_busy,                         wazi_syscall_auxdisplay_is_busy,       "(i)i" ),
	//NSYMBOL ( SYS_auxdisplay_position_blinking_set_enabled,   wazi_syscall_auxdisplay_position_blinking_set_enabled,      "(ii)i" ),
	//NSYMBOL (                     SYS_auxdisplay_write,                           wazi_syscall_auxdisplay_write,     "(iii)i" ),
	//NSYMBOL (                  SYS_bbram_check_invalid,                        wazi_syscall_bbram_check_invalid,       "(i)i" ),
	//NSYMBOL (                    SYS_bbram_check_power,                          wazi_syscall_bbram_check_power,       "(i)i" ),
	//NSYMBOL (            SYS_bbram_check_standby_power,                  wazi_syscall_bbram_check_standby_power,       "(i)i" ),
	//NSYMBOL (                       SYS_bbram_get_size,                             wazi_syscall_bbram_get_size,      "(ii)i" ),
	//NSYMBOL (                           SYS_bbram_read,                                 wazi_syscall_bbram_read,    "(iiii)i" ),
	//NSYMBOL (                          SYS_bbram_write,                                wazi_syscall_bbram_write,    "(iiii)i" ),
	//NSYMBOL (                   SYS_bc12_set_result_cb,                         wazi_syscall_bc12_set_result_cb,     "(iii)i" ),
	//NSYMBOL (                        SYS_bc12_set_role,                              wazi_syscall_bc12_set_role,      "(ii)i" ),
	//NSYMBOL (               SYS_can_add_rx_filter_msgq,                     wazi_syscall_can_add_rx_filter_msgq,     "(iii)i" ),
	//NSYMBOL (                      SYS_can_calc_timing,                            wazi_syscall_can_calc_timing,    "(iiii)i" ),
	//NSYMBOL (                 SYS_can_calc_timing_data,                       wazi_syscall_can_calc_timing_data,    "(iiii)i" ),
	//NSYMBOL (                 SYS_can_get_capabilities,                       wazi_syscall_can_get_capabilities,      "(ii)i" ),
	//NSYMBOL (                   SYS_can_get_core_clock,                         wazi_syscall_can_get_core_clock,      "(ii)i" ),
	//NSYMBOL (                  SYS_can_get_max_bitrate,                        wazi_syscall_can_get_max_bitrate,      "(ii)i" ),
	//NSYMBOL (                  SYS_can_get_max_filters,                        wazi_syscall_can_get_max_filters,      "(ii)i" ),
	//NSYMBOL (                         SYS_can_get_mode,                               wazi_syscall_can_get_mode,       "(i)i" ),
	//NSYMBOL (                        SYS_can_get_state,                              wazi_syscall_can_get_state,     "(iii)i" ),
	//NSYMBOL (              SYS_can_get_timing_data_max,                    wazi_syscall_can_get_timing_data_max,       "(i)i" ),
	//NSYMBOL (              SYS_can_get_timing_data_min,                    wazi_syscall_can_get_timing_data_min,       "(i)i" ),
	//NSYMBOL (                   SYS_can_get_timing_max,                         wazi_syscall_can_get_timing_max,       "(i)i" ),
	//NSYMBOL (                   SYS_can_get_timing_min,                         wazi_syscall_can_get_timing_min,       "(i)i" ),
	//NSYMBOL (                  SYS_can_get_transceiver,                        wazi_syscall_can_get_transceiver,       "(i)i" ),
	//NSYMBOL (                          SYS_can_recover,                                wazi_syscall_can_recover,      "(ii)i" ),
	//NSYMBOL (                 SYS_can_remove_rx_filter,                       wazi_syscall_can_remove_rx_filter,       "(ii)" ),
	//NSYMBOL (                             SYS_can_send,                                   wazi_syscall_can_send,   "(iiiii)i" ),
	//NSYMBOL (                      SYS_can_set_bitrate,                            wazi_syscall_can_set_bitrate,      "(ii)i" ),
	//NSYMBOL (                 SYS_can_set_bitrate_data,                       wazi_syscall_can_set_bitrate_data,      "(ii)i" ),
	//NSYMBOL (                         SYS_can_set_mode,                               wazi_syscall_can_set_mode,      "(ii)i" ),
	//NSYMBOL (                       SYS_can_set_timing,                             wazi_syscall_can_set_timing,      "(ii)i" ),
	//NSYMBOL (                  SYS_can_set_timing_data,                        wazi_syscall_can_set_timing_data,      "(ii)i" ),
	//NSYMBOL (                            SYS_can_start,                                  wazi_syscall_can_start,       "(i)i" ),
	//NSYMBOL (             SYS_can_stats_get_ack_errors,                   wazi_syscall_can_stats_get_ack_errors,       "(i)i" ),
	//NSYMBOL (            SYS_can_stats_get_bit0_errors,                  wazi_syscall_can_stats_get_bit0_errors,       "(i)i" ),
	//NSYMBOL (            SYS_can_stats_get_bit1_errors,                  wazi_syscall_can_stats_get_bit1_errors,       "(i)i" ),
	//NSYMBOL (             SYS_can_stats_get_bit_errors,                   wazi_syscall_can_stats_get_bit_errors,       "(i)i" ),
	//NSYMBOL (             SYS_can_stats_get_crc_errors,                   wazi_syscall_can_stats_get_crc_errors,       "(i)i" ),
	//NSYMBOL (            SYS_can_stats_get_form_errors,                  wazi_syscall_can_stats_get_form_errors,       "(i)i" ),
	//NSYMBOL (            SYS_can_stats_get_rx_overruns,                  wazi_syscall_can_stats_get_rx_overruns,       "(i)i" ),
	//NSYMBOL (           SYS_can_stats_get_stuff_errors,                 wazi_syscall_can_stats_get_stuff_errors,       "(i)i" ),
	//NSYMBOL (                             SYS_can_stop,                                   wazi_syscall_can_stop,       "(i)i" ),
	//NSYMBOL (                SYS_charger_charge_enable,                      wazi_syscall_charger_charge_enable,      "(ii)i" ),
	//NSYMBOL (                     SYS_charger_get_prop,                           wazi_syscall_charger_get_prop,     "(iii)i" ),
	//NSYMBOL (                     SYS_charger_set_prop,                           wazi_syscall_charger_set_prop,     "(iii)i" ),
	//NSYMBOL (         SYS_counter_cancel_channel_alarm,               wazi_syscall_counter_cancel_channel_alarm,      "(ii)i" ),
	//NSYMBOL (                SYS_counter_get_frequency,                      wazi_syscall_counter_get_frequency,       "(i)i" ),
	//NSYMBOL (             SYS_counter_get_guard_period,                   wazi_syscall_counter_get_guard_period,      "(ii)i" ),
	//NSYMBOL (            SYS_counter_get_max_top_value,                  wazi_syscall_counter_get_max_top_value,       "(i)i" ),
	//NSYMBOL (          SYS_counter_get_num_of_channels,                wazi_syscall_counter_get_num_of_channels,       "(i)i" ),
	//NSYMBOL (              SYS_counter_get_pending_int,                    wazi_syscall_counter_get_pending_int,       "(i)i" ),
	//NSYMBOL (                SYS_counter_get_top_value,                      wazi_syscall_counter_get_top_value,       "(i)i" ),
	//NSYMBOL (                    SYS_counter_get_value,                          wazi_syscall_counter_get_value,      "(ii)i" ),
	//NSYMBOL (                 SYS_counter_get_value_64,                       wazi_syscall_counter_get_value_64,      "(ii)i" ),
	//NSYMBOL (               SYS_counter_is_counting_up,                     wazi_syscall_counter_is_counting_up,       "(i)i" ),
	//NSYMBOL (            SYS_counter_set_channel_alarm,                  wazi_syscall_counter_set_channel_alarm,     "(iii)i" ),
	//NSYMBOL (             SYS_counter_set_guard_period,                   wazi_syscall_counter_set_guard_period,     "(iii)i" ),
	//NSYMBOL (                SYS_counter_set_top_value,                      wazi_syscall_counter_set_top_value,      "(ii)i" ),
	//NSYMBOL (                        SYS_counter_start,                              wazi_syscall_counter_start,       "(i)i" ),
	//NSYMBOL (                         SYS_counter_stop,                               wazi_syscall_counter_stop,       "(i)i" ),
	//NSYMBOL (                  SYS_counter_ticks_to_us,                        wazi_syscall_counter_ticks_to_us,      "(ii)I" ),
	//NSYMBOL (                  SYS_counter_us_to_ticks,                        wazi_syscall_counter_us_to_ticks,      "(iI)i" ),
	//NSYMBOL (                    SYS_dac_channel_setup,                          wazi_syscall_dac_channel_setup,      "(ii)i" ),
	//NSYMBOL (                      SYS_dac_write_value,                            wazi_syscall_dac_write_value,     "(iii)i" ),
	//NSYMBOL (                   SYS_device_get_binding,                         wazi_syscall_device_get_binding,       "(i)i" ),
	//NSYMBOL (                      SYS_device_is_ready,                            wazi_syscall_device_is_ready,       "(i)i" ),
	//NSYMBOL (                    SYS_devmux_select_get,                          wazi_syscall_devmux_select_get,       "(i)i" ),
	//NSYMBOL (                    SYS_devmux_select_set,                          wazi_syscall_devmux_select_set,      "(ii)i" ),
	//NSYMBOL (                      SYS_dma_chan_filter,                            wazi_syscall_dma_chan_filter,     "(iii)i" ),
	//NSYMBOL (                  SYS_dma_release_channel,                        wazi_syscall_dma_release_channel,       "(ii)" ),
	//NSYMBOL (                  SYS_dma_request_channel,                        wazi_syscall_dma_request_channel,      "(ii)i" ),
	//NSYMBOL (                           SYS_dma_resume,                                 wazi_syscall_dma_resume,      "(ii)i" ),
	//NSYMBOL (                            SYS_dma_start,                                  wazi_syscall_dma_start,      "(ii)i" ),
	//NSYMBOL (                             SYS_dma_stop,                                   wazi_syscall_dma_stop,      "(ii)i" ),
	//NSYMBOL (                          SYS_dma_suspend,                                wazi_syscall_dma_suspend,      "(ii)i" ),
	//NSYMBOL (                      SYS_eeprom_get_size,                            wazi_syscall_eeprom_get_size,       "(i)i" ),
	//NSYMBOL (                          SYS_eeprom_read,                                wazi_syscall_eeprom_read,    "(iiii)i" ),
	//NSYMBOL (                         SYS_eeprom_write,                               wazi_syscall_eeprom_write,    "(iiii)i" ),
	//NSYMBOL (    SYS_emul_fuel_gauge_is_battery_cutoff,          wazi_syscall_emul_fuel_gauge_is_battery_cutoff,      "(ii)i" ),
	//NSYMBOL ( SYS_emul_fuel_gauge_set_battery_charging,       wazi_syscall_emul_fuel_gauge_set_battery_charging,     "(iii)i" ),
	//NSYMBOL (                  SYS_entropy_get_entropy,                        wazi_syscall_entropy_get_entropy,     "(iii)i" ),
	//NSYMBOL (                          SYS_espi_config,                                wazi_syscall_espi_config,      "(ii)i" ),
	//NSYMBOL (                     SYS_espi_flash_erase,                           wazi_syscall_espi_flash_erase,      "(ii)i" ),
	//NSYMBOL (              SYS_espi_get_channel_status,                    wazi_syscall_espi_get_channel_status,      "(ii)i" ),
	//NSYMBOL (                      SYS_espi_read_flash,                            wazi_syscall_espi_read_flash,      "(ii)i" ),
	//NSYMBOL (                SYS_espi_read_lpc_request,                      wazi_syscall_espi_read_lpc_request,     "(iii)i" ),
	//NSYMBOL (                    SYS_espi_read_request,                          wazi_syscall_espi_read_request,      "(ii)i" ),
	//NSYMBOL (                     SYS_espi_receive_oob,                           wazi_syscall_espi_receive_oob,      "(ii)i" ),
	//NSYMBOL (                   SYS_espi_receive_vwire,                         wazi_syscall_espi_receive_vwire,     "(iii)i" ),
	//NSYMBOL (                    SYS_espi_saf_activate,                          wazi_syscall_espi_saf_activate,       "(i)i" ),
	//NSYMBOL (                      SYS_espi_saf_config,                            wazi_syscall_espi_saf_config,      "(ii)i" ),
	//NSYMBOL (                 SYS_espi_saf_flash_erase,                       wazi_syscall_espi_saf_flash_erase,      "(ii)i" ),
	//NSYMBOL (                  SYS_espi_saf_flash_read,                        wazi_syscall_espi_saf_flash_read,      "(ii)i" ),
	//NSYMBOL (             SYS_espi_saf_flash_unsuccess,                   wazi_syscall_espi_saf_flash_unsuccess,      "(ii)i" ),
	//NSYMBOL (                 SYS_espi_saf_flash_write,                       wazi_syscall_espi_saf_flash_write,      "(ii)i" ),
	//NSYMBOL (          SYS_espi_saf_get_channel_status,                wazi_syscall_espi_saf_get_channel_status,       "(i)i" ),
	//NSYMBOL (      SYS_espi_saf_set_protection_regions,            wazi_syscall_espi_saf_set_protection_regions,      "(ii)i" ),
	//NSYMBOL (                        SYS_espi_send_oob,                              wazi_syscall_espi_send_oob,      "(ii)i" ),
	//NSYMBOL (                      SYS_espi_send_vwire,                            wazi_syscall_espi_send_vwire,     "(iii)i" ),
	//NSYMBOL (                     SYS_espi_write_flash,                           wazi_syscall_espi_write_flash,      "(ii)i" ),
	//NSYMBOL (               SYS_espi_write_lpc_request,                     wazi_syscall_espi_write_lpc_request,     "(iii)i" ),
	//NSYMBOL (                   SYS_espi_write_request,                         wazi_syscall_espi_write_request,      "(ii)i" ),
	//NSYMBOL (                          SYS_flash_erase,                                wazi_syscall_flash_erase,     "(iii)i" ),
	//NSYMBOL (                          SYS_flash_ex_op,                                wazi_syscall_flash_ex_op,    "(iiii)i" ),
	//NSYMBOL (                 SYS_flash_get_page_count,                       wazi_syscall_flash_get_page_count,       "(i)i" ),
	//NSYMBOL (           SYS_flash_get_page_info_by_idx,                 wazi_syscall_flash_get_page_info_by_idx,     "(iii)i" ),
	//NSYMBOL (          SYS_flash_get_page_info_by_offs,                wazi_syscall_flash_get_page_info_by_offs,     "(iii)i" ),
	//NSYMBOL (                 SYS_flash_get_parameters,                       wazi_syscall_flash_get_parameters,       "(i)i" ),
	//NSYMBOL (           SYS_flash_get_write_block_size,                 wazi_syscall_flash_get_write_block_size,       "(i)i" ),
	//NSYMBOL (                           SYS_flash_read,                                 wazi_syscall_flash_read,    "(iiii)i" ),
	//NSYMBOL (                  SYS_flash_read_jedec_id,                        wazi_syscall_flash_read_jedec_id,      "(ii)i" ),
	//NSYMBOL (                      SYS_flash_sfdp_read,                            wazi_syscall_flash_sfdp_read,    "(iiii)i" ),
	//NSYMBOL (           SYS_flash_simulator_get_memory,                 wazi_syscall_flash_simulator_get_memory,      "(ii)i" ),
	//NSYMBOL (                          SYS_flash_write,                                wazi_syscall_flash_write,    "(iiii)i" ),
	//NSYMBOL (            SYS_fuel_gauge_battery_cutoff,                  wazi_syscall_fuel_gauge_battery_cutoff,       "(i)i" ),
	//NSYMBOL (           SYS_fuel_gauge_get_buffer_prop,                 wazi_syscall_fuel_gauge_get_buffer_prop,    "(iiii)i" ),
	//NSYMBOL (                  SYS_fuel_gauge_get_prop,                        wazi_syscall_fuel_gauge_get_prop,     "(iii)i" ),
	//NSYMBOL (                 SYS_fuel_gauge_get_props,                       wazi_syscall_fuel_gauge_get_props,    "(iiii)i" ),
	//NSYMBOL (                  SYS_fuel_gauge_set_prop,                        wazi_syscall_fuel_gauge_set_prop,     "(iii)i" ),
	//NSYMBOL (                 SYS_fuel_gauge_set_props,                       wazi_syscall_fuel_gauge_set_props,    "(iiii)i" ),
	//NSYMBOL (             SYS_gnss_get_enabled_systems,                   wazi_syscall_gnss_get_enabled_systems,      "(ii)i" ),
	//NSYMBOL (                    SYS_gnss_get_fix_rate,                          wazi_syscall_gnss_get_fix_rate,      "(ii)i" ),
	//NSYMBOL (             SYS_gnss_get_navigation_mode,                   wazi_syscall_gnss_get_navigation_mode,      "(ii)i" ),
	//NSYMBOL (             SYS_gnss_get_periodic_config,                   wazi_syscall_gnss_get_periodic_config,      "(ii)i" ),
	//NSYMBOL (           SYS_gnss_get_supported_systems,                 wazi_syscall_gnss_get_supported_systems,      "(ii)i" ),
	//NSYMBOL (             SYS_gnss_set_enabled_systems,                   wazi_syscall_gnss_set_enabled_systems,      "(ii)i" ),
	//NSYMBOL (                    SYS_gnss_set_fix_rate,                          wazi_syscall_gnss_set_fix_rate,      "(ii)i" ),
	//NSYMBOL (             SYS_gnss_set_navigation_mode,                   wazi_syscall_gnss_set_navigation_mode,      "(ii)i" ),
	//NSYMBOL (             SYS_gnss_set_periodic_config,                   wazi_syscall_gnss_set_periodic_config,      "(ii)i" ),
	//NSYMBOL (                 SYS_gpio_get_pending_int,                       wazi_syscall_gpio_get_pending_int,       "(i)i" ),
	//NSYMBOL (                   SYS_gpio_pin_configure,                         wazi_syscall_gpio_pin_configure,     "(iii)i" ),
	//NSYMBOL (                  SYS_gpio_pin_get_config,                        wazi_syscall_gpio_pin_get_config,     "(iii)i" ),
	//NSYMBOL (         SYS_gpio_pin_interrupt_configure,               wazi_syscall_gpio_pin_interrupt_configure,     "(iii)i" ),
	//NSYMBOL (             SYS_gpio_port_clear_bits_raw,                   wazi_syscall_gpio_port_clear_bits_raw,      "(ii)i" ),
	//NSYMBOL (              SYS_gpio_port_get_direction,                    wazi_syscall_gpio_port_get_direction,    "(iiii)i" ),
	//NSYMBOL (                    SYS_gpio_port_get_raw,                          wazi_syscall_gpio_port_get_raw,      "(ii)i" ),
	//NSYMBOL (               SYS_gpio_port_set_bits_raw,                     wazi_syscall_gpio_port_set_bits_raw,      "(ii)i" ),
	//NSYMBOL (             SYS_gpio_port_set_masked_raw,                   wazi_syscall_gpio_port_set_masked_raw,     "(iii)i" ),
	//NSYMBOL (                SYS_gpio_port_toggle_bits,                      wazi_syscall_gpio_port_toggle_bits,      "(ii)i" ),
	//NSYMBOL (             SYS_hwinfo_clear_reset_cause,                   wazi_syscall_hwinfo_clear_reset_cause,        "()i" ),
	//NSYMBOL (                 SYS_hwinfo_get_device_id,                       wazi_syscall_hwinfo_get_device_id,      "(ii)i" ),
	//NSYMBOL (               SYS_hwinfo_get_reset_cause,                     wazi_syscall_hwinfo_get_reset_cause,       "(i)i" ),
	//NSYMBOL (     SYS_hwinfo_get_supported_reset_cause,           wazi_syscall_hwinfo_get_supported_reset_cause,       "(i)i" ),
	//NSYMBOL (                SYS_hwspinlock_get_max_id,                      wazi_syscall_hwspinlock_get_max_id,       "(i)i" ),
	//NSYMBOL (                      SYS_hwspinlock_lock,                            wazi_syscall_hwspinlock_lock,       "(ii)" ),
	//NSYMBOL (                   SYS_hwspinlock_trylock,                         wazi_syscall_hwspinlock_trylock,      "(ii)i" ),
	//NSYMBOL (                    SYS_hwspinlock_unlock,                          wazi_syscall_hwspinlock_unlock,       "(ii)" ),
	//NSYMBOL (                        SYS_i2c_configure,                              wazi_syscall_i2c_configure,      "(ii)i" ),
	//NSYMBOL (                       SYS_i2c_get_config,                             wazi_syscall_i2c_get_config,      "(ii)i" ),
	//NSYMBOL (                      SYS_i2c_recover_bus,                            wazi_syscall_i2c_recover_bus,       "(i)i" ),
	//NSYMBOL (           SYS_i2c_target_driver_register,                 wazi_syscall_i2c_target_driver_register,       "(i)i" ),
	//NSYMBOL (         SYS_i2c_target_driver_unregister,               wazi_syscall_i2c_target_driver_unregister,       "(i)i" ),
	//NSYMBOL (                         SYS_i2c_transfer,                               wazi_syscall_i2c_transfer,    "(iiii)i" ),
	//NSYMBOL (                         SYS_i2s_buf_read,                               wazi_syscall_i2s_buf_read,     "(iii)i" ),
	//NSYMBOL (                        SYS_i2s_buf_write,                              wazi_syscall_i2s_buf_write,     "(iii)i" ),
	//NSYMBOL (                        SYS_i2s_configure,                              wazi_syscall_i2s_configure,     "(iii)i" ),
	//NSYMBOL (                          SYS_i2s_trigger,                                wazi_syscall_i2s_trigger,     "(iii)i" ),
	//NSYMBOL (                           SYS_i3c_do_ccc,                                 wazi_syscall_i3c_do_ccc,      "(ii)i" ),
	//NSYMBOL (                         SYS_i3c_transfer,                               wazi_syscall_i3c_transfer,     "(iii)i" ),
	//NSYMBOL (                         SYS_ipm_complete,                               wazi_syscall_ipm_complete,        "(i)" ),
	//NSYMBOL (                SYS_ipm_max_data_size_get,                      wazi_syscall_ipm_max_data_size_get,       "(i)i" ),
	//NSYMBOL (                   SYS_ipm_max_id_val_get,                         wazi_syscall_ipm_max_id_val_get,       "(i)i" ),
	//NSYMBOL (                             SYS_ipm_send,                                   wazi_syscall_ipm_send,   "(iiiii)i" ),
	//NSYMBOL (                      SYS_ipm_set_enabled,                            wazi_syscall_ipm_set_enabled,      "(ii)i" ),
	//NSYMBOL (            SYS_ivshmem_enable_interrupts,                  wazi_syscall_ivshmem_enable_interrupts,      "(ii)i" ),
	//NSYMBOL (                       SYS_ivshmem_get_id,                             wazi_syscall_ivshmem_get_id,       "(i)i" ),
	//NSYMBOL (                SYS_ivshmem_get_max_peers,                      wazi_syscall_ivshmem_get_max_peers,       "(i)i" ),
	//NSYMBOL (                      SYS_ivshmem_get_mem,                            wazi_syscall_ivshmem_get_mem,      "(ii)i" ),
	//NSYMBOL (       SYS_ivshmem_get_output_mem_section,             wazi_syscall_ivshmem_get_output_mem_section,     "(iii)i" ),
	//NSYMBOL (                 SYS_ivshmem_get_protocol,                       wazi_syscall_ivshmem_get_protocol,       "(i)i" ),
	//NSYMBOL (           SYS_ivshmem_get_rw_mem_section,                 wazi_syscall_ivshmem_get_rw_mem_section,      "(ii)i" ),
	//NSYMBOL (                    SYS_ivshmem_get_state,                          wazi_syscall_ivshmem_get_state,      "(ii)i" ),
	//NSYMBOL (                  SYS_ivshmem_get_vectors,                        wazi_syscall_ivshmem_get_vectors,       "(i)i" ),
	//NSYMBOL (                     SYS_ivshmem_int_peer,                           wazi_syscall_ivshmem_int_peer,     "(iii)i" ),
	//NSYMBOL (             SYS_ivshmem_register_handler,                   wazi_syscall_ivshmem_register_handler,     "(iii)i" ),
	//NSYMBOL (                    SYS_ivshmem_set_state,                          wazi_syscall_ivshmem_set_state,      "(ii)i" ),
	//NSYMBOL (                         SYS_kscan_config,                               wazi_syscall_kscan_config,      "(ii)i" ),
	//NSYMBOL (               SYS_kscan_disable_callback,                     wazi_syscall_kscan_disable_callback,       "(i)i" ),
	//NSYMBOL (                SYS_kscan_enable_callback,                      wazi_syscall_kscan_enable_callback,       "(i)i" ),
	//NSYMBOL (                          SYS_k_busy_wait,                                wazi_syscall_k_busy_wait,        "(i)" ),
	//NSYMBOL (                  SYS_k_condvar_broadcast,                        wazi_syscall_k_condvar_broadcast,       "(i)i" ),
	//NSYMBOL (                       SYS_k_condvar_init,                             wazi_syscall_k_condvar_init,       "(i)i" ),
	//NSYMBOL (                     SYS_k_condvar_signal,                           wazi_syscall_k_condvar_signal,       "(i)i" ),
	//NSYMBOL (                       SYS_k_condvar_wait,                             wazi_syscall_k_condvar_wait,     "(iii)i" ),
	//NSYMBOL (                        SYS_k_event_clear,                              wazi_syscall_k_event_clear,      "(ii)i" ),
	//NSYMBOL (                         SYS_k_event_init,                               wazi_syscall_k_event_init,        "(i)" ),
	//NSYMBOL (                         SYS_k_event_post,                               wazi_syscall_k_event_post,      "(ii)i" ),
	//NSYMBOL (                          SYS_k_event_set,                                wazi_syscall_k_event_set,      "(ii)i" ),
	//NSYMBOL (                   SYS_k_event_set_masked,                         wazi_syscall_k_event_set_masked,     "(iii)i" ),
	//NSYMBOL (                         SYS_k_event_wait,                               wazi_syscall_k_event_wait,    "(iiii)i" ),
	//NSYMBOL (                     SYS_k_event_wait_all,                           wazi_syscall_k_event_wait_all,    "(iiii)i" ),
	//NSYMBOL (                      SYS_k_float_disable,                            wazi_syscall_k_float_disable,       "(i)i" ),
	//NSYMBOL (                       SYS_k_float_enable,                             wazi_syscall_k_float_enable,      "(ii)i" ),
	//NSYMBOL (                         SYS_k_futex_wait,                               wazi_syscall_k_futex_wait,     "(iii)i" ),
	//NSYMBOL (                         SYS_k_futex_wake,                               wazi_syscall_k_futex_wake,      "(ii)i" ),
	//NSYMBOL (                  SYS_k_is_preempt_thread,                        wazi_syscall_k_is_preempt_thread,        "()i" ),
	//NSYMBOL ( SYS_k_mem_paging_histogram_backing_store_page_in_get, wazi_syscall_k_mem_paging_histogram_backing_store_page_in_get,        "(i)" ),
	//NSYMBOL ( SYS_k_mem_paging_histogram_backing_store_page_out_get, wazi_syscall_k_mem_paging_histogram_backing_store_page_out_get,        "(i)" ),
	//NSYMBOL (  SYS_k_mem_paging_histogram_eviction_get,        wazi_syscall_k_mem_paging_histogram_eviction_get,        "(i)" ),
	//NSYMBOL (               SYS_k_mem_paging_stats_get,                     wazi_syscall_k_mem_paging_stats_get,        "(i)" ),
	//NSYMBOL (        SYS_k_mem_paging_thread_stats_get,              wazi_syscall_k_mem_paging_thread_stats_get,       "(ii)" ),
	//NSYMBOL (                    SYS_k_msgq_alloc_init,                          wazi_syscall_k_msgq_alloc_init,     "(iii)i" ),
	//NSYMBOL (                           SYS_k_msgq_get,                                 wazi_syscall_k_msgq_get,     "(iii)i" ),
	//NSYMBOL (                     SYS_k_msgq_get_attrs,                           wazi_syscall_k_msgq_get_attrs,       "(ii)" ),
	//NSYMBOL (                  SYS_k_msgq_num_free_get,                        wazi_syscall_k_msgq_num_free_get,       "(i)i" ),
	//NSYMBOL (                  SYS_k_msgq_num_used_get,                        wazi_syscall_k_msgq_num_used_get,       "(i)i" ),
	//NSYMBOL (                          SYS_k_msgq_peek,                                wazi_syscall_k_msgq_peek,      "(ii)i" ),
	//NSYMBOL (                       SYS_k_msgq_peek_at,                             wazi_syscall_k_msgq_peek_at,     "(iii)i" ),
	//NSYMBOL (                         SYS_k_msgq_purge,                               wazi_syscall_k_msgq_purge,        "(i)" ),
	//NSYMBOL (                           SYS_k_msgq_put,                                 wazi_syscall_k_msgq_put,     "(iii)i" ),
	//NSYMBOL (                         SYS_k_mutex_init,                               wazi_syscall_k_mutex_init,       "(i)i" ),
	//NSYMBOL (                         SYS_k_mutex_lock,                               wazi_syscall_k_mutex_lock,      "(ii)i" ),
	//NSYMBOL (                       SYS_k_mutex_unlock,                             wazi_syscall_k_mutex_unlock,       "(i)i" ),
	//NSYMBOL (                SYS_k_object_access_grant,                      wazi_syscall_k_object_access_grant,       "(ii)" ),
	//NSYMBOL (                       SYS_k_object_alloc,                             wazi_syscall_k_object_alloc,       "(i)i" ),
	//NSYMBOL (                  SYS_k_object_alloc_size,                        wazi_syscall_k_object_alloc_size,      "(ii)i" ),
	//NSYMBOL (                     SYS_k_object_release,                           wazi_syscall_k_object_release,        "(i)" ),
	//NSYMBOL (                    SYS_k_pipe_alloc_init,                          wazi_syscall_k_pipe_alloc_init,      "(ii)i" ),
	//NSYMBOL (                  SYS_k_pipe_buffer_flush,                        wazi_syscall_k_pipe_buffer_flush,        "(i)" ),
	//NSYMBOL (                         SYS_k_pipe_flush,                               wazi_syscall_k_pipe_flush,        "(i)" ),
	//NSYMBOL (                           SYS_k_pipe_get,                                 wazi_syscall_k_pipe_get,  "(iiiiii)i" ),
	//NSYMBOL (                           SYS_k_pipe_put,                                 wazi_syscall_k_pipe_put,  "(iiiiii)i" ),
	//NSYMBOL (                    SYS_k_pipe_read_avail,                          wazi_syscall_k_pipe_read_avail,       "(i)i" ),
	//NSYMBOL (                   SYS_k_pipe_write_avail,                         wazi_syscall_k_pipe_write_avail,       "(i)i" ),
	//NSYMBOL (                               SYS_k_poll,                                     wazi_syscall_k_poll,     "(iii)i" ),
	//NSYMBOL (                  SYS_k_poll_signal_check,                        wazi_syscall_k_poll_signal_check,      "(iii)" ),
	//NSYMBOL (                   SYS_k_poll_signal_init,                         wazi_syscall_k_poll_signal_init,        "(i)" ),
	//NSYMBOL (                  SYS_k_poll_signal_raise,                        wazi_syscall_k_poll_signal_raise,      "(ii)i" ),
	//NSYMBOL (                  SYS_k_poll_signal_reset,                        wazi_syscall_k_poll_signal_reset,        "(i)" ),
	//NSYMBOL (                 SYS_k_queue_alloc_append,                       wazi_syscall_k_queue_alloc_append,      "(ii)i" ),
	//NSYMBOL (                SYS_k_queue_alloc_prepend,                      wazi_syscall_k_queue_alloc_prepend,      "(ii)i" ),
	//NSYMBOL (                  SYS_k_queue_cancel_wait,                        wazi_syscall_k_queue_cancel_wait,        "(i)" ),
	//NSYMBOL (                          SYS_k_queue_get,                                wazi_syscall_k_queue_get,      "(ii)i" ),
	//NSYMBOL (                         SYS_k_queue_init,                               wazi_syscall_k_queue_init,        "(i)" ),
	//NSYMBOL (                     SYS_k_queue_is_empty,                           wazi_syscall_k_queue_is_empty,       "(i)i" ),
	//NSYMBOL (                    SYS_k_queue_peek_head,                          wazi_syscall_k_queue_peek_head,       "(i)i" ),
	//NSYMBOL (                    SYS_k_queue_peek_tail,                          wazi_syscall_k_queue_peek_tail,       "(i)i" ),
	//NSYMBOL (         SYS_k_sched_current_thread_query,               wazi_syscall_k_sched_current_thread_query,        "()i" ),
	//NSYMBOL (                      SYS_k_sem_count_get,                            wazi_syscall_k_sem_count_get,       "(i)i" ),
	//NSYMBOL (                           SYS_k_sem_give,                                 wazi_syscall_k_sem_give,        "(i)" ),
	//NSYMBOL (                           SYS_k_sem_init,                                 wazi_syscall_k_sem_init,     "(iii)i" ),
	//NSYMBOL (                          SYS_k_sem_reset,                                wazi_syscall_k_sem_reset,        "(i)" ),
	//NSYMBOL (                           SYS_k_sem_take,                                 wazi_syscall_k_sem_take,      "(ii)i" ),
	//NSYMBOL (                              SYS_k_sleep,                                    wazi_syscall_k_sleep,       "(i)i" ),
	//NSYMBOL (                   SYS_k_stack_alloc_init,                         wazi_syscall_k_stack_alloc_init,      "(ii)i" ),
	//NSYMBOL (                          SYS_k_stack_pop,                                wazi_syscall_k_stack_pop,     "(iii)i" ),
	//NSYMBOL (                         SYS_k_stack_push,                               wazi_syscall_k_stack_push,      "(ii)i" ),
	//NSYMBOL (                            SYS_k_str_out,                                  wazi_syscall_k_str_out,       "(ii)" ),
	//NSYMBOL (                       SYS_k_thread_abort,                             wazi_syscall_k_thread_abort,        "(i)" ),
	//NSYMBOL (                      SYS_k_thread_create,                            wazi_syscall_k_thread_create, "(iiiiiiiiii)i" ),
	//NSYMBOL (             SYS_k_thread_custom_data_get,                   wazi_syscall_k_thread_custom_data_get,        "()i" ),
	//NSYMBOL (             SYS_k_thread_custom_data_set,                   wazi_syscall_k_thread_custom_data_set,        "(i)" ),
	//NSYMBOL (                SYS_k_thread_deadline_set,                      wazi_syscall_k_thread_deadline_set,       "(ii)" ),
	//NSYMBOL (                        SYS_k_thread_join,                              wazi_syscall_k_thread_join,      "(ii)i" ),
	//NSYMBOL (                   SYS_k_thread_name_copy,                         wazi_syscall_k_thread_name_copy,     "(iii)i" ),
	//NSYMBOL (                    SYS_k_thread_name_set,                          wazi_syscall_k_thread_name_set,      "(ii)i" ),
	//NSYMBOL (                SYS_k_thread_priority_get,                      wazi_syscall_k_thread_priority_get,       "(i)i" ),
	//NSYMBOL (                SYS_k_thread_priority_set,                      wazi_syscall_k_thread_priority_set,       "(ii)" ),
	//NSYMBOL (                      SYS_k_thread_resume,                            wazi_syscall_k_thread_resume,        "(i)" ),
	//NSYMBOL (                 SYS_k_thread_stack_alloc,                       wazi_syscall_k_thread_stack_alloc,      "(ii)i" ),
	//NSYMBOL (                  SYS_k_thread_stack_free,                        wazi_syscall_k_thread_stack_free,       "(i)i" ),
	//NSYMBOL (             SYS_k_thread_stack_space_get,                   wazi_syscall_k_thread_stack_space_get,      "(ii)i" ),
	//NSYMBOL (                       SYS_k_thread_start,                             wazi_syscall_k_thread_start,        "(i)" ),
	//NSYMBOL (                     SYS_k_thread_suspend,                           wazi_syscall_k_thread_suspend,        "(i)" ),
	//NSYMBOL (       SYS_k_thread_timeout_expires_ticks,             wazi_syscall_k_thread_timeout_expires_ticks,       "(i)i" ),
	//NSYMBOL (     SYS_k_thread_timeout_remaining_ticks,           wazi_syscall_k_thread_timeout_remaining_ticks,       "(i)i" ),
	//NSYMBOL (                SYS_k_timer_expires_ticks,                      wazi_syscall_k_timer_expires_ticks,       "(i)i" ),
	//NSYMBOL (              SYS_k_timer_remaining_ticks,                    wazi_syscall_k_timer_remaining_ticks,       "(i)i" ),
	//NSYMBOL (                        SYS_k_timer_start,                              wazi_syscall_k_timer_start,      "(iii)" ),
	//NSYMBOL (                   SYS_k_timer_status_get,                         wazi_syscall_k_timer_status_get,       "(i)i" ),
	//NSYMBOL (                  SYS_k_timer_status_sync,                        wazi_syscall_k_timer_status_sync,       "(i)i" ),
	//NSYMBOL (                         SYS_k_timer_stop,                               wazi_syscall_k_timer_stop,        "(i)" ),
	//NSYMBOL (                SYS_k_timer_user_data_get,                      wazi_syscall_k_timer_user_data_get,       "(i)i" ),
	//NSYMBOL (                SYS_k_timer_user_data_set,                      wazi_syscall_k_timer_user_data_set,       "(ii)" ),
	//NSYMBOL (                       SYS_k_uptime_ticks,                             wazi_syscall_k_uptime_ticks,        "()I" ),
	//NSYMBOL (                             SYS_k_usleep,                                   wazi_syscall_k_usleep,       "(i)i" ),
	//NSYMBOL (                             SYS_k_wakeup,                                   wazi_syscall_k_wakeup,        "(i)" ),
	//NSYMBOL (                              SYS_k_yield,                                    wazi_syscall_k_yield,         "()" ),
	//NSYMBOL (                            SYS_led_blink,                                  wazi_syscall_led_blink,    "(iiii)i" ),
	//NSYMBOL (                         SYS_led_get_info,                               wazi_syscall_led_get_info,     "(iii)i" ),
	//NSYMBOL (                              SYS_led_off,                                    wazi_syscall_led_off,      "(ii)i" ),
	//NSYMBOL (                               SYS_led_on,                                     wazi_syscall_led_on,      "(ii)i" ),
	//NSYMBOL (                   SYS_led_set_brightness,                         wazi_syscall_led_set_brightness,     "(iii)i" ),
	//NSYMBOL (                      SYS_led_set_channel,                            wazi_syscall_led_set_channel,     "(iii)i" ),
	//NSYMBOL (                        SYS_led_set_color,                              wazi_syscall_led_set_color,    "(iiii)i" ),
	//NSYMBOL (                   SYS_led_write_channels,                         wazi_syscall_led_write_channels,    "(iiii)i" ),
	//NSYMBOL (                     SYS_log_buffered_cnt,                           wazi_syscall_log_buffered_cnt,        "()i" ),
	//NSYMBOL (                       SYS_log_filter_set,                             wazi_syscall_log_filter_set,    "(iiii)i" ),
	//NSYMBOL (              SYS_log_frontend_filter_set,                    wazi_syscall_log_frontend_filter_set,      "(ii)i" ),
	//NSYMBOL (                            SYS_log_panic,                                  wazi_syscall_log_panic,         "()" ),
	//NSYMBOL (                          SYS_log_process,                                wazi_syscall_log_process,        "()i" ),
	//NSYMBOL (           SYS_maxim_ds3231_get_syncpoint,                 wazi_syscall_maxim_ds3231_get_syncpoint,      "(ii)i" ),
	//NSYMBOL (           SYS_maxim_ds3231_req_syncpoint,                 wazi_syscall_maxim_ds3231_req_syncpoint,      "(ii)i" ),
	//NSYMBOL (                SYS_mbox_max_channels_get,                      wazi_syscall_mbox_max_channels_get,       "(i)i" ),
	//NSYMBOL (                         SYS_mbox_mtu_get,                               wazi_syscall_mbox_mtu_get,       "(i)i" ),
	//NSYMBOL (                            SYS_mbox_send,                                  wazi_syscall_mbox_send,      "(ii)i" ),
	//NSYMBOL (                     SYS_mbox_set_enabled,                           wazi_syscall_mbox_set_enabled,      "(ii)i" ),
	//NSYMBOL (                     SYS_mdio_bus_disable,                           wazi_syscall_mdio_bus_disable,        "(i)" ),
	//NSYMBOL (                      SYS_mdio_bus_enable,                            wazi_syscall_mdio_bus_enable,        "(i)" ),
	//NSYMBOL (                            SYS_mdio_read,                                  wazi_syscall_mdio_read,    "(iiii)i" ),
	//NSYMBOL (                        SYS_mdio_read_c45,                              wazi_syscall_mdio_read_c45,   "(iiiii)i" ),
	//NSYMBOL (                           SYS_mdio_write,                                 wazi_syscall_mdio_write,    "(iiii)i" ),
	//NSYMBOL (                       SYS_mdio_write_c45,                             wazi_syscall_mdio_write_c45,   "(iiiii)i" ),
	//NSYMBOL (                        SYS_net_addr_ntop,                              wazi_syscall_net_addr_ntop,    "(iiii)i" ),
	//NSYMBOL (                        SYS_net_addr_pton,                              wazi_syscall_net_addr_pton,     "(iii)i" ),
	//NSYMBOL (       SYS_net_eth_get_ptp_clock_by_index,             wazi_syscall_net_eth_get_ptp_clock_by_index,       "(i)i" ),
	//NSYMBOL (                  SYS_net_if_get_by_index,                        wazi_syscall_net_if_get_by_index,       "(i)i" ),
	//NSYMBOL (        SYS_net_if_ipv4_addr_add_by_index,              wazi_syscall_net_if_ipv4_addr_add_by_index,    "(iiii)i" ),
	//NSYMBOL (     SYS_net_if_ipv4_addr_lookup_by_index,           wazi_syscall_net_if_ipv4_addr_lookup_by_index,       "(i)i" ),
	//NSYMBOL (         SYS_net_if_ipv4_addr_rm_by_index,               wazi_syscall_net_if_ipv4_addr_rm_by_index,      "(ii)i" ),
	//NSYMBOL (          SYS_net_if_ipv4_set_gw_by_index,                wazi_syscall_net_if_ipv4_set_gw_by_index,      "(ii)i" ),
	//NSYMBOL (     SYS_net_if_ipv4_set_netmask_by_index,           wazi_syscall_net_if_ipv4_set_netmask_by_index,      "(ii)i" ),
	//NSYMBOL (        SYS_net_if_ipv6_addr_add_by_index,              wazi_syscall_net_if_ipv6_addr_add_by_index,    "(iiii)i" ),
	//NSYMBOL (     SYS_net_if_ipv6_addr_lookup_by_index,           wazi_syscall_net_if_ipv6_addr_lookup_by_index,       "(i)i" ),
	//NSYMBOL (         SYS_net_if_ipv6_addr_rm_by_index,               wazi_syscall_net_if_ipv6_addr_rm_by_index,      "(ii)i" ),
	//NSYMBOL (          SYS_net_socket_service_register,                wazi_syscall_net_socket_service_register,    "(iiii)i" ),
	//NSYMBOL (              SYS_nrf_qspi_nor_xip_enable,                    wazi_syscall_nrf_qspi_nor_xip_enable,       "(ii)" ),
	//NSYMBOL (                          SYS_peci_config,                                wazi_syscall_peci_config,      "(ii)i" ),
	//NSYMBOL (                         SYS_peci_disable,                               wazi_syscall_peci_disable,       "(i)i" ),
	//NSYMBOL (                          SYS_peci_enable,                                wazi_syscall_peci_enable,       "(i)i" ),
	//NSYMBOL (                        SYS_peci_transfer,                              wazi_syscall_peci_transfer,      "(ii)i" ),
	//NSYMBOL (                           SYS_ps2_config,                                 wazi_syscall_ps2_config,      "(ii)i" ),
	//NSYMBOL (                 SYS_ps2_disable_callback,                       wazi_syscall_ps2_disable_callback,       "(i)i" ),
	//NSYMBOL (                  SYS_ps2_enable_callback,                        wazi_syscall_ps2_enable_callback,       "(i)i" ),
	//NSYMBOL (                             SYS_ps2_read,                                   wazi_syscall_ps2_read,      "(ii)i" ),
	//NSYMBOL (                            SYS_ps2_write,                                  wazi_syscall_ps2_write,      "(ii)i" ),
	//NSYMBOL (                        SYS_ptp_clock_get,                              wazi_syscall_ptp_clock_get,      "(ii)i" ),
	//NSYMBOL (                   SYS_pwm_capture_cycles,                         wazi_syscall_pwm_capture_cycles,  "(iiiiii)i" ),
	//NSYMBOL (                  SYS_pwm_disable_capture,                        wazi_syscall_pwm_disable_capture,      "(ii)i" ),
	//NSYMBOL (                   SYS_pwm_enable_capture,                         wazi_syscall_pwm_enable_capture,      "(ii)i" ),
	//NSYMBOL (               SYS_pwm_get_cycles_per_sec,                     wazi_syscall_pwm_get_cycles_per_sec,     "(iii)i" ),
	//NSYMBOL (                       SYS_pwm_set_cycles,                             wazi_syscall_pwm_set_cycles,   "(iiiii)i" ),
	//NSYMBOL (                    SYS_reset_line_assert,                          wazi_syscall_reset_line_assert,      "(ii)i" ),
	//NSYMBOL (                  SYS_reset_line_deassert,                        wazi_syscall_reset_line_deassert,      "(ii)i" ),
	//NSYMBOL (                    SYS_reset_line_toggle,                          wazi_syscall_reset_line_toggle,      "(ii)i" ),
	//NSYMBOL (                         SYS_reset_status,                               wazi_syscall_reset_status,     "(iii)i" ),
	//NSYMBOL (                   SYS_retained_mem_clear,                         wazi_syscall_retained_mem_clear,       "(i)i" ),
	//NSYMBOL (                    SYS_retained_mem_read,                          wazi_syscall_retained_mem_read,    "(iiii)i" ),
	//NSYMBOL (                    SYS_retained_mem_size,                          wazi_syscall_retained_mem_size,       "(i)i" ),
	//NSYMBOL (                   SYS_retained_mem_write,                         wazi_syscall_retained_mem_write,    "(iiii)i" ),
	//NSYMBOL (       SYS_rtc_alarm_get_supported_fields,             wazi_syscall_rtc_alarm_get_supported_fields,     "(iii)i" ),
	//NSYMBOL (                   SYS_rtc_alarm_get_time,                         wazi_syscall_rtc_alarm_get_time,    "(iiii)i" ),
	//NSYMBOL (                 SYS_rtc_alarm_is_pending,                       wazi_syscall_rtc_alarm_is_pending,      "(ii)i" ),
	//NSYMBOL (               SYS_rtc_alarm_set_callback,                     wazi_syscall_rtc_alarm_set_callback,    "(iiii)i" ),
	//NSYMBOL (                   SYS_rtc_alarm_set_time,                         wazi_syscall_rtc_alarm_set_time,    "(iiii)i" ),
	//NSYMBOL (                  SYS_rtc_get_calibration,                        wazi_syscall_rtc_get_calibration,      "(ii)i" ),
	//NSYMBOL (                         SYS_rtc_get_time,                               wazi_syscall_rtc_get_time,      "(ii)i" ),
	//NSYMBOL (                  SYS_rtc_set_calibration,                        wazi_syscall_rtc_set_calibration,      "(ii)i" ),
	//NSYMBOL (                         SYS_rtc_set_time,                               wazi_syscall_rtc_set_time,      "(ii)i" ),
	//NSYMBOL (              SYS_rtc_update_set_callback,                    wazi_syscall_rtc_update_set_callback,     "(iii)i" ),
	//NSYMBOL (                    SYS_rtio_cqe_copy_out,                          wazi_syscall_rtio_cqe_copy_out,    "(iiii)i" ),
	//NSYMBOL (          SYS_rtio_cqe_get_mempool_buffer,                wazi_syscall_rtio_cqe_get_mempool_buffer,    "(iiii)i" ),
	//NSYMBOL (                  SYS_rtio_release_buffer,                        wazi_syscall_rtio_release_buffer,      "(iii)" ),
	//NSYMBOL (                      SYS_rtio_sqe_cancel,                            wazi_syscall_rtio_sqe_cancel,       "(i)i" ),
	//NSYMBOL (         SYS_rtio_sqe_copy_in_get_handles,               wazi_syscall_rtio_sqe_copy_in_get_handles,    "(iiii)i" ),
	//NSYMBOL (                          SYS_rtio_submit,                                wazi_syscall_rtio_submit,      "(ii)i" ),
	//NSYMBOL (                       SYS_sdhc_card_busy,                             wazi_syscall_sdhc_card_busy,       "(i)i" ),
	//NSYMBOL (                    SYS_sdhc_card_present,                          wazi_syscall_sdhc_card_present,       "(i)i" ),
	//NSYMBOL (               SYS_sdhc_disable_interrupt,                     wazi_syscall_sdhc_disable_interrupt,      "(ii)i" ),
	//NSYMBOL (                SYS_sdhc_enable_interrupt,                      wazi_syscall_sdhc_enable_interrupt,    "(iiii)i" ),
	//NSYMBOL (                  SYS_sdhc_execute_tuning,                        wazi_syscall_sdhc_execute_tuning,       "(i)i" ),
	//NSYMBOL (                  SYS_sdhc_get_host_props,                        wazi_syscall_sdhc_get_host_props,      "(ii)i" ),
	//NSYMBOL (                        SYS_sdhc_hw_reset,                              wazi_syscall_sdhc_hw_reset,       "(i)i" ),
	//NSYMBOL (                         SYS_sdhc_request,                               wazi_syscall_sdhc_request,     "(iii)i" ),
	//NSYMBOL (                          SYS_sdhc_set_io,                                wazi_syscall_sdhc_set_io,      "(ii)i" ),
	//NSYMBOL (                      SYS_sensor_attr_get,                            wazi_syscall_sensor_attr_get,    "(iiii)i" ),
	//NSYMBOL (                      SYS_sensor_attr_set,                            wazi_syscall_sensor_attr_set,    "(iiii)i" ),
	//NSYMBOL (                   SYS_sensor_channel_get,                         wazi_syscall_sensor_channel_get,     "(iii)i" ),
	//NSYMBOL (                   SYS_sensor_get_decoder,                         wazi_syscall_sensor_get_decoder,      "(ii)i" ),
	//NSYMBOL (        SYS_sensor_reconfigure_read_iodev,              wazi_syscall_sensor_reconfigure_read_iodev,    "(iiii)i" ),
	//NSYMBOL (                  SYS_sensor_sample_fetch,                        wazi_syscall_sensor_sample_fetch,       "(i)i" ),
	//NSYMBOL (             SYS_sensor_sample_fetch_chan,                   wazi_syscall_sensor_sample_fetch_chan,      "(ii)i" ),
	//NSYMBOL (                 SYS_sip_supervisory_call,                       wazi_syscall_sip_supervisory_call, "(iiiiiiiiii)" ),
	//NSYMBOL (           SYS_sip_svc_plat_async_res_req,                 wazi_syscall_sip_svc_plat_async_res_req, "(iiiiiiiiiii)i" ),
	//NSYMBOL (           SYS_sip_svc_plat_async_res_res,                 wazi_syscall_sip_svc_plat_async_res_res,   "(iiiii)i" ),
	//NSYMBOL (         SYS_sip_svc_plat_format_trans_id,               wazi_syscall_sip_svc_plat_format_trans_id,     "(iii)i" ),
	//NSYMBOL (       SYS_sip_svc_plat_free_async_memory,             wazi_syscall_sip_svc_plat_free_async_memory,       "(ii)" ),
	//NSYMBOL (           SYS_sip_svc_plat_func_id_valid,                 wazi_syscall_sip_svc_plat_func_id_valid,     "(iii)i" ),
	//NSYMBOL (          SYS_sip_svc_plat_get_error_code,                wazi_syscall_sip_svc_plat_get_error_code,      "(ii)i" ),
	//NSYMBOL (           SYS_sip_svc_plat_get_trans_idx,                 wazi_syscall_sip_svc_plat_get_trans_idx,      "(ii)i" ),
	//NSYMBOL (         SYS_sip_svc_plat_update_trans_id,               wazi_syscall_sip_svc_plat_update_trans_id,      "(iii)" ),
	//NSYMBOL (                    SYS_smbus_block_pcall,                          wazi_syscall_smbus_block_pcall, "(iiiiiii)i" ),
	//NSYMBOL (                     SYS_smbus_block_read,                           wazi_syscall_smbus_block_read,   "(iiiii)i" ),
	//NSYMBOL (                    SYS_smbus_block_write,                          wazi_syscall_smbus_block_write,   "(iiiii)i" ),
	//NSYMBOL (                 SYS_smbus_byte_data_read,                       wazi_syscall_smbus_byte_data_read,    "(iiii)i" ),
	//NSYMBOL (                SYS_smbus_byte_data_write,                      wazi_syscall_smbus_byte_data_write,    "(iiii)i" ),
	//NSYMBOL (                      SYS_smbus_byte_read,                            wazi_syscall_smbus_byte_read,     "(iii)i" ),
	//NSYMBOL (                     SYS_smbus_byte_write,                           wazi_syscall_smbus_byte_write,     "(iii)i" ),
	//NSYMBOL (                      SYS_smbus_configure,                            wazi_syscall_smbus_configure,      "(ii)i" ),
	//NSYMBOL (                     SYS_smbus_get_config,                           wazi_syscall_smbus_get_config,      "(ii)i" ),
	//NSYMBOL (          SYS_smbus_host_notify_remove_cb,                wazi_syscall_smbus_host_notify_remove_cb,      "(ii)i" ),
	//NSYMBOL (                          SYS_smbus_pcall,                                wazi_syscall_smbus_pcall,   "(iiiii)i" ),
	//NSYMBOL (                          SYS_smbus_quick,                                wazi_syscall_smbus_quick,     "(iii)i" ),
	//NSYMBOL (             SYS_smbus_smbalert_remove_cb,                   wazi_syscall_smbus_smbalert_remove_cb,      "(ii)i" ),
	//NSYMBOL (                 SYS_smbus_word_data_read,                       wazi_syscall_smbus_word_data_read,    "(iiii)i" ),
	//NSYMBOL (                SYS_smbus_word_data_write,                      wazi_syscall_smbus_word_data_write,    "(iiii)i" ),
	//NSYMBOL (                          SYS_spi_release,                                wazi_syscall_spi_release,      "(ii)i" ),
	//NSYMBOL (                       SYS_spi_transceive,                             wazi_syscall_spi_transceive,    "(iiii)i" ),
	//NSYMBOL (                      SYS_syscon_get_base,                            wazi_syscall_syscon_get_base,      "(ii)i" ),
	//NSYMBOL (                      SYS_syscon_get_size,                            wazi_syscall_syscon_get_size,      "(ii)i" ),
	//NSYMBOL (                      SYS_syscon_read_reg,                            wazi_syscall_syscon_read_reg,     "(iii)i" ),
	//NSYMBOL (                     SYS_syscon_write_reg,                           wazi_syscall_syscon_write_reg,     "(iii)i" ),
	//NSYMBOL (  SYS_sys_cache_data_flush_and_invd_range,        wazi_syscall_sys_cache_data_flush_and_invd_range,      "(ii)i" ),
	//NSYMBOL (           SYS_sys_cache_data_flush_range,                 wazi_syscall_sys_cache_data_flush_range,      "(ii)i" ),
	//NSYMBOL (            SYS_sys_cache_data_invd_range,                  wazi_syscall_sys_cache_data_invd_range,      "(ii)i" ),
	//NSYMBOL ( SYS_sys_clock_hw_cycles_per_sec_runtime_get,    wazi_syscall_sys_clock_hw_cycles_per_sec_runtime_get,        "()i" ),
	//NSYMBOL (                       SYS_sys_csrand_get,                             wazi_syscall_sys_csrand_get,      "(ii)i" ),
	//NSYMBOL (                       SYS_sys_rand32_get,                             wazi_syscall_sys_rand32_get,        "()i" ),
	//NSYMBOL (                         SYS_sys_rand_get,                               wazi_syscall_sys_rand_get,       "(ii)" ),
	//NSYMBOL (       SYS_tgpio_pin_config_ext_timestamp,             wazi_syscall_tgpio_pin_config_ext_timestamp,     "(iii)i" ),
	//NSYMBOL (                    SYS_tgpio_pin_disable,                          wazi_syscall_tgpio_pin_disable,      "(ii)i" ),
	//NSYMBOL (            SYS_tgpio_pin_periodic_output,                  wazi_syscall_tgpio_pin_periodic_output,   "(iiIIi)i" ),
	//NSYMBOL (                 SYS_tgpio_pin_read_ts_ec,                       wazi_syscall_tgpio_pin_read_ts_ec,    "(iiii)i" ),
	//NSYMBOL (     SYS_tgpio_port_get_cycles_per_second,           wazi_syscall_tgpio_port_get_cycles_per_second,      "(ii)i" ),
	//NSYMBOL (                  SYS_tgpio_port_get_time,                        wazi_syscall_tgpio_port_get_time,      "(ii)i" ),
	//NSYMBOL (                       SYS_uart_configure,                             wazi_syscall_uart_configure,      "(ii)i" ),
	//NSYMBOL (                      SYS_uart_config_get,                            wazi_syscall_uart_config_get,      "(ii)i" ),
	//NSYMBOL (                         SYS_uart_drv_cmd,                               wazi_syscall_uart_drv_cmd,     "(iii)i" ),
	//NSYMBOL (                       SYS_uart_err_check,                             wazi_syscall_uart_err_check,       "(i)i" ),
	//NSYMBOL (                 SYS_uart_irq_err_disable,                       wazi_syscall_uart_irq_err_disable,        "(i)" ),
	//NSYMBOL (                  SYS_uart_irq_err_enable,                        wazi_syscall_uart_irq_err_enable,        "(i)" ),
	//NSYMBOL (                  SYS_uart_irq_is_pending,                        wazi_syscall_uart_irq_is_pending,       "(i)i" ),
	//NSYMBOL (                  SYS_uart_irq_rx_disable,                        wazi_syscall_uart_irq_rx_disable,        "(i)" ),
	//NSYMBOL (                   SYS_uart_irq_rx_enable,                         wazi_syscall_uart_irq_rx_enable,        "(i)" ),
	//NSYMBOL (                  SYS_uart_irq_tx_disable,                        wazi_syscall_uart_irq_tx_disable,        "(i)" ),
	//NSYMBOL (                   SYS_uart_irq_tx_enable,                         wazi_syscall_uart_irq_tx_enable,        "(i)" ),
	//NSYMBOL (                      SYS_uart_irq_update,                            wazi_syscall_uart_irq_update,       "(i)i" ),
	//NSYMBOL (                   SYS_uart_line_ctrl_get,                         wazi_syscall_uart_line_ctrl_get,     "(iii)i" ),
	//NSYMBOL (                   SYS_uart_line_ctrl_set,                         wazi_syscall_uart_line_ctrl_set,     "(iii)i" ),
	//NSYMBOL (                        SYS_uart_mux_find,                              wazi_syscall_uart_mux_find,       "(i)i" ),
	//NSYMBOL (                         SYS_uart_poll_in,                               wazi_syscall_uart_poll_in,      "(ii)i" ),
	//NSYMBOL (                     SYS_uart_poll_in_u16,                           wazi_syscall_uart_poll_in_u16,      "(ii)i" ),
	//NSYMBOL (                        SYS_uart_poll_out,                              wazi_syscall_uart_poll_out,       "(ii)" ),
	//NSYMBOL (                    SYS_uart_poll_out_u16,                          wazi_syscall_uart_poll_out_u16,       "(ii)" ),
	//NSYMBOL (                      SYS_uart_rx_disable,                            wazi_syscall_uart_rx_disable,       "(i)i" ),
	//NSYMBOL (                       SYS_uart_rx_enable,                             wazi_syscall_uart_rx_enable,    "(iiii)i" ),
	//NSYMBOL (                   SYS_uart_rx_enable_u16,                         wazi_syscall_uart_rx_enable_u16,    "(iiii)i" ),
	//NSYMBOL (                              SYS_uart_tx,                                    wazi_syscall_uart_tx,    "(iiii)i" ),
	//NSYMBOL (                        SYS_uart_tx_abort,                              wazi_syscall_uart_tx_abort,       "(i)i" ),
	//NSYMBOL (                          SYS_uart_tx_u16,                                wazi_syscall_uart_tx_u16,    "(iiii)i" ),
	//NSYMBOL (                SYS_updatehub_autohandler,                      wazi_syscall_updatehub_autohandler,         "()" ),
	//NSYMBOL (                    SYS_updatehub_confirm,                          wazi_syscall_updatehub_confirm,        "()i" ),
	//NSYMBOL (                      SYS_updatehub_probe,                            wazi_syscall_updatehub_probe,        "()i" ),
	//NSYMBOL (                     SYS_updatehub_reboot,                           wazi_syscall_updatehub_reboot,        "()i" ),
	//NSYMBOL (                     SYS_updatehub_update,                           wazi_syscall_updatehub_update,        "()i" ),
	//NSYMBOL (                           SYS_user_fault,                                 wazi_syscall_user_fault,        "(i)" ),
	//NSYMBOL (                   SYS_w1_change_bus_lock,                         wazi_syscall_w1_change_bus_lock,      "(ii)i" ),
	//NSYMBOL (                         SYS_w1_configure,                               wazi_syscall_w1_configure,     "(iii)i" ),
	//NSYMBOL (                   SYS_w1_get_slave_count,                         wazi_syscall_w1_get_slave_count,       "(i)i" ),
	//NSYMBOL (                          SYS_w1_read_bit,                                wazi_syscall_w1_read_bit,       "(i)i" ),
	//NSYMBOL (                        SYS_w1_read_block,                              wazi_syscall_w1_read_block,     "(iii)i" ),
	//NSYMBOL (                         SYS_w1_read_byte,                               wazi_syscall_w1_read_byte,       "(i)i" ),
	//NSYMBOL (                         SYS_w1_reset_bus,                               wazi_syscall_w1_reset_bus,       "(i)i" ),
	//NSYMBOL (                        SYS_w1_search_bus,                              wazi_syscall_w1_search_bus,   "(iiiii)i" ),
	//NSYMBOL (                         SYS_w1_write_bit,                               wazi_syscall_w1_write_bit,      "(ii)i" ),
	//NSYMBOL (                       SYS_w1_write_block,                             wazi_syscall_w1_write_block,     "(iii)i" ),
	//NSYMBOL (                        SYS_w1_write_byte,                              wazi_syscall_w1_write_byte,      "(ii)i" ),
	//NSYMBOL (                          SYS_wdt_disable,                                wazi_syscall_wdt_disable,       "(i)i" ),
	//NSYMBOL (                             SYS_wdt_feed,                                   wazi_syscall_wdt_feed,      "(ii)i" ),
	//NSYMBOL (                            SYS_wdt_setup,                                  wazi_syscall_wdt_setup,      "(ii)i" ),
	//NSYMBOL (                    SYS_xtensa_user_fault,                          wazi_syscall_xtensa_user_fault,        "(i)" ),
#if defined(CONFIG_MINIMAL_LIBC) || defined(CONFIG_PICOLIBC)
	NSYMBOL (                         SYS_zephyr_fputc,                               wazi_syscall_zephyr_fputc,      "(ii)i" ),
	//NSYMBOL (                        SYS_zephyr_fwrite,                              wazi_syscall_zephyr_fwrite,    "(iiii)i" ),
#endif
#ifdef CONFIG_NEWLIB_LIBC
	NSYMBOL (                    SYS_zephyr_read_stdin,                          wazi_syscall_zephyr_read_stdin,      "(ii)i" ),
	NSYMBOL (                  SYS_zephyr_write_stdout,                        wazi_syscall_zephyr_write_stdout,      "(ii)i" ),
#endif
	NSYMBOL (                         SYS_zsock_accept,                               wazi_syscall_zsock_accept,     "(iii)i" ),
	NSYMBOL (                           SYS_zsock_bind,                                 wazi_syscall_zsock_bind,     "(iii)i" ),
	NSYMBOL (                          SYS_zsock_close,                                wazi_syscall_zsock_close,       "(i)i" ),
	NSYMBOL (                        SYS_zsock_connect,                              wazi_syscall_zsock_connect,     "(iii)i" ),
	NSYMBOL (                          SYS_zsock_fcntl,                                wazi_syscall_zsock_fcntl,     "(iii)i" ),
	NSYMBOL (                    SYS_zsock_gethostname,                          wazi_syscall_zsock_gethostname,      "(ii)i" ),
	NSYMBOL (                    SYS_zsock_getpeername,                          wazi_syscall_zsock_getpeername,     "(iii)i" ),
	NSYMBOL (                    SYS_zsock_getsockname,                          wazi_syscall_zsock_getsockname,     "(iii)i" ),
	NSYMBOL (                     SYS_zsock_getsockopt,                           wazi_syscall_zsock_getsockopt,   "(iiiii)i" ),
	//NSYMBOL (             SYS_zsock_get_context_object,                   wazi_syscall_zsock_get_context_object,       "(i)i" ),
	NSYMBOL (                      SYS_zsock_inet_pton,                            wazi_syscall_zsock_inet_pton,     "(iii)i" ),
	//NSYMBOL (                          SYS_zsock_ioctl,                                wazi_syscall_zsock_ioctl,     "(iii)i" ),
	NSYMBOL (                         SYS_zsock_listen,                               wazi_syscall_zsock_listen,      "(ii)i" ),
	NSYMBOL (                           SYS_zsock_poll,                                 wazi_syscall_zsock_poll,     "(iii)i" ),
	NSYMBOL (                       SYS_zsock_recvfrom,                             wazi_syscall_zsock_recvfrom,  "(iiiiii)i" ),
	NSYMBOL (                        SYS_zsock_recvmsg,                              wazi_syscall_zsock_recvmsg,     "(iii)i" ),
	NSYMBOL (                         SYS_zsock_select,                               wazi_syscall_zsock_select,   "(iiiii)i" ),
	NSYMBOL (                        SYS_zsock_sendmsg,                              wazi_syscall_zsock_sendmsg,     "(iii)i" ),
	NSYMBOL (                         SYS_zsock_sendto,                               wazi_syscall_zsock_sendto,  "(iiiiii)i" ),
	NSYMBOL (                     SYS_zsock_setsockopt,                           wazi_syscall_zsock_setsockopt,   "(iiiii)i" ),
	NSYMBOL (                       SYS_zsock_shutdown,                             wazi_syscall_zsock_shutdown,      "(ii)i" ),
	NSYMBOL (                         SYS_zsock_socket,                               wazi_syscall_zsock_socket,     "(iii)i" ),
	NSYMBOL (                     SYS_zsock_socketpair,                           wazi_syscall_zsock_socketpair,    "(iiii)i" ),
	//NSYMBOL (                              SYS_z_errno,                                    wazi_syscall_z_errno,        "()i" ),
	//NSYMBOL (            SYS_z_log_msg_simple_create_0,                  wazi_syscall_z_log_msg_simple_create_0,      "(iii)" ),
	//NSYMBOL (            SYS_z_log_msg_simple_create_1,                  wazi_syscall_z_log_msg_simple_create_1,     "(iiii)" ),
	//NSYMBOL (            SYS_z_log_msg_simple_create_2,                  wazi_syscall_z_log_msg_simple_create_2,    "(iiiii)" ),
	//NSYMBOL (              SYS_z_log_msg_static_create,                    wazi_syscall_z_log_msg_static_create,     "(iiii)" ),
	//NSYMBOL (              SYS_z_sys_mutex_kernel_lock,                    wazi_syscall_z_sys_mutex_kernel_lock,      "(ii)i" ),
	//NSYMBOL (            SYS_z_sys_mutex_kernel_unlock,                  wazi_syscall_z_sys_mutex_kernel_unlock,       "(i)i" ),
	//NSYMBOL (         SYS_z_zsock_getaddrinfo_internal,               wazi_syscall_z_zsock_getaddrinfo_internal,    "(iiii)i" ),
	//NSYMBOL (               SYS___posix_clock_get_base,                     wazi_syscall___posix_clock_get_base,      "(ii)i" ),
	NSYMBOL (               __placeholder__,                     wazi_syscall_placeholder,      "()" )
};

uint32
get_libc_wazi_export_apis(NativeSymbol **p_libc_wazi_apis)
{
    *p_libc_wazi_apis = wazi_native_symbols;
    return sizeof(wazi_native_symbols) / sizeof(NativeSymbol);
}


