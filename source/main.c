#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COMMAND_LEN 200

#include "flash_storage.h"
#include "usb.h"
#include "delay.h"
#include "init.h"

#include "cli.h"
#include "cli_flash.h"

int exit_process(char *args) {
    return -1;
}

CLI_COMMAND exit_command = {
    .name = "exit",
    .help = "Exits command loop.",
    .process = exit_process,
};

int help_process(char *args) {
    puts("Available commands are: ");
    puts("\thelp - Print this message");
    puts("\tflash - Write data to nonvolatile storage");
    puts("\texit - Exit command loop, triggering a reboot");
    return 0;
}

CLI_COMMAND help_command = {
    .name = "help",
    .help = "Prints out the list of commands.",
    .process = help_process,
};

int main() {

    hal_init();

    // Need to ensure USB is connected before reading stdin, or else that will hang
    while (!usb_is_connected()) {
        sleep_ms(5);
    }

    CLI_COMMAND root_commands[] = {
        [0] = help_command, // Add your command name to the printout in help_process!
        [1] = exit_command,
        [2] = flash_command,
        [3] = {}
    };

    cli_run(root_commands);
    puts("Exited CLI");
    hal_deinit();
    hal_reboot();

    return 0;
}