# Changelog

All notable changes to AirLab will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.1.1] - 2025-12-16

### 🎨 Changed
- **Circuit Card Layout**: Improved UI organization
  - Moved "Switch Mode" button to header (next to circuit title) for cleaner layout
  - Moved "Activate/Deactivate" button to bottom of card (centered, larger) for better visibility
  - Increased activate button size for better accessibility (bigger padding, 1.2em font)
  - Reduced switch mode button size for better visual hierarchy (smaller padding, 0.9em font)
- **Code Cleanup**: Removed all debug console.log statements for cleaner console output

### 🐛 Fixed
- Fixed CSS styling for new button positions and sizes
- Improved button visual hierarchy and user experience

## [2.1.0] - 2025-12-09

### ✨ Added
- **Circuit Control Interface**: Added comprehensive circuit control panel to main page
  - 4 independent circuits with individual controls
  - Mode switching: Toggle between Vacuum and Compressed Air for each circuit
  - Circuit activation: Individual activation buttons for each circuit
- **Digital Output Integration**:
  - DOs 1-4: Control circuit activation (ON/OFF)
  - DOs 5-8: Control mode switching (Vacuum=0, Compressed Air=1)
- **Real-Time Status Display**:
  - Live visual indicators for mode (Blue=Vacuum, Orange=Compressed Air)
  - Active state indicators (Green=Active, Gray=Inactive)
  - Dynamic button labels (Activate/Deactivate)
- **Bilingual Support**: Full English and Portuguese translations for circuit controls
- **Auto-Refresh**: Circuit status updates automatically at 2Hz alongside pressure monitoring

### 🔧 Changed
- **Main Page Layout**: Added dedicated "Circuit Controls" section between vacuum monitoring and calibration panel
- **CSS Enhancements**: New styling for circuit cards with hover effects and responsive design
- **JavaScript Functions**:
  - `toggleMode(circuitNum)`: Switch between vacuum/compressed air
  - `toggleActivation(circuitNum)`: Activate/deactivate circuits
  - `updateCircuitStatus()`: Auto-refresh circuit states from digital outputs

### 📚 Documentation
- Updated version numbers across all files (app.py, index.html)
- Enhanced translation system with circuit-specific terminology

### Technical Details
- **API Endpoints Used**:
  - GET `/api/outputs`: Read current digital output states
  - POST `/api/outputs`: Set individual output states
- **Grid Layout**: 2x2 responsive grid for circuit cards
- **Mobile Support**: Responsive design adapts to single column on small screens

---

## [2.0.2] - 2025-12-05

### 🎯 Major Update: Unified EtherCAT Control

This release represents a significant architectural change, merging analog monitoring and digital I/O control into a single unified application. Version 2.x moves AirLab from a monitoring-only system to a complete automation platform.

### Added
- **Unified EtherCAT Application**: Single program controls both slaves simultaneously
  - Slave 0: Engel 4-channel analog input (vacuum sensors)
  - Slave 1: EP2316-003 8-channel digital I/O (valve control)
- **File-Based Control Interface**: 
  - `/tmp/ethercat_outputs.txt` - Control digital outputs (8 channels)
  - `/tmp/ethercat_digital.txt` - Read digital inputs (8 channels)
- **Digital Output Control**: Enable/disable outputs via file interface for automation
- **Enhanced State Management**: Proper INIT → PREOP → SAFEOP → OP transitions
- **Safety Features**: Automatic safe state management for EP2316-003
- **Dual-Slave Domain**: Single EtherCAT domain handles both analog and digital data

