//
// Created by Samuel Jones on 2/26/22.
//

// Include MicroPython API.
#include "py/runtime.h"
#include "py/gc.h"
#include "modmachine.h"
#include "shared/runtime/mpirq.h"

#include "audio_output.h"
#include "button.h"
#include "delay.h"
#include "display_s6b33.h"
#include "flash_storage.h"
#include "init.h"
#include "ir.h"
#include "led_pwm.h"
#include "random.h"
#include "rtc.h"


/* --------------------------------------------------------------- */
// Button HAL bindings

static mp_obj_t button_cb;

STATIC mp_obj_t mp_button_init_gpio(void) {
    // Note there are some SDK resources that are not sharable;
    // particularly w.r.t. interrupt handlers here.

    // So when using the badge button modules, the micropython
    // machine pin needs to be disabled.
    machine_pin_deinit();
    button_init_gpio();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_button_init_gpio_obj, mp_button_init_gpio);

STATIC mp_obj_t mp_button_poll(mp_obj_t button) {
    return mp_obj_new_int(button_poll(mp_obj_get_int(button)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_button_poll_obj, mp_button_poll);

STATIC mp_obj_t mp_button_mask(void) {
    return mp_obj_new_int(button_mask());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_button_mask_obj, mp_button_mask);

STATIC mp_obj_t mp_button_down_latches(void) {
    return mp_obj_new_int(button_down_latches());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_button_down_latches_obj, mp_button_down_latches);

STATIC mp_obj_t mp_button_up_latches(void) {
    return mp_obj_new_int(button_up_latches());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_button_up_latches_obj, mp_button_up_latches);

STATIC mp_obj_t mp_clear_latches(void) {
    button_clear_latches();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_clear_latches_obj, mp_clear_latches);


static void mp_button_callback_handler(BADGE_BUTTON button, bool state) {
    mp_obj_t callback_items[2];
    callback_items[0] = mp_obj_new_int(button);
    callback_items[1] = mp_obj_new_bool(state);
    mp_obj_t list_obj = mp_obj_new_tuple(2, callback_items);

    mp_sched_schedule(button_cb, list_obj);
}

static mp_obj_t mp_button_set_interrupt(mp_obj_t cb) {
    button_set_interrupt(NULL);
    button_cb = cb;
    button_set_interrupt(mp_button_callback_handler);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_button_set_interrupt_obj, mp_button_set_interrupt);

STATIC mp_obj_t mp_button_last_input_timestamp(void) {
    return mp_obj_new_int_from_uint(button_last_input_timestamp());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_button_last_input_timestamp_obj, mp_button_last_input_timestamp);

STATIC mp_obj_t mp_button_reset_last_input_timestamp(void) {
    button_reset_last_input_timestamp();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_button_reset_last_input_timestamp_obj, mp_button_reset_last_input_timestamp);

STATIC const mp_rom_map_elem_t button_globals_table[] = {
        {MP_ROM_QSTR(MP_QSTR___name__),                          MP_ROM_QSTR(MP_QSTR_button)},

        // Button
        {MP_ROM_QSTR(MP_QSTR_button_init_gpio),                  MP_ROM_PTR(&mp_button_init_gpio_obj)},
        {MP_ROM_QSTR(MP_QSTR_button_poll),                       MP_ROM_PTR(&mp_button_poll_obj)},
        {MP_ROM_QSTR(MP_QSTR_button_mask),                       MP_ROM_PTR(&mp_button_mask_obj)},
        {MP_ROM_QSTR(MP_QSTR_button_down_latches),               MP_ROM_PTR(&mp_button_down_latches_obj)},
        {MP_ROM_QSTR(MP_QSTR_button_up_latches),                 MP_ROM_PTR(&mp_button_up_latches_obj)},
        {MP_ROM_QSTR(MP_QSTR_clear_latches),                     MP_ROM_PTR(&mp_clear_latches_obj)},
        {MP_ROM_QSTR(MP_QSTR_button_set_interrupt),              MP_ROM_PTR(&mp_button_set_interrupt_obj)},
        {MP_ROM_QSTR(MP_QSTR_button_last_input_timestamp),       MP_ROM_PTR(&mp_button_last_input_timestamp_obj)},
        {MP_ROM_QSTR(MP_QSTR_button_reset_last_input_timestamp), MP_ROM_PTR(&mp_button_reset_last_input_timestamp_obj)},

        {MP_ROM_QSTR(MP_QSTR_BUTTON_LEFT),                       MP_ROM_INT(BADGE_BUTTON_LEFT)},
        {MP_ROM_QSTR(MP_QSTR_BUTTON_DOWN),                       MP_ROM_INT(BADGE_BUTTON_DOWN)},
        {MP_ROM_QSTR(MP_QSTR_BUTTON_UP),                         MP_ROM_INT(BADGE_BUTTON_UP)},
        {MP_ROM_QSTR(MP_QSTR_BUTTON_RIGHT),                      MP_ROM_INT(BADGE_BUTTON_RIGHT)},
        {MP_ROM_QSTR(MP_QSTR_BUTTON_SW),                         MP_ROM_INT(BADGE_BUTTON_SW)},
        {MP_ROM_QSTR(MP_QSTR_BUTTON_ENCODER_A),                  MP_ROM_INT(BADGE_BUTTON_ENCODER_A)},
        {MP_ROM_QSTR(MP_QSTR_BUTTON_ENCODER_B),                  MP_ROM_INT(BADGE_BUTTON_ENCODER_B)},
};
STATIC MP_DEFINE_CONST_DICT(button_globals, button_globals_table);

const mp_obj_module_t badge_button_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&button_globals,
};


/* --------------------------------------------------------------- */
// Delay HAL bindings

static mp_obj_t mp_sleep_ms(mp_obj_t ms) {
    sleep_ms(mp_obj_get_int(ms));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_sleep_ms_obj, mp_sleep_ms);

static mp_obj_t mp_sleep_us(mp_obj_t us) {
    sleep_us(mp_obj_get_int(us));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_sleep_us_obj, mp_sleep_us);

STATIC const mp_rom_map_elem_t delay_globals_table[] = {
        // Delay is already taken, need a different name.
        {MP_ROM_QSTR(MP_QSTR___name__),                          MP_ROM_QSTR(MP_QSTR_delay_badge)},
        //[vim_insert]
        {MP_ROM_QSTR(MP_QSTR_sleep_ms), MP_ROM_PTR(&mp_sleep_ms_obj)},
        {MP_ROM_QSTR(MP_QSTR_sleep_us), MP_ROM_PTR(&mp_sleep_us_obj)},

};
STATIC MP_DEFINE_CONST_DICT(delay_globals, delay_globals_table);

const mp_obj_module_t badge_delay_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&delay_globals,
};


