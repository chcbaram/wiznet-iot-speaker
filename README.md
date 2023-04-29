# wiznet-iot-speaker

- W5300 칩을 사용해서 네트웍으로 사운드 데이터를 실시간 전달 및 스피커로 출력하는 시스템
- W5300과 MCU를 사용하여 직접 보드를 제작

### 개발환경
- 하드웨어
    - EasyEDA를 이용해서 온라인으로 회로도 및 아트웍
    - JLCPCB SMT
- 펌웨어
    - CMake, GCC, Make, STM32CubeH7
    - STM32CubeMX
- 소프트웨어
    - python, pySide6

### 기본사양
구분 | 품명 | 사양 | 비고
---- | ---- | ---- | ----
MCU | STM32H723ZGT6 | FLASH : 1MB, SRAM : 564KB | 외부 SRAM 버스 16비트
외부 FLASH | W25Q128JVSIQTR |	16MBytes |	펌웨어 업데이트, 데이터 저장용
네트워크 |	W5300 | |		16비트 버스 연결
오디오 H/W #1 |	MAX98357A |	I2S Codec/AMP |	스피커 연결
오디오 H/W #2 |	ES8156 |	I2S Codec	| AMP 연결
오디오 압축 | 	Opus 코덱 | |		https://opus-codec.org


### 메인보드

![main_bd](https://user-images.githubusercontent.com/5537436/235310032-73d421e0-8c94-4bc9-b89e-5fef2ce7070f.jpg)