### Changed
- **Architecture**: Complete rewrite of EtherCAT communication layer
  - Previous: Separate programs for analog and digital (couldn't run simultaneously)
  - Current: Unified application with shared master and domain
- **PDO Configuration**: Explicit PDO mapping for EP2316-003 to ensure reliable operation
- **Offset Management**: Manual calculation of process data offsets to avoid conflicts
- **Cycle Rate**: Maintained 100Hz (10ms) cycle time for real-time performance
- **Update Rates**: 
  - Analog data: 2Hz (500ms writes to file)
  - Digital inputs: 10Hz (100ms writes to file)

### Fixed
- **Critical**: Resolved "only one master per program" limitation
- **PDO Offset Conflicts**: Fixed overlapping memory regions between slaves
- **State Transitions**: EP2316-003 now reliably reaches and maintains OP state
- **Working Counter**: Proper WC validation for both slaves
- **Shutdown Sequence**: Clean output clearing before deactivation

### Technical Details
- **Domain Registration**: Only digital slave uses explicit PDO registration
- **Analog Slave**: Uses default PDO mapping to avoid registration conflicts
- **Memory Layout**: 
  - Analog data starts at offset 0
  - Digital outputs follow analog data in domain
  - Proper gap calculation prevents overlap
- **Communication Integrity**: Both slaves monitored for OP state and working counter

### Known Issues
- None reported in this release

### Migration Notes
If upgrading from v1.x:
1. Stop any running `ethercat_app` instances
2. Replace binary with new unified version
3. Digital output control now available via `/tmp/ethercat_outputs.txt`
4. All previous monitoring functionality preserved
5. No changes required to Flask web application

### Looking Ahead
Version 2.x series will focus on:
- Web-based valve control interface
- Automated test sequences
- Data logging and analysis
- Advanced automation workflows

---

## [1.6.0] - 2025-11-17

### Added
- **Digital Signals Monitoring Page**: Real-time monitoring of 8 digital inputs
- New `/digital` route in Flask application
- Visual representation of digital input states

### Changed
- Navigation structure to include digital signals page

---

## [1.5.0] - 2025-11-03

### Changed
- **UI Reorganization**: Moved Calibrate and Unit buttons to bottom of vacuum values
- **Reset Button**: Moved Reset Calibration from main page to debug page
- **Documentation**: Simplified structure - consolidated into single README.md
- **Version Tracking**: Enhanced version history with CHANGELOG.md

### Removed
- AIRLAB_BRANDING.md - integrated into README
- AIRLAB_OVERVIEW.md - integrated into README

---

## [1.4.1] - 2025-11-02

### Added
- External CSS file (`static/style.css`)
- Bilingual support (English/Portuguese)
- Shutdown and Reboot functionality via API
- Translation system (`static/translations.js`)

### Changed
- Icon changed to 💨 (air/wind emoji)
- Blue gradient theme
- Simplified button styling
- Default unit set to bar

---

## [1.4.0] - 2025-11-01

### Added
- **Unit Toggle Button**: Switch between mbar and bar
- **Project Rename**: Now "AirLab - Pneumatic Test Bench"

### Changed
- Ultra-clean main interface
- Removed clutter from main display
- All detailed information moved to debug page
- Professional branding

---

## [1.3.0] - 2025-10-31

### Added
- Separate debug page (`/debug`) with detailed technical information
- Raw values display
- Offset information
- System diagnostics

### Changed
- Simplified main display
- Pressure readings show as integers (no decimals)
- Removed raw values and offsets from main view

---

## [1.2.0] - 2025-10-30

### Added
- **Moving Average Filter**: 10-sample rolling average for stable readings
- Automatic buffer clearing on calibration/reset
- Reduced display jitter

### Changed
- Pressure values display with 1 decimal place
- More stable readings at 2Hz refresh rate

---

## [1.1.0] - 2025-10-29

### Changed
- **Refresh Rate**: Changed from 10Hz to 2Hz (500ms)
- **Channel Labels**: Updated from "Channel X" to "Vacuum X"

### Removed
- Auto-refresh display indicator
- Vacuum range information from main display

---

## [1.0.0] - 2025-10-28

### Added
- **Initial Release** of AirLab
- Auto-calibration support
- 10Hz refresh rate
- Real-time pressure conversion (ADC to mbar)
- Persistent offset storage
- 4-channel vacuum/pressure monitoring
- EtherCAT communication with Beckhoff modules
- Web-based dashboard with Flask
- Basic API endpoints for data and calibration

---

## Legend

- 🎯 Major feature or architectural change
- ✨ New feature
- 🔧 Enhancement or improvement
- 🐛 Bug fix
- 🔒 Security fix
- 📚 Documentation
- ⚠️ Breaking change
- 🗑️ Deprecation

---

## Version Numbering

AirLab follows Semantic Versioning (MAJOR.MINOR.PATCH):
- **MAJOR**: Incompatible API changes or major architectural redesigns
- **MINOR**: New functionality in a backwards compatible manner
- **PATCH**: Backwards compatible bug fixes

**Version 2.0.x**: Unified automation platform with valve control
**Version 1.x.x**: Monitoring-only system (legacy)
