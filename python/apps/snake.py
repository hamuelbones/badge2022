
import badge_display
import badge_hw
import random
from app import BadgeApp


class SnakeApp(BadgeApp):
    name = "Snake"

    def __init__(self):
        super().__init__()
        # Do any setup here
        self.grid_width = 8
        self.grid_height = 8
        self.grid = bytearray(self.grid_width * self.grid_height)
        self.direction = b'u'
        self.frames_to_wait = 15
        self.grow_length = 2
        self.head_x = 4
        self.head_y = 4
        self.snake = [(4, 4)]
        self.grid_set(4, 4, b's')
        self.dead = False
        self.score = 0
        self.add_food()

    def add_food(self):
        # for now, this might place food over where the snake is.
        self.grid_set(random.randint(0, 7), random.randint(0, 7), b'f')

    def grid_get(self, x, y):
        return self.grid[y * self.grid_width + x]

    def grid_set(self, x, y, c):
        self.grid[y * self.grid_width + x] = ord(c)

    def update(self):
        if self.dead:
            return
        self.frames_to_wait -= 1
        if self.frames_to_wait:
            return
        self.frames_to_wait = 15

        if self.direction == b'l':
            self.head_x -= 1
        if self.direction == b'r':
            self.head_x += 1
        if self.direction == b'u':
            self.head_y -= 1
        if self.direction == b'd':
            self.head_y += 1

        if not ((0 <= self.head_y < self.grid_height) and (0 <= self.head_x < self.grid_width)):
            self.dead = True
            return

        if self.grid_get(self.head_x, self.head_y) == ord(b's'):
            self.dead = True
            return

        if self.grow_length:
            self.grow_length -= 1
        else:
            x, y = self.snake.pop(0)
            self.grid_set(x, y, b'\0')

        if self.grid_get(self.head_x, self.head_y) == ord(b'f'):
            self.grow_length += 1
            self.score += 1
            self.add_food()

        self.grid_set(self.head_x, self.head_y, b's')

        self.snake.append((self.head_x, self.head_y))

    def draw(self):

        for x in range(0, self.grid_width):
            for y in range(0, self.grid_height):
                contents = self.grid_get(x, y)
                offset_x = 26 + x * 10
                offset_y = 26 + y * 10
                badge_display.move(offset_x, offset_y)
                if contents == ord(b's'):
                    badge_display.color(badge_display.GREEN)
                elif contents == ord(b'f'):
                    badge_display.color(badge_display.CYAN)
                else:
                    badge_display.color(badge_display.GREY8)
                badge_display.filled_rectangle(10, 10)

        badge_display.move(26, 106)
        badge_display.color(badge_display.WHITE)
        badge_display.write_line(f"score: {self.score}")

        if self.dead:
            badge_display.move(20, 70)
            badge_display.color(badge_display.RED)
            badge_display.write_line("YOU DIED")

        badge_display.swap_buffers()

    def callback(self):
        if self.button_went_down(badge_hw.button.BUTTON_SW):
            return 1
        if self.button_went_down(badge_hw.button.BUTTON_LEFT):
            self.direction = b'l'
        if self.button_went_down(badge_hw.button.BUTTON_RIGHT):
            self.direction = b'r'
        if self.button_went_down(badge_hw.button.BUTTON_UP):
            self.direction = b'u'
        if self.button_went_down(badge_hw.button.BUTTON_DOWN):
            self.direction = b'd'
        self.update()
        self.draw()
