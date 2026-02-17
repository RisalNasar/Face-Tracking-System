# Face Tracking System for a Directional Audio Beam Speaker

**Final Year Project (2010) · Nanyang Technological University (NTU), Singapore**

- **Full report & institutional repository:** [NTU DR-NTU](https://repository.ntu.edu.sg/handle/10356/39983)

---

## Overview

The Audio Beaming System developed in the Digital Signal Processing Laboratory at NTU provides directional sound. This project extends it by designing and building a **target tracking system** so that directional audio can follow a moving target.

The system uses:

- **Web camera** — live video input  
- **Host PC** — computer vision (face/target detection and tracking)  
- **Motorized pan-tilt** — positions the audio beam  
- **Microcontroller** — drives the pan-tilt from tracking data  

Only the tracked target hears the beamed audio. Applications include gaming, marketing, customer support, museums, and crowd control.

---

## Repository structure

| Path | Description |
|------|-------------|
| `Project_MVC++/` | Windows host application (Visual C++): camera capture, OpenCV face detection + template matching, serial output to microcontroller |
| `Project_MPLAB_IDE/` | Microcontroller firmware (MPLAB IDE): PIC24FJ128GA010, UART, PWM for pan-tilt servos |
| `FYP_Report.pdf` | Final year project report |
| `Installations_Guide.docx` | Setup and installation guide |

---

## Prerequisites

### Host application (PC)

- **OS:** Windows  
- **IDE:** Visual Studio 2005 (or compatible; project is Visual C++ with MFC)  
- **Libraries:** OpenCV **1.x** (C API: `cv`, `cxcore`, `highgui`). Project references `C:\Program Files\OpenCV\...`; adjust paths in the `.vcproj` if your OpenCV is elsewhere.  
- **Camera:** Webcam (default device 0). Resolution set in code (e.g. 1600×900).  
- **Serial:** COM port for host–microcontroller link (default **COM4** in code; change in `main.cpp` if needed).

### Microcontroller

- **IDE:** MPLAB IDE  
- **Chip:** PIC24FJ128GA010  
- **Board:** Explorer 16 (or compatible)  
- **Hardware:** Pan-tilt mechanism driven by PWM (e.g. servos).

---

## Building and running

### Host (face tracking application)

1. Install OpenCV 1.x and ensure paths in `Project_MVC++/Project/Project/Project.vcproj` match your install (include and lib directories).  
2. Open `Project_MVC++/Project/Project.sln` in Visual Studio.  
3. Build the solution (e.g. Debug).  
4. Copy the Haar cascade file used at runtime (e.g. `haarcascade_frontalface_alt_tree.xml`) to the working directory, or ensure the executable runs from the folder that contains it.  
5. Connect the microcontroller (or serial device) on the configured COM port (default COM4).  
6. Run the executable; use the on-screen instructions and keyboard shortcuts for modes (e.g. face detection, template-only tracking, manual pan-tilt, calibration).

Detailed steps (and any tool versions) are in **Installations_Guide.docx**.

### Microcontroller (pan-tilt control)

1. Open `Project_MPLAB_IDE/Project/Project.mcp` in MPLAB IDE.  
2. Build and program the PIC24FJ128GA010 (e.g. via ICD).  
3. Connect UART to the host’s serial port (e.g. 9600 baud) and drive the pan-tilt hardware with the PWM outputs.

---

## Configuration notes

- **COM port:** In `Project_MVC++/Project/Project/main.cpp`, search for `COM4` and change `PortSpecifier` if your serial port is different.  
- **Haar cascade:** The code loads `haarcascade_frontalface_alt_tree.xml` from the current directory; other cascade files are present in the project for experimentation.  
- **Skin detection:** Configurable in code (e.g. HSV, YCbCr, RGB) to improve face detection in different lighting.

---

## Reference and citation

If you use or reference this work, please cite the NTU repository:

- **Face Tracking System for a Directional Audio Beam Speaker.** Nanyang Technological University, 2010.  
  https://repository.ntu.edu.sg/handle/10356/39983

---

## License

This project is released under the **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)** license, consistent with Nanyang Technological University’s DR-NTU repository terms for thesis and FYP submissions. You may share and adapt the work for **non-commercial** use with appropriate attribution. See [LICENSE](LICENSE) for details.
