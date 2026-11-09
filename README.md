# Synchronous Generator & Power Plant Control Room Alive (v2.0)

> **Author & Credentials:**  
> **State Laureate Natsagnyam NAMKHAI**  
> *Firmware & Power Systems Engineer | 2015 APO Regional Innovation Champion (awarded alongside Samsung Executive Chairman Boo-Keun Yoon)*  
> 
> **Dedication:**  
> *"Dedicated as a timeless gift of gratitude to my esteemed mentor, Professor Dr. Peter Crossley at UMIST, Manchester—whose rigorous guidance and brilliance bridge the chasm between raw physical reality and engineering truth."*

---

## ⚡ Overview

**PowerPlantControlRoomAlive_V2** is a high-performance, hardware-independent open-source power plant control room simulation and telemetry daemon. Designed from first principles to correct common academic textbook approximations that plague modern engineering education, this tool accurately models synchronous generator dynamics. 

Crucially, **Active Power (MW) adjustments scale armature current magnitude ($I$) directly without altering the phase angle ($\phi$)**, leaving Reactive Power (MVAR / $Q$) solely in charge of phase shifting.

Built with a native lightweight C socket server daemon (`src/telemetry.c`), it serves raw JSON telemetry and the real-time HTML5 capability diagram (`www/pq_diagram.html`) on **port 9000** with zero external server dependencies.

---

## 📂 Repository Structure

```text
PowerPlantControlRoomAlive_V2/
├── src/               # C source code (telemetry daemon, socket server, control logic)
├── include/           # Header files and hardware control definitions
├── www/               # Real-time web UI dashboard (pq_diagram.html / Natska_PPC.html)
├── Makefile           # Build automation configuration
├── Dockerfile         # Alpine-based container configuration for global portability
├── deployment.yaml    # Kubernetes deployment and service manifests
└── README.md          # Project documentation and usage guide
🌍 How Students & Lecturers Can Use It (Open Source Options)
Students, educators, and researchers worldwide can access and run this laboratory tool via multiple flexible methods:

1. Instant Web View (GitHub Pages)
Students can view and interact with the graphical UI instantly in any browser without needing to run a backend server locally:

Live Web Link: https://Natsagnyam.github.io/PowerPlantControlRoomAlive_V2/www/pq_diagram.html

2. Connected to Live Hardware / Tailscale Telemetry Daemon
If connected to the live C telemetry daemon running on the embedded node via Tailscale, the dashboard streams real-time physical simulation data:

Tailscale Secure Endpoint: http://100.114.205.29:9000

3. Run Natively (Linux / Edge Hardware like BeagleBone Black)
Clone the repository:

Bash
git clone [https://github.com/Natsagnyam/PowerPlantControlRoomAlive_V2.git](https://github.com/Natsagnyam/PowerPlantControlRoomAlive_V2.git)
cd PowerPlantControlRoomAlive_V2
Build the C daemon:

Bash
make clean && make
Run the control server:

Bash
sudo ./bbb_gen_ctrl
Access locally at http://localhost:9000 or via Tailscale at http://100.114.205.29:9000.

4. Run via Docker (Cross-Platform Container)
Build the image:

Bash
docker build -t powerplant-ctrl-v2 .
Run the container:

Bash
docker run -d -p 9000:9000 --name powerplant-lab powerplant-ctrl-v2
Open http://localhost:9000 in your browser.

📜 Open Educational Resource (OER) License
Released freely into the global public domain as an educational gift to empower students and eradicate flawed textbook approximations.

“Truth in engineering is found in physical reality, not abstract approximations.”
