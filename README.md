# ESP32 Server Dashboard

A small hardware dashboard that displays live system statistics from a Linux/Proxmox server on a **2.8" ILI9341 TFT display** driven by an **ESP32-C3 SuperMini**.

The ESP32 receives JSON data via **USB Serial** and renders a modern dashboard with circular gauges for CPU, RAM and disk usage, plus additional information such as CPU temperature and power consumption.

![Dashboard](/Assets/HardwareMonitor-Display.svg)

---

# Features

* Live CPU utilization
* Live RAM utilization
* Live disk utilization
* CPU package temperature
* CPU package power consumption (Intel RAPL)
* USB Serial communication (no WiFi required)
* Smooth circular gauges
* Lightweight Python backend
* Optimized for Proxmox / Ubuntu Server

---

# Hardware

## Components

* ESP32-C3 SuperMini
* 2.8" 240×320 SPI TFT Display (ILI9341)
* USB cable

Optional:

* 3D printed enclosure

---

# Software

Server:

* Proxmox VE
* Python 3
* psutil
* pyserial

ESP:

* Arduino IDE
* ESP32 Arduino Core
* TFT_eSPI
* ArduinoJson

---

# Dashboard Layout

```
+--------------------------------+
|        CPU | RAM | Disk        |
|            |     |             |
|            |     |             |
| Temp                  CPU Watt |
+--------------------------------+
```

---

# Communication

The server sends JSON every few seconds over USB:

```json
{
    "cpu": 12.5,
    "ram": 46.8,
    "disk": 63.2,
    "temp": 42.7,
    "watt": 18.9
}
```

The ESP parses this JSON and updates the display.

---

# Installation

## 1. Flash the ESP

Install the required libraries:

* TFT_eSPI
* ArduinoJson

Upload the ESP firmware.

---

## 2. Install Python dependencies

```bash
sudo apt update

sudo apt install python3 python3-pip

pip install psutil pyserial --break-system-packages
```

(or install `python3-serial` from your package manager)

---

## 3. Connect the ESP

Connect the ESP via USB.

Verify the serial port:

```bash
ls /dev/ttyACM*
```

Example:

```
/dev/ttyACM0
```

---

## 4. Run the server script

```bash
python3 esp_systemdata.py
```

---

## 5. (Recommended) Run as a systemd service

Create:

```
/etc/systemd/system/espdisplay.service
```

Then:

```bash
sudo systemctl daemon-reload
sudo systemctl enable espdisplay
sudo systemctl start espdisplay
```

---

# Power Measurement

The dashboard displays **Intel CPU Package Power** using the Linux RAPL interface:

```
/sys/class/powercap/intel-rapl:0/energy_uj
```

If RAPL is unavailable, the watt display will remain empty.

---

# Repository Structure

```
.
├── ESP32/
│   └── ESP_Dashboard.ino
│
├── Server/
│   └── esp_systemdata.py
│
├── docs/
│   ├── dashboard.jpg
│   └── wiring.png
│
└── README.md
```

---

# Future Ideas

* Network throughput
* SSD temperature
* GPU statistics
* Multiple dashboard pages
* Touchscreen support
* Animated transitions
* Brightness control
* Automatic USB port detection
* Docker/LXC monitoring
* Historical graphs

---

# License

MIT License

Feel free to modify and improve this project.

---

# Acknowledgements

* Espressif
* Bodmer's TFT_eSPI library
* ArduinoJson
* psutil
* The open-source community
