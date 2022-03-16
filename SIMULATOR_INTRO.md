
This is a basic intro about how to add apps to the HackRVA badge, and how
to do so in such a way that most of your work of compiling, testing, and
debugging can be done on a computer rather than on the badge which is quite a
bit more convenient.

There is a linux/posix environment
that mimics much of the badge environment which allows you to develop badge
applications on linux without having to flash the code to the badge to check
every little thing. When you are reasonably sure that your code is working,
then you can flash it to the badge and check if it works as expected in the
real badge environment.

The emulator should work in the following environments:

* Linux, tested more in Debian-like systems
* macOS
* Windows Subsystem for Linux (you'll need a desktop environment in WSL).

<h2>Step 0: Install the compilers on your system.</h2>

For the badge emulator, you'll need your system's native compiler and
CMake. On an ubuntu-based system, `sudo apt-get install build-essentials cmake`. 
Other systems will differ. You will also need some GTK libs, 
`sudo apt-get install libgtk2.0-dev` at least (again, it will 
differ on systems not based on debian.)

For macOS you will need a CMake, a C compiler (probably the easiest thing 
to do is to intall the Xcode Command Line tools) in addition to
GTK2 (you can get this via homebrew: `brew install gtk+`).

<h2>Step 1: Install badge compiler</h2>

If you are building only for the emulator, you can skip this step. 

To build for the badge, you will need a cross-compiler (i.e. a compiler
that builds for another system.) This is available on the 
[https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads]
(ARM developer site).

Version 9-2020-q2 is recommended, though newer versions likely work fine.
Check the instructions in the package for full installation. At a minimum,
you will need to add the compiler program directory (`bin` inside the download) 
to your system's PATH environment variable.

Some operating systems may have packaged versions that you can install. 


<h2>Step 2: Clone the linux badge environment:</h2>

```
	git clone git@github.com:HackRVA/badge2022.git
```

or, if you don't have a github account, you can download the ZIP file
from this page: https://github.com/HackRVA/badge2022 (click the green
"Code" button and select the "Download Zip" option, then unzip the file.

<h2>Step 3: Write your code:</h2>

```
	cd badge2022/source/

	(hack on your code.  Add your app under the badge_apps directory)

	See source/apps/sample_app.c for an example and source/apps/badge-app-template.c
	for a template that you can copy and customize to make your badge app. There are
	instructions at the top of badge-app-template.c that explain how to do this, and
	most of the details explained below are already done for you in this file.

	Modify source/apps/CMakeLists.txt to add your badge app following the other
	examples there.
```

Instead of `myapp_callback`, use whatever function name you used
for your app's callback entry point name. Apps will be called at a 30 hz rate.

Check TODO ADD REFERENCE
and header files to see what functions you may call.  You may need
to include some header files to access some functions. You can include `hal.h`
(`source/hal/hal.h`) to include functions that access low-level/hardware 
functionality.

You will want to reference `framebuffer.h` (`source/display/framebuffer.h`) to 
write things to the display.

In general, headers in the `source/core`, `source/display`, and `source/hal`
folders contain potentially useful functionality.

Generally, C standard library headers are safe to use as well when it comes to
running on the badge. At least with the current badge software, though, dynamic
memory usage (`malloc` and `free`) should be avoided.

--

See [BADGE-APP-HOWTO.md](https://github.com/HackRVA/badge2020/blob/master/BADGE-APP-HOWTO.md) for
more information about the functions and variables your badge app might use.

Look also in include/fb.h and linux/linuxcompat.h as well as other badge apps
to get an idea of what functions you may call from your badge app to draw
or print messages on the screen, receive user input, etc. Unfortunately,
the header files and documentation for that type of thing is not currently
very good. Feel free to improve the situation.

<h2>Step 5: Build and run your app on linux:</h2>

```bash
you@yourmachine ~/github/badge2022/ $ ./run_cmake_sim.sh
you@yourmachine ~/github/badge2022/ $ cd build_sim
you@yourmachine ~/github/badge2022/build_sim $ make
you@yourmachine ~/github/badge2020/build_sim $ ./source/badge2022_c
```

You should see a program start, and a window come up showing the badge menus/interface.

Your app should be in the apps/games menu where you added it.

<h2>Step 6. Build it for the badge...</h2>

```bash
you@yourmachine ~/github/badge2022/ $ ./run_cmake.sh
you@yourmachine ~/github/badge2022/ $ cd build
you@yourmachine ~/github/badge2022/build_sim $ make
```

<h2>Step 7.  Flash the badge</h2>

Hold the button on the Pico as you plug it into the USB port. The LEDs on the badge
should turn very slightly on.

Additionally, you should see a PICO flash drive show up on your computer. Copy the file
`build/source/badge2022_c.uf2` to the drive. This will flash the firmware for the badge.

At this point your program should be accessible via the main menu.
