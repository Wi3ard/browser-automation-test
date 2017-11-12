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
