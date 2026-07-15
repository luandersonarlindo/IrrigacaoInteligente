# Requirements

## Project
irrigacao-inteligente - Sistema de irrigação inteligente com ESP32

## Functional Requirements

| ID  | Requirement | Acceptance Criteria | Priority | Status |
|-----|-------------|---------------------|----------|--------|
| R1  | Irrigação manual por setor (1-8) | - [ ] Clique curto abre/fecha relé do setor selecionado<br>- [ ] Timeout manual fecha automaticamente após tempo configurado | must-have | done |
| R2  | Agendamento semanal automático | - [ ] Até 4 agendas globais<br>- [ ] Cada agenda define hora, minuto, dias (diasMask) e setores (setoresMask) | must-have | done |
| R3  | Execução sequencial por lotes | - [ ] Respeita `MAX_SETOR_SIMULTANEOS_AGENDA`<br>- [ ] Retoma janela ativa no lote correto com tempo remanescente após reboot<br>- [ ] Em conflito no mesmo setor, mantém a maior duração | must-have | done |
| R4  | Dashboard web local via AP | - [ ] Exibe status, válvulas, agendas, runtime e RTC<br>- [ ] Acessível via AP sem depender de internet<br>- [ ] mDNS ativo quando STA conectado | must-have | done |
| R5  | Leitura de temperatura/umidade (DHT11) | - [ ] Exibido no OLED<br>- [ ] Exibido em card no dashboard | should-have | done |
| R6  | Persistência em NVS | - [ ] Agenda, cache de execução diária e config runtime persistem com versão+CRC<br>- [ ] Banco reinicializado com padrão seguro se versão/CRC inválido | must-have | done |
| R7  | API REST do dashboard | - [ ] Endpoints de válvula (toggle/set/off-all)<br>- [ ] Endpoints de agenda (save/delete/clear)<br>- [ ] Endpoint de config runtime e RTC<br>- [ ] Erros retornam JSON com `ok=false` | must-have | done |
| R8  | Histórico de eventos/alertas | - [ ] Buffer circular com até 40 registros<br>- [ ] Cobre rede, irrigação, agenda, configuração e RTC | should-have | done |
| R9  | Botão de emergência "desligar todas" | - [ ] Disponível no dashboard<br>- [ ] Exige confirmação antes de executar | must-have | done |

## Non-Functional Requirements

| ID   | Requirement | Metric/Target | Priority | Status |
|------|-------------|---------------|----------|--------|
| NF1  | Recuperação de configuração corrompida | Versão/CRC inválido no boot → reinicializa banco padrão seguro | must-have | done |
| NF2  | Clamp de segurança em runtime config | Timeout manual 1-120min, duração padrão 1-240min | must-have | done |
| NF3  | Dashboard standalone | Sem fontes ou recursos externos; funciona isolado no AP | must-have | done |
| NF4  | Push de status em tempo real | WebSocket porta 81, intervalo 2s, fallback polling HTTP | should-have | done |
| NF5  | Resiliência sem RTC | Sistema continua operacional sem hora real se RTC ausente | must-have | done |

## Constraints
- Hardware fixo: ESP32, 8 módulos de relé, OLED SSD1306 I2C, encoder rotativo, RTC DS3231, DHT11
- Modelo de agenda atual: 4 agendas globais (não 4 agendas por setor como o contrato original ABNT previa) — desalinhamento conhecido entre documentação e código
- Bibliotecas: U8g2, RTClib, ESP32Encoder, DHTesp/DHT Adafruit, WebSockets (Links2004, opcional)

## Out of Scope (v1)
- Testes automatizados (agenda, persistência, API web)
- UX de configuração mais rasa no OLED (fluxo atual é profundo)
- Modelo de 16 agendas (4 setores x 4 cada) do contrato original

## Success Criteria
1. Setor acionado corresponde ao selecionado, sem travamento na navegação
2. Dados de agenda e config persistem corretamente após reboot
3. Disparo automático ocorre no minuto esperado
4. Rotas da API respondem sem erro
5. Sistema opera corretamente com RTC ausente (sem hora real)

## Discussion Notes
- Sessão de discussão focada em documentar formalmente o v1 já implementado, sem mudança de código
- Requisitos extraídos do README.md (comportamento real do firmware) e confirmados pelo usuário
- Próximos passos sugeridos pelo próprio README: definir modelo final de agenda, evoluir UX de configurações, adicionar testes automatizados, checklist de homologação de campo
