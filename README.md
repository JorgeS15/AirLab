# AirLab - Pneumatic Test Bench System

**Version: 1.5.0**

AirLab is a pneumatic test bench system for testing air valves and vacuum systems. Built on Raspberry Pi with Beckhoff modules via EtherCAT, it provides real-time monitoring and control of pneumatic components with automatic calibration.

## Quick Start

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
Main Dashboard:  http://<your-pi-ip>:5000/
Debug Page:      http://<your-pi-ip>:5000/debug
```

## Features

- **Real-time Pressure Display** - 4-channel vacuum/pressure monitoring at 2 Hz
- **Unit Toggle** - Switch between mbar and bar with one click
- **Automatic Calibration** - Zero all channels at atmospheric pressure
- **Moving Average Filter** - 10-sample rolling average for stable readings
- **Clean Interface** - Simple, focused main display with detailed debug page
- **Persistent Calibration** - Offsets saved and restored across restarts
- **Bilingual** - English and Portuguese language support

## System Overview

**Hardware Platform:** Raspberry Pi with Beckhoff I/O modules
**Communication:** EtherCAT industrial protocol
**Channels:** 4x vacuum/pressure sensors
**Range:** 0 to -1000 mbar (0 to -1 bar)
**Update Rate:** 2 Hz (500ms)

## Calibration

1. **Expose all suction cups to atmospheric pressure** (no vacuum)
2. **Click "Calibrate"** on the main page
3. All channels will now read approximately 0 mbar

## Installation

```bash
# Install Flask
pip3 install flask

# Create templates directory
mkdir -p templates static

# Start the application
python3 app.py
```

## Files

- **app.py** - Flask web server
- **templates/index.html** - Main dashboard
- **templates/debug.html** - Debug/technical page
- **static/style.css** - Styling
- **static/translations.js** - Language files
- **ethercat_app** - EtherCAT master binary

## API Endpoints

- `GET /api/data` - Get current readings
- `POST /api/calibrate` - Calibrate all channels
- `POST /api/reset_calibration` - Reset offsets
- `POST /api/shutdown` - Shutdown Raspberry Pi
- `POST /api/reboot` - Reboot Raspberry Pi

## Troubleshooting

**Dashboard shows "Failed to read data"**
- Make sure `ethercat_app` is running
- Check that `/tmp/ethercat_data.txt` exists
- Verify EtherCAT master: `ethercat slaves`

**Readings seem incorrect**
- Calibrate at atmospheric pressure (no vacuum)
- Try "Reset Calibration" and recalibrate

## Version History

### v1.5.0 (Current)
- Reorganized button layout - moved to bottom of vacuum values
- Moved Reset button to debug page for cleaner main interface
- Simplified documentation structure
- Enhanced version tracking

### v1.4.0
- Renamed to AirLab - Professional branding
- Added mbar/bar unit toggle button
- Ultra-clean main interface with detailed debug page

### v1.3.0
- Simplified main display
- Added separate debug page

### v1.2.0
- Added 10-sample moving average filter
- Reduced display jitter

### v1.1.0
- Changed to 2Hz refresh rate
- Updated channel labels

### v1.0.0
- Initial release with auto-calibration

## License

Free to use and modify for your pneumatic testing needs.
