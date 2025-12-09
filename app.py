#!/usr/bin/env python3
"""
AirLab - Pneumatic Test Bench System
Version: 1.5.0
Changes:
  - v1.5.0: Reorganized UI - moved buttons to bottom, Reset moved to debug page, cleaned docs
  - v1.4.1: Changed icon to Ã°Å¸â€™Â¨, blue theme, simplified buttons, default unit bar
  - v1.4.0: Added mbar/bar unit toggle, simplified main page, renamed to AirLab
  - v1.3.0: Rounded averages to integers, moved debug info to separate page
  - v1.2.0: Added moving average filtering (10 samples) to stabilize readings
  - v1.1.0: Changed refresh rate to 2Hz (500ms), updated channel labels to "Vacuum X"
  - v1.0.0: Initial release with auto-calibration support
"""
from flask import Flask, render_template, jsonify, request, send_from_directory
from datetime import datetime
from collections import deque
import json
import os

app = Flask(__name__, static_folder='static', static_url_path='/static')
VERSION = "1.5.0"
PROJECT_NAME = "AirLab"

# File to store calibration offsets
OFFSET_FILE = '/tmp/vacuum_offsets.json'

# Moving average buffers for each channel (store last 10 samples)
BUFFER_SIZE = 10
channel_buffers = {
    'ch1': deque(maxlen=BUFFER_SIZE),
    'ch2': deque(maxlen=BUFFER_SIZE),
    'ch3': deque(maxlen=BUFFER_SIZE),
    'ch4': deque(maxlen=BUFFER_SIZE)
}

def load_offsets():
    """Load calibration offsets from file"""
    if os.path.exists(OFFSET_FILE):
        try:
            with open(OFFSET_FILE, 'r') as f:
                return json.load(f)
        except:
            pass
    return {'ch1': 0, 'ch2': 0, 'ch3': 0, 'ch4': 0}

def save_offsets(offsets):
    """Save calibration offsets to file"""
    with open(OFFSET_FILE, 'w') as f:
        json.dump(offsets, f)

def raw_to_pressure(raw_value, offset):
    """
    Convert raw value to pressure in mbar
    RAW 1000 = 0 bar (atmospheric)
    RAW 0 = -1000 mbar (full vacuum)
    
    Formula: pressure_mbar = -(1000 - (raw - offset))
    Simplified: pressure_mbar = (raw - offset) - 1000
    """
    calibrated_raw = raw_value - offset
    pressure_mbar = calibrated_raw - 1000
    return pressure_mbar

def calculate_moving_average(channel_name, new_value):
    """
    Calculate moving average for a channel
    Adds new value to buffer and returns average of all values in buffer
    """
    channel_buffers[channel_name].append(new_value)
    if len(channel_buffers[channel_name]) > 0:
        return sum(channel_buffers[channel_name]) / len(channel_buffers[channel_name])
    return new_value

def clear_buffers():
    """Clear all moving average buffers (useful after calibration)"""
    for key in channel_buffers:
        channel_buffers[key].clear()


def read_values_from_file():
    """Read values from the file written by ethercat_app"""
    try:
        with open('/tmp/ethercat_data.txt', 'r') as f:
            line = f.read().strip()
            values_list = [int(x) for x in line.split(',')]
            
            if len(values_list) == 4:
                offsets = load_offsets()
                
                result = {}
                for i, ch_name in enumerate(['ch1', 'ch2', 'ch3', 'ch4']):
                    raw_value = values_list[i]
                    offset = offsets[ch_name]
                    
                    # Calculate instant pressure (without averaging)
                    instant_pressure = raw_to_pressure(raw_value, offset)
                    
                    # Calculate moving average pressure
                    avg_pressure = calculate_moving_average(ch_name, instant_pressure)
                    
                    result[ch_name] = {
                        'raw_value': raw_value,
                        'offset': offset,
                        'pressure_mbar': round(avg_pressure),  # Rounded to integer
                        'instant_pressure': round(instant_pressure),
                        'samples_in_avg': len(channel_buffers[ch_name]),
                        'status': 'OK',
                        'error': False,
                        'underrange': False,
                        'overrange': False
                    }
                
                return result
    except Exception as e:
        print(f"Error reading file: {e}")
    return None

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/debug')
def debug():
    return render_template('debug.html')

