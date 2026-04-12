// ============================================================
//  display_driver_oled.cpp — Implementação do DisplayDriverOled
// ============================================================

#include "display_driver_oled.h"

// U8G2_R0 = sem rotação
// U8X8_PIN_NONE = sem pino de reset
DisplayDriverOled::DisplayDriverOled()
    : _u8g2(U8G2_R0, U8X8_PIN_NONE)
{
}

void DisplayDriverOled::begin()
{
    _u8g2.begin();
    _u8g2.setFont(u8g2_font_6x10_tf); // Fonte padrão do sistema
    _u8g2.setFontRefHeightExtendedText();
    _u8g2.setDrawColor(1);
    _u8g2.setFontPosTop(); // y = topo do caractere (mais intuitivo)

    if (DEBUG_SERIAL)
    {
        Serial.println("[OLED] Inicializado.");
    }
}

void DisplayDriverOled::limpar()
{
    _u8g2.clearBuffer();
}

void DisplayDriverOled::renderizar()
{
    _u8g2.sendBuffer();
}

void DisplayDriverOled::desenharTexto(int x, int y, const char *texto)
{
    _u8g2.setFont(u8g2_font_6x10_tf);
    _u8g2.drawStr(x, y, texto);
}

void DisplayDriverOled::desenharTextoMini(int x, int y, const char *texto)
{
    _u8g2.setFont(u8g2_font_4x6_tf);
    _u8g2.drawStr(x, y, texto);
    _u8g2.setFont(u8g2_font_6x10_tf);
}

void DisplayDriverOled::desenharTextoGrande(int x, int y, const char *texto)
{
    // Fonte maior para títulos ou valores em destaque
    _u8g2.setFont(u8g2_font_10x20_tf);
    _u8g2.drawStr(x, y, texto);
    _u8g2.setFont(u8g2_font_6x10_tf); // Restaura fonte padrão
}

void DisplayDriverOled::desenharTextoFonteTitulo(int x, int y, const char *texto)
{
    _u8g2.setFont(u8g2_font_helvB10_tr);
    _u8g2.drawStr(x, y, texto);
    _u8g2.setFont(u8g2_font_6x10_tf);
}

void DisplayDriverOled::desenharLinha(int x1, int y1, int x2, int y2)
{
    _u8g2.drawLine(x1, y1, x2, y2);
}

void DisplayDriverOled::desenharRetangulo(int x, int y, int largura, int altura)
{
    _u8g2.drawFrame(x, y, largura, altura);
}

void DisplayDriverOled::desenharRetanguloPreenchido(int x, int y, int largura, int altura)
{
    _u8g2.drawBox(x, y, largura, altura);
}

void DisplayDriverOled::desenharIcone32(int x, int y, uint16_t glifo)
{
    _u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
    _u8g2.drawGlyph(x, y, glifo);
    _u8g2.setFont(u8g2_font_6x10_tf);
}

int DisplayDriverOled::larguraTexto(const char *texto)
{
    _u8g2.setFont(u8g2_font_6x10_tf);
    return _u8g2.getStrWidth(texto);
}

int DisplayDriverOled::alturaFonte()
{
    return _u8g2.getMaxCharHeight();
}

void DisplayDriverOled::setCorDesenho(uint8_t cor)
{
    _u8g2.setDrawColor(cor);
}
