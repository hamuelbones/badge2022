import badge_hw


class BadgeApp:
    FRAME_MS = 33
    auto_probe_inputs = True

    def __init__(self):
        self.down_latches = 0
        self.button_state = 0
        self.up_latches = 0
        self.rotation = 0

    def button_went_down(self, button):
        return self.down_latches & (1 << button)

    def button_is_pressed(self, button):
        return self.button_state & (1 << button)

    def button_went_up(self, button):
        return self.up_latches & (1 << button)

    def callback(self):
        raise NotImplementedError("App has no callback function!")

    def run(self):
        while True:
            start_frame_ms = badge_hw.rtc.rtc_get_ms_since_boot()
            if self.auto_probe_inputs:
                self.down_latches = badge_hw.button.button_down_latches()
                self.up_latches = badge_hw.button.button_up_latches()
                self.button_state = badge_hw.button.button_mask()
                self.rotation = badge_hw.button.button_get_rotation()
            ret = self.callback()
            if ret:
                break
            end_frame_ms = badge_hw.rtc.rtc_get_ms_since_boot()
            delay_time = self.FRAME_MS - (end_frame_ms - start_frame_ms)
            if delay_time > 0:
                badge_hw.delay_badge.sleep_ms(delay_time)