@app.route('/signals')
def signals():
    return render_template('signals.html')

@app.route('/api/data')
def api_data():
    values = read_values_from_file()
    
    if values:
        return jsonify({
            'success': True,
            'timestamp': datetime.now().isoformat(),
            'channels': values
        })
    
    return jsonify({
        'success': False,
        'error': 'Failed to read data from /tmp/ethercat_data.txt'
    })

@app.route('/api/digital')
def api_digital():
    """Read digital inputs from file"""
    try:
        with open('/tmp/ethercat_digital.txt', 'r') as f:
            line = f.read().strip()
            values_list = [int(x) for x in line.split(',')]
            
            if len(values_list) == 8:
                result = {}
                for i in range(8):
                    result[f'input_{i+1}'] = {
                        'value': values_list[i],
                        'state': 'ON' if values_list[i] == 1 else 'OFF'
                    }
                
                return jsonify({
                    'success': True,
                    'timestamp': datetime.now().isoformat(),
                    'inputs': result
                })
    except Exception as e:
        print(f"Error reading digital inputs: {e}")
    
    return jsonify({
        'success': False,
        'error': 'Failed to read data from /tmp/ethercat_digital.txt'
    })

@app.route('/api/outputs', methods=['GET'])
def api_outputs_get():
    """Read current digital outputs state"""
    try:
        with open('/tmp/ethercat_outputs.txt', 'r') as f:
            line = f.read().strip()
            values_list = [int(x) for x in line.split(',')]
            
            if len(values_list) == 8:
                result = {}
                for i in range(8):
                    result[f'output_{i+1}'] = {
                        'value': values_list[i],
                        'state': 'ON' if values_list[i] == 1 else 'OFF'
                    }
                
                return jsonify({
                    'success': True,
                    'timestamp': datetime.now().isoformat(),
                    'outputs': result
                })
    except Exception as e:
        print(f"Error reading digital outputs: {e}")
    
    return jsonify({
        'success': False,
        'error': 'Failed to read outputs state'
    })

@app.route('/api/outputs', methods=['POST'])
def api_outputs_set():
    """Set digital outputs state"""
    try:
        data = request.get_json()
        output_num = data.get('output')  # 1-8
        value = data.get('value')  # 0 or 1
        
        if output_num is None or value is None:
            return jsonify({
                'success': False,
                'error': 'Missing output or value parameter'
            })
        
        output_num = int(output_num)
        value = int(value)
        
        if output_num < 1 or output_num > 8:
            return jsonify({
                'success': False,
                'error': 'Output must be between 1 and 8'
            })
        
        if value not in [0, 1]:
            return jsonify({
                'success': False,
                'error': 'Value must be 0 or 1'
            })
        
        # Read current state
        outputs = [0, 0, 0, 0, 0, 0, 0, 0]
        try:
            with open('/tmp/ethercat_outputs.txt', 'r') as f:
                line = f.read().strip()
                outputs = [int(x) for x in line.split(',')]
        except:
            pass
        
        # Update the specified output
        outputs[output_num - 1] = value
        
        # Write back to file
        with open('/tmp/ethercat_outputs.txt', 'w') as f:
            f.write(','.join(map(str, outputs)) + '\n')
        
        return jsonify({
            'success': True,
            'message': f'Output {output_num} set to {value}',
            'output': output_num,
            'value': value
        })
        
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        })

