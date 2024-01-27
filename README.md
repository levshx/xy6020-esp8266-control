# xy6020-esp8266-control
Monitor and control the sinilink xy6020 power module

Board:

> NodeMCU_v3

Framework:

> Arduino



### Communication Interface

The communication protocol is `MODBUS-RTU`.

Factory Modbus ID setting is `01H`

The communication interface is TTL serial port;

The data format is `8,N,1` `(8 data, no parity, one stop bit(s))`

The factory baudrate setting is `115200`

Logic high level is `3.3V`

Interface Connector:
- VCC: 5V supply 
- TX: Serial data input (connect it to master’s transmitter output)
- RX: Serial data output (connect it to master’s receiver input)
- GND: Ground

### Function Codes

This machine only supports 0x03, 0x06, 0x10 function codes.

This means, all mentioned registers are ‘Holding Registers’

| Function code | Definition               | Operation (binary)                                       |
|---------------|--------------------------|----------------------------------------------------------|
| 0x03          | Read register data       | Read data from one or more registers                     |
| 0x06          | Write a single register  | Write a set of binary data to a single register          |
| 0x10          | Write multiple registers | Write multiple sets of binary data to multiple registers |

### Register Set (Holding Registers)

Rem: Each register consists of 16 bits, transferred as 2 bytes.

| Name   | Description                                 | Bytes | Dec. | Unit | Read and write | Register address |
|--------|---------------------------------------------|-------|------|------|----------------|------------------|
| V-SET  | Voltage setting                             | 2     | 2    | V    | R/W            | 0000/H           |
| I-SET  | Current setting                             | 2     | 2    | A    | R/W            | 0001/H           |
| VOUT   | Output voltage display value                | 2     | 2    | V    | R              | 0002/H           |
| IOUT   | Output current display value                | 2     | 2    | A    | R              | 0003/H           |
| POWER  | Output power display value                  | 2     | 1    | W    | R              | 0004/H           |


### Communication example

The host reads the output voltage and output current display values

The message format sent by the host:

| Data Item                    | Number of bytes | Data sent | Remark                              |
|------------------------------|-----------------|-----------|-------------------------------------|
| Slave address                | 1               | 01        | Send to slave with address 01       |
| Function code                | 1               | 03        | Read register                       |
| Register start address       | 2               | 0002/H    | Register start address              |
| Number of register addresses | 2               | 0002/H    | 2 bytes in total                    |
| CRC code                     | 2               | 65CB/H    | The CRC code calculated by the host |

For example, if the current display value is 05.00V, 1.500A, then the

Message format returned by the slave machine:

| Data Item                    | Number of bytes | Data sent | Remark                              |
|------------------------------|-----------------|-----------|-------------------------------------|
| Slave address                | 1               | 01        | Received from slave with address 01 |
| Function code                | 1               | 03        | Read register                       |
| Bytes in response            | 1               | 04        | Number of data bytes in response    |
| Content of register 0002/H   | 2               | 01F4/H    | Output voltage display value        |
| Content of register 0003/H   | 2               | 05DC/H    | Output current display value        |
| CRC code                     | 2               | B8F4/H    | The CRC code calculated by the slave|
