# ledcon

Server and client for controlling LEDs on an orange pi microcomputer.

## Prerequisites

* `gcc`
* a cross-compiler (e.g. `aarch64-buildroot-linux-gnu-gcc-13.4.0.br_real`)
* `make`
* a microcomputer connected to a PC over a network

## Download

You can obtain the source code in one of two ways:

### Download the archive:

```bash
curl -L -o ledcon.zip https://github.com/zverorx/ledcon/archive/refs/tags/v1.0.zip
```

```bash
unzip ledcon.zip
```

### Clone a repository:

```bash
git clone https://github.com/zverorx/ledcon.git
```

## Compilation

Go to the project directory, and run:

```bash
make CROSS_COMPILE=aarch64-buildroot-linux-gnu-gcc-13.4.0.br_real
```

To find out more about Makefile features, run:

```bash
make help
```

The executable files can be found in the client/bin/ and server/bin directories.

## Usage

### On a microcomputer:

```bash
./ledcon-server <port>
```

By default, the file path for LED devices looks like this:

* green LED: /sys/class/leds/green:status/brightness
* red LED: /sys/class/leds/red:power/brightness

If these paths differ on your model, configure the environment variables GREEN_DEV and RED_DEV before starting the server.

### On PC:

```bash
./ledcon-client <ip:port> <green|red> <on|off>
```

* ip:port - server socket address
* green|red - microcomputer LED
* on|off - LED mode

## License

This project is licensed under the GPLv3. See the LICENSE file for more details.

## Feedback

To contact the developer, you can email zveror1806@gmail.com.
