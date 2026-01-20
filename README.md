# AirLab - Pneumatic Test Bench System

**Version: 2.1.3**

AirLab is a comprehensive pneumatic test bench automation system for testing air valves and vacuum systems. Built on Raspberry Pi with Beckhoff modules via EtherCAT, it provides real-time monitoring and control of pneumatic components with automatic calibration and valve control capabilities.

## 🚀 Quick Start

### 1. Start the EtherCAT Application
```bash
sudo ./ethercat_app
```

### 2. Start the Web Dashboard
```bash
python3 app.py
```

### 3. Access the Dashboard
```
Main Dashboard:     http://<your-pi-ip>:5000/
Digital Signals:    http://<your-pi-ip>:5000/digital
Debug Page:         http://<your-pi-ip>:5000/debug
```

## ✨ Features

### Monitoring
- **Real-time Pressure Display** - 4-channel vacuum/pressure monitoring at 2 Hz
- **Digital Input Monitoring** - 8 digital input channels from EP2316-003
- **Unit Toggle** - Switch between mbar and bar with one click
- **Moving Average Filter** - 10-sample rolling average for stable readings
- **Automatic Calibration** - Zero all channels at atmospheric pressure
- **Persistent Calibration** - Offsets saved and restored across restarts

### Control (Version 2.x)
- **Digital Output Control** - Control 8 digital outputs via file interface
- **Valve Automation** - Prepare for automated pneumatic test sequences
- **Safety Features** - Proper safe state management for industrial I/O

### Interface
- **Clean Main Display** - Simple, focused vacuum display
- **Digital Signals Page** - Real-time input/output monitoring
- **Debug Page** - Detailed technical diagnostics
- **Bilingual** - English and Portuguese language support

## 🔧 System Overview

**Hardware Platform:** Raspberry Pi 5 with Beckhoff I/O modules
**Communication:** EtherCAT industrial protocol
**EtherCAT Master:** IgH EtherCAT Master from GitLab

### Connected Slaves
- **Slave 0:** Engel 4-channel analog input (0x0ea0c252) - Vacuum sensors
- **Slave 1:** EP2316-003 8-channel digital I/O (0x090c4052) - Valve control

### Specifications
- **Analog Channels:** 4x vacuum/pressure sensors
- **Digital I/O:** 8 inputs, 8 outputs
- **Range:** 0 to -1000 mbar (0 to -1 bar)
- **Update Rate:** 2 Hz (analog), 10 Hz (digital)
- **Cycle Rate:** 100 Hz (10ms EtherCAT cycle)

## 📁 File Interface

Version 2.x introduces a file-based control interface for integration with automation scripts:

### Input Files (Read by ethercat_app)
- **`/tmp/ethercat_outputs.txt`** - Control digital outputs
  - Format: 8 comma-separated values (0 or 1)
  - Example: `1,0,1,0,0,0,0,0` (outputs 1 and 3 ON)

### Output Files (Written by ethercat_app)
- **`/tmp/ethercat_data.txt`** - Vacuum pressure readings (2Hz update)
  - Format: `ch1_mbar,ch2_mbar,ch3_mbar,ch4_mbar`
  - Example: `125.45,230.12,0.00,0.00`

- **`/tmp/ethercat_digital.txt`** - Digital input states (10Hz update)
  - Format: 8 comma-separated values (0 or 1)
  - Example: `0,1,0,0,0,0,0,0` (input 2 active)

### Control Examples
```bash
# Turn on output 1 (e.g., open valve 1)
echo '1,0,0,0,0,0,0,0' | sudo tee /tmp/ethercat_outputs.txt

# Turn on outputs 1, 3, and 5
echo '1,0,1,0,1,0,0,0' | sudo tee /tmp/ethercat_outputs.txt

# Turn off all outputs
echo '0,0,0,0,0,0,0,0' | sudo tee /tmp/ethercat_outputs.txt

# Monitor digital inputs
watch -n 0.1 cat /tmp/ethercat_digital.txt

# Monitor vacuum pressure
watch -n 0.5 cat /tmp/ethercat_data.txt
```

## 🎯 Calibration

1. **Expose all suction cups to atmospheric pressure** (no vacuum)
2. **Click "Calibrate"** on the main page
3. All channels will now read approximately 0 mbar

The calibration offsets are automatically saved to `/tmp/ethercat_calibration.txt` and restored on restart.

## 💻 Installation

### Prerequisites
```bash
# Install Flask
pip3 install flask

# Ensure EtherCAT master is installed
lsmod | grep ec_master
```

### Setup
```bash
# Create required directories
mkdir -p templates static

# Copy files to working directory
# - ethercat_app (compiled C application)
# - app.py (Flask web server)
# - templates/ (HTML templates)
# - static/ (CSS, JavaScript)

# Make ethercat_app executable
chmod +x ethercat_app

# Start the application
python3 app.py
```

### Systemd Service (Optional)
Create `/etc/systemd/system/airlab.service`:
```ini
[Unit]
Description=AirLab EtherCAT Application
After=network.target

[Service]
Type=simple
ExecStart=/home/automation/ethercat_dashboard/ethercat_app
WorkingDirectory=/home/automation/ethercat_dashboard
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
```