/* --------------------------------------------------------------- */
// Display HAL bindings

static mp_obj_t mp_S6B33_init_device(void) {
    S6B33_init_device();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_S6B33_init_device_obj, mp_S6B33_init_device);

static mp_obj_t mp_S6B33_init_gpio(void) {
    S6B33_init_gpio();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_S6B33_init_gpio_obj, mp_S6B33_init_gpio);

static mp_obj_t mp_S6B33_reset(void) {
    S6B33_reset();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_S6B33_reset_obj, mp_S6B33_reset);

static mp_obj_t mp_S6B33_rect(unsigned int count, const mp_obj_t *args) {
    S6B33_rect(mp_obj_get_int(args[0]), mp_obj_get_int(args[1]), mp_obj_get_int(args[2]), mp_obj_get_int(args[3]));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_S6B33_rect_obj, 4, mp_S6B33_rect);

static mp_obj_t mp_S6B33_pixel(mp_obj_t pixel) {
    S6B33_pixel(mp_obj_get_int(pixel));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_S6B33_pixel_obj, mp_S6B33_pixel);

// Won't have super great performance, use frame buffers instead to write the whole frame
static mp_obj_t mp_S6B33_pixels(mp_obj_t pixels) {
    uint16_t pixel_buffer[100];
    int pixel_count = 0;
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(pixels, &iter_buf);
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        pixel_buffer[pixel_count++] = mp_obj_get_int(item);
        if (pixel_count == 100) {
            S6B33_pixels(pixel_buffer, pixel_count);
            pixel_count = 0;
        }
    }
    if (pixel_count) {
        S6B33_pixels(pixel_buffer, pixel_count);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_S6B33_pixels_obj, mp_S6B33_pixels);

static mp_obj_t mp_S6B33_set_display_mode_inverted(void) {
    S6B33_set_display_mode_inverted();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_S6B33_set_display_mode_inverted_obj, mp_S6B33_set_display_mode_inverted);

static mp_obj_t mp_S6B33_set_display_mode_noninverted(void) {
    S6B33_set_display_mode_noninverted();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_S6B33_set_display_mode_noninverted_obj, mp_S6B33_set_display_mode_noninverted);

