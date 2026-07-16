# Brainstorm: Redesign do dashboard Status (tela inicial)

**Data:** 2026-07-16
**Contexto:** Menu Principal e submenus já migraram pro estilo cards com ícones Lucide. Última tela pendente de melhoria visual: `desenharTelaStatus` (dashboard inicial, `EstadoMenu::STATUS`), que ainda usa ícones custom 10x10 desenhados a mão.

## Problema identificado

Canto superior direito (x=82-128, y=0-26) mostra "ON N/N" + ícone de irrigação (torneira) — dado redundante, já repetido na linha abaixo (y=13: "MANUAL S1/8", "AUTO S2/8" etc). Ocupa espaço sem agregar informação nova.

## Decisão

1. **Remover** o bloco "ON N/N" + ícone de irrigação do canto superior direito.
2. **Substituir pelo status de conectividade WiFi** — informação que hoje não aparece em nenhum lugar do dashboard, mas é relevante (usuário depende do WebServer pra dashboard web). Só ícone, sem texto: `wifi` (Lucide) quando `_webAp.staConectada()==true`, `wifi-off` quando falso. 12x12px, canto superior direito (~x=112-124, y=2-14).
3. **Trocar os 3 ícones custom pequenos** (`desenharIconeStatusRelogio`, `desenharIconeStatusIrrigacao`, `desenharIconeStatusAgenda`, todos 10x10) por bitmaps Lucide equivalentes, aumentados pra 14x14 (espaço extra liberado pela remoção do "ON"):
   - Relógio (bloco superior) → `clock`
   - Irrigação (mantém indicação ativa/inativa, mas SEM o "ON N/N" ao lado — só o ícone já comunica via posição/contexto da linha de texto abaixo) → decisão: remover esse ícone específico também, já que ativa/inativa fica claro pelo texto da linha (y=13). Ver nota abaixo.
   - Próxima agenda (bloco do meio) → `calendar-clock` (mais específico que calendário genérico, já que a linha mostra dia+hora)

   Nota sobre irrigação: o ícone de torneira (`desenharIconeStatusIrrigacao`, x=82,y=3) ficava ao lado do "ON N/N" especificamente para reforçar visualmente o estado ativo/inativo. Como o texto da linha 13 já diz isso em palavras ("MANUAL S1/8", "Nenhum setor ativo" etc), remover o ícone junto com o "ON" simplifica sem perder informação — o espaço todo do canto direito vira só o indicador WiFi.

## Layout resultante (bloco superior, y=0-26)

```
[clock14] HH:MM:SS                          [wifi12]
MANUAL S1/8 AUTO S2/8  (ou "Nenhum setor ativo" etc)
```

- Ícone relógio: x=3, y=3, 14x14 (era 10x10 em x=3,y=3)
- Hora: reposicionar x pra acomodar ícone maior (era x=17, novo x≈21)
- WiFi: canto direito, x=OLED_LARGURA-16≈112, y=2, 12x12
- Linha de status de irrigação (y=13): sem mudança de posição/texto

## Bloco "Próxima agenda" (y=29-49)

- Ícone `desenharIconeStatusAgenda` (10x10, x=4,y=38) → `calendar-clock` 14x14, reajustar x do texto ao lado (era x=17, novo x≈21)

## Fora de escopo

- RSSI / força de sinal WiFi (só 2 estados: conectado/desconectado, decisão já tomada — não expor novo getter em WebApManager além do `staConectada()` já existente)
- `desenharIconeStatusValvula` — órfã, sem callers, não usada em nenhuma tela; não mexer agora (fora do pedido)
- Outras telas (Sensores, Irrigação, WebServer) — não fazem parte deste pedido

## Ícones Lucide novos necessários

`clock` (14x14, já usado em 28x28 nos cards — reaproveitar rasterização em novo tamanho), `droplets` (14x14, decisão: não usado nesta tela afinal — descartar da lista se a nota acima for aplicada), `calendar-clock` (14x14, novo), `wifi` (12x12, novo), `wifi-off` (12x12, novo).

## Próximo passo

Implementar direto (mudança contida em 1 função + novos arrays de bitmap + 1 novo getter opcional). Sem necessidade de plano formal de múltiplas tasks — é desenho puro, mesmo padrão já repetido 3x nesta sessão.
