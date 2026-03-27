# 🌾 Grain Weevil Robot

> **An autonomous IoT-powered grain bin safety and management robot** — eliminating human entry into hazardous confined spaces through embedded systems, real-time sensor fusion, and automated control logic.

---

## 🖼️ Prototype Images

<p align="center">
  <img src="./docs/images/prototype_front.png" width="45%" alt="Grain Weevil Robot - Front View"/>
  &nbsp;&nbsp;
  <img src="./docs/images/prototype_side.png" width="45%" alt="Grain Weevil Robot - Side View"/>
</p>
<p align="center">
  <em>Left: Front view of the robot chassis &nbsp;|&nbsp; Right: Side/angled view showing structural frame</em>
</p>

---

## 📖 Overview

The **Grain Weevil Robot** is an embedded IoT platform designed for autonomous grain bin operations. Built on an ESP32 microcontroller, it continuously monitors environmental conditions, navigates granular terrain using custom Archimedes screw wheels, and executes safety protocols — all without human intervention.

| Attribute | Detail |
|---|---|
| **Platform** | ESP32 (Wi-Fi + Bluetooth capable) |
| **Sensing** | HC-SR04 Ultrasonic Array |
| **Actuation** | L298N Dual H-Bridge + 12 kg·cm DC Motors |
| **Navigation** | Archimedes Screw Wheels (granular media optimized) |
| **Operation Mode** | Fully Autonomous (no remote control required) |
| **Target Environment** | Grain bins / Confined agricultural storage |

---

## 🧩 System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    ESP32 MAIN CONTROLLER                │
│                                                         │
│  ┌──────────────┐    ┌──────────────┐    ┌───────────┐ │
│  │  Sensor      │    │  Navigation  │    │  Safety   │ │
│  │  Manager     │───▶│  Engine      │───▶│  Monitor  │ │
│  └──────────────┘    └──────────────┘    └───────────┘ │
│         │                   │                  │        │
│         ▼                   ▼                  ▼        │
│  ┌──────────────┐    ┌──────────────┐    ┌───────────┐ │
│  │ HC-SR04 x2   │    │ Motor Driver │    │ Battery   │ │
│  │ Ultrasonic   │    │ L298N H-Brdg │    │ ADC Mon.  │ │
│  └──────────────┘    └──────────────┘    └───────────┘ │
└─────────────────────────────────────────────────────────┘
         │                   │                  │
         ▼                   ▼                  ▼
   Distance Data       PWM Motor Ctrl      Low-Volt Alert
   Obstacle Map        Torque Control      Buzzer/Ascent
```

---

## ⚙️ Hardware Components

### Control & Processing
| Component | Role | Specs |
|---|---|---|
| **ESP32** | Main MCU | Dual-core 240 MHz, Wi-Fi/BT, 34 GPIO |
| **L298N H-Bridge** | Motor Driver | Dual channel, 2A per channel |
| **Voltage Divider** | Battery Monitor | ADC-based, threshold detection |

### Sensing
| Sensor | Role | Range |
|---|---|---|
| **HC-SR04 (x2)** | Obstacle Detection | 2 cm – 400 cm |
| **Current Monitor** | Entrapment Detection | Motor load tracking |

### Actuation
| Component | Role | Specs |
|---|---|---|
| **DC Motors (x4)** | Wheel Drive | 12 kg·cm torque, 500 RPM |
| **Archimedes Screw Wheels** | Granular Navigation | Custom-designed for grain media |
| **Buzzer** | Low Battery Alert | Audible surface ascent trigger |

---

## 🔄 Firmware Logic & Control Flow

```
BOOT
 └──▶ Self-Diagnostics & Sensor Calibration
       └──▶ Main Loop
             ├──▶ [SENSE]   Poll HC-SR04 sensors (both sides)
             ├──▶ [DECIDE]  Evaluate obstacle / edge conditions
             │               ├── Clear path → Continue forward
             │               ├── Obstacle detected → Calculate optimal turn
             │               └── Edge detected → Execute boundary avoidance
             ├──▶ [ACT]     Send PWM commands via L298N
             ├──▶ [MONITOR] Check motor current draw
             │               └── High current (entrapment) → Recovery protocol
             └──▶ [BATTERY] Read ADC voltage level
                             └── Below threshold → Surface ascent + Alert
```

### Key Software Modules

| Module | Description |
|---|---|
| `sensor_manager` | Polls ultrasonic sensors, returns distance readings |
| `nav_engine` | Implements straight-line + obstacle avoidance logic |
| `entrapment_monitor` | Tracks motor current; triggers recovery on anomaly |
| `battery_watchdog` | ADC-based voltage check; initiates ascent + buzzer |
| `motor_controller` | Abstracts PWM generation for L298N channels |

---

## 🌐 IoT Capabilities & Connectivity

The ESP32's onboard Wi-Fi and Bluetooth enable future cloud and edge integration:

```
Robot (ESP32)
     │
     ├──▶ Wi-Fi (802.11 b/g/n)
     │         └──▶ MQTT Broker (e.g., HiveMQ / Mosquitto)
     │                   └──▶ IoT Dashboard (Node-RED / Grafana)
     │                               ├── Live sensor telemetry
     │                               ├── Battery status
     │                               └── Alert notifications
     │
     └──▶ Bluetooth (BLE)
               └──▶ Mobile App (future: config & diagnostics)