Enable and start:
```bash
sudo systemctl daemon-reload
sudo systemctl enable airlab.service
sudo systemctl start airlab.service
```

## 📡 API Endpoints

### Data & Monitoring
- `GET /api/data` - Get current vacuum readings
- `GET /api/digital` - Get digital input states

### Calibration
- `POST /api/calibrate` - Calibrate all channels to zero
- `POST /api/reset_calibration` - Reset calibration offsets

### System Control
- `POST /api/shutdown` - Shutdown Raspberry Pi
- `POST /api/reboot` - Reboot Raspberry Pi

## 🔍 Troubleshooting

### Dashboard shows "Failed to read data"
1. Check that `ethercat_app` is running: `ps aux | grep ethercat_app`
2. Verify data files exist: `ls -l /tmp/ethercat_*.txt`
3. Check EtherCAT master: `sudo ethercat slaves`
4. View logs: `sudo journalctl -u airlab -f` (if using systemd)

### Readings seem incorrect
1. Calibrate at atmospheric pressure (no vacuum applied)
2. Try "Reset Calibration" and recalibrate
3. Check sensor connections and power supply

### Digital outputs not working
1. Verify file format: `cat /tmp/ethercat_outputs.txt`
2. Check for safe state warnings in console
3. Ensure slave 1 reaches OP state at startup
4. Monitor EP2316-003 status LEDs

### EtherCAT communication issues
1. Check network cable connections
2. Verify correct vendor/product codes: `sudo ethercat slaves -v`
3. Ensure only one program accesses the master
4. Check for kernel module: `lsmod | grep ec_master`

## 📋 Files

### Core Application
- **ethercat_app** - EtherCAT master application (C binary)
- **app.py** - Flask web server

### Web Interface
- **templates/index.html** - Main vacuum dashboard
- **templates/digital.html** - Digital I/O monitoring page
- **templates/debug.html** - Debug/technical page
- **static/style.css** - Styling
- **static/translations.js** - Language files

### Data Files (Runtime)
- **/tmp/ethercat_data.txt** - Vacuum readings
- **/tmp/ethercat_digital.txt** - Digital inputs
- **/tmp/ethercat_outputs.txt** - Digital outputs control
- **/tmp/ethercat_calibration.txt** - Calibration offsets

## 🔄 Version History

### v2.0.2 (Current) - 2025-12-05
**Major Update: Unified EtherCAT Control**
- Merged analog monitoring and digital I/O into single application
- Single EtherCAT master handles both slaves simultaneously
- Resolved PDO offset conflicts between slaves
- File-based digital output control interface
- Enhanced state management and error handling
- Foundation for automated test sequences (Version 2.x series)

### v1.6.0 - 2025-11-17
- Added digital signals monitoring page
- Real-time display of 8 digital inputs from EP2316-003

### v1.5.0 - 2025-11-03
- UI reorganization: moved buttons to bottom
- Reset button moved to debug page
- Simplified documentation structure
- Enhanced version tracking

### v1.4.1 - 2025-11-02
- External CSS file
- Bilingual support (English/Portuguese)
- Shutdown and reboot functionality
- Blue gradient theme

### v1.4.0 - 2025-11-01
- Unit toggle button (mbar/bar)
- Renamed to "AirLab - Pneumatic Test Bench"
- Ultra-clean main interface

### v1.3.0 - 2025-10-31
- Separate debug page
- Simplified main display
- Integer pressure display

### v1.2.0 - 2025-10-30
- 10-sample moving average filter
- Automatic buffer clearing

### v1.1.0 - 2025-10-29
- 2Hz refresh rate
- Updated channel labels

### v1.0.0 - 2025-10-28
- Initial release
- Auto-calibration
- 4-channel monitoring

## 🎓 Technical Details

### EtherCAT Configuration
- **Cycle Time:** 10ms (100 Hz)
- **Domain Size:** Calculated based on slave PDO mappings
- **Working Counter:** Monitored for communication integrity

### Analog Input Processing
- 16-bit signed integer values from ADC
- Voltage to pressure conversion with calibration
- Moving average filtering for stability

### Digital I/O Safety
- Safe state management for EP2316-003
- Proper state transition sequence (INIT → PREOP → SAFEOP → OP)
- Automatic output clearing on shutdown

## 🛠️ Development

### Compiling ethercat_app
```bash
gcc -o ethercat_app ethercat_app.c \
    -I/opt/etherlab/include \
    -L/opt/etherlab/lib \
    -lethercat \
    -Wall
```

### Testing
```bash
# Test EtherCAT communication
sudo ethercat slaves -v

# Monitor real-time data
watch -n 0.5 'cat /tmp/ethercat_data.txt /tmp/ethercat_digital.txt'

# Check application status
sudo systemctl status airlab
```

## 📄 License

Free to use and modify for your pneumatic testing needs.

---

**AirLab** - Professional Pneumatic Test Automation
