# AirLab - Pneumatic Test Bench System

**Current Version: 1.4.0**

AirLab is a pneumatic test bench system for testing air valves and vacuum systems. Built on Raspberry Pi with Beckhoff modules via EtherCAT, it provides real-time monitoring and control of pneumatic components with automatic calibration.

## Version History

### v1.4.0 (Current)
- **Renamed to AirLab** - Professional branding for pneumatic test bench
- Added mbar/bar unit toggle button on main page
- Removed clutter from main display (timestamp, instructions, status info)
- Ultra-clean main interface - only essential controls and readings
- All detailed info moved to debug page

### v1.3.0
- Simplified main display - removed raw values and offsets
- Pressure readings now show as integers (no decimals)
- Added separate debug page with detailed information
- Clean, focused main interface

### v1.2.0
- Added moving average filter (10 samples) to stabilize readings
- Reduced display jitter at 2Hz refresh rate
- Pressure values now show 1 decimal place
- Buffers automatically cleared on calibration/reset

### v1.1.0
- Changed refresh rate to 2Hz (500ms)
- Updated channel labels from "Channel X" to "Vacuum X"
- Removed auto-refresh and vacuum range info from display
- Added version tracking

### v1.0.0
- Initial release with auto-calibration support
- 10Hz refresh rate
- Real-time pressure conversion
- Persistent offset storage

## Features

## System Overview

**AirLab** is designed for pneumatic system testing and validation:

- **Hardware Platform:** Raspberry Pi with Beckhoff I/O modules
- **Communication:** EtherCAT industrial protocol
- **Application:** Test bench for air valves and vacuum systems
- **Monitoring:** 4-channel vacuum/pressure measurement
- **Control:** Air valve and vacuum valve automation

### Architecture
```
┌─────────────────┐
│  Raspberry Pi   │
│    (AirLab)     │
└────────┬────────┘
         │ EtherCAT
         │
┌────────▼────────┐
│ Beckhoff Modules│
│  (Analog I/O)   │
└────────┬────────┘
         │
    ┌────┴────┐
    │ Vacuum  │ Air Valves
    │ Sensors │ & Systems
    └─────────┘
```

## Features

✅ **Automatic Offset Calibration** - Zero all channels at atmospheric pressure
✅ **Unit Toggle** - Switch between mbar and bar with one click
✅ **Moving Average Filter** - 10-sample rolling average for stable, smooth readings
✅ **Ultra-Clean Interface** - Simple, focused main display
✅ **Real-time Pressure Display** - Shows pressure in mbar or bar (-1000 to 0 mbar range)
✅ **Visual Indicators** - Vacuum indicators show when suction is active
✅ **2 Hz Update Rate** - Real-time monitoring at 500ms intervals
✅ **Debug Page** - Detailed technical information available when needed
✅ **Persistent Calibration** - Offsets saved to file and persist across restarts

## Pressure Conversion

The system uses the following conversion:
- **RAW 1000** = 0 mbar (atmospheric pressure)
- **RAW 0** = -1000 mbar (full vacuum)
- **Formula**: `pressure_mbar = (raw_value - offset) - 1000`

## Moving Average Filter

To reduce jitter and provide stable readings at the 2Hz refresh rate, the system implements a **10-sample moving average filter**:

- Each channel maintains a buffer of the last 10 pressure readings
- The displayed value is the average of all samples in the buffer
- This significantly reduces noise and provides smooth, stable readings
- The buffer fills gradually (takes ~5 seconds to fill at 2Hz)
- Buffers are automatically cleared when you calibrate or reset

**Example:**
```
Raw readings: [-502, -498, -503, -501, -499, -500, -502, -498, -501, -500]
Averaged display: -500.4 mbar (smooth and stable)
```

## Installation

1. Make sure you have Flask installed:
```bash
pip3 install flask
```

2. Create the templates directory:
```bash
mkdir -p templates
```

3. Place the files:
   - `app.py` - Main Flask application
   - `templates/index.html` - Web dashboard
   - `ethercat_app.c` - EtherCAT master application (unchanged)

## Usage

### 1. Start the EtherCAT Application

In one terminal:
```bash
sudo ./ethercat_app
```

This will:
- Initialize the EtherCAT master
- Read analog values from all 4 channels
- Write data to `/tmp/ethercat_data.txt` every 100ms

### 2. Start the Web Dashboard

In another terminal:
```bash
python3 app.py
```

### 3. Access the Dashboard

