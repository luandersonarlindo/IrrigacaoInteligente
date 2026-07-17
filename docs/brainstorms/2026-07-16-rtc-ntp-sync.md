# Brainstorm: RTC perde hora no boot + sincronização NTP

**Data:** 2026-07-16
**Contexto:** Usuário ajustou hora corretamente via dashboard web, mas após reiniciar o ESP32 a hora "saiu" (voltou errada).

## Diagnóstico

`RtcDriverDs3231::begin()` (`rtc_driver_ds3231.cpp:17-26`) chama `_rtc.adjust(DateTime(F(__DATE__), F(__TIME__)))` sempre que `perdeuEnergia()` (`DS3231::lostPower()`, lê o Oscillator Stop Flag) retorna `true`. Esse flag pode ficar setado por instabilidade elétrica momentânea no boot mesmo com a bateria CR2032 do módulo presente e carregada (comportamento documentado em módulos DS3231 genéricos) — não significa necessariamente perda real de dados de hora.

Quando isso dispara, o RTC é sobrescrito com `__DATE__`/`__TIME__` — a hora em que o firmware foi **compilado**, não a hora real atual. Isso explica o sintoma: hora certa ajustada via web, reinicia, RTC "esquece" e volta pra hora de compilação (que pode ser dias/semanas no passado).

## Decisões

1. **Remover o fallback perigoso.** Se `perdeuEnergia()==true`, não sobrescrever mais com hora de compilação — só logar aviso (`DEBUG_SERIAL`) e manter a hora que o RTC já tem (mesmo que potencialmente incerta). Evita regressão silenciosa da hora certa por um falso positivo do OSF.

2. **Sincronização automática via NTP.** Quando o WiFi (modo STA) conectar ou reconectar, buscar hora real via NTP (`configTime()` do ESP32, servidores padrão `pool.ntp.org`, fuso `UTC-3` fixo pra Brasília — sem horário de verão, já abolido no Brasil desde 2019) e ajustar o RTC automaticamente. Corrige tanto o caso de boot com RTC "perdido" quanto drift acumulado ao longo do tempo.

## Design

- `WebApManager::tentarConexaoSta()` (`web_ap_manager.cpp:2104`) já detecta a transição de status STA (`_ultimoStatusSta`), mas hoje só usa isso pra lógica de retry. Adicionar: quando a transição for `!= WL_CONNECTED` → `== WL_CONNECTED`, disparar sincronização NTP.
- Novo método privado `sincronizarHoraNtp()` em `WebApManager`: chama `configTime(-3*3600, 0, "pool.ntp.org", "a.st1.ntp.br")` (servidor brasileiro como segundo fallback, mais preciso pra fuso local), espera `getLocalTime()` retornar sucesso (com timeout curto, não bloquear o loop principal por muito tempo), converte pra `DateTime` e chama `_rtc.ajustarHora(...)`.
- Timeout de espera do NTP: síncrono mas curto (ex: até 5s, não mais — já que `tentarConexaoSta` roda dentro do loop principal, um bloqueio longo travaria toda a lógica de irrigação/agendas por vários ciclos).
- Falha de NTP (sem resposta dentro do timeout) não é erro fatal — só loga e segue; o RTC continua com o que já tinha.

## Fora de escopo

- Scraping de site (horariodebrasilia.org) — descartado, NTP é padrão mais confiável e nativo do ESP32.
- Horário de verão — não existe mais no Brasil (abolido 2019), fuso fixo UTC-3 é suficiente.
- Sincronização periódica independente de reconexão WiFi (ex: a cada X horas mesmo sem queda de conexão) — fora do pedido, pode ser considerado depois se drift for um problema real.

## Próximo passo

Implementar direto — mudança contida em 2 arquivos (`rtc_driver_ds3231.cpp` remove fallback; `web_ap_manager.h/.cpp` adiciona sincronização NTP no evento de conexão). Sem TDD formal (lógica depende de hardware real — RTC físico, WiFi real, servidor NTP — não testável em host nativo); validação por leitura cuidadosa + revisão + teste em campo.
