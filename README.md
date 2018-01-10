# ASD
**ASD** is the multi-functional platform for C++ development. The main goal of this project is to make development rapid and clear, to avoid unnecessary complexity while maintaining the effectiveness of applications in the end. It is intended to be cross-platform in the future (and supports Windows and Linux now). The focus is made on compile-time programming, so there are lots of templates can be found inside. We are trying to move the complexity from user space to library space in this way.

The **ASD** project is inspired by Boost but is meant to provide more high-level components and tools to deal with graphics, user input, physics, scene management and so on. At this point, the development is at an early stage, so some of these components are slightly crude (or broken due to some big changes), and documentation is WIP. There is a lot of work to be done to make things work as intended, so any help will be __*VERY*__ appreciated.

## Features
### Meta-programming
- Tight integration with the build system (dllimport/dllexport are managed authomatically with the `api(module)` macro, for example)
- Template function selection (*useif*, *skipif* and *selectif* conditions)
- Integer type indices generation to create type-to-object mappings

### Common
- Generalized graphic context management and extensions (partially implemented OpenGL backend and FreeType font extension)
- Mathematical objects and functions (matrices, vectors, geometrical objects; SIMD support)

Libraries in **ASD** are as small and independent as possible. Each library has its own power. There is no need to use all the libraries, feel free to take only libraries which you need!

## Build system
**ASD** is not only its libraries, it is the platform which helps you to manage your projects, their dependencies and resources

Build system is based on __*CMake*__, CMake scripts and some system-dependent scripts. These tools will help you to create new libraries, add sources and dependencies in very simple way. It is really something different.

To make **ASD** project files use:

```
git clone https://gitlab.com/BrightComposite/asd
asd make vs64
```