Open your browser and navigate to:
```
http://<your-pi-ip>:5000
```

## Calibration Procedure

### To Zero/Calibrate All Channels:

1. **Expose all suction cups to atmospheric pressure**
   - Turn off vacuum or disconnect all suction cups
   - Ensure no vacuum is being applied to any channel

2. **Click "Calibrate All Channels"** in the web interface
   - This will automatically calculate and save the offset for each channel
   - All channels will now read approximately 0 mbar

3. **Apply vacuum to test**
   - When vacuum is applied, readings will show negative values (e.g., -500 mbar)
   - The 💨 indicator will appear when vacuum exceeds -50 mbar

### To Reset Calibration:

Click "Reset Calibration" to set all offsets back to zero (factory default).

## How Calibration Works

The calibration system works by:

1. **Reading current raw values** when you click "Calibrate All Channels"
2. **Calculating the offset** needed to make current reading = 0 mbar
   - Formula: `offset = raw_value - 1000`
3. **Saving offsets** to `/tmp/vacuum_offsets.json`
4. **Applying offsets** to all future readings

Example:
- If raw value at atmospheric pressure = 950
- Offset will be set to: 950 - 1000 = -50
- Future readings: pressure = (950 - (-50)) - 1000 = 0 mbar ✓

## Display Information

### Main Display (`/`)
Ultra-clean interface showing:
- **Pressure** - Integer values in mbar or bar (toggle with button)
- **💨 Vacuum indicator** - Shows when vacuum > -50 mbar
- **Status** - OK/ERROR/WARNING badge
- **Unit Toggle Button** - Switch between mbar and bar
- **Calibration Buttons** - Quick access to calibrate/reset

**Unit Conversion:**
- mbar: `-647 mbar`
- bar: `-0.647 bar`

### Debug Page (`/debug`)
Detailed technical information:
- **Calibration Instructions**
- **System Status Info**
- **Last Update Timestamp**
- **Pressure (mbar)** - Main averaged reading
- **Raw Value** - Uncalibrated sensor reading
- **Offset** - Current calibration offset
- **Instant Pressure** - Non-averaged pressure value
- **Samples in Avg** - Buffer fill level (x/10)

## File Locations

- **Data file**: `/tmp/ethercat_data.txt` - Written by ethercat_app
- **Calibration file**: `/tmp/vacuum_offsets.json` - Stores offsets
- **Format**: `{"ch1": offset1, "ch2": offset2, "ch3": offset3, "ch4": offset4}`

## Troubleshooting

### Readings seem incorrect after calibration
- Ensure you calibrated at true atmospheric pressure (no vacuum)
- Try "Reset Calibration" and recalibrate

### Offset values look strange
- Normal offsets are typically between -200 and +200
- Large offsets (>500) may indicate sensor drift or calibration at wrong pressure

### Dashboard shows "Failed to read data"
- Make sure `ethercat_app` is running
- Check that `/tmp/ethercat_data.txt` exists and is being updated
- Verify EtherCAT master is working: `ethercat slaves`

### Calibration doesn't persist after reboot
- Offsets are stored in `/tmp/` which is cleared on reboot
- You'll need to recalibrate after each reboot
- To make permanent, change OFFSET_FILE path in app.py to non-tmp location

## Example Readings

**Main Display (mbar mode):**
```
Vacuum 1: -650 mbar 💨
Vacuum 2: -450 mbar 💨
Vacuum 3: -800 mbar 💨
Vacuum 4: -100 mbar 💨
```

**Main Display (bar mode):**
```
Vacuum 1: -0.650 bar 💨
Vacuum 2: -0.450 bar 💨
Vacuum 3: -0.800 bar 💨
Vacuum 4: -0.100 bar 💨
```

**Debug Page (Detailed Information):**
```
Vacuum 1: -650 mbar
  Raw Value: 350
  Offset: 0
  Instant Pressure: -650 mbar
  Samples in Avg: 10/10
```

## Technical Details

### Conversion Formula
```
pressure_mbar = (raw_value - offset) - 1000
```

Where:
- `raw_value` = Current sensor reading (0-1000 range)
- `offset` = Calibration offset (set during calibration)
- Result is in millibar (mbar)

### API Endpoints

- `GET /api/data` - Get current readings
- `POST /api/calibrate` - Calibrate all channels to 0 mbar
- `POST /api/reset_calibration` - Reset all offsets to 0

## License

Free to use and modify for your vacuum monitoring needs!
