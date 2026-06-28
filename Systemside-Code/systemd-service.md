# ESP32 Display Data Service – Setup Guide

## 1. Move the Python script to `/usr/local/bin/`

(e.g. name it the way we used it last time)

```bash
mv /root/esp_systemdata.py /usr/local/bin/esp_systemdata.py
chmod +x /usr/local/bin/esp_systemdata.py
```

## 2. Create the systemd service

Create the file:

```bash
nano /etc/systemd/system/espdisplay.service
```

Paste in the following content:

```ini
[Unit]
Description=ESP32 Display Data Service
After=multi-user.target

[Service]
ExecStart=/usr/bin/python3 /usr/local/bin/esp_systemdata.py
Restart=always
RestartSec=3
User=root
# Important so that USB devices don't block:
Environment="PYTHONUNBUFFERED=1"

[Install]
WantedBy=multi-user.target
```

Save → close.

## 3. Enable + start

```bash
systemctl daemon-reload
systemctl enable espdisplay.service
systemctl start espdisplay.service
```

## 4. Check the status

```bash
systemctl status espdisplay.service
```

→ Should show **"active (running)"**.

## 5. View the logs

```bash
journalctl -u espdisplay.service -f
```