@app.route('/api/outputs/all', methods=['POST'])
def api_outputs_all():
    """Set all digital outputs at once"""
    try:
        data = request.get_json()
        outputs = data.get('outputs')  # Array of 8 values [0,1,0,1,...]
        
        if not outputs or len(outputs) != 8:
            return jsonify({
                'success': False,
                'error': 'Must provide array of 8 output values'
            })
        
        # Validate all values are 0 or 1
        for val in outputs:
            if val not in [0, 1]:
                return jsonify({
                    'success': False,
                    'error': 'All values must be 0 or 1'
                })
        
        # Write to file
        with open('/tmp/ethercat_outputs.txt', 'w') as f:
            f.write(','.join(map(str, outputs)) + '\n')
        
        return jsonify({
            'success': True,
            'message': 'All outputs updated',
            'outputs': outputs
        })
        
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        })

@app.route('/api/calibrate', methods=['POST'])
def api_calibrate():
    """
    Calibrate all channels to zero at current atmospheric pressure
    This sets the offset so current reading becomes 0 mbar
    """
    try:
        # Read current raw values
        with open('/tmp/ethercat_data.txt', 'r') as f:
            line = f.read().strip()
            values_list = [int(x) for x in line.split(',')]
            
            if len(values_list) == 4:
                # Current offsets
                offsets = load_offsets()
                
                # Calculate new offsets so that current pressure reads as 0 mbar
                # If pressure_mbar = (raw - offset) - 1000 = 0
                # Then: offset = raw - 1000
                new_offsets = {
                    'ch1': values_list[0] - 1000,
                    'ch2': values_list[1] - 1000,
                    'ch3': values_list[2] - 1000,
                    'ch4': values_list[3] - 1000
                }
                
                save_offsets(new_offsets)
                
                # Clear moving average buffers after calibration
                clear_buffers()
                
                return jsonify({
                    'success': True,
                    'message': 'All channels calibrated to 0 mbar',
                    'offsets': new_offsets
                })
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        })
    
    return jsonify({
        'success': False,
        'error': 'Failed to calibrate'
    })

@app.route('/api/reset_calibration', methods=['POST'])
def api_reset_calibration():
    """Reset all offsets to zero"""
    offsets = {'ch1': 0, 'ch2': 0, 'ch3': 0, 'ch4': 0}
    save_offsets(offsets)
    
    # Clear moving average buffers
    clear_buffers()
    
    return jsonify({
        'success': True,
        'message': 'Calibration reset to factory defaults',
        'offsets': offsets
    })

@app.route('/api/activate')
def api_activate():
    # We don't need to activate since ethercat_app handles it
    return jsonify({'success': True, 'message': 'ethercat_app is handling activation'})

@app.route('/api/version')
def api_version():
    return jsonify({'version': VERSION})

@app.route('/api/shutdown', methods=['POST'])
def api_shutdown():
    """Shutdown the Raspberry Pi"""
    try:
        import subprocess
        subprocess.Popen(['sudo', 'shutdown', '-h', 'now'])
        return jsonify({
            'success': True,
            'message': 'Raspberry Pi is shutting down...'
        })
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        })

@app.route('/api/reboot', methods=['POST'])
def api_reboot():
    """Reboot the Raspberry Pi"""
    try:
        import subprocess
        subprocess.Popen(['sudo', 'reboot'])
        return jsonify({
            'success': True,
            'message': 'Raspberry Pi is rebooting...'
        })
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        })

if __name__ == '__main__':
    print("\n" + "="*60)
    print(f"{PROJECT_NAME} - Pneumatic Test Bench v{VERSION}")
    print("="*60)
    print(f"\nAccess the dashboard at: http://<your-pi-ip>:5000")
    print("Make sure ethercat_app is running in another terminal!")
    print("\nFeatures:")
    print("  - Moving average filter (10 samples) for stable readings")
    print("  - 2Hz refresh rate (500ms)")
    print("  - mbar/bar unit toggle")
    print("\nCalibration:")
    print("  - Expose all suction cups to atmospheric pressure")
    print("  - Click 'Calibrate All Channels' in the web interface")
    print("\nPress Ctrl+C to stop\n")
    
    app.run(host='0.0.0.0', port=5000, debug=False)