static mp_obj_t mp_S6B33_get_display_mode(void) {
    return mp_obj_new_int(S6B33_get_display_mode());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_S6B33_get_display_mode_obj, mp_S6B33_get_display_mode);

static mp_obj_t mp_S6B33_get_rotation(void) {
    return mp_obj_new_int(S6B33_get_rotation());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_S6B33_get_rotation_obj, mp_S6B33_get_rotation);

static mp_obj_t mp_S6B33_set_rotation(mp_obj_t yes) {
    S6B33_set_rotation(mp_obj_get_int(yes));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_S6B33_set_rotation_obj, mp_S6B33_set_rotation);

static mp_obj_t mp_S6B33_color(mp_obj_t pixel) {
    S6B33_color(mp_obj_get_int(pixel));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_S6B33_color_obj, mp_S6B33_color);


STATIC const mp_rom_map_elem_t lcd_globals_table[] = {
        {MP_ROM_QSTR(MP_QSTR___name__),                          MP_ROM_QSTR(MP_QSTR_led)},
        //[vim_insert]
        {MP_ROM_QSTR(MP_QSTR_S6B33_init_device), MP_ROM_PTR(&mp_S6B33_init_device_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_init_gpio), MP_ROM_PTR(&mp_S6B33_init_gpio_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_reset), MP_ROM_PTR(&mp_S6B33_reset_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_rect), MP_ROM_PTR(&mp_S6B33_rect_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_pixel), MP_ROM_PTR(&mp_S6B33_pixel_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_pixels), MP_ROM_PTR(&mp_S6B33_pixels_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_set_display_mode_inverted), MP_ROM_PTR(&mp_S6B33_set_display_mode_inverted_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_set_display_mode_noninverted), MP_ROM_PTR(&mp_S6B33_set_display_mode_noninverted_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_get_display_mode), MP_ROM_PTR(&mp_S6B33_get_display_mode_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_get_rotation), MP_ROM_PTR(&mp_S6B33_get_rotation_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_set_rotation), MP_ROM_PTR(&mp_S6B33_set_rotation_obj)},
        {MP_ROM_QSTR(MP_QSTR_S6B33_color), MP_ROM_PTR(&mp_S6B33_color_obj)},

        {MP_ROM_QSTR(MP_QSTR_LCD_XSIZE), MP_ROM_INT(132)},
        {MP_ROM_QSTR(MP_QSTR_LCD_YSIZE), MP_ROM_INT(132)},
};
STATIC MP_DEFINE_CONST_DICT(lcd_globals, lcd_globals_table);

const mp_obj_module_t badge_lcd_module = {
        .base = {&mp_type_module},
        .globals = (mp_obj_dict_t *) &lcd_globals,
};

/* --------------------------------------------------------------- */
// Flash storage HAL bindings

// TODO
size_t flash_data_read(uint8_t page, uint16_t offset, uint8_t *buf, size_t len);
size_t flash_data_write(uint8_t page, uint16_t offset, const uint8_t *buf, size_t len);
void flash_erase(uint8_t page);
void flash_erase_all(void);

/* --------------------------------------------------------------- */
// Init HAL bindings

// TODO
void hal_init(void);
int hal_run_main(int (*main_func)(int, char**), int argc, char** argv);
void hal_deinit(void);
void hal_reboot(void);
uint32_t hal_disable_interrupts(void);
void hal_restore_interrupts(uint32_t state);


/* --------------------------------------------------------------- */
// IR HAL bindings

// TODO
void ir_init(void);
bool ir_add_callback(ir_data_callback data_cb, IR_APP_ID app_id);
bool ir_remove_callback(ir_data_callback data_cb, IR_APP_ID app_id);
bool ir_transmitting(void);
void ir_send_complete_message(const IR_DATA *data);
uint8_t ir_send_partial_message(const IR_DATA *data, uint8_t starting_sequence_num);
bool ir_messages_seen(bool reset);
int ir_message_count(void);

/* --------------------------------------------------------------- */
// LED HAL bindings


STATIC mp_obj_t mp_led_pwm_init_gpio(void) {
    led_pwm_init_gpio();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_led_pwm_init_gpio_obj, mp_led_pwm_init_gpio);

STATIC mp_obj_t mp_led_pwm_enable(mp_obj_t led_enum, mp_obj_t duty) {
    led_pwm_enable(mp_obj_get_int(led_enum), mp_obj_get_int(duty));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_led_pwm_enable_obj, mp_led_pwm_enable);

