# rs2Mono2Stereo

[![License: GPLv3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENCE)
[![Made with JUCE](https://img.shields.io/badge/Made%20with-JUCE-green)](https://juce.com)

A JUCE-based **VST3/AU plugin** technique that turns mono audio sources into natural, wide stereo — **without breaking mono compatibility**.

> **Note:** This repository provides **source files only** (no Projucer/CMake project).  
> Create your own JUCE Audio Plug-In project, then **copy these files into it**.

---

## ✨ Features
- **Stereo from Mono** – Creates a wide, spacious image from mono sources.
- **Mono Safe** – Phase-aware approach that folds down cleanly.
- **DAW Agnostic** – Works in any modern VST3/AU host once built in your project.
- **Lightweight** – Low CPU overhead.

---

## 🛠 Requirements
- **JUCE** (6.x or newer): <https://juce.com/get-juce>  
- **C++17** (or newer) toolchain  
- **Your own JUCE Audio Plug-In project** (Projucer or CMake)  
- A DAW that supports **VST3** (Win/macOS) or **Audio Unit** (macOS)

---

## 🚀 Quick Start (Integrate into *your* JUCE project)

You have two common paths:

### Option A — Projucer project
1. Open **Projucer** → **New Project** → **Audio Plug-In**.
2. In **Project Settings**:
   - Uncheck *Plugin is a Synth* (this is an audio effect).
   - Ensure plugin formats you want are enabled (VST3 / AU).
   - Set **Plugin Channel Configurations** to support **mono-in → stereo-out** (e.g. add `{"1,2"}`; optionally also `{"2,2"}`).
3. In the Projucer **Files** panel, right-click **Source** → **Add Existing Files…**  
   Add the **four source/header files** from this repository.
4. Save and open in your IDE (Xcode / Visual Studio) and **Build**.
5. Point your DAW at the built plugin or copy it to your system plugin folder.

### Option B — CMake-based JUCE project
1. Create a new CMake JUCE Audio Plug-In project (see the JUCE examples/templates).
2. Copy the **four source/header files** from this repository into your project’s `Source/` (or a suitable folder).
3. Add them to your target, e.g.:
   
       target_sources(${PROJECT_NAME} PRIVATE
         Source/MainEditor.cpp
         Source/MainProcessor.cpp
         Source/rs2Mono2Stereo.cpp
         Source/rs2Mono2Stereo.h
       )
   
   *(Adjust names/paths to match the actual files in this repo.)*
4. Ensure your plugin supports **mono-in → stereo-out**. Common approaches:
   - Define preferred channel layouts via `JucePlugin_PreferredChannelConfigurations` (legacy define), **or**
   - Configure **bus layouts** in your `AudioProcessor` code to accept 1-in/2-out (and optionally 2-in/2-out).
5. Configure & build, then load the plugin in your DAW.

---

## 🎚 Usage
1. Insert **rs2Mono2Stereo** on a **mono** track.
2. Adjust its controls (width/voicing) to taste.
3. Enjoy a wider image that remains **mono-compatible** on fold-down.

---

## ❓ FAQ

**Why isn’t there a full JUCE project here?**  
This repo is intentionally minimal: it provides the **core processing code** so you can drop it into **your existing JUCE setup** (Projucer or CMake) without imposing a particular project structure.

**Where do I put the files?**  
Add them to your JUCE project’s `Source/` folder (or similar) and include/compile them as part of your plug-in target.

**Do you provide binaries?**  
No. Build inside your own JUCE project for your platform(s) and preferred formats.

---

## 🤝 Contributing
Issues and PRs are welcome. By contributing, you agree your changes are licensed under **GPLv3**.

- Fork the repo  
- Create a branch (`feature/my-feature`)  
- Commit changes  
- Open a Pull Request

---

## 🐞 Issues
Bug reports and feature requests:  
<https://github.com/domminney/rs2Mono2Stereo/issues>

---

## 📜 License
Licensed under the **GNU General Public License v3.0**. See **[LICENCE](LICENCE)** for details.

---

## 💡 About JUCE
JUCE is a cross-platform C++ framework for audio apps and plugins: <https://juce.com>
