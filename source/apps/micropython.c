//
// Created by Samuel Jones on 3/2/22.
//


#include "py/runtime.h"
#include "modmachine.h"

#include "blinkenlights.h"
#include "conductor.h"
#include "lunarlander.h"
#include "badge_monsters.h"
#include "spacetripper.h"
#include "smashout.h"
#include "hacking_simulator.h"
#include "cube.h"
#include "game_of_life.h"

static mp_obj_t mp_blinkenlights_cb() {
    return mp_obj_new_int(blinkenlights_cb());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_blinkenlights_cb_obj, mp_blinkenlights_cb);


static mp_obj_t mp_conductor_cb() {
    return mp_obj_new_int(conductor_cb());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_conductor_cb_obj, mp_conductor_cb);



static mp_obj_t mp_lunarlander_cb() {
    return mp_obj_new_int(lunarlander_cb());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_lunarlander_cb_obj, mp_lunarlander_cb);



static mp_obj_t mp_badge_monsters_cb() {
    return mp_obj_new_int(badge_monsters_cb());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_badge_monsters_cb_obj, mp_badge_monsters_cb);



static mp_obj_t mp_spacetripper_cb() {
    return mp_obj_new_int(spacetripper_cb());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_spacetripper_cb_obj, mp_spacetripper_cb);



static mp_obj_t mp_smashout_cb() {
    return mp_obj_new_int(smashout_cb());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_smashout_cb_obj, mp_smashout_cb);



static mp_obj_t mp_hacking_simulator_cb() {
    return mp_obj_new_int(hacking_simulator_cb());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_hacking_simulator_cb_obj, mp_hacking_simulator_cb);



static mp_obj_t mp_cube_cb() {
    return mp_obj_new_int(cube_cb());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_cube_cb_obj, mp_cube_cb);



static mp_obj_t mp_game_of_life_cb() {
    return mp_obj_new_int(game_of_life_cb());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_game_of_life_cb_obj, mp_game_of_life_cb);


static const mp_rom_map_elem_t badge_c_app_globals_table[] = {
        {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_badge_c_apps)},
        {MP_ROM_QSTR(MP_QSTR_blinkenlights), MP_ROM_PTR(&mp_blinkenlights_cb_obj)},
        {MP_ROM_QSTR(MP_QSTR_conductor), MP_ROM_PTR(&mp_conductor_cb_obj)},
        {MP_ROM_QSTR(MP_QSTR_lunar_rescue), MP_ROM_PTR(&mp_lunarlander_cb_obj)},
        {MP_ROM_QSTR(MP_QSTR_badge_monsters), MP_ROM_PTR(&mp_badge_monsters_cb_obj)},
        {MP_ROM_QSTR(MP_QSTR_space_tripper), MP_ROM_PTR(&mp_spacetripper_cb_obj)},
        {MP_ROM_QSTR(MP_QSTR_smashout), MP_ROM_PTR(&mp_smashout_cb_obj)},
        {MP_ROM_QSTR(MP_QSTR_hacking_simulator), MP_ROM_PTR(&mp_hacking_simulator_cb_obj)},
        {MP_ROM_QSTR(MP_QSTR_cube), MP_ROM_PTR(&mp_cube_cb_obj)},
        {MP_ROM_QSTR(MP_QSTR_game_of_life), MP_ROM_PTR(&mp_game_of_life_cb_obj)},
};
STATIC MP_DEFINE_CONST_DICT(badge_c_app_globals, badge_c_app_globals_table);

// Define module object.
const mp_obj_module_t badge_c_app_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t *)&badge_c_app_globals,
}

MP_REGISTER_MODULE(MP_QSTR_badge_c_apps, badge_c_app_module, 1);