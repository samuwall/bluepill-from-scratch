# `09-i2c`

### tl;dr

demonstrates the I2C peripheral by interfacing with an EEPROM.

it performs a sequential read to display the initial contents of the EEPROM, writes bytes 0-15 to the EEPROM using a page write, and then reads the EEPROM data back.

### from scratch

#### hardware setup

add a 4.7k pull-up resistor between SCL and Vdd (3.3v) 
and another between SDA and Vdd.

#### eeprom explanation

all operations involving the 24LC16 eeprom start with
the "control byte". it is the first byte received after
the start condition. 

the 4 most significant bits are the
"control code", which for the 24XX16H is hard coded as
`1010`. the next 3 bits are the block select bits, which
are used internally to select which of the eight 256-byte
blocks of memory is to be accessed.

in the context of the master device, this "control byte"
is simply the slave address. 

because the address pins are not functional, all 24LC16
eeproms share the same slave address per block, meaning 
only one can be used per i2c bus.


here is the 7 bit slave address for the 24LC16 EEPROM:

```c
eeprom_slave_addr = 0b1010000;
```

bits 6:3 are the control code, which distinguish between the EEPROM families.

bits 2:0 are the block select bits. 3 bits = 8 blocks (0-7).


the next byte usually sent is the "word address byte". this value
specifies which byte in the block you want to access.

because you usually want to specify where you are reading from,
even a read operation is usually a write -> read operation. (repeated start)

```c
byte_address = 0b00000000;
```

it's an 8 bit value, which makes sense since each block is made up of 256 bytes (0-255)

the following explains all write/read operations available on the 24LC16:


#### Write Operations

We can perform either a **byte write** or a **page write**.

- **Byte Write**: Send the slave address, send the byte address (the location within the block), send the data byte, and send a stop condition. After the stop, the EEPROM initiates the internal write cycle and does not generate ACK signals until it has finished writing.
- **Page Write**: Allows you to send up to 16 data bytes to be written before sending a stop condition. If more than 16 bytes are sent, the on-chip page buffer gets overwritten in a FIFO manner. Each page write must be confined within a single physical page (16 bytes). The boundaries are multiples of the page size, i.e., bytes 0-15, 16-31, etc. Bytes written past the end of a page boundary wrap around to the beginning of that page instead of proceeding to the next page.

#### Read Operations

Three options are available:

1. **Current Address Read**: Reads from the address following the last accessed address (`n + 1`). No need to send a byte address. The master sends a stop condition after receiving the data byte.
2. **Random Read**: Send the byte address of the desired data, followed by a repeated start condition with the R/W bit set to read (`1`). Receive the data byte and send a stop condition.
3. **Sequential Read**: Similar to a random read, but instead of sending a stop condition after receiving a data byte, the master sends an ACK to receive the next byte. This process continues until the master sends a NACK followed by a stop condition. There are no page boundary limitations when reading.


---

- The 24LC16 requires up to 5 ms to complete an internal write cycle. During this time, it will not acknowledge any I2C communication.
