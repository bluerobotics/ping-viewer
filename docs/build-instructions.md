# Build instructions

The following topic will explain how to get *Ping-Viewer* source code and how to build it.

## Daily Builds

If you just want to test (and not debug) a recent build of *Ping-Viewer* you can use the [Continuous Build](https://github.com/bluerobotics/ping-viewer/releases/tag/continuous). Versions are provided for all platforms [Windows, Linux and Mac].

## Source Code

Source code for *Ping-Viewer* is kept on GitHub here: [https://github.com/bluerobotics/ping-viewer](https://github.com/bluerobotics/ping-viewer), and it's [licensed under GPLv3](https://github.com/bluerobotics/ping-viewer/blob/master/LICENSE.md).

To get the source files:

1. Clone the repo (or your fork) including submodules and go inside folder:
    - `git clone https://github.com/bluerobotics/ping-viewer --recursive && cd ping-viewer`
2. Update submodules (required each time you pull new source code):
    - `git submodule update --init --recursive`

> **Note:** Github source-code zip files cannot be used because these do not contain the appropriate submodule source code. You must use git!


## Build Ping-Viewer

### Native Builds

*Ping-Viewer* builds are supported for Windows, Linux and macOS. This is possible with the [Qt toolkit](https://www.qt.io) as its cross-platform support library.

- **macOS:** v10.14 or higher
- **Linux:** 64 bit, gcc-8 or clang-7 compiler versions or higher
- **Windows:** 10 or higher, Visual Studio 2017 compiler (64 bit)
- **Qt version:** 5.15 **(only)**

> **Tip:** For more information see: [Qt 5 supported platform list](https://doc.qt.io/qt-5/supported-platforms.html).

#### Install Visual Studio 2017 (Windows Only)

The Windows compiler can be found here: [Visual Studio 2017 compiler](https://visualstudio.microsoft.com/vs/older-downloads/) (64 bit).

When installing, you must minimally select all Visual C++ components.

#### Install Qt

1. Download and run the [Qt Online Installer](https://www.qt.io/download-open-source).
2. In the installer *Select Components* dialog choose: 5.15.

   Then install just the following components:
   - **Qt binary version:**
     - **Windows**: *MSVC 2017 64-bit*
     - **MacOS**: *macOS Clang 64-bit*
     - **Linux**: *Desktop gcc 64-bit*
   - Everything else may be necessary except Android tools.

3. Install Additional Packages (Platform Specific)
   - **Windows:** [USB Driver](https://cdn.sparkfun.com/assets/learn_tutorials/7/4/CDM21228_Setup.exe).

#### Building using Qt Creator

1. Launch *Qt Creator* and open the **CMakeLists.txt** project.
2. Select the appropriate kit for your needs:
  - **macOS:** Desktop Qt 5 Clang 64 bit
  - **Linux:** Desktop Qt 5.15 GCC 64 bit
  - **Windows:** Desktop Qt 5.15 MSVC2017 64 bit
3. Build and run with the **Green Play icon**.

#### Building with terminal

Since Ping-Viewer v2.2.0, the project now uses CMake to simplify the build process.
For more information about CMake with Qt based projects, check the [Qt CMake manual](https://doc.qt.io/qt-5/cmake-manual.html);

1. Use CMake to configure the project
    - `cmake -B build -DCMAKE_BUILD_TYPE=Debug`
    > **Note**: CMake may fail to find the necessary `*.cmake` files for configuration, for that, you can set the `Qt5_DIR` variable with the path for the cmake/Qt5 folder.
    > Like: `cmake -B build -DCMAKE_BUILD_TYPE=Debug -DQt5_DIR=$(brew --prefix qt5)/lib/cmake/Qt5`
2. Use CMake again to build it
    - `cmake --build build --parallel --config Debug`
3. Go inside build folder
    - `cd build`
4. And after that you'll be able to run the project
    - `./pingviewer`
    > **Note**: macOS uses a different path for application binaries, the binary will be available under: `./pingviewer.app/Contents/MacOS/pingviewer`

##### QMake build

For Ping-Viewer versions under v2.2.0.

1. Go outside the cloned folder of Ping-Viewer
2. Create the build folder
    - `mkdir ping-viewer-build`
3. Go inside build folder
    - `cd ping-viewer-build`
4. Run qmake command
    - `qmake ../ping-viewer`
5. And build it
    - `make`
6. And after that you'll be able to run the project
    - `./pingviewer`

> **Tip**: You can use `make -jN` where N is the number of threads in your computer to improve the compiling speed.

## Build documentation

1. Follow the [Source Code](#source-code) guide.
2. Checkout to the documentation branch:
    - `git checkout docs`
3. Create and activate a virtual environment:
    - `python -m venv .venv`
    - `source .venv/bin/activate`
4. Install necessary tools:
    - `pip install mkdocs-material`
5. Build the documentation:
    - `mkdocs build`
6. Serve the documentation to view it locally:
    - `mkdocs serve`
    - Copy the serving URL (e.g. `http://127.0.0.1:8000`) to your browser to view
7. Stop serving the docs site:
    - Press `CTRL+C` while the terminal is in focus
8. Deactivate the virtual environment:
    - `deactivate`
