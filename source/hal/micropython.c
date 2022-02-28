//
// Created by Samuel Jones on 2/26/22.
//

#include <string.h>

// Include MicroPython API.
#include "py/runtime.h"
#include "py/gc.h"
#include "modmachine.h"

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

STATIC mp_obj_t mp_button_get_rotation(void) {
    return mp_obj_new_int(button_get_rotation());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_button_get_rotation_obj, mp_button_get_rotation);

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
        {MP_ROM_QSTR(MP_QSTR_button_get_rotation),               MP_ROM_PTR(&mp_button_get_rotation_obj)},

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

STATIC mp_obj_t mp_flash_data_read(mp_obj_t page, mp_obj_t offset, mp_obj_t len) {
    uint8_t *bytes = mp_local_alloc(mp_obj_get_int(len));
    size_t data_read = flash_data_read(mp_obj_get_int(page), mp_obj_get_int(offset), bytes, mp_obj_get_int(len));
    mp_obj_t bytes_obj = mp_obj_new_bytes(bytes, data_read);
    mp_local_free(bytes);
    return bytes_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_flash_data_read_obj, mp_flash_data_read);

STATIC mp_obj_t mp_flash_data_write(mp_obj_t page, mp_obj_t offset, mp_obj_t data) {

    size_t len = 0;
    const uint8_t *byte_data = (const uint8_t*) mp_obj_str_get_data(data, &len);
    if (len && byte_data) {
        len = flash_data_write(mp_obj_get_int(page), mp_obj_get_int(offset), byte_data, len);
    }

    return mp_obj_new_int_from_uint(len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_flash_data_write_obj, mp_flash_data_write);

STATIC mp_obj_t mp_flash_erase(mp_obj_t page) {
    flash_erase(mp_obj_get_int(page));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_flash_erase_obj, mp_flash_erase);

STATIC mp_obj_t mp_flash_erase_all(void) {
    flash_erase_all();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_flash_erase_all_obj, mp_flash_erase_all);


STATIC const mp_rom_map_elem_t flash_globals_table[] = {
        {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_flash)},

        {MP_ROM_QSTR(MP_QSTR_flash_data_read), MP_ROM_PTR(&mp_flash_data_read_obj)},
        {MP_ROM_QSTR(MP_QSTR_flash_data_write), MP_ROM_PTR(&mp_flash_data_write_obj)},
        {MP_ROM_QSTR(MP_QSTR_flash_erase), MP_ROM_PTR(&mp_flash_erase_obj)},
        {MP_ROM_QSTR(MP_QSTR_flash_erase_all), MP_ROM_PTR(&mp_flash_erase_all_obj)},

        {MP_ROM_QSTR(MP_QSTR_NUM_DATA_SECTORS), MP_ROM_INT(NUM_DATA_SECTORS)},
        {MP_ROM_QSTR(MP_QSTR_FLASH_SECTOR_SIZE), MP_ROM_INT(FLASH_SECTOR_SIZE)},
};

STATIC MP_DEFINE_CONST_DICT(flash_globals, flash_globals_table);

const mp_obj_module_t badge_flash_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&flash_globals,
};



/* --------------------------------------------------------------- */
// Init HAL bindings

// Note: only HAL init and deinit bindings are available.

STATIC mp_obj_t mp_hal_init(void) {
    machine_pin_deinit();
    hal_init();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_hal_init_obj, mp_hal_init);

STATIC mp_obj_t mp_hal_deinit(void) {
    hal_deinit();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_hal_deinit_obj, mp_hal_deinit);

STATIC const mp_rom_map_elem_t init_globals_table[] = {
        {MP_ROM_QSTR(MP_QSTR___name__),                          MP_ROM_QSTR(MP_QSTR_init_badge)},

        {MP_ROM_QSTR(MP_QSTR_hal_init), MP_ROM_PTR(&mp_hal_init_obj)},
        {MP_ROM_QSTR(MP_QSTR_hal_deinit), MP_ROM_PTR(&mp_hal_deinit_obj)},
};

STATIC MP_DEFINE_CONST_DICT(init_globals, init_globals_table);

const mp_obj_module_t badge_init_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&init_globals,
};



/* --------------------------------------------------------------- */
// IR HAL bindings


const mp_obj_type_t ir_type;

typedef struct {
    mp_obj_base_t base;
    uint16_t recipient_address;
    uint8_t app_address;
    uint8_t data_length;
    uint8_t data[MAX_IR_MESSAGE_SIZE];
} MP_IR_DATA;

