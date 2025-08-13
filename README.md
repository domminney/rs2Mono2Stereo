# rs2Mono2Stereo

Licensed under the [GNU General Public License v3.0](LICENSE).

You are free to use, modify, and share this software under the same license.
Any distributed modified versions must also be open-sourced under GPLv3.

[![Made with JUCE](https://img.shields.io/badge/Made%20with-JUCE-green)](https://juce.com)

A JUCE-based **VST3/AU plugin** that turns mono audio sources into natural, wide stereo — without breaking mono compatibility.

---

## ✨ Features
- **Stereo from Mono** – Create a wide, spacious sound from mono sources.
- **Mono Safe** – Maintains perfect mono compatibility.
- **Cross-Platform** – Works in any modern VST3/AU-compatible DAW.
- **Lightweight** – Low CPU usage for big sessions.

---

## 🛠 Requirements
- [JUCE](https://juce.com/get-juce) (latest version)
- C++17 or newer compiler
- CMake (if building from command line)
- DAW that supports VST3 or AU plugins

---

## 📦 Installation

### 1. Clone the repository
    git clone https://github.com/domminney/rs2Mono2Stereo.git
    cd rs2Mono2Stereo

### 2. Install JUCE
Download JUCE from [juce.com/get-juce](https://juce.com/get-juce) and install it.

### 3. Build the plugin

**Using Projucer**
1. Open `rs2Mono2Stereo.jucer` in Projucer.
2. Set the JUCE path.
3. Choose your export target (Xcode, Visual Studio, etc.).
4. Build.

**Using CMake**
    cmake -Bbuild
    cmake --build build

---

## 🎚 Usage
1. Insert `rs2Mono2Stereo` on a mono track in your DAW.
2. Adjust stereo controls to taste.
3. Enjoy a wider sound that stays mono-compatible.

---

## 📜 License
Licensed under the [MIT License](LICENSE).  
You’re free to use, modify, and distribute — just credit the original author.

---

## 🤝 Contributing
Pull requests welcome!  
1. Fork the repo  
2. Create a branch (`feature/my-feature`)  
3. Commit changes  
4. Open a Pull Request  

---

## 🐞 Issues
Found a bug or want a feature? [Open an issue](https://github.com/domminney/rs2Mono2Stereo/issues).

---

## 💡 About JUCE
JUCE is an open-source C++ framework for cross-platform audio apps and plugins.  
Learn more at [juce.com](https://juce.com).
