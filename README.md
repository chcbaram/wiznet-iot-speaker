# wiznet-iot-speaker

- System using the W5300 chip for real-time delivery of sound data over a network and output to speakers
- Build the board using the W5300 and an MCU

### Development Environment
- Hardware
    - Schematic and Artwork Online with EasyEDA
    - JLCPCB SMT
-  Firmware
    - CMake, GCC, Make, STM32CubeH7
    - STM32CubeMX
- Software
    - python, pySide6

### System Block Diagram
![main_block](https://user-images.githubusercontent.com/5537436/235310490-4343d7cb-b62b-47c4-9ad0-486fb1853087.jpg)

### Specifications
Category | Product name | Specifications | Remarks
---- | ---- | ---- | ----
MCU | STM32H723ZGT6 | FLASH : 1MB, SRAM : 564KB | External SRAM bus 16-bit
External FLASH | W25Q128JVSIQTR |	16MBytes |	For firmware updates, data storage
Network |	W5300 | |		16-bit bus connection
Audio H/W #1 |	MAX98357A |	I2S Codec/AMP |	Speaker Connection
Audio H/W #2 |	ES8156 |	I2S Codec	| AMP Connection
Audio Compression | 	Opus Codec | |		https://opus-codec.org


### Main Board

![main_bd](https://user-images.githubusercontent.com/5537436/235310032-73d421e0-8c94-4bc9-b89e-5fef2ce7070f.jpg)

### I/O Board

![Monosnap EasyEDA(Standard) - A Simple and Powerful Electronic Circuit Design Tool 2023-05-20 11-05-31](https://github.com/chcbaram/wiznet-iot-speaker/assets/5537436/c5259e71-b67a-44a9-bfba-e30727a3b760)

### Production Board

![Monosnap Monosnap 2023-05-14 21-57-32](https://github.com/chcbaram/wiznet-iot-speaker/assets/5537436/5b104284-f17a-4f77-9830-f25a502e559b)
