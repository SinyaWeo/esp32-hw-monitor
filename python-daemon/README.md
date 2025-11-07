# Hardware Monitor Daemon

A Linux daemon that monitors system hardware (CPU temperature, CPU load, GPU temperature) and transmits the data via serial port as comma-separated values.

## Features

- **CPU Temperature**: Reads from system thermal sensors
- **CPU Load**: Monitors CPU utilization percentage
- **GPU Temperature**: Supports NVIDIA and AMD GPUs
- **Serial Output**: Sends data as CSV format (cpu_temp,cpu_load,gpu_temp)
- **Automatic Reconnection**: Handles serial port disconnections
- **Systemd Integration**: Runs as a system service

## Requirements

- Python 3.6+
- Linux system with thermal sensors
- Serial port for data transmission

## Installation

1. **Install Python dependencies:**
   psutil>=5.9.0
   pyserial>=3.5

2. **Configure the daemon:**
   Edit `hw_monitor_daemon.py` and adjust the configuration variables:
   ```python
   SERIAL_PORT = '/dev/ttyUSB0'  # Your serial port
   BAUD_RATE = 9600              # Your baud rate
   UPDATE_INTERVAL = 2           # Update interval in seconds
   ```

3. **Test the daemon manually:**
   ```bash
   sudo python3 hw_monitor_daemon.py
   ```

4. **Install as a systemd service:**
   ```bash
   sudo cp hw-monitor.service /etc/systemd/system/
   sudo systemctl daemon-reload
   sudo systemctl enable hw-monitor.service
   sudo systemctl start hw-monitor.service
   ```

## Usage

### Manual execution:
```bash
sudo python3 hw_monitor_daemon.py
```

### As a service:
```bash
# Start the service
sudo systemctl start hw-monitor.service

# Stop the service
sudo systemctl stop hw-monitor.service

# Check status
sudo systemctl status hw-monitor.service

# View logs
sudo journalctl -u hw-monitor.service -f
```

## Output Format

Data is sent via serial as a comma-separated string with newline terminator:
```
cpu_temp,cpu_load,gpu_temp\n
```

Example:
```
65.5,45.2,72.0
```

If a sensor is unavailable, it will output `N/A`:
```
65.5,45.2,N/A
```

## Configuration

### Serial Port
Find your serial port with:
```bash
ls /dev/ttyUSB* /dev/ttyACM*
```

Common ports:
- `/dev/ttyUSB0` - USB to serial adapter
- `/dev/ttyACM0` - Arduino and similar devices
- `/dev/ttyS0` - Built-in serial port

### Permissions
The daemon needs root privileges to access hardware sensors and serial ports. Alternatively, add your user to the `dialout` group for serial access:
```bash
sudo usermod -a -G dialout $USER
```

### GPU Support
- **NVIDIA**: Requires `nvidia-smi` utility (comes with NVIDIA drivers)
- **AMD**: Uses sysfs hwmon interface

## Troubleshooting

### No sensors found
Check available thermal sensors:
```bash
ls /sys/class/thermal/thermal_zone*/type
cat /sys/class/thermal/thermal_zone*/type
```

Check hardware monitoring:
```bash
ls /sys/class/hwmon/hwmon*/name
cat /sys/class/hwmon/hwmon*/name
```

### Serial port issues
- Verify port exists: `ls -l /dev/ttyUSB0`
- Check permissions: `sudo chmod 666 /dev/ttyUSB0`
- Verify no other program is using the port

### View logs
```bash
# Service logs
sudo journalctl -u hw-monitor.service -f

# Log file (if running manually)
tail -f /var/log/hw_monitor_daemon.log
```

## License

MIT License
