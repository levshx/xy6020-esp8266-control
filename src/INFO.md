
> [!NOTE]
> Communication example

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
