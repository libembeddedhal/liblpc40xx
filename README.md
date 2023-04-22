# libhal-lpc40

[![✅ Checks](https://github.com/libhal/libhal-lpc40/actions/workflows/ci.yml/badge.svg)](https://github.com/libhal/libhal-lpc40/actions/workflows/ci.yml)
[![Coverage](https://libhal.github.io/libhal-lpc40/coverage/coverage.svg)](https://libhal.github.io/libhal-lpc40/coverage/)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/b084e6d5962d49a9afcb275d62cd6586)](https://www.codacy.com/gh/libhal/libhal-lpc40/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=libhal/libhal-lpc40&amp;utm_campaign=Badge_Grade)
[![GitHub stars](https://img.shields.io/github/stars/libhal/libhal-lpc40.svg)](https://github.com/libhal/libhal-lpc40/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/libhal/libhal-lpc40.svg)](https://github.com/libhal/libhal-lpc40/network)
[![GitHub issues](https://img.shields.io/github/issues/libhal/libhal.svg)](https://github.com/libhal/libhal/issues)
[![Latest Version](https://libhal.github.io/libhal-lpc40/latest_version.svg)](https://github.com/libhal/libhal-lpc40/blob/main/conanfile.py)
[![ConanCenter Version](https://repology.org/badge/version-for-repo/conancenter/libhal-lpc40.svg)](https://conan.io/center/libhal-lpc40)

Target library for the lpc40xx series of microcontrollers by NXP conforming to
the libhal interface specification.

## [📚 Software APIs](https://libhal.github.io/libhal-lpc40/api)

## 🧰 Setup

1. [Setup libhal tools](https://libhal.github.io/prerequisites/)
2. Add `libhal-trunk` remote conan server

    ```bash
    conan remote add libhal-trunk https://libhal.jfrog.io/artifactory/api/conan/trunk-conan
    conan config set general.revisions_enabled=True
    ```

    > The "trunk" repository represents the latest packaged code based on
    > github.
    >
    > This command will insert `libhal-trunk` as the first server to check
    > before checking the conan center index. The second command will enable
    > revision mode which is required to use the `libhal-trunk` conan package
    > repository.

## 🏗️ Building Demos

Before building any demos, we have to make the build directory

```bash
cd demos
mkdir build
cd build
```

### Debug Builds

Debug builds are helpful as they reduce the amount of compile time optimizations
in order to make the debugging experience better. This comes at the cost of
slower code and larger binary sizes.

To build with this level:

```
conan install .. -s build_type=Debug --build=missing
cmake .. -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
make -j
```

This will build every project for every MCU family in the LPC40xx family.

### Release Builds

Release builds are harder to debug but are faster and have smaller binary sizes.

To build with this level:

```
conan install .. -s build_type=Release --build=missing
cmake .. -D CMAKE_BUILD_TYPE=Release" -D CMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
make
```

This will build every project for every MCU family in the LPC40xx family.

### Specifying an Application

To specify a specific application, add a target to the build command. Here
are some examples:

```
make lpc4078_adc
make lpc4074_can
make lpc4088_interrupt_pin
```

The naming convention is "linker_script_name" (without the .ld extension) and
application name (without the .cpp extension)

## 💾 Flashing/Programming

There are a few ways to flash an LPC40 series MCU. The recommended methods are
via serial UART and JTAG/SWD.

### Using Serial/UART over nxpprog

`nxpprog` is a script for programming and flashing LPC40 series chips over
serial/UART. Using it will require a USB to serial/uart adaptor.

See the README on [nxpprog](https://github.com/libhal/nxpprog), for details on
how to use NXPPROG.

To install nxpprog:

```
python3 -m pip install -U nxpprog
```

For reference the flash command is:

```
nxpprog --control --binary="app.bin" --device="/dev/tty.usbserial-140"
```

- Replace `app.bin` with the path to your binary.
- Replace `/dev/tty.usbserial-140` with the path to your serial port on your
  machine.
    - Don't know which serial port to use? Use this guide from the MATLAB docs
      to your port for your operating system. Simply ignore that its made for
      Arduino, this guide will work for any serial USB device: [Find Arduino Port on
      Windows, Mac, and
      Linux](https://www.mathworks.com/help/supportpkg/arduinoio/ug/find-arduino-port-on-windows-mac-and-linux.html)

### Using JTAG/SWD over PyOCD

`PyOCD` is a debugging interface for programming and also debugging ARM Cortex M
processor devices over JTAG and SWD.

This will require a JTAG or SWD debugger. The recommended debugger for the
LPC40 series of devices is the STLink v2 (cheap variants can be found on
Amazon).

Installation steps can be found here: https://pyocd.io/docs/installing

For reference the flashing command is:

```
pyocd flash lpc4078_blinker.elf.bin --target lpc4088
```

Note that target `lpc4088` works for all lpc40 series microcontrollers.

## 📦 Adding `libhal-lpc40` to your project

### `conanfile.txt`

Add `libhal-lpc40` to your `conanfile.txt`:

```
[requires]
libhal-lpc40/0.3.5
```

Replace `0.3.5` with the which ever version you prefer (latest version is
recommended). See the [libhal-lpc40
package](https://libhal.jfrog.io/ui/packages/conan:%2F%2Fliblpc40xx)

Add the following tools to your `[tool_requires]` section:

```
[tool_requires]
gnu-arm-embedded-toolchain/11.3.0
cmake-arm-embedded/0.1.1
```

- `gnu-arm-embedded-toolchain/11.3.0`: The ARM embedded cross compiler for
  compiling the source code.
- `cmake-arm-embedded/0.1.1`: Provides the toolchain cmake files which know how
  to use the ARM cross compiler

### Using CMake

After your `project()` declaration you can add the following line to find
the libhal-lpc40 library

```cmake
find_package(libhal-lpc40 REQUIRED CONFIG)
```

To use the `libhal-lpc40` with your library you need to add it as a target
link library as shown below:

```cmake
target_link_libraries(${PROJECT_NAME} PRIVATE libhal::lpc4078)
```

In the above cmake directive, change `${PROJECT_NAME}` with the name of your
executable, and change `lpc4078` with the correct microcontroller on your
development board. The following microcontrollers are available:

- lpc4072
- lpc4074
- lpc4076
- lpc4078
- lpc4088

After that, you'll want to run the `arm_cortex_post_build()` function which
comes from the `cmake-arm-toolchain` tool dependency. This function turns the
executable, which is in the ELF format, into a `.hex` and `.bin` formats which
are used for programming the device.

```cmake
arm_cortex_post_build(${PROJECT_NAME})
```

Replace `${PROJECT_NAME}` with your executable name as you did with
`target_link_libraries`.

#### ✨ Special CMake Component Target: `lpc40xx`

This target is used for unit testing and host side development. Unlike the other
components, this one doesn't inject a linker script OR any ARM architecture
flags into the compiler arguments. Those arguments would NOT work on any system
with an operating system and thus need to be removed when performing host side
testing.

### Configuring `libhal.tweaks.hpp`

Set the `platform` configuration variable to the name of the microcontroller
you are using. Example `lpc4078`.

## 🏁 Initializing the device

First step in the program right after main is called is to initialize your RAM:

```C++
hal::cortex_m::initialize_data_section();
```

Next enable the floating point unit:

```C++
// Do NOT enable the FPU for the lpc4074 and lpc4072 microcontrollers as they
// do not have an FPU. Doing so will crash the device.
if constexpr (!hal::is_platform("lpc4074") && !hal::is_platform("lpc4072")) {
  hal::cortex_m::system_control::initialize_floating_point_unit();
}
```

### 🏎️ Setting Clock Speed

To setting the CPU clock speed to the maximum of 120MHz, include the line below,
with the rest of the includes:

```C++
#include <libhal-lpc40/system_controller.hpp>
```

Next run the following command but replace `12.0_MHz` with the crystal
oscillator frequency connected to the microcontroller. This command REQUIRES
that there be a crystal oscillator attached to the microcontroller. Calling
this without the oscillator will cause the device to freeze as it will attempt
to use a clock that does not exist.

```C++
hal::lpc40xx::clock::maximum(12.0_MHz);
```

#### 🕰️ Detailed Clock Tree Control 🟡

Coming soon...

## 🔎 On Chip Software Debugging

### Using PyOCD (✅ RECOMMENDED)

In one terminal:

```
pyocd gdbserver --target=lpc4088 --persist
```

In another terminal:

```
arm-none-eabi-gdb lpc4078_blinker.elf -ex "target remote :3333"
```

- Replace `lpc4078_blinker.elf` with the path to your binary.

### Using OpenOCD 🟡

Coming soon... (its more complicated)

## :busts_in_silhouette: Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for details.

## License

Apache 2.0; see [`LICENSE`](LICENSE) for details.

## Disclaimer

This project is not an official Google project. It is not supported by
Google and Google specifically disclaims all warranties as to its quality,
merchantability, or fitness for a particular purpose.
