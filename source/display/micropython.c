

#include "py/runtime.h"
#include "modmachine.h"

#include "framebuffer.h"
#include "assets.h"
#include "assetList.h"
#include "colors.h"

STATIC mp_obj_t mb_init(void) {
    FbInit();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mb_init_obj, mb_init);
STATIC mp_obj_t mp_move(
        mp_obj_t x_obj,
        mp_obj_t y_obj) {
    mp_int_t x = mp_obj_get_int(x_obj);
    mp_int_t y = mp_obj_get_int(y_obj);
    FbMove(x, y);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_move_obj, mp_move);

STATIC mp_obj_t mp_move_relative(
        mp_obj_t x_obj,
        mp_obj_t y_obj) {
    mp_int_t x = mp_obj_get_int(x_obj);
    mp_int_t y = mp_obj_get_int(y_obj);
    FbMoveRelative(x, y);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_move_relative_obj, mp_move_relative);

STATIC mp_obj_t mp_move_x(
        mp_obj_t x_obj) {
    int x = mp_obj_get_int(x_obj);
    FbMoveX(x);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_move_x_obj, mp_move_x);

STATIC mp_obj_t mp_move_y(
        mp_obj_t y_obj) {
    int y = mp_obj_get_int(y_obj);
    FbMoveY(y);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_move_y_obj, mp_move_y);

STATIC mp_obj_t mp_clear() {
    FbClear();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_clear_obj, mp_clear);

