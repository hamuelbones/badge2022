
import badge_c_apps
from app import BadgeApp


class BadgeCApp(BadgeApp):
    auto_probe_inputs = False


class BlinkenlightsApp(BadgeCApp):
    name = "Blinkenlights"

    def callback(self):
        return badge_c_apps.blinkenlights()


class ConductorApp(BadgeCApp):
    name = "Conductor"
    auto_probe_inputs = False

    def callback(self):
        return badge_c_apps.conductor()


class LunarRescueApp(BadgeCApp):
    name = "Lunar Rescue"

    def callback(self):
        return badge_c_apps.lunar_rescue()


class BadgeMonstersApp(BadgeCApp):
    name = "Badge Monsters"

    def callback(self):
        return badge_c_apps.badge_monsters()


class SpaceTripperApp(BadgeCApp):
    name = "Space Tripper"

    def callback(self):
        return badge_c_apps.space_tripper()


class SmashoutApp(BadgeCApp):
    name = "Smashout"

    def callback(self):
        return badge_c_apps.smashout()


class HackingSimulatorApp(BadgeCApp):
    name = "Hacking Simulator"

    def callback(self):
        return badge_c_apps.hacking_simulator()


class CubeApp(BadgeCApp):
    name = "SpinningCube"

    def callback(self):
        return badge_c_apps.cube()


class GameOfLifeApp(BadgeCApp):
    name = "Game of Life"

    def callback(self):
        return badge_c_apps.game_of_life()

