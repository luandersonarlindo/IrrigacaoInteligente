#pragma once

// ============================================================
//  display_driver_lcd16x2.h — Driver do LCD 16x2 via I2C
//  Biblioteca: LiquidCrystal_I2C
//
//  Responsabilidade: abstrair o hardware do display LCD.
//  NAO decide o que mostrar — apenas escreve linhas de texto.
// ============================================================

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Config.h"

class DisplayDriverLcd16x2
{
public:
    DisplayDriverLcd16x2();

    void begin();
    void limpar();
    void escreverLinha(uint8_t linha, const char *texto);
    void escreverLinha(uint8_t linha, const String &texto);
    void escreverTexto(uint8_t linha, uint8_t coluna, const char *texto);
    void renderizar();
    void setBacklight(bool ligado);
    bool backlightOn() const;

private:
    LiquidCrystal_I2C _lcd;
    char _buffer[LCD_LINHAS][LCD_COLUNAS + 1];
    char _ultimoRender[LCD_LINHAS][LCD_COLUNAS + 1];
    bool _precisaRender;
    bool _backlightOn;

    static void normalizarLinha(const String &origem, char destino[LCD_COLUNAS + 1]);
};