STATIC void ir_data_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    MP_IR_DATA *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "ir data - badge id: ");
    mp_obj_print_helper(print, mp_obj_new_int(self->recipient_address), PRINT_REPR);
    mp_print_str(print, ", app: ");
    mp_obj_print_helper(print, mp_obj_new_int(self->app_address), PRINT_REPR);
    mp_print_str(print, ", data: ");
    mp_obj_print_helper(print, mp_obj_new_bytes(self->data, self->data_length), PRINT_REPR);
}

STATIC mp_obj_t ir_data_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 3, 3, true);

    MP_IR_DATA *ir = m_new_obj(MP_IR_DATA);
    ir->base.type = &ir_type;
    ir->recipient_address = mp_obj_get_int(args[0]);
    ir->app_address = mp_obj_get_int(args[1]);
    size_t data_len = 0;
    const char *str_data = mp_obj_str_get_data(args[2], &data_len);
    if (data_len) {
        if (data_len > MAX_IR_MESSAGE_SIZE) {
            data_len = MAX_IR_MESSAGE_SIZE;
        }
        memcpy(ir->data, str_data, data_len);
    }
    ir->data_length = data_len;
    return MP_OBJ_FROM_PTR(ir);
}

STATIC void ir_data_attr(mp_obj_t self, qstr attribute, mp_obj_t *destination) {
    MP_IR_DATA *ir = MP_OBJ_TO_PTR(self);
    if(attribute == MP_QSTR_data) {
        destination[0] = mp_obj_new_bytes(ir->data, ir->data_length);
    } else if (attribute == MP_QSTR_recipient_address) {
        destination[0] = mp_obj_new_int(ir->recipient_address);
    } else if (attribute == MP_QSTR_app_address) {
        destination[0] = mp_obj_new_int(ir->app_address);
    }
}

const mp_obj_type_t ir_type = {
    { &mp_type_type },
    .name = MP_QSTR_IRData,
    .print = ir_data_print,
    .make_new = ir_data_make_new,
    .attr = ir_data_attr,
};

static mp_obj_t ir_cb[IR_MAX_ID];


STATIC mp_obj_t mp_ir_init(void) {
    for (int i=0; i<IR_MAX_ID; i++) {
        ir_cb[i] = mp_const_none;
    }
    ir_init();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_ir_init_obj, mp_ir_init);

static void mp_ir_callback_handler(const IR_DATA *data) {

    MP_IR_DATA *mp_ir = m_new_obj(MP_IR_DATA);
    mp_ir->base.type = &ir_type;
    mp_ir->app_address = data->app_address;
    mp_ir->recipient_address = data->recipient_address;
    memcpy(mp_ir->data, data->data, data->data_length);
    mp_ir->data_length = data->data_length;

    if (ir_cb[data->app_address] != mp_const_none) {
        mp_sched_schedule(ir_cb[data->app_address], MP_OBJ_FROM_PTR(mp_ir));
    }
}