```

**Planned telemetry data points:**
- Obstacle event log (timestamp + distance)
- Entrapment recovery events
- Battery voltage history
- Motor current readings
- Operational uptime

---

## 🚀 Getting Started

### Prerequisites

```bash
# Tools Required
- Arduino IDE 2.x  OR  PlatformIO (VSCode)
- ESP32 Board Package (Espressif)
- Required Libraries:
    - NewPing          (ultrasonic sensor abstraction)
    - ESP32 core       (motor PWM / ADC)
```

### Installation

```bash
# 1. Clone the repository
git clone https://github.com/your-org/grain-weevil-robot.git
cd grain-weevil-robot

# 2. Open in Arduino IDE or PlatformIO
#    File → Open → src/main.ino

# 3. Install dependencies
#    Arduino IDE: Sketch → Include Library → Manage Libraries
#    Search and install: NewPing

# 4. Configure board settings
#    Tools → Board → ESP32 Arduino → ESP32 Dev Module
#    Tools → Upload Speed → 115200

# 5. Flash firmware
#    Connect ESP32 via USB → Click Upload
```

### Pin Configuration

```cpp
// Motor Driver (L298N)
#define MOTOR_A_IN1   25
#define MOTOR_A_IN2   26
#define MOTOR_B_IN1   27
#define MOTOR_B_IN2   14
#define ENA_PIN       32   // PWM speed control - Motor A
#define ENB_PIN       33   // PWM speed control - Motor B

// Ultrasonic Sensors (HC-SR04)
#define TRIG_FRONT    18
#define ECHO_FRONT    19
#define TRIG_SIDE     22
#define ECHO_SIDE     23

// Miscellaneous
#define BUZZER_PIN    15
#define BATTERY_ADC   34   // Analog read for voltage monitoring
```

---

## 📊 Performance Metrics

| Metric | Result |
|---|---|
| Autonomous Operation | ✅ Continuous, no human intervention |
| Obstacle Detection Range | 2 – 400 cm (HC-SR04) |
| Entrapment Recovery | ✅ Automated uni-directional reverse sequence |
| Battery Low Alert | ✅ Audible buzzer + surface ascent triggered |
| Edge / Boundary Detection | ✅ Software-defined perimeter via sensor thresholds |
| Human Entry Required | ❌ None |

---

## 🔭 Future Scope

### Sensor Expansion
- [ ] **DHT22 / SHT31** — Temperature & humidity monitoring (grain health)
- [ ] **NIR Spectroscopy Module** — Grain quality & contamination detection
- [ ] **IMU (MPU-6050)** — Tilt/orientation sensing for terrain feedback

### Software & AI Enhancements
- [ ] **MQTT telemetry** — Real-time cloud dashboard (Node-RED / AWS IoT)
- [ ] **Computer Vision (ESP32-CAM)** — Automated pest detection via YOLOv8
- [ ] **Reinforcement Learning** — Navigation strategy optimization
- [ ] **OTA Firmware Updates** — Remote update via Wi-Fi

### Multi-Agent / Swarm
- [ ] **Multi-robot coordination** — Distributed coverage algorithms
- [ ] **Swarm intelligence** — Optimized area coverage for large-scale bins

### Mobile & Web Interface
- [ ] **BLE companion app** — Robot config & live diagnostics
- [ ] **Web dashboard** — Historical sensor data, event logs, alerts

---

## 📁 Project Structure

```
grain-weevil-robot/
├── src/
│   ├── main.ino                # Entry point, main loop
│   ├── sensor_manager.h/.cpp   # Ultrasonic sensor abstraction
│   ├── nav_engine.h/.cpp       # Navigation & obstacle avoidance
│   ├── motor_controller.h/.cpp # L298N PWM control
│   ├── entrapment_monitor.h    # Current-based entrapment detection
│   └── battery_watchdog.h      # ADC voltage monitoring
├── docs/
│   ├── schematic.pdf           # Circuit diagram
│   └── architecture.png        # System block diagram
├── hardware/
│   └── bom.csv                 # Bill of Materials
├── README.md
└── LICENSE
```

---

## 📚 References

1. Zarboubi et al., *"Smart Pest Control in Grain Warehouses: YOLOv8-powered IoT Robot Car"*, IEEE IoT Journal, 2024
2. Jin et al., *"Influence of screw blades on screw-drive granary robot performance"*, Robotics, 2023
3. Singh & Yogi, *"IoT-Based Devices/Robots in Agriculture 4.0"*, Springer, 2022
4. Panda et al., *"Robotics for material handling in food plants"*, IEEE, 2024
5. Albiero et al., *"Swarm robots in mechanized agricultural operations"*, IEEE, 2024
6. Nagrale et al., *"Modern Food Grain Storage with RFID & IoT"*, IEEE, 2024

---

## 👥 Authors

**Department of Electronics & Communication Engineering**  
Indore Institute of Science & Technology (IIST)

| Name | Contact |
|---|---|
| Ayush Malvi | — |
| Harshita Mahant | — |
| Abhishek Parmar | Abhi.pa.9301@gmail.com |
| Chirag Jain | — |

---

## 📄 License

This project is for academic and research purposes.  
© 2025 Malvi, Mahant, Parmar, Jain — IIST. All rights reserved.
