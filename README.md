# ⚙️ Custom C++ Game Engine
Hi! Welcome to Kayson's personal custom engine which based on pure C++ and DirectX11, I used this engine to create all my personal projects and it's very light weight and easy to use. Hope you can enjoy it and give some feedback when you meet some bugs or have some issues.
![image](https://github.com/user-attachments/assets/3afe5e83-d135-45b5-b670-7fc3030e61b6)
![image](https://github.com/user-attachments/assets/f9bbdb91-9884-4a17-96c6-3e585afcf09a)
![image](https://github.com/user-attachments/assets/e25ee31b-d743-40aa-accc-9280eea86e3c)

# 🌟 Key Features
🎨 **DirectX 11 Rendering Pipeline (2D & 3D)**  
Custom renderer supporting textured meshes, instancing, alpha blending, lighting, and post-processing for both 2D sprites and 3D models.

📐 **Math & Physics Utility Library**  
Includes vector/matrix/quaternion math, AABB/collision detection, and physics helpers for 2D/3D transformations and simulations.

🖱️ **Multiple Input Device Support**  
Handles mouse, keyboard, and Xbox controller inputs with customizable mappings and real-time state polling.

🔔 **Event System**  
Built-in event subscription and dispatch framework supporting broadcast events, targeted messages, and custom listeners.

🖥️ **Developer Console**  
Fully functional in-game console for logging, command parsing, runtime variable tweaking, and developer utilities.

🧵 **Multithreaded Job System**  
Simple job queue system for parallel task execution across multiple worker threads, ideal for asset loading, pathfinding, and background computation.

🧪 **Debug Rendering System**  
Visual debugging support with primitives (lines, spheres, text), world-space annotations, and runtime toggles.

🌐 **Networking System (LAN Mode)**  
Local area network system with support for sending/receiving packets, client-server architecture, and message serialization.

🔊 **2D & 3D Audio System (FMOD)**  
Integration with FMOD Studio for spatial audio, environmental reverb, 3D sound emitters, and streaming background music.

📦 **File & Data I/O Support**  
XML parsing (TinyXML2), OBJ model loading, and utilities for reading/writing both binary and plaintext files.

# How to use
### 🔧 Prerequisites

Make sure the following are installed:

- ✅ Windows 10 / 11  
- ✅ Visual Studio 2022 (with *Desktop development with C++*)  
- ✅ Windows SDK 10.0+  
- ✅ DirectX 11 Runtime  
- ✅ FMOD Studio API

---

### 🚀 Build & Run Instructions

1. **Clone the repository:**

   ```bash
   git clone https://github.com/YOUR_USERNAME/YOUR_ENGINE_REPO.git
   cd YOUR_ENGINE_REPO
2. **Open in Visual Studio:**  
Add Engine.project file to your solution using Visual Studio.
Set the startup project to Game (if available).
Edit the solution's include file path to make sure include the Code foleder of the engine.
Select the x64 configuration and choose either Debug or Release.

4. **Run the Engine:**  
Press F5 to build and run.
