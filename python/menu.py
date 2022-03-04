
import badge_hw
import badge_display

from app import BadgeApp

class BadgeAppMenu(BadgeApp):

    def __init__(self, name, apps, is_root=False):
        super().__init__()
        self.apps = apps
        self.center_app_drawn = 3
        self.selected_app = 0
        self.is_root = is_root
        self.name = name
        self.callback = self.menu

    def process_inputs(self, app_count):

        if self.rotation > 0 or self.button_went_down(badge_hw.button.BUTTON_DOWN):
            if self.selected_app < app_count-1:
                self.selected_app += 1
                if self.selected_app - 3 > self.center_app_drawn:
                    self.center_app_drawn += 1

        elif self.rotation < 0 or self.button_went_down(badge_hw.button.BUTTON_UP):
            if self.selected_app > 0:
                self.selected_app -= 1
                if self.selected_app + 3 < self.center_app_drawn:
                    self.center_app_drawn -= 1

        elif self.button_went_down(badge_hw.button.BUTTON_SW):
            self.apps[self.selected_app]().run()

    def draw(self, app_count):
        badge_display.clear()
        for draw_num, app_num in enumerate(range(self.center_app_drawn-4, self.center_app_drawn+5)):
            if app_num < 0 or app_num >= app_count:
                continue
            offset = 3 + 12 * draw_num
            badge_display.move(8, offset)
            if app_num == self.selected_app:
                badge_display.color(badge_display.CYAN)
            else:
                badge_display.color(badge_display.GREY8)
            badge_display.write_line(self.apps[app_num].name)
        badge_display.push_buffer()

    def menu(self):
        app_count = len(self.apps)
        self.process_inputs(app_count)
        self.draw(app_count)
        return
