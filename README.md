# UAV Tactical Simulation Engine

This project is a **UAV Ground Control Station Simulation** developed using **C++** and **OpenCV**.

---

## 🛠 Requirements

* **Compiler:** Visual Studio (Desktop Development with C++ workload must be installed)
* **Library:** OpenCV (Version 4.x recommended)

---

## ⚙️ Installation & Configuration

To build and run the project, you need to configure the OpenCV library in Visual Studio.

### 1. Include Directory

Navigate to:

```text
Project Properties
└── C/C++
    └── General
        └── Additional Include Directories
```

Add the path to the OpenCV **include** directory.

---

### 2. Library Directory

Navigate to:

```text
Project Properties
└── Linker
    └── General
        └── Additional Library Directories
```

Add the path to the OpenCV **lib** directory.

---

### 3. Additional Dependencies

Navigate to:

```text
Project Properties
└── Linker
    └── Input
        └── Additional Dependencies
```

Add the corresponding OpenCV library file for your installed version.

Example:

```text
opencv_world412.lib
```

---

### 4. DLL File

Copy the corresponding OpenCV DLL (e.g., `opencv_world412.dll`) into the directory containing the generated executable (`.exe`).

---

## 🚀 Running the Project

1. Open the `UAVsim.sln` solution in Visual Studio.
2. Set the build configuration to **x64**.
3. Click **Local Windows Debugger** to build and launch the simulation.