STATIC mp_obj_t mp_led_pwm_disable(mp_obj_t led_enum) {
    led_pwm_disable(mp_obj_get_int(led_enum));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_led_pwm_disable_obj, mp_led_pwm_disable);

STATIC const mp_rom_map_elem_t led_globals_table[] = {
        {MP_ROM_QSTR(MP_QSTR___name__),                          MP_ROM_QSTR(MP_QSTR_led)},

        // LED functions
        {MP_ROM_QSTR(MP_QSTR_led_pwm_init_gpio), MP_ROM_PTR(&mp_led_pwm_init_gpio_obj)},
        {MP_ROM_QSTR(MP_QSTR_led_pwm_enable), MP_ROM_PTR(&mp_led_pwm_enable_obj)},
        {MP_ROM_QSTR(MP_QSTR_led_pwm_disable), MP_ROM_PTR(&mp_led_pwm_disable_obj)},
        // LED constants
        {MP_ROM_QSTR(MP_QSTR_LED_RED), MP_ROM_INT(BADGE_LED_RGB_RED)},
        {MP_ROM_QSTR(MP_QSTR_LED_GREEN), MP_ROM_INT(BADGE_LED_RGB_GREEN)},
        {MP_ROM_QSTR(MP_QSTR_LED_BLUE), MP_ROM_INT(BADGE_LED_RGB_BLUE)},
        {MP_ROM_QSTR(MP_QSTR_DISPLAY_BACKLIGHT), MP_ROM_INT(BADGE_LED_DISPLAY_BACKLIGHT)},
};
STATIC MP_DEFINE_CONST_DICT(led_globals, led_globals_table);

const mp_obj_module_t badge_led_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&led_globals,
};

/* --------------------------------------------------------------- */
// RNG HAL Bindings

void random_insecure_bytes(uint8_t *bytes, size_t len);

STATIC mp_obj_t mp_random_insecure_bytes(mp_obj_t len) {
    uint8_t *bytes = mp_local_alloc(mp_obj_get_int(len));
    random_insecure_bytes(bytes, mp_obj_get_int(len));
    mp_obj_t return_obj = mp_obj_new_bytes(bytes, mp_obj_get_int(len));
    mp_local_free(bytes);
    return return_obj;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_random_insecure_bytes_obj, mp_random_insecure_bytes);

STATIC const mp_rom_map_elem_t rng_globals_table[] = {
        {MP_ROM_QSTR(MP_QSTR___name__),                          MP_ROM_QSTR(MP_QSTR_rng)},
        {MP_ROM_QSTR(MP_QSTR_random_insecure_bytes), MP_ROM_PTR(&mp_random_insecure_bytes_obj)},
};
STATIC MP_DEFINE_CONST_DICT(rng_globals, rng_globals_table);

const mp_obj_module_t badge_rng_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&rng_globals,
};

/* --------------------------------------------------------------- */
// RTC HAL Bindings

// TODO
void mp_rtc_init_badge(time_t start_time);
void mp_rtc_set_time(time_t start_time);
time_t mp_rtc_get_unix_seconds(void);
struct timeval mp_rtc_get_time_of_day(void);
uint64_t mp_rtc_get_ms_since_boot(void);
uint64_t mp_rtc_get_us_since_boot(void);


/* --------------------------------------------------------------- */
// HAL Module Definition

static const mp_rom_map_elem_t badge_hw_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__),                          MP_ROM_QSTR(MP_QSTR_badge_hw)},
    {MP_ROM_QSTR(MP_QSTR_led),                               MP_ROM_PTR(&badge_led_module)},
    {MP_ROM_QSTR(MP_QSTR_button),                            MP_ROM_PTR(&badge_button_module)},
    {MP_ROM_QSTR(MP_QSTR_delay_badge),                       MP_ROM_PTR(&badge_delay_module)},
    {MP_ROM_QSTR(MP_QSTR_lcd),                               MP_ROM_PTR(&badge_lcd_module)},
    {MP_ROM_QSTR(MP_QSTR_rng),                               MP_ROM_PTR(&badge_rng_module)},
};


STATIC MP_DEFINE_CONST_DICT(badge_hw_globals, badge_hw_globals_table);

// Define module object.
const mp_obj_module_t badge_hal_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&badge_hw_globals,
};

// Register the module to make it available in Python.
// Note: the "1" in the third argument means this module is always enabled.
// This "1" can be optionally replaced with a macro like MODULE_CEXAMPLE_ENABLED
// which can then be used to conditionally enable this module.
MP_REGISTER_MODULE(MP_QSTR_badge_hw, badge_hal_module, 1);