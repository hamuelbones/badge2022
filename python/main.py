import badge_hw
import badge_display

from apps.snake import SnakeApp
from apps.c_apps import BlinkenlightsApp, ConductorApp, LunarRescueApp, BadgeMonstersApp, SpaceTripperApp, \
                        SmashoutApp, HackingSimulatorApp, CubeApp, GameOfLifeApp

from menu import BadgeAppMenu

badge_hw.init_badge.hal_init()
badge_display.init()
badge_display.clear()

badge_apps = [
    SnakeApp,
    BlinkenlightsApp,
    ConductorApp,
    LunarRescueApp,
    BadgeMonstersApp,
    SpaceTripperApp,
    SmashoutApp,
    HackingSimulatorApp,
    CubeApp,
    GameOfLifeApp,
]

menu = BadgeAppMenu("root", badge_apps, True)
menu.run()