STATIC mp_obj_t mp_color(
        mp_obj_t color_obj) {
    mp_int_t color = mp_obj_get_int(color_obj);
    FbColor(color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_color_obj, mp_color);

STATIC mp_obj_t mp_background_color(
        mp_obj_t color_obj) {
    mp_int_t color = mp_obj_get_int(color_obj);
    FbBackgroundColor(color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_background_color_obj, mp_background_color);

STATIC mp_obj_t mp_transparency(
        mp_obj_t color_obj) {
    mp_int_t transparency_mask = mp_obj_get_int(color_obj);
    FbTransparency(transparency_mask);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_transparency_obj, mp_transparency);

STATIC mp_obj_t mp_transparent_index(
        mp_obj_t color_obj) {
    mp_int_t color = mp_obj_get_int(color_obj);
    FbTransparentIndex(color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_transparent_index_obj, mp_transparent_index);

STATIC mp_obj_t mp_character(
        mp_obj_t charin_obj) {
    mp_int_t charin = mp_obj_get_int(charin_obj);
    FbCharacter(charin);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_character_obj, mp_character);

STATIC mp_obj_t mp_filled_rectangle(
        mp_obj_t width_obj,
        mp_obj_t height_obj) {
    mp_int_t width = mp_obj_get_int(width_obj);
    mp_int_t height = mp_obj_get_int(height_obj);
    FbFilledRectangle(width, height);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_filled_rectangle_obj, mp_filled_rectangle);

STATIC mp_obj_t mp_point(
        mp_obj_t x_obj,
        mp_obj_t y_obj) {
    mp_int_t x = mp_obj_get_int(x_obj);
    mp_int_t y = mp_obj_get_int(y_obj);
    FbPoint(x, y);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_point_obj, mp_point);

STATIC mp_obj_t mp_horizontal_line(size_t n_args, const mp_obj_t *args) {
    mp_int_t x0 = mp_obj_get_int(args[0]);
    mp_int_t y0 = mp_obj_get_int(args[1]);
    mp_int_t x1 = mp_obj_get_int(args[2]);
    mp_int_t y1 = mp_obj_get_int(args[3]);
    FbHorizontalLine(x0, y0, x1, y1);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_horizontal_line_obj, 4, 4, mp_horizontal_line);

STATIC mp_obj_t mp_vertical_line(size_t n_args, const mp_obj_t *args) {
    mp_int_t x0 = mp_obj_get_int(args[0]);
    mp_int_t y0 = mp_obj_get_int(args[1]);
    mp_int_t x1 = mp_obj_get_int(args[2]);
    mp_int_t y1 = mp_obj_get_int(args[3]);
    FbVerticalLine(x0, y0, x1, y1);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_vertical_line_obj, 4, 4, mp_vertical_line);

STATIC mp_obj_t mp_line(size_t n_args, const mp_obj_t *args) {
    mp_int_t x0 = mp_obj_get_int(args[0]);
    mp_int_t y0 = mp_obj_get_int(args[1]);
    mp_int_t x1 = mp_obj_get_int(args[2]);
    mp_int_t y1 = mp_obj_get_int(args[3]);
    FbLine(x0, y0, x1, y1);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_line_obj, 4, 4, mp_line);

STATIC mp_obj_t mp_write_line(mp_obj_t str_obj) {
    const char* str = mp_obj_str_get_str(str_obj);
    FbWriteLine(str);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_write_line_obj, mp_write_line);

STATIC mp_obj_t mp_write_string(mp_obj_t str_obj) {
    const char* str = mp_obj_str_get_str(str_obj);
    FbWriteString(str);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_write_string_obj, mp_write_string);

STATIC mp_obj_t mp_rectangle(mp_obj_t width, mp_obj_t height) {
    FbRectangle(mp_obj_get_int(width), mp_obj_get_int(height));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_rectangle_obj, mp_rectangle);

/* FbDrawObject() draws an object at x, y.  The coordinates of drawing[] should be centered at
 * (0, 0).  The coordinates in drawing[] are multiplied by scale, then divided by 1024 (via a shift)
 * so for 1:1 size, use scale of 1024.  Smaller values will scale the object down. This is different
 * than FbPolygonFromPoints() or FbDrawVectors() in that drawing[] contains signed chars, and the
 * polygons can be constructed via this program: https://github.com/smcameron/vectordraw
 * as well as allowing scaling.
 */
STATIC mp_obj_t mp_draw_object(size_t n_args, const mp_obj_t *args) {

    int point_len = mp_obj_get_int(mp_obj_len(args[0]));

    struct point *points = mp_local_alloc(sizeof(struct point)*point_len);

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t item, iterable = mp_getiter(args[0], &iter_buf);
    size_t point_num = 0;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {

        mp_obj_t *point_data = NULL;
        size_t len = 0;
        mp_obj_tuple_get(item, &len, &point_data);

        if (len < 2 || !point_data) {
            mp_raise_ValueError("Point doesn't have enough axes");
        }
        points[point_num].x = mp_obj_get_int(point_data[0]);
        points[point_num].y = mp_obj_get_int(point_data[1]);
        point_num++;
        if (point_num == point_len) {
            break;
        }
    }

    mp_int_t color = mp_obj_get_int(args[1]);
    mp_int_t x = mp_obj_get_int(args[2]);
    mp_int_t y = mp_obj_get_int(args[3]);
    mp_int_t scale = mp_obj_get_int(args[4]);

    FbDrawObject(points, point_num, color, x, y, scale);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_draw_object_obj, 5, 5, mp_draw_object);


STATIC mp_obj_t mp_push_buffer(void) {
    FbPushBuffer();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_push_buffer_obj, mp_push_buffer);

STATIC mp_obj_t mp_draw_asset(mp_obj_t asset_id) {
    if (mp_obj_get_int(asset_id) >= LASTASSET) {
        mp_raise_ValueError("Asset not recognized");
    }
    drawAsset(mp_obj_get_int(asset_id));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_draw_asset_obj, mp_draw_asset);

STATIC mp_obj_t mp_swap_buffers(void) {
    FbSwapBuffers();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_swap_buffers_obj, mp_swap_buffers);

STATIC mp_obj_t mp_paint_new_rows(void) {
    FbPaintNewRows();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_paint_new_rows_obj, mp_paint_new_rows);


/* --------------------------------------------------------------- */
// Display Python module

static const mp_rom_map_elem_t badge_display_globals_table[] = {
        {MP_ROM_QSTR(MP_QSTR___name__),                          MP_ROM_QSTR(MP_QSTR_badge_hw)},

        {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mb_init_obj)},

        {MP_ROM_QSTR(MP_QSTR_move), MP_ROM_PTR(&mp_move_obj) },
        {MP_ROM_QSTR(MP_QSTR_move_relative), MP_ROM_PTR(&mp_move_relative_obj) },
        {MP_ROM_QSTR(MP_QSTR_move_x), MP_ROM_PTR(&mp_move_y_obj) },
        {MP_ROM_QSTR(MP_QSTR_move_y), MP_ROM_PTR(&mp_move_y_obj) },
        {MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&mp_clear_obj) },
        {MP_ROM_QSTR(MP_QSTR_color), MP_ROM_PTR(&mp_color_obj) },
        {MP_ROM_QSTR(MP_QSTR_background_color), MP_ROM_PTR(&mp_background_color_obj) },
        {MP_ROM_QSTR(MP_QSTR_transparency), MP_ROM_PTR(&mp_transparency_obj) },
        {MP_ROM_QSTR(MP_QSTR_transparent_index), MP_ROM_PTR(&mp_transparent_index_obj) },
        {MP_ROM_QSTR(MP_QSTR_character), MP_ROM_PTR(&mp_character_obj) },
        {MP_ROM_QSTR(MP_QSTR_filled_rectangle), MP_ROM_PTR(&mp_filled_rectangle_obj) },
        {MP_ROM_QSTR(MP_QSTR_point), MP_ROM_PTR(&mp_point_obj) },
        {MP_ROM_QSTR(MP_QSTR_vertical_line), MP_ROM_PTR(&mp_vertical_line_obj)},
        {MP_ROM_QSTR(MP_QSTR_horizontal_line), MP_ROM_PTR(&mp_horizontal_line_obj)},
        {MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&mp_line_obj)},
        {MP_ROM_QSTR(MP_QSTR_write_line), MP_ROM_PTR(&mp_write_line_obj)},
        {MP_ROM_QSTR(MP_QSTR_write_string), MP_ROM_PTR(&mp_write_string_obj)},
        {MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&mp_rectangle_obj)},
        {MP_ROM_QSTR(MP_QSTR_draw_object), MP_ROM_PTR(&mp_draw_object)},
        {MP_ROM_QSTR(MP_QSTR_push_buffer), MP_ROM_PTR(&mp_push_buffer_obj)},
        {MP_ROM_QSTR(MP_QSTR_draw_asset), MP_ROM_PTR(&mp_draw_asset_obj)},
        {MP_ROM_QSTR(MP_QSTR_swap_buffers), MP_ROM_PTR(&mp_swap_buffers_obj)},
        {MP_ROM_QSTR(MP_QSTR_paint_new_rows), MP_ROM_PTR(&mp_paint_new_rows_obj)},

        {MP_ROM_QSTR(MP_QSTR_ASSET_HACKRVA), MP_ROM_INT(HACKRVA4)},
        {MP_ROM_QSTR(MP_QSTR_ASSET_RVASEC), MP_ROM_INT(RVASEC2016)},
        {MP_ROM_QSTR(MP_QSTR_ASSET_FONT), MP_ROM_INT(FONT)},
        {MP_ROM_QSTR(MP_QSTR_ASSET_DRBOB), MP_ROM_INT(DRBOB)},

        {MP_ROM_QSTR(MP_QSTR_MAX_X), MP_ROM_INT(LCD_XSIZE)},
        {MP_ROM_QSTR(MP_QSTR_MAX_Y), MP_ROM_INT(LCD_YSIZE)},

        {MP_ROM_QSTR(MP_QSTR_BLUE), MP_ROM_INT(BLUE)},
        {MP_ROM_QSTR(MP_QSTR_GREEN), MP_ROM_INT(GREEN)},
        {MP_ROM_QSTR(MP_QSTR_RED), MP_ROM_INT(RED)},
        {MP_ROM_QSTR(MP_QSTR_BLACK), MP_ROM_INT(BLACK)},
        {MP_ROM_QSTR(MP_QSTR_GREY1), MP_ROM_INT(GREY1)},
        {MP_ROM_QSTR(MP_QSTR_GREY2), MP_ROM_INT(GREY2)},
        {MP_ROM_QSTR(MP_QSTR_GREY4), MP_ROM_INT(GREY4)},
        {MP_ROM_QSTR(MP_QSTR_GREY8), MP_ROM_INT(GREY8)},
        {MP_ROM_QSTR(MP_QSTR_GREY16), MP_ROM_INT(GREY16)},
        {MP_ROM_QSTR(MP_QSTR_WHITE), MP_ROM_INT(WHITE)},
        {MP_ROM_QSTR(MP_QSTR_CYAN), MP_ROM_INT(CYAN)},
        {MP_ROM_QSTR(MP_QSTR_YELLOW), MP_ROM_INT(YELLOW)},
        {MP_ROM_QSTR(MP_QSTR_MAGENTA), MP_ROM_INT(MAGENTA)},
};

STATIC MP_DEFINE_CONST_DICT(badge_display_globals, badge_display_globals_table);

// Define module object.
const mp_obj_module_t badge_display_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&badge_display_globals,
};

// Register the module to make it available in Python.
// Note: the "1" in the third argument means this module is always enabled.
// This "1" can be optionally replaced with a macro like MODULE_CEXAMPLE_ENABLED
// which can then be used to conditionally enable this module.
MP_REGISTER_MODULE(MP_QSTR_badge_display, badge_display_module, 1);