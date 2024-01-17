# Build FCS Server

FCS Server supports build for ARM64 (since HPS is ARM based) and x86 for testing purpose.

# Related components

- [Verifier](https://github.com/altera-opensource/verifier)

## Build for x86

To build x86 version, g++ compiler is needed. Run:

```
make x86
```
optionally:
```
make test
```

## Build for aarch64

To build ARM64 executable on the Linux x86, AArch64 toolchain is needed. E.g. on Ubuntu 18, install:

```
apt-get install g++-aarch64-linux-gnu
```

Run:

```
make aarch64
```

## Build for All architectures

To build for both architectures, run:

```
make all
```

Output files in "out" directory:

1. fcsServer.**x86**
1. fcsServer.**aarch64**
1. fcsServer _where fcsServer is a copy of fcsServer.aarch64_

## Build with DEBUG symbols and NO security protections

```
make DEBUG=1 all
```

### Running FCS Server on HPS (Yocto linux)

After building FCS Server, copy following files to HPS running Yocto OS to one folder:

- **fcsServer** - executable
- **fcsServer.service** - service unit file used by systemd
- **install.sh** - install script

To change default log level (Info) or default port (50001), edit fcsServer.service file:

```
ExecStart=/usr/sbin/fcsServer [PORT_NUMBER] [LOG_LEVEL]
```

Possible log levels: Debug, Info, Error, Fatal

To install FCS Server, run install.sh within the folder script is located, with root privileges. FCS Server will
automatically start and will persist after system reboot.

SYSTEMCTL commands:

- Checking FCS Server status
  ```
  systemctl status fcsServer
  ```
- Start/stop FCS Server
  ```
  systemctl start fcsServer
  systemctl stop fcsServer
  ```

- Restart FCS Server
  ```
  systemctl restart fcsServer
  ```
- Changing FCS Server settings after installation: edit service unit file `/etc/systemd/system/fcsServer.service`,
  reload systemd manager configuration
  ```
  systemctl daemon-reload
  ```
  and restart service
    ```
    systemctl restart fcsServer
    ```
- You can test FCS Server by sending a command using the files in FCSFilter/test/testfiles
    ```
    netcat [ FCS Server IP address ] [ FCS Server port] < ./get_chip_id.bin
    e.g. netcat localhost 50001 < ./get_chip_id.bin
    ```
### Logs

To view FCS Server logs, run journalctl:

```
journalctl -u fcsServer
```
