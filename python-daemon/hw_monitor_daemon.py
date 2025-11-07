#!/usr/bin/env python3
"""
Hardware Monitor Daemon
Collects CPU temp, CPU load, and GPU temp and sends via serial as CSV.
"""

import time
import serial
import psutil
import logging
import signal
import sys
from pathlib import Path

# Configuration
SERIAL_PORT = '/dev/ttyACM0'  # Change to your serial port
BAUD_RATE = 9600
UPDATE_INTERVAL = 2  # seconds
LOG_FILE = '/var/log/hw_monitor_daemon.log'

# Setup logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler(LOG_FILE),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger('HW_Monitor')


class HardwareMonitor:
    """Monitor system hardware statistics."""
    
    def __init__(self):
        self.thermal_zones = self._find_thermal_zones()
        self.gpu_thermal_path = self._find_gpu_thermal()
        
    def _find_thermal_zones(self):
        """Find CPU thermal zone paths."""
        zones = []
        thermal_base = Path('/sys/class/thermal')
        
        if thermal_base.exists():
            for zone in thermal_base.glob('thermal_zone*'):
                try:
                    type_file = zone / 'type'
                    if type_file.exists():
                        zone_type = type_file.read_text().strip()
                        # Look for CPU-related thermal zones
                        if any(keyword in zone_type.lower() for keyword in ['cpu', 'x86_pkg', 'coretemp', 'k10temp']):
                            temp_file = zone / 'temp'
                            if temp_file.exists():
                                zones.append(temp_file)
                                logger.info(f"Found CPU thermal zone: {zone_type} at {temp_file}")
                except Exception as e:
                    logger.debug(f"Error reading thermal zone {zone}: {e}")
        
        return zones
    
    def _find_gpu_thermal(self):
        """Find GPU thermal sensor path."""
        # NVIDIA GPU via hwmon
        hwmon_base = Path('/sys/class/hwmon')
        if hwmon_base.exists():
            for hwmon in hwmon_base.glob('hwmon*'):
                try:
                    name_file = hwmon / 'name'
                    if name_file.exists():
                        name = name_file.read_text().strip()
                        # Check for NVIDIA or AMD GPU
                        if any(keyword in name.lower() for keyword in ['nvidia', 'amdgpu', 'radeon']):
                            # Try different temp file names
                            for temp_name in ['temp1_input', 'temp2_input', 'temp_input']:
                                temp_file = hwmon / temp_name
                                if temp_file.exists():
                                    logger.info(f"Found GPU thermal sensor: {name} at {temp_file}")
                                    return temp_file
                except Exception as e:
                    logger.debug(f"Error reading hwmon {hwmon}: {e}")
        
        # Try NVIDIA SMI as fallback
        try:
            import subprocess
            result = subprocess.run(['nvidia-smi', '--query-gpu=temperature.gpu', '--format=csv,noheader,nounits'],
                                  capture_output=True, text=True, timeout=2)
            if result.returncode == 0:
                logger.info("Using nvidia-smi for GPU temperature")
                return 'nvidia-smi'
        except Exception:
            pass
        
        logger.warning("No GPU thermal sensor found")
        return None
    
    def get_cpu_temp(self):
        """Get CPU temperature in Celsius."""
        try:
            # Try psutil first (works on many systems)
            if hasattr(psutil, 'sensors_temperatures'):
                temps = psutil.sensors_temperatures()
                if temps:
                    # Try common CPU sensor names
                    for sensor_name in ['coretemp', 'k10temp', 'cpu_thermal', 'soc_thermal']:
                        if sensor_name in temps:
                            entries = temps[sensor_name]
                            if entries:
                                # Return the first temperature or package temp
                                for entry in entries:
                                    if 'package' in entry.label.lower() or 'tctl' in entry.label.lower():
                                        return round(entry.current, 1)
                                return round(entries[0].current, 1)
            
            # Fallback to reading from thermal zones
            if self.thermal_zones:
                temps = []
                for zone in self.thermal_zones:
                    try:
                        temp_str = zone.read_text().strip()
                        temp = float(temp_str) / 1000.0  # Convert from millidegrees
                        temps.append(temp)
                    except Exception:
                        pass
                
                if temps:
                    return round(max(temps), 1)  # Return highest CPU temp
            
            return None
        except Exception as e:
            logger.error(f"Error reading CPU temperature: {e}")
            return None
    
    def get_cpu_load(self):
        """Get CPU load percentage."""
        try:
            # Get average CPU utilization over a short interval
            return round(psutil.cpu_percent(interval=0.5), 1)
        except Exception as e:
            logger.error(f"Error reading CPU load: {e}")
            return None
    
    def get_gpu_temp(self):
        """Get GPU temperature in Celsius."""
        try:
            if self.gpu_thermal_path is None:
                return None
            
            if self.gpu_thermal_path == 'nvidia-smi':
                import subprocess
                result = subprocess.run(['nvidia-smi', '--query-gpu=temperature.gpu', '--format=csv,noheader,nounits'],
                                      capture_output=True, text=True, timeout=2)
                if result.returncode == 0:
                    return round(float(result.stdout.strip()), 1)
            else:
                temp_str = self.gpu_thermal_path.read_text().strip()
                temp = float(temp_str) / 1000.0  # Convert from millidegrees
                return round(temp, 1)
            
            return None
        except Exception as e:
            logger.error(f"Error reading GPU temperature: {e}")
            return None


