## This is the script that reads system metrics and sends them as a JSON string over serial to the ESP32.

#!/usr/bin/env python3
import serial
import json
import psutil
import time
import os

SERIAL_PORT = "/dev/ttyACM0"
BAUD = 115200

RAPL_PATH = "/sys/class/powercap/intel-rapl:0/energy_uj"
has_rapl = os.path.exists(RAPL_PATH)


def read_pkg_watt():
    """Reads CPU package power in Watt using RAPL."""
    if not has_rapl:
        return None

    try:
        with open(RAPL_PATH) as f:
            e1 = int(f.read())
        time.sleep(1)
        with open(RAPL_PATH) as f:
            e2 = int(f.read())
        watt = (e2 - e1) / 1_000_000.0
        return round(watt, 1)
    except:
        return None


def read_metrics():
    cpu = psutil.cpu_percent(interval=None)
    ram = psutil.virtual_memory().percent
    disk = psutil.disk_usage("/").percent

    temp = None
    try:
        temps = psutil.sensors_temperatures()
        for key, arr in temps.items():
            for sensor in arr:
                if "cpu" in sensor.label.lower() or "package" in sensor.label.lower():
                    temp = sensor.current
                    break
    except:
        pass

    watt = read_pkg_watt()

    return {
        "cpu": cpu,
        "ram": ram,
        "disk": disk,
        "temp": temp,
        "watt": watt
    }


def main():
    print("Opening serial port:", SERIAL_PORT)
    ser = serial.Serial(SERIAL_PORT, BAUD, timeout=1)

    while True:
        data = read_metrics()
        ser.write((json.dumps(data) + "\n").encode("utf-8"))
        ser.flush()

        time.sleep(3)   # <<--- HIER ist das 3-Sekunden-Intervall


if __name__ == "__main__":
    main()