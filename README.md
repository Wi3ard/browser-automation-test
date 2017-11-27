# Qt Browser Automation Test

## Configuring build environment

### Prerequisites

* CMake 3.5.0+
* Qt 5.*

#### Windows specific

* Microsoft Visual Studio 2015

The `QT_ROOT` environment variable should point to the root of the Qt installation, for example:

```
set QT_ROOT=C:\Qt\5.9.2
```

## Build

### Windows

Run `scripts\build-vc14-x64-release.cmd` batch file to build the project. The installation package will be created in the `deploy` directory.

### Ubuntu 16.04 

Install Qt development libraries:

```
sudo add-apt-repository ppa:beineri/opt-qt593-xenial
sudo apt-get update
sudo apt install -y qt59-meta-dbg-full qt59webengine mesa-common-dev
```

Run `scripts/build-gcc-x64-debug` or `scripts/build-gcc-x64-release` shell script to build the project. The application executable will be created in the `build/gcc-x64-debug/modules/App` or `build/gcc-x64-release/modules/App` directory respectively.
