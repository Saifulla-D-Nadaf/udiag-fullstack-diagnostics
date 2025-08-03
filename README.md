# µDiag – Full-Stack Embedded + RTL Fault Diagnostics System

![License](https://img.shields.io/badge/license-MIT-blue)
![Platform](https://img.shields.io/badge/platform-FPGA%20%7C%20ESP32%20%7C%20Cloud-orange)

A complete diagnostic system for **automotive, robotics, and industrial hardware**, combining:
- 📡 RTL simulation (Verilog)
- 🔌 Embedded fault logging (ESP32/STM32)
- 🌐 Real-time dashboard (React/Firebase)

Detects sensor faults (temp, voltage) at the **hardware level** and streams alerts to a **live web dashboard**.

---

## 🧩 Features

| Layer       | Tech Used |
|------------|----------|
| **RTL**    | Verilog, LFSR, FSM, AXI/UART |
| **FPGA Sim** | Verilator, GTKWave, SystemVerilog (Assertions) |
| **Embedded** | ESP32, UART, OLED, WiFi, Firebase |
| **Cloud**  | React.js, Node.js, MQTT/REST, Firebase |

---

## 🖼️ Dashboard Preview

![Dashboard Screenshot](docs/images/dashboard-preview.png)

> Live fault monitoring with timestamps and alerts.

---

## 📦 Project Structure
