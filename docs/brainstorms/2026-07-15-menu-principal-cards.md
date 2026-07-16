# Menu Principal — redesign em cards empilhados

**Data:** 2026-07-15
**Status:** Aprovado, aguardando plano de implementação

## Contexto

Interface atual do Menu Principal (`DisplayManager::desenharMenuPrincipal`) usa carrossel horizontal de ícones 32x32 com moldura tripla no item selecionado. Referência visual trazida pelo usuário: app "Pirx The Pilot" (receptor FPV STM32, display 128x64 monocromático, botão único), que usa cards verticais empilhados com highlight no item selecionado — ver [Designing for a 128x64 pixel display](https://uxdesign.cc/designing-for-a-128x64-pixel-display-3f3361aca7a0).

Ferramentas de apoio identificadas:
- [Wokwi OLED Animator](https://animator.wokwi.com/) — gera código C de animações prontas (ícones, loaders) pra Arduino/ESP32, tamanhos 32/48/64px, baseado no catálogo Icons8.
- Icons8 (catálogo de ícones) — acesso direto bloqueado (403), mas alcançável via Wokwi Animator.

## Decisão de escopo

Redesign cobre **somente o Menu Principal** (`EstadoMenu::STATUS` com `menuAtivo()==true`). As demais 6 telas (Status/relógio, Sensores, Irrigação, Programar, Config, WebServer) não mudam nesta iteração.

## Design aprovado

**Layout escolhido: Opção B — 2 linhas, destaque maior** (validado via mockup no companheiro visual de brainstorming).

### Estrutura visual

- **Linha superior — item selecionado** (~28px altura): borda 2px, ícone 32x32 à esquerda (reaproveita `desenharIconeMenuPrincipal` como está — coordenadas fixas, não escaláveis), label em fonte grande (`desenharTextoGrande`), seta `▸` à direita indicando "OK confirma/entra".
- **Linha inferior — próximo item** (dimmed): borda fina 1px, **sem ícone** (ícones atuais têm coordenadas fixas em 32x32, não escaláveis para uma versão reduzida sem reescrever os 5 desenhos vetoriais — decisão: manter simples, só texto), label em fonte padrão (`desenharTexto`).

### Comportamento de navegação

- Girar **horário**: item atualmente "próximo" (linha de baixo) vira selecionado (linha de cima); o item seguinte na lista aparece embaixo.
- Girar **anti-horário**: inverte a direção.
- **Wrap circular** nas pontas — do último item (Configurações) volta pro primeiro (Irrigar Agora), e vice-versa. Consistente com o wrap já existente em `MenuController::navegarProximo/navegarAnterior`.
- **Troca instantânea**, sem animação de frames — redesenha a cada chamada de `DisplayManager::atualizar()`, mesmo modelo usado hoje no resto do sistema. Sem loop de animação tipo o check/lixeira existente.

### Itens (ordem atual, sem mudança)

1. Irrigar Agora
2. Programar
3. WebServer
4. Sensores
5. Configurações

## Reuso de código existente

- Ícones: reaproveita `desenharIconeMenuPrincipal()` já existente, ajustando escala (hoje desenha em 32x32 fixo — precisa parametrizar tamanho ou criar variante reduzida para a linha "próximo" em 16x16).
- Dados de navegação: `MenuController` **não muda** — já expõe `itemSelecionado()`, `totalItens()`, `nomeItem(indice)`, e a navegação com wrap circular já está implementada em `navegarProximo()`/`navegarAnterior()`.
- Único ponto de mudança de código: `DisplayManager::desenharMenuPrincipal()` em `display_manager.cpp`.

## Fora de escopo (não fazer agora)

- Redesign das outras 6 telas.
- Animação de transição (slide) entre cards — descartada em favor de troca instantânea.
- Uso do Wokwi Animator para ícones animados — não solicitado para esta mudança específica; ícones atuais (desenho vetorial via primitivas `desenharLinha`/`desenharRetangulo`) são suficientes e mantêm consistência com o resto do sistema.
