# Changelog

All notable changes to AirLab will be documented in this file.

## [1.5.0] - 2025-11-03

### Changed
- **UI Reorganization**: Moved Calibrate and Unit buttons to bottom of vacuum values for better visual flow
- **Reset Button**: Moved Reset Calibration button from main page to debug page for cleaner interface
- **Documentation**: Simplified documentation structure - consolidated into single README.md
- **Version Tracking**: Enhanced version history tracking with CHANGELOG.md

### Removed
- AIRLAB_BRANDING.md - information integrated into README
- AIRLAB_OVERVIEW.md - information integrated into README

## [1.4.1] - 2025-11-02

### Added
- External CSS file (static/style.css)
- Bilingual support (English/Portuguese)
- Shutdown and Reboot functionality

### Changed
- Icon changed to 💨 (air/wind emoji)
- Blue gradient theme
- Simplified button styling
- Default unit set to bar

## [1.4.0] - 2025-11-01

### Added
- Unit toggle button (mbar/bar)
- Project renamed to "AirLab - Pneumatic Test Bench"

### Changed
- Ultra-clean main interface
- Removed clutter from main display
- All detailed info moved to debug page

## [1.3.0] - 2025-10-31

### Added
- Separate debug page with detailed information

### Changed
- Simplified main display
- Pressure readings show as integers (no decimals)
- Removed raw values and offsets from main view

## [1.2.0] - 2025-10-30

### Added
- Moving average filter (10 samples) for stable readings
- Automatic buffer clearing on calibration/reset

### Changed
- Pressure values show 1 decimal place
- Reduced display jitter at 2Hz refresh rate

## [1.1.0] - 2025-10-29

### Changed
- Refresh rate changed to 2Hz (500ms)
- Channel labels updated from "Channel X" to "Vacuum X"

### Removed
- Auto-refresh display indicator
- Vacuum range info from main display

## [1.0.0] - 2025-10-28

### Added
- Initial release
- Auto-calibration support
- 10Hz refresh rate
- Real-time pressure conversion
- Persistent offset storage
- 4-channel vacuum/pressure monitoring
