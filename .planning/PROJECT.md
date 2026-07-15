# irrigacao-inteligente

## Vision
Sistema de irrigação inteligente com ESP32.

## Core Values
- Confiabilidade: agenda dispara no minuto certo, persiste após reboot
- Autonomia: dashboard e menu OLED funcionam offline, sem depender de internet/serviços externos

## Constraints
- Hardware fixo: ESP32, 8 módulos de relé, OLED SSD1306 I2C, encoder rotativo, RTC DS3231, DHT11
- Modelo de agenda: 4 agendas globais (código atual diverge do contrato ABNT original de 4 setores x 4 agendas)

## Tech Stack
- Firmware: Arduino/C++ para ESP32
- Bibliotecas: U8g2, RTClib, ESP32Encoder, DHTesp/DHT Adafruit, WebSockets (Links2004, opcional)
- Persistência: NVS via Preferences (versão+CRC)
- Web: servidor HTTP + WebSocket embutido, dashboard standalone (sem dependências externas)

## Created
2026-07-15
