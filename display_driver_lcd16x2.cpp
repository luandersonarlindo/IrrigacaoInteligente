// ============================================================
//  display_driver_lcd16x2.cpp — Implementacao do DisplayDriverLcd16x2
// ============================================================

#include "display_driver_lcd16x2.h"

DisplayDriverLcd16x2::DisplayDriverLcd16x2()
    : _lcd(LCD_I2C_ENDERECO, LCD_COLUNAS, LCD_LINHAS),
      _precisaRender(true)
{
    for (int linha = 0; linha < LCD_LINHAS; linha++)
    {
        for (int coluna = 0; coluna < LCD_COLUNAS; coluna++)
        {
            _buffer[linha][coluna] = ' ';
            _ultimoRender[linha][coluna] = '\0';
        }
        _buffer[linha][LCD_COLUNAS] = '\0';
        _ultimoRender[linha][LCD_COLUNAS] = '\0';
    }
}

void DisplayDriverLcd16x2::begin()
{
    _lcd.init();
    _lcd.backlight();
    _lcd.clear();
    _precisaRender = true;

    if (DEBUG_SERIAL)
    {
        Serial.printf("[LCD] Inicializado: addr=0x%02X, %dx%d\n",
                      LCD_I2C_ENDERECO,
                      LCD_COLUNAS,
                      LCD_LINHAS);
    }
}

void DisplayDriverLcd16x2::limpar()
{
    for (int linha = 0; linha < LCD_LINHAS; linha++)
    {
        for (int coluna = 0; coluna < LCD_COLUNAS; coluna++)
        {
            _buffer[linha][coluna] = ' ';
        }
        _buffer[linha][LCD_COLUNAS] = '\0';
    }
    _precisaRender = true;
}

void DisplayDriverLcd16x2::normalizarLinha(const String &origem, char destino[LCD_COLUNAS + 1])
{
    int i = 0;
    for (; i < LCD_COLUNAS; i++)
    {
        if (i < (int)origem.length())
        {
            char c = origem[i];
            destino[i] = (c >= 32 && c <= 126) ? c : ' ';
        }
        else
        {
            destino[i] = ' ';
        }
    }
    destino[LCD_COLUNAS] = '\0';
}

void DisplayDriverLcd16x2::escreverLinha(uint8_t linha, const char *texto)
{
    if (linha >= LCD_LINHAS)
        return;

    String origem = texto ? String(texto) : String("");
    char normalizada[LCD_COLUNAS + 1];
    normalizarLinha(origem, normalizada);

    bool mudou = false;
    for (int coluna = 0; coluna < LCD_COLUNAS; coluna++)
    {
        if (_buffer[linha][coluna] != normalizada[coluna])
        {
            _buffer[linha][coluna] = normalizada[coluna];
            mudou = true;
        }
    }

    if (mudou)
    {
        _precisaRender = true;
    }
}

void DisplayDriverLcd16x2::escreverLinha(uint8_t linha, const String &texto)
{
    escreverLinha(linha, texto.c_str());
}

void DisplayDriverLcd16x2::escreverTexto(uint8_t linha, uint8_t coluna, const char *texto)
{
    if (linha >= LCD_LINHAS || coluna >= LCD_COLUNAS)
        return;

    if (texto == nullptr)
        return;

    bool mudou = false;
    int i = 0;
    while (texto[i] != '\0' && (coluna + i) < LCD_COLUNAS)
    {
        char c = texto[i];
        char normalizado = (c >= 32 && c <= 126) ? c : ' ';

        if (_buffer[linha][coluna + i] != normalizado)
        {
            _buffer[linha][coluna + i] = normalizado;
            mudou = true;
        }
        i++;
    }

    if (mudou)
    {
        _precisaRender = true;
    }
}

void DisplayDriverLcd16x2::renderizar()
{
    if (!_precisaRender)
    {
        return;
    }

    for (int linha = 0; linha < LCD_LINHAS; linha++)
    {
        bool mudouLinha = false;
        for (int coluna = 0; coluna < LCD_COLUNAS; coluna++)
        {
            if (_ultimoRender[linha][coluna] != _buffer[linha][coluna])
            {
                mudouLinha = true;
                break;
            }
        }

        if (!mudouLinha)
            continue;

        _lcd.setCursor(0, linha);
        _lcd.print(_buffer[linha]);

        for (int coluna = 0; coluna < LCD_COLUNAS; coluna++)
        {
            _ultimoRender[linha][coluna] = _buffer[linha][coluna];
        }
        _ultimoRender[linha][LCD_COLUNAS] = '\0';
    }

    _precisaRender = false;
}