STATIC mp_obj_t mp_ir_add_callback(mp_obj_t data_cb, mp_obj_t app_id) {
    // Note: only one callback per ID supported in Python
    int app = mp_obj_get_int(app_id);
    if (app < IR_MAX_ID) {
        ir_cb[app] = data_cb;
        ir_add_callback(mp_ir_callback_handler, app);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_ir_add_callback_obj, mp_ir_add_callback);

STATIC mp_obj_t mp_ir_remove_callback(mp_obj_t data_cb, mp_obj_t app_id) {

    int app = mp_obj_get_int(app_id);
    if (app < IR_MAX_ID) {
        ir_remove_callback(mp_ir_callback_handler, app);
        ir_cb[app] = mp_const_none;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_ir_remove_callback_obj, mp_ir_remove_callback);

STATIC mp_obj_t mp_ir_transmitting(void) {
    return mp_obj_new_bool(ir_transmitting());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_ir_transmitting_obj, mp_ir_transmitting);

STATIC mp_obj_t mp_ir_send_complete_message(mp_obj_t data) {
    if(!mp_obj_is_type(data, &ir_type)) {
        mp_raise_TypeError("Argument needs to be an ir_data object");
    }
    MP_IR_DATA *ir = MP_OBJ_TO_PTR(data);

    IR_DATA c_data = {
        .recipient_address = ir->recipient_address,
        .app_address = ir->app_address,
        .data_length = ir->data_length,
        .data = ir->data,
    };

    ir_send_complete_message(&c_data);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_ir_send_complete_message_obj, mp_ir_send_complete_message);

STATIC mp_obj_t mp_ir_send_partial_message(mp_obj_t data, mp_obj_t starting_seqn) {
    if(!mp_obj_is_type(data, &ir_type)) {
        mp_raise_TypeError("Argument needs to be an ir_data object");
    }
    MP_IR_DATA *ir = MP_OBJ_TO_PTR(data);

    IR_DATA c_data = {
            .recipient_address = ir->recipient_address,
            .app_address = ir->app_address,
            .data_length = ir->data_length,
            .data = ir->data,
    };

    int sent = ir_send_partial_message(&c_data, mp_obj_get_int(starting_seqn));
    return mp_obj_new_int(sent);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_ir_send_partial_message_obj, mp_ir_send_partial_message);

STATIC mp_obj_t mp_ir_messages_seen(mp_obj_t reset) {
    return mp_obj_new_int(ir_messages_seen(mp_obj_get_int(reset)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_ir_messages_seen_obj, mp_ir_messages_seen);

STATIC mp_obj_t mp_ir_message_count(void) {
    return mp_obj_new_int(ir_message_count());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_ir_message_count_obj, mp_ir_message_count);



STATIC const mp_rom_map_elem_t ir_globals_table[] = {
        {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ir)},
        {MP_OBJ_NEW_QSTR(MP_QSTR_IRData), (mp_obj_t)&ir_type},

        //[vim_insert]
        {MP_ROM_QSTR(MP_QSTR_ir_init), MP_ROM_PTR(&mp_ir_init_obj)},
        {MP_ROM_QSTR(MP_QSTR_ir_add_callback), MP_ROM_PTR(&mp_ir_add_callback_obj)},
        {MP_ROM_QSTR(MP_QSTR_ir_remove_callback), MP_ROM_PTR(&mp_ir_remove_callback_obj)},
        {MP_ROM_QSTR(MP_QSTR_ir_transmitting), MP_ROM_PTR(&mp_ir_transmitting_obj)},
        {MP_ROM_QSTR(MP_QSTR_ir_send_complete_message), MP_ROM_PTR(&mp_ir_send_complete_message_obj)},
        {MP_ROM_QSTR(MP_QSTR_ir_send_partial_message), MP_ROM_PTR(&mp_ir_send_partial_message_obj)},
        {MP_ROM_QSTR(MP_QSTR_ir_messages_seen), MP_ROM_PTR(&mp_ir_messages_seen_obj)},
        {MP_ROM_QSTR(MP_QSTR_ir_message_count), MP_ROM_PTR(&mp_ir_message_count_obj)},

        {MP_ROM_QSTR(MP_QSTR_IR_LED_ID), MP_ROM_INT(IR_LED)},
        {MP_ROM_QSTR(MP_QSTR_IR_LIVEAUDIO_ID), MP_ROM_INT(IR_LIVEAUDIO)},
        {MP_ROM_QSTR(MP_QSTR_IR_PING_ID), MP_ROM_INT(IR_PING)},
        {MP_ROM_QSTR(MP_QSTR_IR_APP0_ID), MP_ROM_INT(IR_APP0)},
        {MP_ROM_QSTR(MP_QSTR_IR_APP1_ID), MP_ROM_INT(IR_APP1)},
        {MP_ROM_QSTR(MP_QSTR_IR_APP2_ID), MP_ROM_INT(IR_APP2)},
        {MP_ROM_QSTR(MP_QSTR_IR_APP3_ID), MP_ROM_INT(IR_APP3)},
        {MP_ROM_QSTR(MP_QSTR_IR_APP4_ID), MP_ROM_INT(IR_APP4)},
        {MP_ROM_QSTR(MP_QSTR_IR_APP5_ID), MP_ROM_INT(IR_APP5)},
        {MP_ROM_QSTR(MP_QSTR_IR_APP6_ID), MP_ROM_INT(IR_APP6)},
        {MP_ROM_QSTR(MP_QSTR_IR_APP7_ID), MP_ROM_INT(IR_APP7)},
        {MP_ROM_QSTR(MP_QSTR_IR_MAX_ID), MP_ROM_INT(IR_MAX_ID)},

        {MP_ROM_QSTR(MP_QSTR_BADGE_ID_BROADCAST), MP_ROM_INT(IR_BADGE_ID_BROADCAST)},
        {MP_ROM_QSTR(MP_QSTR_MAX_IR_MESSAGE_SIZE), MP_ROM_INT(MAX_IR_MESSAGE_SIZE)},
};

STATIC MP_DEFINE_CONST_DICT(ir_globals, ir_globals_table);

const mp_obj_module_t badge_ir_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&ir_globals,
};


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

// Provided for completeness. Using the builtin `random` library may be easier.

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

// Provided for completeness. Using the builtin `time` library may be easier.

STATIC mp_obj_t mp_rtc_init_badge(mp_obj_t start_time) {
    rtc_init_badge(mp_obj_get_int(start_time));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_rtc_init_badge_obj, mp_rtc_init_badge);

STATIC mp_obj_t mp_rtc_set_time(mp_obj_t start_time) {
    rtc_set_time(mp_obj_int_get_uint_checked(start_time));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_rtc_set_time_obj, mp_rtc_set_time);

STATIC mp_obj_t mp_rtc_get_unix_seconds(void) {
    return mp_obj_new_int_from_ull(rtc_get_unix_seconds());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_rtc_get_unix_seconds_obj, mp_rtc_get_unix_seconds);

STATIC mp_obj_t mp_rtc_get_time_of_day(void) {
    return mp_obj_new_int_from_ull(rtc_get_time_of_day());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_rtc_get_time_of_day_obj, mp_rtc_get_time_of_day);

STATIC mp_obj_t mp_rtc_get_ms_since_boot(void) {
    return mp_obj_new_int_from_ull(rtc_get_ms_since_boot());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_rtc_get_ms_since_boot_obj, mp_rtc_get_ms_since_boot);

STATIC mp_obj_t mp_rtc_get_us_since_boot(void) {
    return mp_obj_new_int_from_ull(rtc_get_us_since_boot());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_rtc_get_us_since_boot_obj, mp_rtc_get_us_since_boot);


STATIC const mp_rom_map_elem_t rtc_globals_table[] = {
        {MP_ROM_QSTR(MP_QSTR___name__),                          MP_ROM_QSTR(MP_QSTR_rtc)},

        //[vim_insert]
        {MP_ROM_QSTR(MP_QSTR_rtc_init_badge), MP_ROM_PTR(&mp_rtc_init_badge_obj)},
        {MP_ROM_QSTR(MP_QSTR_rtc_set_time), MP_ROM_PTR(&mp_rtc_set_time_obj)},
        {MP_ROM_QSTR(MP_QSTR_rtc_get_unix_seconds), MP_ROM_PTR(&mp_rtc_get_unix_seconds_obj)},
        {MP_ROM_QSTR(MP_QSTR_rtc_get_time_of_day), MP_ROM_PTR(&mp_rtc_get_time_of_day_obj)},
        {MP_ROM_QSTR(MP_QSTR_rtc_get_ms_since_boot), MP_ROM_PTR(&mp_rtc_get_ms_since_boot_obj)},
        {MP_ROM_QSTR(MP_QSTR_rtc_get_us_since_boot), MP_ROM_PTR(&mp_rtc_get_us_since_boot_obj)},
};

STATIC MP_DEFINE_CONST_DICT(rtc_globals, rtc_globals_table);

const mp_obj_module_t badge_rtc_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&rtc_globals,
};

/* --------------------------------------------------------------- */
// HAL Module Definition

static const mp_rom_map_elem_t badge_hw_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__),                          MP_ROM_QSTR(MP_QSTR_badge_hw)},
    {MP_ROM_QSTR(MP_QSTR_button),                            MP_ROM_PTR(&badge_button_module)},
    {MP_ROM_QSTR(MP_QSTR_delay_badge),                       MP_ROM_PTR(&badge_delay_module)},
    {MP_ROM_QSTR(MP_QSTR_flash),                             MP_ROM_PTR(&badge_flash_module)},
    {MP_ROM_QSTR(MP_QSTR_init_badge),                        MP_ROM_PTR(&badge_init_module)},
    {MP_ROM_QSTR(MP_QSTR_ir),                                MP_ROM_PTR(&badge_ir_module)},
    {MP_ROM_QSTR(MP_QSTR_lcd),                               MP_ROM_PTR(&badge_lcd_module)},
    {MP_ROM_QSTR(MP_QSTR_led),                               MP_ROM_PTR(&badge_led_module)},
    {MP_ROM_QSTR(MP_QSTR_rng),                               MP_ROM_PTR(&badge_rng_module)},
    {MP_ROM_QSTR(MP_QSTR_rtc),                               MP_ROM_PTR(&badge_rtc_module)},
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