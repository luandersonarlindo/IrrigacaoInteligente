#pragma once

// ============================================================
//  display_driver_oled.h — Driver do Display OLED SSD1306
//  Biblioteca: U8g2
//
//  Responsabilidade: abstrair o hardware do display.
//  NÃO decide o que mostrar — só sabe como mostrar.
//  Quem decide o conteúdo é o display_manager.
// ============================================================

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "Config.h"

class DisplayDriverOled
{
public:
    DisplayDriverOled();

    // Inicializa o display
    void begin();

    // Limpa o buffer interno
    void limpar();

    // Envia o buffer para o display (deve chamar após desenhar)
    void renderizar();

    // --- Primitivas de desenho ---
    void desenharTexto(int x, int y, const char *texto);
    void desenharTextoMini(int x, int y, const char *texto);
    void desenharTextoGrande(int x, int y, const char *texto);
    void desenharTextoFonteTitulo(int x, int y, const char *texto);
    void desenharLinha(int x1, int y1, int x2, int y2);
    void desenharRetangulo(int x, int y, int largura, int altura);
    void desenharRetanguloPreenchido(int x, int y, int largura, int altura);
    void desenharIcone32(int x, int y, uint16_t glifo);

    // Utilitários para layout
    int larguraTexto(const char *texto);

    // Cor de desenho: 1 = branco (padrão), 0 = preto (para texto sobre fundo branco)
    void setCorDesenho(uint8_t cor);

    // Altura de uma linha de texto na fonte padrão (útil para calcular posição)
    int alturaFonte();

private:
    // Modo: SSD1306 128x64, I2C, sem reset
    // U8G2_SSD1306_128X64_NONAME_F_HW_I2C usa buffer completo (mais fluido)
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C _u8g2;
};
