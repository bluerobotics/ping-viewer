# Build instructions

The following topic will explain how to get *Ping-Viewer* source code and how to build it.

## Daily Builds

If you just want to test (and not debug) a recent build of *Ping-Viewer* you can use the [Continuous Build](https://github.com/bluerobotics/ping-viewer/releases/tag/continuous). Versions are provided for all platforms [Windows, Linux and Mac].

## Source Code

Source code for *Ping-Viewer* is kept on GitHub here: https://github.com/bluerobotics/ping-viewer, and it's [licensed under GPLv3](https://github.com/bluerobotics/ping-viewer/blob/master/LICENSE.md).

To get the source files:
1. Clone the repo (or your fork) including submodules and go inside folder:
   ```
   git clone https://github.com/bluerobotics/ping-viewer --recursive && cd ping-viewer
   ```
2. Update submodules (required each time you pull new source code):
   ```
   git submodule update --init --recursive
   ```

> **Note:** Github source-code zip files cannot be used because these do not contain the appropriate submodule source code. You must use git!


## Build Ping-Viewer

### Native Builds

*Ping-Viewer* builds are supported for Windows, Linux and macOS. This is possible with the [Qt toolkit](http://www.qt.io) as its cross-platform support library.

- **macOS:** v10.14 or higher
- **Linux:** 64 bit, gcc-8 or clang-7 compiler versions or higher
- **Windows:** 10 or higher, Visual Studio 2017 compiler (64 bit)
- **Qt version:** 5.12 **(only)**

> **Tip:** For more information see: [Qt 5 supported platform list](http://doc.qt.io/qt-5/supported-platforms.html).

#### Install Visual Studio 2017 (Windows Only)

The Windows compiler can be found here: [Visual Studio 2017 compiler](https://visualstudio.microsoft.com/vs/older-downloads/) (64 bit).

When installing, you must minimally select all Visual C++ components.

#### Install Qt

1. Download and run the [Qt Online Installer](http://www.qt.io/download-open-source).
2. In the installer *Select Components* dialog choose: 5.12.

   Then install just the following components:
   - **Qt binary version:**
     - **Windows**: *MSVC 2017 64-bit*
     - **MacOS**: *macOS Clang 64-bit*
     - **Linux**: *Desktop gcc 64-bit*
   - Everything else may be necessary except Android tools.

3. Install Additional Packages (Platform Specific)
   - **Windows:** [USB Driver](https://cdn.sparkfun.com/assets/learn_tutorials/7/4/CDM21228_Setup.exe).

#### Building using Qt Creator

1. Launch *Qt Creator* and open the **pingviewer.pro** project.
2. Select the appropriate kit for your needs:
  - **macOS:** Desktop Qt 5.12 Clang 64 bit
  - **Linux:** Desktop Qt 5.12 GCC 64 bit
  - **Windows:** Desktop Qt 5.12 MSVC2017 64 bit
3. Build and run with the **Green Play icon**.

#### Building with terminal

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
