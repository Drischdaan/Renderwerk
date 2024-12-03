<a name="readme-top"></a>

<div align="center">
  <a href="https://github.com/Backyard-Studios/Renderwerk">
    <img src="https://raw.githubusercontent.com/Backyard-Studios/Branding/24006b7753330c3193ae399bb792a810304aea37/Logo/Logo.svg" alt="Logo" width="320" height="120">
  </a>
  <h3 align="center">Renderwerk</h3>
  <p>DirectX 12 powered Game/Rendering Engine</p>
  <p align="center">
    <a href="https://github.com/Drischdaan/Research/blob/main/Projects/Renderwerk/README.md"><strong>Explore my Notes »</strong></a>
  </p>
</div>

<div align="center">

[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

</div>

## 🛣️ Roadmap

The roadmap is a rough outline of the features that are planned to be implemented in the engine. The list is not exhaustive and will be updated as the project progresses. The features are not listed in any particular order.

### 🚗 Engine

- [ ] Job System
- [ ] Memory Allocators
- [ ] Memory Tracking (with support for stl structures)
- [ ] Channel based logging using [spdlog](https://github.com/gabime/spdlog)
- [ ] Performance Profiling with [tracy](https://github.com/wolfpld/tracy)
- [ ] DirectX12 Rendering
- [ ] Mesh Shading Pipeline
- [ ] Custom container structures (Vector, Map, etc.)
- [ ] String hashing for fast string comparisons and lookups (Unreal Engines [`FName`](https://dev.epicgames.com/documentation/en-us/unreal-engine/fname-in-unreal-engine) as an example)
- [ ] [Slang](https://github.com/shader-slang/slang) Support

### ✏️ Editor

- [ ] Node based Editors for Materials, Shaders, Renderpasses, etc.
- [ ] Debugging tools

## 🚀 Getting Started

### 📦 Prerequisites

- [Python 3.9 or above](https://www.python.org/downloads/)
- [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) or [Visual Studio Build Tools 2022](https://visualstudio.microsoft.com/downloads/?q=build+tools) with the following components:
  - Desktop development with C++
  - MSVC v143
  - Windows 11 SDK (10.0.26100.0 or above recommended, not tested with older versions)

### 🛠️ Setting up the workspace

After cloning the repository, run the following commands in the root directory (Choose the appropriate scripts for your OS):

```bash
# Windows
.\CLI.bat setup
```

This command will install the necessary dependencies and tools required to build the project. It will also generate the Visual Studio solution file.

### 🧹 Cleaning the workspace

If you encounter any issues with the build system, you can clean the workspace with the following commands:

```bash
# Windows
.\CLI.bat clean
```

This will remove all build artifacts, intermediate files, manually downloaded dependencies and tools. You need to run the setup script again to restore the workspace to a working state.

## 📜 License

See [LICENSE](./LICENSE) for more information.

## 🫱🏽‍🫲🏽 Contact

[![Github][github]][github-url]
[![Discord][discord]][discord-url]
[![Twitter][twitter]][twitter-url]

<div>
    <a href="https://www.buymeacoffee.com/Drischdaan">
      <img src="https://img.buymeacoffee.com/button-api/?text=Buy me a pizza&emoji=🍕&slug=Drischdaan&button_colour=BD5FFF&font_colour=ffffff&font_family=Lato&outline_colour=000000&coffee_colour=FFDD00" />
    </a>
</div>

<!-- Variables -->

[issues-shield]: https://img.shields.io/github/issues/Backyard-Studios/Renderwerk.svg?style=for-the-badge
[issues-url]: https://github.com/Backyard-Studios/Renderwerk/issues
[license-shield]: https://img.shields.io/github/license/Backyard-Studios/Renderwerk.svg?style=for-the-badge
[license-url]: https://github.com/Backyard-Studios/Renderwerk/blob/master/LICENSE.txt

<!-- Socials -->

[github]: https://skillicons.dev/icons?i=github
[github-url]: https://github.com/Drischdaan
[discord]: https://skillicons.dev/icons?i=discord
[discord-url]: https://discord.com/users/244115221776433152
[twitter]: https://skillicons.dev/icons?i=twitter
[twitter-url]: https://twitter.com/Drischdaan
