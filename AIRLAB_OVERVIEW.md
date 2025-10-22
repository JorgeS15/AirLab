# AirLab - Project Overview

## 🔬 What is AirLab?

**AirLab** is a professional pneumatic test bench system designed for testing and validating air valves and vacuum systems in industrial applications.

## 🎯 Purpose

AirLab provides:
- **Real-time monitoring** of vacuum pressure across 4 channels
- **Automated control** of air and vacuum valves
- **Precise calibration** for accurate measurements
- **Professional interface** for operators and technicians

## 🏗️ System Architecture

### Hardware Stack
```
┌──────────────────────────────────────┐
│         Raspberry Pi                 │
│  ┌────────────────────────────────┐  │
│  │   AirLab Software v1.4.0       │  │
│  │   - Flask Web Server           │  │
│  │   - EtherCAT Master            │  │
│  │   - Real-time Control          │  │
│  └────────────────────────────────┘  │
└─────────────┬────────────────────────┘
              │
              │ EtherCAT Protocol
              │ (Industrial Fieldbus)
              │
┌─────────────▼────────────────────────┐
│      Beckhoff I/O Modules            │
│  ┌─────────────────────────────┐     │
│  │  Analog Input Module        │     │
│  │  (4-channel vacuum sensors) │     │
│  └─────────────────────────────┘     │
│  ┌─────────────────────────────┐     │
│  │  Digital Output Modules     │     │
│  │  (Valve control)            │     │
│  └─────────────────────────────┘     │
└─────────────┬────────────────────────┘
              │
    ┌─────────┴──────────┐
    │                    │
┌───▼────┐         ┌────▼────┐
│Vacuum  │         │  Air    │
│Sensors │         │ Valves  │
└───┬────┘         └────┬────┘
    │                   │
┌───▼───────────────────▼────┐
│  Pneumatic System Under    │
│         Test (SUT)         │
└────────────────────────────┘
```

## 🔧 Key Components

### 1. Software Layer
- **Flask Web Server** - User interface and API
- **EtherCAT Master** - Industrial communication
- **Python Backend** - Data processing and control logic
- **Moving Average Filter** - Signal smoothing

### 2. Communication Layer
- **EtherCAT Protocol** - Real-time industrial Ethernet
- **RESTful API** - Web interface communication
- **JSON Data Format** - Structured data exchange

### 3. Hardware Layer
- **Raspberry Pi** - Computing platform
- **Beckhoff Modules** - Industrial I/O
- **Vacuum Sensors** - 4x analog pressure sensors
- **Valve Controllers** - Air/vacuum valve actuation

## 📊 Measured Parameters

### Vacuum Channels (4x)
- **Range:** 0 to -1000 mbar (0 to -1 bar)
- **Resolution:** Integer values (1 mbar steps)
- **Update Rate:** 2 Hz (500ms)
- **Filtering:** 10-sample moving average

### Display Units
- **mbar** - Millibar (default)
- **bar** - Bar (toggle available)

## 🎮 User Interface

### Main Dashboard
- **Clean operator view**
- 4-channel vacuum display
- Unit toggle (mbar/bar)
- Calibration controls
- Status indicators

### Debug Page
- **Technical details**
- Raw sensor values
- Calibration offsets
- Buffer status
- System diagnostics

## 🌐 Web Access

```
Main Dashboard:  http://raspberry-pi-ip:5000/
Debug Page:      http://raspberry-pi-ip:5000/debug
API Endpoint:    http://raspberry-pi-ip:5000/api/data
```

## 🔬 Test Bench Applications

AirLab is designed for:

1. **Vacuum System Testing**
   - Suction cup performance
   - Vacuum generator validation
   - Leak detection

2. **Air Valve Testing**
   - Response time measurement
   - Flow characteristics
   - Pressure regulation

3. **Pneumatic Component QA**
   - Production testing
   - Quality control
   - Performance validation

4. **System Integration**
   - Multi-component testing
   - System behavior analysis
   - Performance optimization

## 🎯 Key Features

### Automatic Calibration
- One-click zero calibration
- Atmospheric pressure reference
- Persistent offset storage

### Real-time Monitoring
- 2 Hz update rate
- Moving average filtering
- Stable, smooth readings

### Professional Interface
- Operator-focused main view
- Technician debug mode
- Clean, distraction-free design

### Industrial Communication
- EtherCAT protocol support
- Beckhoff module compatibility
- Reliable data acquisition

## 📈 Technical Specifications

| Parameter | Value |
|-----------|-------|
| **Platform** | Raspberry Pi (any model) |
| **OS** | Raspberry Pi OS / Linux |
| **Protocol** | EtherCAT |
| **I/O Modules** | Beckhoff analog/digital |
| **Channels** | 4x vacuum/pressure |
| **Range** | 0 to -1000 mbar |
| **Update Rate** | 2 Hz (500ms) |
| **Filter** | 10-sample moving average |
| **Interface** | Web browser (any device) |
| **API** | RESTful JSON |

## 🚀 Typical Workflow

1. **Setup**
   - Connect pneumatic system
   - Power on Raspberry Pi
   - Start AirLab software

2. **Calibration**
   - Ensure atmospheric pressure
   - Click "Calibrate All Channels"
   - Verify zero readings

3. **Testing**
   - Apply vacuum/pressure
   - Monitor real-time values
   - Control valves as needed

4. **Analysis**
   - View debug information
   - Check raw values
   - Validate performance

5. **Reporting**
   - Record test results
   - Document behavior
   - Quality sign-off

## 💡 Design Philosophy

**Simplicity**
- Clean interface
- Essential controls only
- No distractions

**Reliability**
- Industrial protocols
- Proven hardware
- Stable software

**Flexibility**
- Multiple units (mbar/bar)
- Debug mode available
- Configurable channels

**Professional**
- Production-ready
- Industrial quality
- Test bench grade

## 🔄 Future Enhancements

Potential additions:
- Data logging to file/database
- Historical graphs and trends
- Alarm thresholds
- Automated test sequences
- Valve control interface
- Multi-test bench support
- Remote monitoring
- Report generation

## 📚 Documentation

- **README.md** - Complete user guide
- **VERSION_HISTORY.md** - Change log
- **QUICK_REFERENCE.md** - Quick start
- **API docs** - Developer reference

## 🏆 Why AirLab?

**AirLab** = Air + Laboratory

The name reflects:
- **Professional** - Laboratory-grade testing
- **Technical** - Engineering focus
- **Modern** - Contemporary design
- **Memorable** - Easy to remember
- **Appropriate** - Fits the application

Perfect for a pneumatic test bench system!

---

**AirLab v1.4.0 - Professional Pneumatic Testing** 🔬
