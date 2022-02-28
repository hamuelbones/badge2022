
add_library(badge_hw INTERFACE)


pico_generate_pio_header(badge_hw ${CMAKE_CURRENT_LIST_DIR}/ir_library/nec_transmit_library/nec_carrier_burst.pio)
pico_generate_pio_header(badge_hw ${CMAKE_CURRENT_LIST_DIR}/ir_library/nec_transmit_library/nec_carrier_control.pio)
pico_generate_pio_header(badge_hw ${CMAKE_CURRENT_LIST_DIR}/ir_library/nec_receive_library/nec_receive.pio)

target_sources(badge_hw INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/delay_rp2040.c
        ${CMAKE_CURRENT_LIST_DIR}/flash_storage_rp2040.c
        ${CMAKE_CURRENT_LIST_DIR}/init_rp2040.c
        #${CMAKE_CURRENT_LIST_DIR}/usb_rp2040.c
        ${CMAKE_CURRENT_LIST_DIR}/display_s6b33_rp2040.c
        ${CMAKE_CURRENT_LIST_DIR}/led_pwm_rp2040.c
        ${CMAKE_CURRENT_LIST_DIR}/button_rp2040.c
        ${CMAKE_CURRENT_LIST_DIR}/ir_rp2040.c
        ${CMAKE_CURRENT_LIST_DIR}/audio_output_rp2040.c
        ${CMAKE_CURRENT_LIST_DIR}/rtc_rp2040.c
        ${CMAKE_CURRENT_LIST_DIR}/random_rp2040.c

        ${CMAKE_CURRENT_LIST_DIR}/micropython.c

        ${CMAKE_CURRENT_LIST_DIR}/ir_library/nec_receive_library/nec_receive.c
        ${CMAKE_CURRENT_LIST_DIR}/ir_library/nec_transmit_library/nec_transmit.c
)

# add the `binary` directory so that the generated headers are included in the project
#
target_include_directories(badge_hw INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/ir_library/nec_receive_library/
        ${CMAKE_CURRENT_LIST_DIR}/ir_library/nec_transmit_library/
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        )

target_link_libraries(usermod INTERFACE badge_hw)