class SerialSender:
    """Handle serial communication."""
    
    def __init__(self, port, baudrate):
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.connect()
    
    def connect(self):
        """Connect to serial port."""
        try:
            self.serial = serial.Serial(self.port, self.baudrate, timeout=1)
            logger.info(f"Connected to serial port {self.port} at {self.baudrate} baud")
        except Exception as e:
            logger.error(f"Failed to connect to serial port {self.port}: {e}")
            self.serial = None
    
    def send(self, data):
        """Send data via serial."""
        if self.serial is None or not self.serial.is_open:
            logger.warning("Serial port not connected, attempting reconnect...")
            self.connect()
            if self.serial is None:
                return False
        
        try:
            message = f"{data}\n"
            self.serial.write(message.encode('utf-8'))
            self.serial.flush()
            return True
        except Exception as e:
            logger.error(f"Error sending data via serial: {e}")
            self.serial = None
            return False
    
    def close(self):
        """Close serial connection."""
        if self.serial and self.serial.is_open:
            self.serial.close()
            logger.info("Serial port closed")


class Daemon:
    """Main daemon class."""
    
    def __init__(self):
        self.running = False
        self.monitor = HardwareMonitor()
        self.serial = SerialSender(SERIAL_PORT, BAUD_RATE)
        
        # Setup signal handlers
        signal.signal(signal.SIGTERM, self.signal_handler)
        signal.signal(signal.SIGINT, self.signal_handler)
    
    def signal_handler(self, signum, frame):
        """Handle shutdown signals."""
        logger.info(f"Received signal {signum}, shutting down...")
        self.running = False
    
    def run(self):
        """Main daemon loop."""
        logger.info("Hardware Monitor Daemon starting...")
        self.running = True
        
        while self.running:
            try:
                # Collect hardware data
                cpu_temp = self.monitor.get_cpu_temp()
                cpu_load = self.monitor.get_cpu_load()
                gpu_temp = self.monitor.get_gpu_temp()
                
                # Format as CSV: cpu_temp,cpu_load,gpu_temp
                csv_data = f"{cpu_temp if cpu_temp is not None else 'N/A'}," \
                          f"{cpu_load if cpu_load is not None else 'N/A'}," \
                          f"{gpu_temp if gpu_temp is not None else 'N/A'}"
                
                # Send via serial
                if self.serial.send(csv_data):
                    logger.debug(f"Sent: {csv_data}")
                
                # Wait for next update
                time.sleep(UPDATE_INTERVAL)
                
            except Exception as e:
                logger.error(f"Error in main loop: {e}")
                time.sleep(UPDATE_INTERVAL)
        
        # Cleanup
        self.serial.close()
        logger.info("Hardware Monitor Daemon stopped")


def main():
    """Entry point."""
    daemon = Daemon()
    try:
        daemon.run()
    except Exception as e:
        logger.error(f"Fatal error: {e}")
        sys.exit(1)


if __name__ == '__main__':
    main()
