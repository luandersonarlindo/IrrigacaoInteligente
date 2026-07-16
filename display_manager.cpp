// ============================================================
//  display_manager.cpp — Implementação do DisplayManager
// ============================================================

#include "display_manager.h"
#include <string.h>

namespace
{
    constexpr int WEBSERVER_MAX_LINHAS = 24;
    constexpr int WEBSERVER_LINHAS_POR_PAGINA = 4;
    constexpr int WEBSERVER_Y_INICIAL = 14;
    constexpr int WEBSERVER_ALTURA_LINHA = 10;
    constexpr int WEBSERVER_LARGURA_MAX_PX = OLED_LARGURA - 2;
    int adicionarTextoQuebrado(DisplayDriverOled &display,
                               const String &textoOriginal,
                               String linhas[WEBSERVER_MAX_LINHAS],
                               int totalAtual)
    {
        if (totalAtual >= WEBSERVER_MAX_LINHAS)
        {
            return totalAtual;
        }

        String restante = textoOriginal;
        restante.trim();
        if (restante.length() == 0)
        {
            return totalAtual;
        }

        while (restante.length() > 0 && totalAtual < WEBSERVER_MAX_LINHAS)
        {
            int tamanho = (int)restante.length();
            int corte = tamanho;

            while (corte > 0)
            {
                String candidata = restante.substring(0, corte);
                candidata.trim();

                if (candidata.length() == 0)
                {
                    corte--;
                    continue;
                }

                if (display.larguraTexto(candidata.c_str()) <= WEBSERVER_LARGURA_MAX_PX)
                {
                    break;
                }

                int ultimoEspaco = candidata.lastIndexOf(' ');
                if (ultimoEspaco > 0)
                {
                    corte = ultimoEspaco;
                }
                else
                {
                    corte--;
                }
            }

            if (corte <= 0)
            {
                corte = 1;
                while (corte < tamanho)
                {
                    String candidata = restante.substring(0, corte + 1);
                    if (display.larguraTexto(candidata.c_str()) > WEBSERVER_LARGURA_MAX_PX)
                    {
                        break;
                    }
                    corte++;
                }
            }

            String linha = restante.substring(0, corte);
            linha.trim();
            if (linha.length() == 0)
            {
                linha = restante.substring(0, 1);
                corte = 1;
            }

            linhas[totalAtual++] = linha;

            if (corte >= tamanho)
            {
                break;
            }

            restante = restante.substring(corte);
            restante.trim();
        }

        return totalAtual;
    }

    // Ícones dos submenus de Configurações, gerados a partir da biblioteca
    // Lucide (lucide.dev, ISC license) via rasterização SVG->1bpp 28x28.
    // Lucide "clock" 28x28, 1bpp
    const uint8_t ICON_CLOCK_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x01, 0x00, 0x00, 0xFF, 0x0F, 0x00,
        0x80, 0xFF, 0x1F, 0x00, 0xE0, 0x03, 0x7C, 0x00, 0xF0, 0x00, 0xF0, 0x00,
        0x70, 0x60, 0xE0, 0x00, 0x38, 0x60, 0xC0, 0x01, 0x1C, 0x60, 0x80, 0x03,
        0x1C, 0x60, 0x80, 0x03, 0x0C, 0x60, 0x00, 0x03, 0x0E, 0x60, 0x00, 0x07,
        0x0E, 0x60, 0x00, 0x07, 0x0E, 0xE0, 0x00, 0x07, 0x0E, 0xE0, 0x01, 0x07,
        0x0E, 0xC0, 0x07, 0x07, 0x0E, 0x00, 0x0F, 0x07, 0x0C, 0x00, 0x00, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x38, 0x00, 0xC0, 0x01,
        0x70, 0x00, 0xE0, 0x00, 0xF0, 0x00, 0xF0, 0x00, 0xE0, 0x03, 0x7C, 0x00,
        0x80, 0xFF, 0x1F, 0x00, 0x00, 0xFF, 0x0F, 0x00, 0x00, 0xF8, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "settings" 28x28, 1bpp
    const uint8_t ICON_SETTINGS_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0xF8, 0x01, 0x00,
        0x00, 0xFC, 0x03, 0x00, 0x00, 0x9C, 0x03, 0x00, 0x00, 0x0C, 0x03, 0x00,
        0xF0, 0x0F, 0xFF, 0x00, 0xF8, 0x07, 0xFE, 0x01, 0x18, 0x01, 0x88, 0x01,
        0x1C, 0x60, 0x80, 0x03, 0x1C, 0xF8, 0x81, 0x03, 0x38, 0xFC, 0xC3, 0x01,
        0x70, 0x0C, 0xE3, 0x00, 0x60, 0x0E, 0x67, 0x00, 0x60, 0x0E, 0x67, 0x00,
        0x70, 0x0C, 0xE3, 0x00, 0x38, 0xFC, 0xC3, 0x01, 0x1C, 0xF8, 0x81, 0x03,
        0x1C, 0x60, 0x80, 0x03, 0x18, 0x00, 0x80, 0x01, 0xF8, 0x07, 0xFE, 0x01,
        0xF0, 0x0F, 0xFF, 0x00, 0x00, 0x0C, 0x03, 0x00, 0x00, 0x9C, 0x03, 0x00,
        0x00, 0xFC, 0x03, 0x00, 0x00, 0xF8, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "timer" 28x28, 1bpp
    const uint8_t ICON_TIMER_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x01, 0x00, 0x00, 0xF8, 0x01, 0x00,
        0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xF8, 0x01, 0x00, 0x00, 0xFE, 0x07, 0x00, 0x80, 0x9F, 0x1F, 0x00,
        0xC0, 0x03, 0x3C, 0x00, 0xE0, 0x00, 0x70, 0x00, 0xE0, 0x00, 0x70, 0x00,
        0x70, 0x00, 0xE7, 0x00, 0x30, 0x80, 0xC3, 0x00, 0x30, 0xC0, 0xC1, 0x00,
        0x30, 0xE0, 0xC0, 0x00, 0x38, 0x60, 0xC0, 0x01, 0x30, 0x00, 0xC0, 0x00,
        0x30, 0x00, 0xC0, 0x00, 0x70, 0x00, 0xE0, 0x00, 0x70, 0x00, 0xE0, 0x00,
        0xE0, 0x00, 0x70, 0x00, 0xC0, 0x01, 0x38, 0x00, 0xC0, 0x03, 0x3C, 0x00,
        0x00, 0xFF, 0x0F, 0x00, 0x00, 0xFE, 0x07, 0x00, 0x00, 0xF0, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "calendar-days" 28x28, 1bpp
    const uint8_t ICON_CALENDAR_DAYS_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x07, 0x0E, 0x00,
        0x00, 0x07, 0x0E, 0x00, 0xF8, 0xFF, 0xFF, 0x01, 0xF8, 0xFF, 0xFF, 0x01,
        0x1C, 0x07, 0x8E, 0x03, 0x1C, 0x03, 0x8C, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0xFC, 0xFF, 0xFF, 0x03,
        0xFC, 0xFF, 0xFF, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x1C, 0x62, 0x84, 0x03, 0x1C, 0x63, 0x8C, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0x63, 0x8C, 0x03,
        0x1C, 0x63, 0x8C, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x3C, 0x00, 0xC0, 0x03, 0xF8, 0xFF, 0xFF, 0x01, 0xF0, 0xFF, 0xFF, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "calendar" 28x28, 1bpp
    const uint8_t ICON_CALENDAR_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x07, 0x0E, 0x00,
        0x00, 0x07, 0x0E, 0x00, 0xF8, 0xFF, 0xFF, 0x01, 0xF8, 0xFF, 0xFF, 0x01,
        0x1C, 0x07, 0x8E, 0x03, 0x1C, 0x03, 0x8C, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0xFC, 0xFF, 0xFF, 0x03,
        0xFC, 0xFF, 0xFF, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x3C, 0x00, 0xC0, 0x03, 0xF8, 0xFF, 0xFF, 0x01, 0xF0, 0xFF, 0xFF, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "calendar-range" 28x28, 1bpp
    const uint8_t ICON_CALENDAR_RANGE_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x07, 0x0E, 0x00,
        0x00, 0x07, 0x0E, 0x00, 0xF8, 0xFF, 0xFF, 0x01, 0xF8, 0xFF, 0xFF, 0x01,
        0x1C, 0x07, 0x8E, 0x03, 0x1C, 0x03, 0x8C, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0xFC, 0xFF, 0xFF, 0x03,
        0xFC, 0xFF, 0xFF, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x9C, 0xF1, 0x9F, 0x03, 0x9C, 0xF1, 0x9F, 0x03, 0x1C, 0xE0, 0x8F, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x9C, 0xFF, 0x98, 0x03,
        0x9C, 0xFF, 0x98, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x3C, 0x00, 0xC0, 0x03, 0xF8, 0xFF, 0xFF, 0x01, 0xF0, 0xFF, 0xFF, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "hourglass" 28x28, 1bpp
    const uint8_t ICON_HOURGLASS_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x7F, 0x00, 0xE0, 0xFF, 0x7F, 0x00,
        0xC0, 0xFF, 0x3F, 0x00, 0x80, 0x01, 0x18, 0x00, 0x80, 0x01, 0x18, 0x00,
        0x80, 0x01, 0x18, 0x00, 0x80, 0x01, 0x18, 0x00, 0x80, 0x03, 0x1C, 0x00,
        0x00, 0x07, 0x0E, 0x00, 0x00, 0x0E, 0x07, 0x00, 0x00, 0x9C, 0x03, 0x00,
        0x00, 0xF8, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00,
        0x00, 0xF8, 0x01, 0x00, 0x00, 0x9C, 0x03, 0x00, 0x00, 0x0E, 0x07, 0x00,
        0x00, 0x07, 0x0E, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x01, 0x18, 0x00,
        0x80, 0x01, 0x18, 0x00, 0x80, 0x01, 0x18, 0x00, 0x80, 0x01, 0x18, 0x00,
        0x80, 0x03, 0x1C, 0x00, 0xE0, 0xFF, 0x7F, 0x00, 0xE0, 0xFF, 0x7F, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "save" 28x28, 1bpp
    const uint8_t ICON_SAVE_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x03, 0x00,
        0xF8, 0xFF, 0x0F, 0x00, 0xF8, 0xFF, 0x1F, 0x00, 0x9C, 0x01, 0x38, 0x00,
        0x9C, 0x01, 0x70, 0x00, 0x9C, 0x01, 0xE0, 0x00, 0x9C, 0xFF, 0xC3, 0x01,
        0x9C, 0xFF, 0x87, 0x03, 0x1C, 0xFE, 0x83, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0xFF, 0x8F, 0x03,
        0x9C, 0xFF, 0x9F, 0x03, 0x9C, 0x01, 0x98, 0x03, 0x9C, 0x01, 0x98, 0x03,
        0x9C, 0x01, 0x98, 0x03, 0x9C, 0x01, 0x98, 0x03, 0x9C, 0x01, 0x98, 0x03,
        0x9C, 0x01, 0x98, 0x03, 0x9C, 0x01, 0x98, 0x03, 0xF8, 0xFF, 0xFF, 0x01,
        0xF8, 0xFF, 0xFF, 0x01, 0xE0, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "droplets" 28x28, 1bpp
    const uint8_t ICON_DROPLETS_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x80, 0x03, 0x00, 0x00, 0x80, 0x03, 0x00, 0x80, 0xC1, 0x03, 0x00,
        0x80, 0xC3, 0x07, 0x00, 0xC0, 0xC3, 0x07, 0x00, 0xC0, 0x47, 0x0E, 0x00,
        0xE0, 0x07, 0x1C, 0x00, 0x70, 0x0E, 0x38, 0x00, 0x38, 0x1C, 0x70, 0x00,
        0x18, 0x38, 0xE0, 0x00, 0x1C, 0x38, 0xC0, 0x01, 0x1C, 0x30, 0xC0, 0x01,
        0x18, 0x38, 0x80, 0x01, 0x18, 0x38, 0x80, 0x03, 0x38, 0x1C, 0x80, 0x03,
        0xF0, 0x0F, 0x80, 0x03, 0xC0, 0x07, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01,
        0x00, 0x00, 0xC0, 0x01, 0x00, 0x04, 0xE0, 0x00, 0x00, 0x1E, 0xF0, 0x00,
        0x00, 0x7C, 0x7E, 0x00, 0x00, 0xF0, 0x1F, 0x00, 0x00, 0xC0, 0x07, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "trash-2" 28x28, 1bpp
    const uint8_t ICON_TRASH_2_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x03, 0x00, 0x00, 0xFE, 0x07, 0x00,
        0x00, 0xFF, 0x0F, 0x00, 0x00, 0x07, 0x0E, 0x00, 0x00, 0x07, 0x0E, 0x00,
        0xFC, 0xFF, 0xFF, 0x03, 0xFC, 0xFF, 0xFF, 0x03, 0x60, 0x00, 0x60, 0x00,
        0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00,
        0x60, 0x98, 0x61, 0x00, 0x60, 0x9C, 0x63, 0x00, 0x60, 0x9C, 0x63, 0x00,
        0x60, 0x9C, 0x63, 0x00, 0x60, 0x9C, 0x63, 0x00, 0x60, 0x9C, 0x63, 0x00,
        0x60, 0x9C, 0x63, 0x00, 0x60, 0x9C, 0x63, 0x00, 0x60, 0x98, 0x61, 0x00,
        0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00, 0x60, 0x00,
        0xE0, 0x00, 0x70, 0x00, 0xE0, 0xFF, 0x7F, 0x00, 0xC0, 0xFF, 0x3F, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "rotate-ccw" 28x28, 1bpp
    const uint8_t ICON_ROTATE_CCW_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x60, 0x00, 0x00,
        0x1C, 0xFE, 0x07, 0x00, 0x9C, 0xFF, 0x1F, 0x00, 0xDC, 0x03, 0x3C, 0x00,
        0xFC, 0x01, 0x78, 0x00, 0x7C, 0x00, 0xE0, 0x00, 0xFC, 0x03, 0xE0, 0x00,
        0xFC, 0x03, 0xC0, 0x01, 0xF8, 0x01, 0x80, 0x01, 0x00, 0x00, 0x80, 0x01,
        0x00, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03, 0x1C, 0x00, 0x80, 0x03,
        0x1C, 0x00, 0x80, 0x03, 0x18, 0x00, 0x80, 0x01, 0x18, 0x00, 0x80, 0x01,
        0x38, 0x00, 0xC0, 0x01, 0x70, 0x00, 0xE0, 0x00, 0x70, 0x00, 0xE0, 0x00,
        0xE0, 0x01, 0x78, 0x00, 0xC0, 0x03, 0x3C, 0x00, 0x80, 0xFF, 0x1F, 0x00,
        0x00, 0xFE, 0x07, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "info" 28x28, 1bpp
    const uint8_t ICON_INFO_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x01, 0x00, 0x00, 0xFF, 0x0F, 0x00,
        0x80, 0xFF, 0x1F, 0x00, 0xE0, 0x03, 0x7C, 0x00, 0xF0, 0x00, 0xF0, 0x00,
        0x70, 0x00, 0xE0, 0x00, 0x38, 0x00, 0xC0, 0x01, 0x1C, 0x60, 0x80, 0x03,
        0x1C, 0x60, 0x80, 0x03, 0x0C, 0x00, 0x00, 0x03, 0x0E, 0x00, 0x00, 0x07,
        0x0E, 0x00, 0x00, 0x07, 0x0E, 0x60, 0x00, 0x07, 0x0E, 0x60, 0x00, 0x07,
        0x0E, 0x60, 0x00, 0x07, 0x0E, 0x60, 0x00, 0x07, 0x0C, 0x60, 0x00, 0x03,
        0x1C, 0x60, 0x80, 0x03, 0x1C, 0x60, 0x80, 0x03, 0x38, 0x00, 0xC0, 0x01,
        0x70, 0x00, 0xE0, 0x00, 0xF0, 0x00, 0xF0, 0x00, 0xE0, 0x03, 0x7C, 0x00,
        0x80, 0xFF, 0x1F, 0x00, 0x00, 0xFF, 0x0F, 0x00, 0x00, 0xF8, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Lucide "corner-up-left" 28x28, 1bpp
    const uint8_t ICON_CORNER_UP_LEFT_28X28[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
        0x80, 0x03, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00,
        0xF0, 0xFF, 0x07, 0x00, 0xF8, 0xFF, 0x3F, 0x00, 0xF0, 0xFF, 0x7F, 0x00,
        0xE0, 0x00, 0xE0, 0x00, 0xC0, 0x01, 0xC0, 0x00, 0x80, 0x03, 0xC0, 0x00,
        0x00, 0x07, 0xC0, 0x01, 0x00, 0x0E, 0xC0, 0x01, 0x00, 0x04, 0xC0, 0x01,
        0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0xC0, 0x01,
        0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0xC0, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00};

    // Wi-Fi icon derived from the provided icons8 image (id 9922), converted to 1-bit 24x18.
    const uint8_t WIFI_ICON_9922_24X18[] = {
        0x00, 0xFF, 0x00, 0xE0, 0xFF, 0x07, 0xF8, 0xFF, 0x1F, 0xFC, 0xFF, 0x3F,
        0xFE, 0x00, 0x7F, 0x3F, 0x00, 0xFC, 0x0E, 0x7E, 0x70, 0xC4, 0xFF, 0x23,
        0xE0, 0xFF, 0x07, 0xF0, 0xFF, 0x0F, 0xE0, 0x81, 0x07, 0xC0, 0x00, 0x03,
        0x00, 0x3C, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x3C, 0x00,
        0x00, 0x18, 0x00, 0x00, 0x00, 0x00};

    // Corta o texto (com "..." no final) até caber em larguraMaxPx na fonte padrão.
    void truncarParaLargura(DisplayDriverOled &d, char *texto, int larguraMaxPx)
    {
        if (d.larguraTexto(texto) <= larguraMaxPx)
            return;

        int len = (int)strlen(texto);
        while (len > 0)
        {
            len--;
            texto[len] = '\0';
            char comReticencias[40];
            snprintf(comReticencias, sizeof(comReticencias), "%s...", texto);
            if (d.larguraTexto(comReticencias) <= larguraMaxPx)
            {
                strncpy(texto, comReticencias, len + 4);
                texto[len + 4] = '\0';
                return;
            }
        }
    }

    void desenharBitmap1Bpp(DisplayDriverOled &d,
                            int x,
                            int y,
                            int largura,
                            int altura,
                            const uint8_t *dados)
    {
        int bytesPorLinha = (largura + 7) / 8;

        for (int linha = 0; linha < altura; linha++)
        {
            int inicioSegmento = -1;

            for (int coluna = 0; coluna <= largura; coluna++)
            {
                bool aceso = false;
                if (coluna < largura)
                {
                    int indiceByte = (linha * bytesPorLinha) + (coluna / 8);
                    uint8_t mascara = (uint8_t)(1U << (coluna % 8));
                    aceso = (dados[indiceByte] & mascara) != 0;
                }

                if (aceso && inicioSegmento < 0)
                {
                    inicioSegmento = coluna;
                }
                else if (!aceso && inicioSegmento >= 0)
                {
                    d.desenharLinha(x + inicioSegmento, y + linha, x + coluna - 1, y + linha);
                    inicioSegmento = -1;
                }
            }
        }
    }

    // Ícones simples em linha para OLED monocromático.
    void desenharIconeMenuPrincipal(DisplayDriverOled &d, int x, int y, int tipo)
    {
        // Moldura base 32x32
        d.desenharRetangulo(x, y, 32, 32);

        if (tipo == 0)
        {
            // Irrigar agora: aspersor com gotas e jatos (estilo técnico mono)
            d.desenharRetanguloPreenchido(x + 11, y + 4, 10, 4); // topo do bico
            d.desenharRetanguloPreenchido(x + 13, y + 9, 6, 11); // corpo central

            // gotas superiores
            d.desenharRetanguloPreenchido(x + 3, y + 8, 2, 2);
            d.desenharRetanguloPreenchido(x + 8, y + 11, 2, 2);
            d.desenharRetanguloPreenchido(x + 24, y + 11, 2, 2);
            d.desenharRetanguloPreenchido(x + 29, y + 8, 2, 2);

            // jatos inferiores em triângulo
            d.desenharLinha(x + 6, y + 28, x + 9, y + 22);
            d.desenharLinha(x + 9, y + 22, x + 12, y + 28);

            d.desenharLinha(x + 12, y + 28, x + 15, y + 22);
            d.desenharLinha(x + 15, y + 22, x + 18, y + 28);

            d.desenharLinha(x + 18, y + 28, x + 21, y + 22);
            d.desenharLinha(x + 21, y + 22, x + 24, y + 28);

            d.desenharLinha(x + 24, y + 28, x + 27, y + 22);
            d.desenharLinha(x + 27, y + 22, x + 30, y + 28);
        }
        else if (tipo == 1)
        {
            // Programar: calculadora/teclado numérico
            d.desenharRetangulo(x + 5, y + 4, 22, 24);
            d.desenharRetangulo(x + 8, y + 7, 16, 4); // display superior

            // grade de teclas 3x3
            int kx = x + 9;
            int ky = y + 14;
            for (int r = 0; r < 3; r++)
            {
                for (int c = 0; c < 3; c++)
                {
                    d.desenharRetanguloPreenchido(kx + (c * 5), ky + (r * 4), 2, 2);
                }
            }

            // tecla lateral (estilo função)
            d.desenharRetanguloPreenchido(x + 24, y + 14, 2, 10);
        }
        else if (tipo == (int)ItemMenu::WEBSERVER)
        {
            // WebServer: Wi-Fi icon based on the image provided by the user.
            desenharBitmap1Bpp(d, x + 4, y + 7, 24, 18, WIFI_ICON_9922_24X18);
        }
        else if (tipo == (int)ItemMenu::SENSORES)
        {
            // Sensores: termometro + gota (icone simples)
            d.desenharRetangulo(x + 7, y + 6, 6, 18);
            d.desenharRetanguloPreenchido(x + 9, y + 14, 2, 8);
            d.desenharRetanguloPreenchido(x + 6, y + 22, 8, 8);

            d.desenharLinha(x + 22, y + 10, x + 24, y + 14);
            d.desenharLinha(x + 24, y + 14, x + 26, y + 10);
            d.desenharLinha(x + 22, y + 10, x + 24, y + 6);
            d.desenharLinha(x + 24, y + 6, x + 26, y + 10);
        }
        else
        {
            // Configurações: sliders
            d.desenharLinha(x + 6, y + 10, x + 26, y + 10);
            d.desenharLinha(x + 6, y + 16, x + 26, y + 16);
            d.desenharLinha(x + 6, y + 22, x + 26, y + 22);
            d.desenharRetanguloPreenchido(x + 10, y + 8, 4, 4);
            d.desenharRetanguloPreenchido(x + 18, y + 14, 4, 4);
            d.desenharRetanguloPreenchido(x + 13, y + 20, 4, 4);
        }
    }

    void desenharIconeSubmenu(DisplayDriverOled &d, int x, int y, int opcao)
    {
        // área útil: 8x8
        if (opcao == 0)
        {
            // Hora: relógio
            d.desenharRetangulo(x, y, 8, 8);
            d.desenharLinha(x + 4, y + 4, x + 4, y + 2);
            d.desenharLinha(x + 4, y + 4, x + 6, y + 5);
        }
        else if (opcao == 1)
        {
            // Minuto: cronômetro
            d.desenharRetangulo(x + 1, y + 1, 6, 6);
            d.desenharRetanguloPreenchido(x + 3, y, 2, 1);
            d.desenharLinha(x + 4, y + 4, x + 5, y + 3);
        }
        else if (opcao == 2)
        {
            // Duração: barra
            d.desenharRetangulo(x, y + 2, 8, 4);
            d.desenharRetanguloPreenchido(x + 1, y + 3, 5, 2);
        }
        else if (opcao == 3)
        {
            // Dias: calendário mini
            d.desenharRetangulo(x, y + 1, 8, 7);
            d.desenharLinha(x, y + 3, x + 7, y + 3);
            d.desenharRetanguloPreenchido(x + 2, y, 1, 2);
            d.desenharRetanguloPreenchido(x + 5, y, 1, 2);
        }
        else if (opcao == 4)
        {
            // Setores: grade 2x2
            d.desenharRetangulo(x, y, 3, 3);
            d.desenharRetangulo(x + 5, y, 3, 3);
            d.desenharRetangulo(x, y + 5, 3, 3);
            d.desenharRetangulo(x + 5, y + 5, 3, 3);
        }
        else if (opcao == 5)
        {
            // Salvar: disquete simplificado
            d.desenharRetangulo(x, y, 8, 8);
            d.desenharRetanguloPreenchido(x + 1, y + 1, 5, 2);
            d.desenharRetangulo(x + 2, y + 5, 4, 2);
        }
        else
        {
            // Excluir: lixeira simplificada
            d.desenharRetangulo(x + 1, y + 2, 6, 6);
            d.desenharRetanguloPreenchido(x, y + 1, 8, 1);
            d.desenharRetanguloPreenchido(x + 3, y, 2, 1);
        }
    }

    void desenharIconeAgendaSlot(DisplayDriverOled &d, int x, int y, bool ativa)
    {
        // Moldura do slot
        d.desenharRetangulo(x, y, 16, 16);
        d.desenharRetangulo(x + 2, y + 2, 12, 4);

        if (ativa)
        {
            // Relógio pequeno (agenda ativa)
            d.desenharRetangulo(x + 4, y + 8, 8, 6);
            d.desenharLinha(x + 8, y + 11, x + 8, y + 9);
            d.desenharLinha(x + 8, y + 11, x + 10, y + 12);
        }
        else
        {
            // Sinal de mais (slot vazio)
            d.desenharLinha(x + 8, y + 8, x + 8, y + 13);
            d.desenharLinha(x + 6, y + 11, x + 10, y + 11);
        }
    }

    void desenharIconeStatusIrrigacao(DisplayDriverOled &d, int x, int y, bool ativa)
    {
        d.desenharRetangulo(x, y, 10, 10);
        d.desenharRetanguloPreenchido(x + 4, y + 2, 2, 4);
        d.desenharLinha(x + 2, y + 7, x + 7, y + 7);

        if (ativa)
        {
            d.desenharRetanguloPreenchido(x + 1, y + 8, 1, 1);
            d.desenharRetanguloPreenchido(x + 4, y + 8, 1, 1);
            d.desenharRetanguloPreenchido(x + 7, y + 8, 1, 1);
        }
    }

    void desenharIconeStatusValvula(DisplayDriverOled &d, int x, int y)
    {
        d.desenharRetangulo(x, y, 10, 10);
        d.desenharRetanguloPreenchido(x + 2, y + 4, 6, 2);
        d.desenharLinha(x + 2, y + 2, x + 4, y + 4);
        d.desenharLinha(x + 8, y + 2, x + 6, y + 4);
        d.desenharLinha(x + 2, y + 8, x + 4, y + 6);
        d.desenharLinha(x + 8, y + 8, x + 6, y + 6);
    }

    void desenharIconeStatusAgenda(DisplayDriverOled &d, int x, int y)
    {
        d.desenharRetangulo(x, y + 1, 10, 9);
        d.desenharLinha(x, y + 3, x + 9, y + 3);
        d.desenharRetanguloPreenchido(x + 2, y, 1, 2);
        d.desenharRetanguloPreenchido(x + 7, y, 1, 2);
        d.desenharRetanguloPreenchido(x + 2, y + 5, 1, 1);
        d.desenharRetanguloPreenchido(x + 4, y + 5, 1, 1);
        d.desenharRetanguloPreenchido(x + 6, y + 5, 1, 1);
    }

    void desenharIconeStatusRelogio(DisplayDriverOled &d, int x, int y)
    {
        d.desenharRetangulo(x, y, 10, 10);
        d.desenharLinha(x + 5, y + 5, x + 5, y + 2);
        d.desenharLinha(x + 5, y + 5, x + 7, y + 6);
    }

    void desenharAnimacaoCheck(DisplayDriverOled &d, int x, int y)
    {
        // Animacao simples baseada no tempo (4 quadros em loop)
        const unsigned long quadroMs = 90;
        int quadro = (millis() / quadroMs) % 4;

        // Caixa do check
        d.desenharRetangulo(x, y, 10, 10);

        // Quadro 1: primeiro segmento curto
        if (quadro >= 1)
        {
            d.desenharLinha(x + 2, y + 5, x + 4, y + 7);
        }

        // Quadro 2: alonga primeiro segmento
        if (quadro >= 2)
        {
            d.desenharLinha(x + 3, y + 6, x + 5, y + 8);
        }

        // Quadro 3+: completa check
        if (quadro >= 3)
        {
            d.desenharLinha(x + 4, y + 7, x + 8, y + 3);
            d.desenharLinha(x + 5, y + 8, x + 9, y + 4);
        }
    }

    void desenharSegmentoParcial(DisplayDriverOled &d, int x1, int y1, int x2, int y2, int passo, int totalPassos)
    {
        if (passo <= 0)
            return;
        if (passo > totalPassos)
            passo = totalPassos;

        int dx = x2 - x1;
        int dy = y2 - y1;
        int xp = x1 + (dx * passo) / totalPassos;
        int yp = y1 + (dy * passo) / totalPassos;
        d.desenharLinha(x1, y1, xp, yp);
    }

    void desenharAnimacaoCheck48(DisplayDriverOled &d, int frame)
    {
        const int x = 40;
        const int y = 8;
        const int w = 48;
        const int h = 48;
        d.desenharRetangulo(x, y, w, h);

        // Coordenadas do check dentro da caixa 48x48
        const int ax = x + 12;
        const int ay = y + 26;
        const int bx = x + 21;
        const int by = y + 35;
        const int cx = x + 37;
        const int cy = y + 18;

        // 16 quadros totais por loop
        // quadros 0..7: primeira perna, 8..15: segunda perna
        if (frame >= 2)
        {
            int p1 = frame - 1;
            desenharSegmentoParcial(d, ax, ay, bx, by, p1, 7);
            desenharSegmentoParcial(d, ax, ay + 1, bx, by + 1, p1, 7);
        }

        if (frame >= 8)
        {
            int p2 = frame - 7;
            desenharSegmentoParcial(d, bx, by, cx, cy, p2, 8);
            desenharSegmentoParcial(d, bx, by + 1, cx, cy + 1, p2, 8);
        }
    }

    void desenharAnimacaoLixeira48(DisplayDriverOled &d, int frame)
    {
        const int x = 40;
        const int y = 8;

        // Corpo da lixeira
        d.desenharRetangulo(x + 12, y + 10, 24, 30);

        // Tampa animada (abre e fecha)
        int fase = frame % 16;
        int off = 0;
        if (fase >= 3 && fase <= 7)
            off = (fase - 2);
        else if (fase >= 8 && fase <= 12)
            off = (13 - fase);

        d.desenharLinha(x + 10, y + 9 - off, x + 38, y + 9 - off);
        d.desenharRetanguloPreenchido(x + 22, y + 7 - off, 4, 2);

        // Linhas internas
        d.desenharLinha(x + 18, y + 14, x + 18, y + 36);
        d.desenharLinha(x + 24, y + 14, x + 24, y + 36);
        d.desenharLinha(x + 30, y + 14, x + 30, y + 36);

        // "Papel" caindo durante a abertura
        if (fase >= 4 && fase <= 11)
        {
            int py = y + 10 + (fase - 4) * 3;
            if (py < y + 38)
                d.desenharRetanguloPreenchido(x + 24, py, 3, 3);
        }
    }

}

DisplayManager::DisplayManager(DisplayDriverOled &display,
                               MenuController &menu,
                               RtcDriverDs3231 &rtc,
                               IrrigationController &irrigacao,
                               WebApManager &webAp,
                               Dht11Driver &dht11)
    : _display(display),
      _menu(menu),
      _rtc(rtc),
      _dht11(dht11),
      _irrigacao(irrigacao),
      _webAp(webAp),
      _ultimaAtualizacao(0),
      _agendaExecucaoAtiva(false),
      _agendaAguardandoIntervalo(false),
      _agendaSetoresEmLote(0),
      _agendaSetoresPendentes(0),
      _agendaSetoresLoteMask(0),
      _agendaSetoresPendentesMask(0)
{
}

void DisplayManager::atualizarEstadoAgendaSequencial(bool ativa,
                                                     bool aguardandoIntervalo,
                                                     uint8_t setoresEmLote,
                                                     uint8_t setoresPendentes,
                                                     uint16_t setoresLoteMask,
                                                     uint16_t setoresPendentesMask)
{
    _agendaExecucaoAtiva = ativa;
    _agendaAguardandoIntervalo = aguardandoIntervalo;
    _agendaSetoresEmLote = setoresEmLote;
    _agendaSetoresPendentes = setoresPendentes;
    _agendaSetoresLoteMask = setoresLoteMask;
    _agendaSetoresPendentesMask = setoresPendentesMask;
}

void DisplayManager::begin()
{
    if (DEBUG_SERIAL)
    {
        Serial.println("[DisplayManager] Inicializado.");
    }
}

void DisplayManager::atualizar()
{
    unsigned long agora = millis();
    unsigned long intervalo = INTERVALO_ATUALIZACAO_MS;
    if (_menu.menuAtivo())
        intervalo = 80;

    if (agora - _ultimaAtualizacao < intervalo)
        return;
    _ultimaAtualizacao = agora;

    _display.limpar();

    switch (_menu.estadoAtual())
    {
    case EstadoMenu::STATUS:
        // Status puro = tela de relógio
        // Menu ativo = usuário começou a navegar
        if (_menu.menuAtivo())
            desenharMenuPrincipal();
        else
            desenharTelaStatus();
        break;
    case EstadoMenu::IRRIGACAO_MANUAL:
        desenharTelaIrrigacao();
        break;
    case EstadoMenu::PROGRAMAR:
        desenharTelaProgramar();
        break;
    case EstadoMenu::CONFIGURACOES:
        desenharTelaConfig();
        break;
    case EstadoMenu::WEBSERVER:
        desenharTelaWebServer();
        break;
    case EstadoMenu::SENSORES:
        desenharTelaSensores();
        break;
    }

    _display.renderizar();
}

// ============================================================
//  Telas
// ============================================================

void DisplayManager::desenharMenuPrincipal()
{
    int totalItens = _menu.totalItens();
    int itemAtual = _menu.itemSelecionado();
    int itemProximo = MenuCards::proximoIndice(itemAtual, totalItens);

    // --- Card superior: item selecionado (em destaque) ---
    const int cardSelY = 4;
    const int cardSelH = 34;
    const int iconX = 2;
    const int iconY = cardSelY + 1;

    _display.desenharRetangulo(0, cardSelY, OLED_LARGURA, cardSelH);
    _display.desenharRetangulo(1, cardSelY + 1, OLED_LARGURA - 2, cardSelH - 2);

    desenharIconeMenuPrincipal(_display, iconX, iconY, itemAtual);

    const char *rotuloSelecionado = _menu.nomeItem(itemAtual);
    _display.desenharTexto(40, cardSelY + 12, rotuloSelecionado);

    // Seta indicando "OK confirma"
    const int setaX = OLED_LARGURA - 10;
    const int setaY = cardSelY + (cardSelH / 2);
    _display.desenharLinha(setaX, setaY - 4, setaX + 4, setaY);
    _display.desenharLinha(setaX + 4, setaY, setaX, setaY + 4);

    // --- Card inferior: próximo item (dimmed, só texto) ---
    const int cardProxY = cardSelY + cardSelH + 2;
    const int cardProxH = 12;

    _display.desenharRetangulo(0, cardProxY, OLED_LARGURA, cardProxH);

    const char *rotuloProximo = _menu.nomeItem(itemProximo);
    _display.desenharTexto(6, cardProxY + 1, rotuloProximo);

    desenharRodapeDica("Gire | OK confirma");
}

void DisplayManager::desenharTelaStatus()
{
    // Dashboard operacional: foco em irrigacao e agenda.
    DateTime agora = _rtc.agora();

    int qtdMan = _irrigacao.contarAbertasManual();
    int qtdAuto = _irrigacao.contarAbertasAutomatico();
    int totalAbertas = qtdMan + qtdAuto;

    _display.desenharRetangulo(0, 0, OLED_LARGURA, 26);
    desenharIconeStatusRelogio(_display, 3, 3);

    char linhaHora[16];
    snprintf(linhaHora, sizeof(linhaHora), "%02d:%02d:%02d", agora.hour(), agora.minute(), agora.second());
    _display.desenharTexto(17, 2, linhaHora);

    bool agendaAtivaNoCiclo = _agendaExecucaoAtiva &&
                              (_agendaAguardandoIntervalo || _agendaSetoresEmLote > 0 || _agendaSetoresPendentes > 0);
    desenharIconeStatusIrrigacao(_display, 82, 3, (totalAbertas > 0) || agendaAtivaNoCiclo);
    char linhaOn[16];
    snprintf(linhaOn, sizeof(linhaOn), "ON %d/%d", totalAbertas, NUM_VALVULAS);
    _display.desenharTextoMini(95, 4, linhaOn);

    char linhaIrrigacao[32];

    if (totalAbertas > 0)
    {
        if (qtdMan > 0 && qtdAuto > 0)
            snprintf(linhaIrrigacao, sizeof(linhaIrrigacao), "MAN S%d/%d AUTO S%d/%d", qtdMan, NUM_VALVULAS, qtdAuto, NUM_VALVULAS);
        else if (qtdMan > 0)
            snprintf(linhaIrrigacao, sizeof(linhaIrrigacao), "MANUAL S%d/%d", qtdMan, NUM_VALVULAS);
        else if (qtdAuto > 0)
            snprintf(linhaIrrigacao, sizeof(linhaIrrigacao), "AUTO S%d/%d", qtdAuto, NUM_VALVULAS);
        else
            snprintf(linhaIrrigacao, sizeof(linhaIrrigacao), "ATIVO S%d/%d", totalAbertas, NUM_VALVULAS);
    }
    else if (agendaAtivaNoCiclo)
    {
        uint16_t mask = (uint16_t)(_agendaSetoresLoteMask | _agendaSetoresPendentesMask);
        int qtdAutoCiclo = contarBitsSetor(mask);

        if (qtdAutoCiclo > 0)
            snprintf(linhaIrrigacao, sizeof(linhaIrrigacao), "AUTO S%d/%d", qtdAutoCiclo, NUM_VALVULAS);
        else if (_agendaAguardandoIntervalo)
            snprintf(linhaIrrigacao, sizeof(linhaIrrigacao), "Agenda ativa: intervalo");
        else if (_agendaSetoresPendentes > 0)
            snprintf(linhaIrrigacao, sizeof(linhaIrrigacao), "Agenda ativa: prox lote");
        else
            snprintf(linhaIrrigacao, sizeof(linhaIrrigacao), "Agenda ativa em execucao");
    }
    else
    {
        snprintf(linhaIrrigacao, sizeof(linhaIrrigacao), "Nenhum setor ativo");
    }
    truncarParaLargura(_display, linhaIrrigacao, OLED_LARGURA - 6);
    _display.desenharTexto(3, 13, linhaIrrigacao);

    _display.desenharRetangulo(0, 29, OLED_LARGURA, 20);
    _display.desenharTextoMini(3, 31, "PROXIMA AGENDA");

    DateTime proximaDataHora;
    AgendaSetor proximaAgenda;
    int slotProximo = -1;
    bool temProxima = _menu.proximaAgenda(agora, proximaDataHora, proximaAgenda, slotProximo);

    if (temProxima)
    {
        const char *dias[7] = {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"};
        int dow = proximaDataHora.dayOfTheWeek();
        if (dow < 0 || dow > 6)
            dow = 0;

        desenharIconeStatusAgenda(_display, 4, 38);
        char linhaProxima[24];
        snprintf(linhaProxima, sizeof(linhaProxima), "%s %02d:%02d",
                 dias[dow], proximaDataHora.hour(), proximaDataHora.minute());
        _display.desenharTextoMini(17, 38, linhaProxima);

        int qtdSetoresProxima = contarBitsSetor(proximaAgenda.setoresMask);

        char linhaSetores[32];
        if (qtdSetoresProxima == 0)
            snprintf(linhaSetores, sizeof(linhaSetores), "Setores: S0/%d", NUM_VALVULAS);
        else
            snprintf(linhaSetores, sizeof(linhaSetores), "Setores: S%d/%d", qtdSetoresProxima, NUM_VALVULAS);
        _display.desenharTextoMini(64, 38, linhaSetores);
    }
    else
    {
        _display.desenharTexto(3, 38, "Nenhuma agenda ativa");
    }

    desenharRodapeDica("Gire para abrir menu");
}

void DisplayManager::desenharTelaSensores()
{
    desenharCabecalho("SENSOR DHT11");

    float tempC = _dht11.temperaturaC();
    float umidade = _dht11.umidadePct();
    bool sensorOk = _dht11.leituraValida();
    const char *statusTxt = _dht11.statusTexto();

    char linha1[24];
    char linha2[24];

    if (sensorOk)
    {
        snprintf(linha1, sizeof(linha1), "%.1fC", tempC);
        snprintf(linha2, sizeof(linha2), "%.0f%%", umidade);
    }
    else
    {
        snprintf(linha1, sizeof(linha1), "--.-C");
        snprintf(linha2, sizeof(linha2), "--%%");
    }

    _display.desenharTexto(0, 16, linha1);
    _display.desenharTexto(0, 30, linha2);

    if (!sensorOk)
    {
        char linhaErro[24];
        if (statusTxt == nullptr || statusTxt[0] == '\0')
            snprintf(linhaErro, sizeof(linhaErro), "Sem leitura");
        else
            snprintf(linhaErro, sizeof(linhaErro), "%s", statusTxt);
        _display.desenharTextoMini(0, 42, linhaErro);
    }

    // Mini-gráfico de temperatura (últimas amostras, ~4h de janela)
    const SensorHistorico::Buffer &hist = _dht11.historico();
    int totalPontos = SensorHistorico::total(hist);

    const int graficoX = 46;
    const int graficoY = 14;
    const int graficoW = OLED_LARGURA - graficoX;
    const int graficoH = 30;

    _display.desenharRetangulo(graficoX, graficoY, graficoW, graficoH);

    if (totalPontos >= 2)
    {
        float minTemp, maxTemp;
        SensorHistorico::faixaTemperatura(hist, minTemp, maxTemp);
        if ((maxTemp - minTemp) < 1.0f)
        {
            // Evita gráfico "achatado" quando a variação é mínima.
            minTemp -= 0.5f;
            maxTemp += 0.5f;
        }

        int usavelW = graficoW - 2;
        int usavelH = graficoH - 2;

        float tempAnterior, umidAnterior;
        SensorHistorico::obter(hist, 0, tempAnterior, umidAnterior);
        int xAnterior = graficoX + 1;
        int yAnterior = graficoY + 1 + usavelH - (int)(((tempAnterior - minTemp) / (maxTemp - minTemp)) * usavelH);

        for (int i = 1; i < totalPontos; i++)
        {
            float temp, umid;
            SensorHistorico::obter(hist, i, temp, umid);
            int x = graficoX + 1 + (usavelW * i) / (totalPontos - 1);
            int y = graficoY + 1 + usavelH - (int)(((temp - minTemp) / (maxTemp - minTemp)) * usavelH);

            _display.desenharLinha(xAnterior, yAnterior, x, y);
            xAnterior = x;
            yAnterior = y;
        }
    }
    else
    {
        _display.desenharTextoMini(graficoX + 6, graficoY + 12, "Coletando...");
    }

    _display.desenharTextoMini(0, 46, "4h");

    desenharRodapeDica("OK: voltar");
}

void DisplayManager::desenharTelaIrrigacao()
{
    desenharCabecalho("IRRIGAR AGORA");

    int setorAtual = _menu.setorAtual();
    int totalItens = NUM_VALVULAS + 1; // setores + item Voltar

    const int linhasVisiveis = 3;
    int inicio = (setorAtual / linhasVisiveis) * linhasVisiveis;
    int fim = inicio + linhasVisiveis;
    if (fim > totalItens)
        fim = totalItens;

    // Exibe uma janela paginada com setores e item Voltar
    for (int i = inicio; i < fim; i++)
    {
        int linhaIdx = i - inicio;
        int y = 16 + (linhaIdx * 11);

        bool itemVoltar = (i == NUM_VALVULAS);
        bool aberta = false;
        if (!itemVoltar)
        {
            aberta = (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA);
        }
        bool selecionado = (i == setorAtual);

        if (selecionado)
        {
            _display.setCorDesenho(1);
            _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 11);
            _display.setCorDesenho(0);
        }

        char linha[22];
        if (itemVoltar)
        {
            snprintf(linha, sizeof(linha), "< Voltar");
        }
        else
        {
            snprintf(linha, sizeof(linha), "Setor %d  %s",
                     i + 1,
                     aberta ? "[LIGADO ]" : "[------]");
        }

        _display.desenharTexto(2, y, linha);

        if (selecionado)
            _display.setCorDesenho(1);
    }

    // Aviso de timeout se ocorreu
    if (_menu.timeoutOcorreu())
    {
        _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
        _display.desenharTexto(0, 56, "! Timeout: setor fechado");
    }
    else
    {
        char rodape[28];
        snprintf(rodape, sizeof(rodape), "OK=toggle Pg %d/%d", (inicio / linhasVisiveis) + 1,
                 (totalItens + linhasVisiveis - 1) / linhasVisiveis);
        if (_menu.opcaoVoltarIrrigacaoSelecionada())
        {
            snprintf(rodape, sizeof(rodape), "OK=voltar Pg %d/%d", (inicio / linhasVisiveis) + 1,
                     (totalItens + linhasVisiveis - 1) / linhasVisiveis);
        }
        desenharRodapeDica(rodape);
    }
}

void DisplayManager::desenharTelaProgramar()
{
    EtapaProgramacao etapa = _menu.etapaProgramacao();
    int agenda = _menu.agendaProgramacao() + 1;

    auto desenharBarra = [&](int x, int y, int largura, int altura, int valor, int maximo)
    {
        if (maximo <= 0)
            return;
        _display.desenharRetangulo(x, y, largura, altura);
        int interno = largura - 2;
        int preenchido = (interno * valor) / maximo;
        if (preenchido < 0)
            preenchido = 0;
        if (preenchido > interno)
            preenchido = interno;
        _display.desenharRetanguloPreenchido(x + 1, y + 1, preenchido, altura - 2);
    };

    if (etapa == EtapaProgramacao::SELECIONAR_AGENDA)
    {
        static bool animFeedbackAtivo = false;
        static unsigned long animFeedbackInicioMs = 0;
        static FeedbackProgramacao animTipo = FeedbackProgramacao::NENHUM;

        FeedbackProgramacao fb = _menu.feedbackProgramacao();
        if (fb == FeedbackProgramacao::SALVO || fb == FeedbackProgramacao::EXCLUIDO)
        {
            const unsigned long FRAME_DELAY_MS = 42;
            const int FRAME_COUNT = 16;
            const int LOOP_COUNT = 3;

            if (!animFeedbackAtivo || animTipo != fb)
            {
                animFeedbackAtivo = true;
                animFeedbackInicioMs = millis();
                animTipo = fb;
            }

            unsigned long elapsed = millis() - animFeedbackInicioMs;
            unsigned long frameGlobal = elapsed / FRAME_DELAY_MS;
            unsigned long totalFrames = (unsigned long)FRAME_COUNT * LOOP_COUNT;

            if (frameGlobal >= totalFrames)
            {
                animFeedbackAtivo = false;
                animTipo = FeedbackProgramacao::NENHUM;
                _menu.limparFeedbackProgramacao();
            }
            else
            {
                int frameLocal = (int)(frameGlobal % FRAME_COUNT);
                if (fb == FeedbackProgramacao::SALVO)
                    desenharAnimacaoCheck48(_display, frameLocal);
                else
                    desenharAnimacaoLixeira48(_display, frameLocal);
                return;
            }
        }
        else
        {
            animFeedbackAtivo = false;
            animTipo = FeedbackProgramacao::NENHUM;
        }

        desenharCabecalho("PROGRAMAR");

        bool opcaoVoltar = _menu.opcaoVoltarProgramacaoSelecionada();
        char linhaAgenda[24];
        if (opcaoVoltar)
            snprintf(linhaAgenda, sizeof(linhaAgenda), "Opcao: Voltar");
        else
            snprintf(linhaAgenda, sizeof(linhaAgenda), "Agenda %d de %d", agenda, MAX_AGENDAS_TOTAIS);
        _display.desenharTexto(0, 16, linhaAgenda);

        if (opcaoVoltar)
        {
            _display.desenharRetangulo(0, 27, OLED_LARGURA, 22);
            _display.desenharLinha(10, 38, 28, 38);
            _display.desenharLinha(10, 38, 15, 33);
            _display.desenharLinha(10, 38, 15, 43);
            _display.desenharTexto(34, 34, "< Voltar");
            desenharRodapeDica("OK sair da programacao");
            return;
        }

        AgendaSetor ag = _menu.agendaSelecionada();
        desenharIconeAgendaSlot(_display, 0, 27, ag.ativa);

        if (ag.ativa)
        {
            char linhaHorario[24];
            snprintf(linhaHorario, sizeof(linhaHorario), "%02dh:%02d  %umin", ag.hora, ag.minuto, ag.duracaoMin);

            int totalDias = 0;
            for (int i = 0; i < 7; i++)
                if (ag.diasMask & (1 << i))
                    totalDias++;
            int totalSetores = contarBitsSetor(ag.setoresMask);

            _display.desenharTexto(19, 28, linhaHorario);

            char linhaResumo[24];
            snprintf(linhaResumo, sizeof(linhaResumo), "Dias:%d  Set:%d", totalDias, totalSetores);
            _display.desenharTexto(19, 38, linhaResumo);
            // Sem texto de acao: o proprio icone indica que a agenda e editavel.
        }
        else
        {
            _display.desenharTexto(19, 30, "Slot vazio");
            _display.desenharTextoMini(19, 48, "OK para criar agenda");
        }

        if (fb == FeedbackProgramacao::ERRO_SEM_DIA)
            _display.desenharTexto(0, 48, "Marque ao menos 1 dia");
        else if (fb == FeedbackProgramacao::ERRO_SEM_SETOR)
            _display.desenharTexto(0, 48, "Marque ao menos 1 setor");
        else if (fb == FeedbackProgramacao::ERRO_DUPLICADA)
            _display.desenharTexto(0, 48, "Agenda duplicada");
        else if (fb == FeedbackProgramacao::ERRO_PERSISTENCIA)
            _display.desenharTexto(0, 48, "Erro ao salvar");

        desenharRodapeDica("Gire agenda | OK editar");
        return;
    }

    AgendaSetor ag = _menu.agendaEdicao();
    if (etapa == EtapaProgramacao::SUBMENU_AGENDA)
    {
        desenharCabecalho("EDITAR AGENDA");

        const char *opcoes[8] = {
            "Hora",
            "Minuto",
            "Duracao",
            "Dias",
            "Setores",
            "Salvar",
            "Excluir",
            "Voltar"};

        int selecionada = _menu.opcaoSubmenuProgramacao();
        int inicio = 0;
        if (selecionada > 2)
            inicio = selecionada - 2;
        if (inicio > 4)
            inicio = 4;

        for (int i = 0; i < 4; i++)
        {
            int idx = inicio + i;
            int y = 16 + (i * 10);
            if (idx >= 8)
                break;

            if (idx == selecionada)
            {
                _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 9);
                _display.setCorDesenho(0);
            }

            char linha[28];
            if (idx == 0)
                snprintf(linha, sizeof(linha), "Hora: %02dh", ag.hora);
            else if (idx == 1)
                snprintf(linha, sizeof(linha), "Minuto: %02dmin", ag.minuto);
            else if (idx == 2)
                snprintf(linha, sizeof(linha), "Duracao: %umin", ag.duracaoMin);
            else
                snprintf(linha, sizeof(linha), "%s", opcoes[idx]);

            if (idx <= 6)
            {
                desenharIconeSubmenu(_display, 2, y, idx);
            }
            else
            {
                _display.desenharLinha(3, y + 4, 10, y + 4);
                _display.desenharLinha(3, y + 4, 6, y + 1);
                _display.desenharLinha(3, y + 4, 6, y + 7);
            }
            _display.desenharTexto(14, y, linha);

            if (idx == selecionada)
                _display.setCorDesenho(1);
        }

        desenharRodapeDica("OK: entrar");
        return;
    }

    if (etapa == EtapaProgramacao::CONFIRMAR_EXCLUSAO)
    {
        desenharCabecalho("EXCLUIR AGENDA?");
        _display.desenharTexto(0, 20, "Confirma exclusao?");
        _display.desenharTextoMini(0, 30, "Gire: SIM/NAO | OK confirma");

        int opc = _menu.opcaoConfirmarExclusao(); // 0=SIM, 1=NÃO
        desenharConfirmacaoSimNao(opc);

        desenharRodapeDica("Gire para NAO");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_HORA)
    {
        desenharCabecalho("EDITAR HORA");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02dh", ag.hora);
        _display.desenharTextoGrande(44, 16, valor);
        desenharBarra(8, 38, 112, 10, ag.hora, 23);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_MINUTO)
    {
        desenharCabecalho("EDITAR MINUTO");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02dmin", ag.minuto);
        _display.desenharTextoGrande(32, 16, valor);
        desenharBarra(8, 38, 112, 10, ag.minuto, 59);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_DURACAO)
    {
        desenharCabecalho("EDITAR DURACAO");
        char valor[18];
        snprintf(valor, sizeof(valor), "%umin", ag.duracaoMin);
        _display.desenharTextoGrande(30, 16, valor);
        desenharBarra(8, 38, 112, 10, ag.duracaoMin, 240);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_DIAS)
    {
        desenharCabecalho("EDITAR DIAS");
        const char *diasSigla[7] = {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"};
        int xBase = 2;
        int passo = 18;

        for (int i = 0; i < 7; i++)
        {
            int x = xBase + (i * passo);
            _display.desenharTextoMini(x, 22, diasSigla[i]);

            bool marcado = (ag.diasMask & (1 << i)) != 0;
            if (marcado)
                _display.desenharRetanguloPreenchido(x + 4, 34, 6, 6);
            else
                _display.desenharRetangulo(x + 4, 34, 6, 6);

            if (i == _menu.cursorDiaProgramacao())
            {
                _display.desenharRetangulo(x + 2, 32, 10, 10);
            }
        }

        bool voltarSelecionado = (_menu.cursorDiaProgramacao() == 7);
        if (voltarSelecionado)
        {
            _display.desenharRetanguloPreenchido(0, 43, OLED_LARGURA, 10);
            _display.setCorDesenho(0);
        }
        _display.desenharTexto(2, 44, "< Voltar");
        if (voltarSelecionado)
        {
            _display.setCorDesenho(1);
        }

        _display.desenharTextoMini(0, 50, "Gire cursor | OK marca");
        _display.desenharTextoMini(0, 56, "Segure: voltar submenu");
        return;
    }

    if (etapa == EtapaProgramacao::EDIT_SETORES)
    {
        desenharCabecalho("EDITAR SETORES");

        const int linhasVisiveis = 3;
        int cursor = _menu.cursorSetorProgramacao();
        int totalItens = NUM_VALVULAS + 1; // setores + voltar
        int inicio = (cursor / linhasVisiveis) * linhasVisiveis;
        int fim = inicio + linhasVisiveis;
        if (fim > totalItens)
            fim = totalItens;

        for (int i = inicio; i < fim; i++)
        {
            int linhaIdx = i - inicio;
            int y = 16 + (linhaIdx * 11);
            bool itemVoltar = (i == NUM_VALVULAS);
            bool marcado = false;
            if (!itemVoltar)
            {
                marcado = (ag.setoresMask & (1 << i)) != 0;
            }
            if (i == _menu.cursorSetorProgramacao())
            {
                _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 10);
                _display.setCorDesenho(0);
            }

            char linha[24];
            if (itemVoltar)
                snprintf(linha, sizeof(linha), "< Voltar");
            else
                snprintf(linha, sizeof(linha), "Setor %d   [%c]", i + 1, marcado ? 'X' : ' ');
            _display.desenharTexto(2, y, linha);

            if (i == _menu.cursorSetorProgramacao())
            {
                _display.setCorDesenho(1);
            }
        }
        char rodape[26];
        if (_menu.cursorSetorProgramacao() == NUM_VALVULAS)
            snprintf(rodape, sizeof(rodape), "OK voltar Pg %d/%d",
                     (inicio / linhasVisiveis) + 1,
                     (totalItens + linhasVisiveis - 1) / linhasVisiveis);
        else
            snprintf(rodape, sizeof(rodape), "Gire/OK marca Pg %d/%d",
                     (inicio / linhasVisiveis) + 1,
                     (totalItens + linhasVisiveis - 1) / linhasVisiveis);
        _display.desenharTextoMini(0, 56, rodape);
    }

    FeedbackProgramacao fb = _menu.feedbackProgramacao();
    if (fb == FeedbackProgramacao::ERRO_SEM_DIA)
        _display.desenharTexto(0, 50, "Marque ao menos 1 dia");
    else if (fb == FeedbackProgramacao::ERRO_SEM_SETOR)
        _display.desenharTexto(0, 50, "Marque ao menos 1 setor");
    else if (fb == FeedbackProgramacao::ERRO_DUPLICADA)
        _display.desenharTexto(0, 50, "Agenda duplicada");
    else if (fb == FeedbackProgramacao::ERRO_PERSISTENCIA)
        _display.desenharTexto(0, 50, "Erro ao salvar");
    else if (fb == FeedbackProgramacao::ERRO_EXCLUSAO)
        _display.desenharTexto(0, 50, "Erro ao excluir");
}

void DisplayManager::desenharCardsOpcao(const char *rotuloAtual, const char *rotuloProximo,
                                         const uint8_t *iconeAtual, int posicaoAtual, int totalOpcoes)
{
    const int cardSelY = 4;
    const int cardSelH = 34;
    const int iconX = 2;
    const int iconY = cardSelY + 1;
    const int iconTam = 28;

    _display.desenharRetangulo(0, cardSelY, OLED_LARGURA, cardSelH);
    _display.desenharRetangulo(1, cardSelY + 1, OLED_LARGURA - 2, cardSelH - 2);

    desenharBitmap1Bpp(_display, iconX, iconY, iconTam, iconTam, iconeAtual);

    char linhaAtual[40];
    snprintf(linhaAtual, sizeof(linhaAtual), "%s", rotuloAtual);
    truncarParaLargura(_display, linhaAtual, OLED_LARGURA - 36);
    _display.desenharTexto(34, cardSelY + 12, linhaAtual);

    const int cardProxY = cardSelY + cardSelH + 2;
    const int cardProxH = 12;

    _display.desenharRetangulo(0, cardProxY, OLED_LARGURA, cardProxH);

    char linhaProxima[40];
    snprintf(linhaProxima, sizeof(linhaProxima), "%s", rotuloProximo);
    truncarParaLargura(_display, linhaProxima, OLED_LARGURA - 12);
    _display.desenharTexto(6, cardProxY + 1, linhaProxima);

    char rodape[24];
    snprintf(rodape, sizeof(rodape), "%d/%d  OK entra", posicaoAtual + 1, totalOpcoes);
    desenharRodapeDica(rodape);
}

void DisplayManager::desenharTelaConfig()
{
    EtapaConfiguracao etapa = _menu.etapaConfiguracao();
    static bool animSalvoAtiva = false;
    static bool animLimpoAtiva = false;
    static bool animRestauradoAtiva = false;
    static unsigned long animSalvoInicioMs = 0;
    static unsigned long animLimpoInicioMs = 0;
    static unsigned long animRestauradoInicioMs = 0;

    const unsigned long FRAME_DELAY_MS = 42;
    const int FRAME_COUNT = 16;
    const int LOOP_COUNT = 3;

    if (_menu.feedbackConfiguracaoSalvo())
    {
        animSalvoAtiva = true;
        animSalvoInicioMs = millis();
        _menu.limparFeedbackConfiguracaoSalvo();
    }

    if (_menu.feedbackConfiguracaoLimpo())
    {
        animLimpoAtiva = true;
        animLimpoInicioMs = millis();
        _menu.limparFeedbackConfiguracaoLimpo();
    }

    if (_menu.feedbackConfiguracaoRestaurado())
    {
        animRestauradoAtiva = true;
        animRestauradoInicioMs = millis();
        _menu.limparFeedbackConfiguracaoRestaurado();
    }

    if (animSalvoAtiva)
    {
        desenharCabecalho("CONFIGURACOES");
        unsigned long elapsed = millis() - animSalvoInicioMs;
        unsigned long frameGlobal = elapsed / FRAME_DELAY_MS;
        unsigned long totalFrames = (unsigned long)FRAME_COUNT * LOOP_COUNT;
        if (frameGlobal >= totalFrames)
        {
            animSalvoAtiva = false;
            return;
        }

        int frameLocal = (int)(frameGlobal % FRAME_COUNT);
        desenharAnimacaoCheck48(_display, frameLocal);
        _display.desenharTextoMini(34, 56, "Relogio salvo!");
        return;
    }

    if (animLimpoAtiva)
    {
        desenharCabecalho("CONFIGURACOES");
        unsigned long elapsed = millis() - animLimpoInicioMs;
        unsigned long frameGlobal = elapsed / FRAME_DELAY_MS;
        unsigned long totalFrames = (unsigned long)FRAME_COUNT * LOOP_COUNT;
        if (frameGlobal >= totalFrames)
        {
            animLimpoAtiva = false;
            return;
        }

        int frameLocal = (int)(frameGlobal % FRAME_COUNT);
        desenharAnimacaoLixeira48(_display, frameLocal);
        _display.desenharTextoMini(20, 56, "Agendas apagadas!");
        return;
    }

    if (animRestauradoAtiva)
    {
        desenharCabecalho("CONFIGURACOES");
        unsigned long elapsed = millis() - animRestauradoInicioMs;
        unsigned long frameGlobal = elapsed / FRAME_DELAY_MS;
        unsigned long totalFrames = (unsigned long)FRAME_COUNT * LOOP_COUNT;
        if (frameGlobal >= totalFrames)
        {
            animRestauradoAtiva = false;
            return;
        }

        int frameLocal = (int)(frameGlobal % FRAME_COUNT);
        desenharAnimacaoCheck48(_display, frameLocal);
        _display.desenharTextoMini(16, 56, "Padrao restaurado!");
        return;
    }

    if (etapa == EtapaConfiguracao::MENU)
    {
        const char *opcoes[3] = {"Relogio", "Sistema", "Voltar"};
        const uint8_t *icones[3] = {ICON_CLOCK_28X28, ICON_SETTINGS_28X28, ICON_CORNER_UP_LEFT_28X28};

        int selecionada = _menu.opcaoConfiguracao();
        int proxima = MenuCards::proximoIndice(selecionada, 3);

        desenharCardsOpcao(opcoes[selecionada], opcoes[proxima],
                            icones[selecionada], selecionada, 3);
        return;
    }

    if (etapa == EtapaConfiguracao::SUBMENU_RELOGIO)
    {
        const char *opcoes[8] = {
            "Hora", "Minuto", "Dia", "Mes", "Ano",
            "Timeout", "Salvar relogio", "Voltar"};

        auto formatarRotulo = [this, &opcoes](int idx, char *out, size_t outLen)
        {
            if (idx == 0)
                snprintf(out, outLen, "Hora: %02d", _menu.configHora());
            else if (idx == 1)
                snprintf(out, outLen, "Minuto: %02d", _menu.configMinuto());
            else if (idx == 2)
                snprintf(out, outLen, "Dia: %02d", _menu.configDia());
            else if (idx == 3)
                snprintf(out, outLen, "Mes: %02d", _menu.configMes());
            else if (idx == 4)
                snprintf(out, outLen, "Ano: %04d", _menu.configAno());
            else if (idx == 5)
                snprintf(out, outLen, "Timeout: %d min", _menu.configTimeoutManualMin());
            else
                snprintf(out, outLen, "%s", opcoes[idx]);
        };
        static const uint8_t *iconesRelogio[8] = {
            ICON_CLOCK_28X28, ICON_TIMER_28X28, ICON_CALENDAR_DAYS_28X28,
            ICON_CALENDAR_28X28, ICON_CALENDAR_RANGE_28X28, ICON_HOURGLASS_28X28,
            ICON_SAVE_28X28, ICON_CORNER_UP_LEFT_28X28};

        int selecionada = _menu.opcaoConfiguracao();
        int proxima = MenuCards::proximoIndice(selecionada, 8);

        char rotuloAtual[32];
        char rotuloProximo[32];
        formatarRotulo(selecionada, rotuloAtual, sizeof(rotuloAtual));
        formatarRotulo(proxima, rotuloProximo, sizeof(rotuloProximo));

        desenharCardsOpcao(rotuloAtual, rotuloProximo,
                            iconesRelogio[selecionada], selecionada, 8);
        return;
    }

    if (etapa == EtapaConfiguracao::SUBMENU_SISTEMA)
    {
        const char *opcoes[6] = {
            "Duracao padrao", "Teste valvulas", "Limpar agendas",
            "Restaurar padrao", "Info do sistema", "Voltar"};
        static const uint8_t *iconesSistema[6] = {
            ICON_TIMER_28X28, ICON_DROPLETS_28X28, ICON_TRASH_2_28X28,
            ICON_ROTATE_CCW_28X28, ICON_INFO_28X28, ICON_CORNER_UP_LEFT_28X28};

        auto formatarRotulo = [this, &opcoes](int idx, char *out, size_t outLen)
        {
            if (idx == 0)
                snprintf(out, outLen, "Duracao: %d min", _menu.configDuracaoPadraoMin());
            else
                snprintf(out, outLen, "%s", opcoes[idx]);
        };

        int selecionada = _menu.opcaoConfiguracao();
        int proxima = MenuCards::proximoIndice(selecionada, 6);

        char rotuloAtual[32];
        char rotuloProximo[32];
        formatarRotulo(selecionada, rotuloAtual, sizeof(rotuloAtual));
        formatarRotulo(proxima, rotuloProximo, sizeof(rotuloProximo));

        desenharCardsOpcao(rotuloAtual, rotuloProximo,
                            iconesSistema[selecionada], selecionada, 6);
        return;
    }

    if (etapa == EtapaConfiguracao::TESTE_VALVULAS)
    {
        desenharCabecalho("TESTE VALVULAS");

        int setorAtual = _menu.configSetorTeste();
        const int linhasVisiveis = 3;
        const int totalItens = NUM_VALVULAS + 1;
        int inicio = (setorAtual / linhasVisiveis) * linhasVisiveis;
        int fim = inicio + linhasVisiveis;
        if (fim > totalItens)
            fim = totalItens;

        for (int i = inicio; i < fim; i++)
        {
            int linhaIdx = i - inicio;
            int y = 16 + (linhaIdx * 11);
            bool itemVoltar = (i == NUM_VALVULAS);
            bool aberta = false;
            if (!itemVoltar)
            {
                aberta = (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA);
            }

            if (i == setorAtual)
            {
                _display.desenharRetanguloPreenchido(0, y - 1, OLED_LARGURA, 10);
                _display.setCorDesenho(0);
            }

            char linha[24];
            if (itemVoltar)
                snprintf(linha, sizeof(linha), "< Voltar");
            else
                snprintf(linha, sizeof(linha), "Setor %d   [%c]", i + 1, aberta ? 'X' : ' ');
            _display.desenharTexto(2, y, linha);

            if (i == setorAtual)
            {
                _display.setCorDesenho(1);
            }
        }

        char rodape[28];
        if (_menu.opcaoVoltarTesteValvulasSelecionada())
            snprintf(rodape, sizeof(rodape), "OK voltar | Pg %d/%d", (inicio / linhasVisiveis) + 1,
                     (totalItens + linhasVisiveis - 1) / linhasVisiveis);
        else
            snprintf(rodape, sizeof(rodape), "OK toggle | Pg %d/%d", (inicio / linhasVisiveis) + 1,
                     (totalItens + linhasVisiveis - 1) / linhasVisiveis);
        _display.desenharLinha(0, 50, OLED_LARGURA - 1, 50);
        _display.desenharTextoMini(0, 52, "MODO MANUTENCAO");
        _display.desenharTextoMini(0, 56, rodape);
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_HORA)
    {
        desenharCabecalho("AJUSTAR HORA");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02d", _menu.configHora());
        _display.desenharTextoGrande(52, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_MINUTO)
    {
        desenharCabecalho("AJUSTAR MINUTO");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02d", _menu.configMinuto());
        _display.desenharTextoGrande(52, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_DIA)
    {
        desenharCabecalho("AJUSTAR DIA");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02d", _menu.configDia());
        _display.desenharTextoGrande(52, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_MES)
    {
        desenharCabecalho("AJUSTAR MES");
        char valor[16];
        snprintf(valor, sizeof(valor), "%02d", _menu.configMes());
        _display.desenharTextoGrande(52, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_ANO)
    {
        desenharCabecalho("AJUSTAR ANO");
        char valor[16];
        snprintf(valor, sizeof(valor), "%04d", _menu.configAno());
        _display.desenharTextoGrande(40, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK volta");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_TIMEOUT_MANUAL)
    {
        desenharCabecalho("TIMEOUT MANUAL");
        char valor[16];
        snprintf(valor, sizeof(valor), "%dmin", _menu.configTimeoutManualMin());
        _display.desenharTextoGrande(36, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK salva");
        return;
    }

    if (etapa == EtapaConfiguracao::EDIT_DURACAO_PADRAO)
    {
        desenharCabecalho("DURACAO PADRAO");
        char valor[16];
        snprintf(valor, sizeof(valor), "%dmin", _menu.configDuracaoPadraoMin());
        _display.desenharTextoGrande(36, 22, valor);
        _display.desenharTextoMini(0, 56, "Gire ajusta | OK salva");
        return;
    }

    if (etapa == EtapaConfiguracao::CONFIRMAR_LIMPAR_AGENDAS)
    {
        desenharCabecalho("LIMPAR AGENDAS?");
        _display.desenharTextoMini(0, 20, "Apagar todas as agendas?");
        _display.desenharTextoMini(0, 30, "Gire: SIM/NAO | OK confirma");

        int opc = _menu.opcaoConfirmarLimparAgendas();
        desenharConfirmacaoSimNao(opc);

        desenharRodapeDica("Gire para NAO");
        return;
    }

    if (etapa == EtapaConfiguracao::CONFIRMAR_RESTAURAR_PADRAO)
    {
        desenharCabecalho("RESTAURAR PADRAO?");
        _display.desenharTextoMini(0, 20, "Resetar timeout/duracao?");
        _display.desenharTextoMini(0, 30, "Gire: SIM/NAO | OK confirma");

        int opc = _menu.opcaoConfirmarRestaurarPadrao();
        desenharConfirmacaoSimNao(opc);

        desenharRodapeDica("Gire para NAO");
        return;
    }

    if (etapa == EtapaConfiguracao::INFO_SISTEMA)
    {
        desenharCabecalho("INFO SISTEMA");
        DateTime agora = _rtc.agora();

        char linha1[24];
        snprintf(linha1, sizeof(linha1), "Agendas: %d", _menu.totalAgendasAtivas());
        _display.desenharTexto(0, 16, linha1);

        char linha2[24];
        snprintf(linha2, sizeof(linha2), "RTC: %02d/%02d %02d:%02d", agora.day(), agora.month(), agora.hour(), agora.minute());
        _display.desenharTextoMini(0, 30, linha2);

        char linha3[24];
        snprintf(linha3, sizeof(linha3), "T.Man: %dmin", _menu.configTimeoutManualMin());
        _display.desenharTextoMini(0, 40, linha3);

        char linha4[24];
        snprintf(linha4, sizeof(linha4), "Dur.Pad: %dmin", _menu.configDuracaoPadraoMin());
        _display.desenharTextoMini(0, 48, linha4);

        desenharRodapeDica("OK para voltar");
        return;
    }
}

void DisplayManager::desenharTelaWebServer()
{
    desenharCabecalho("WEBSERVER");

    String linhas[WEBSERVER_MAX_LINHAS];
    int totalLinhas = 0;

    if (!_webAp.ativo())
    {
        totalLinhas = adicionarTextoQuebrado(_display, "Falha ao iniciar AP", linhas, totalLinhas);
        totalLinhas = adicionarTextoQuebrado(_display, "Tente voltar e entrar novamente", linhas, totalLinhas);
    }
    else
    {
        totalLinhas = adicionarTextoQuebrado(_display, "Conecte no Wi-Fi do ESP32", linhas, totalLinhas);
        totalLinhas = adicionarTextoQuebrado(_display, String("SSID: ") + _webAp.ssid(), linhas, totalLinhas);
        totalLinhas = adicionarTextoQuebrado(_display, String("Senha: ") + _webAp.senha(), linhas, totalLinhas);
        totalLinhas = adicionarTextoQuebrado(_display, String("IP AP: ") + _webAp.ipTexto(), linhas, totalLinhas);

        String urlAp = _webAp.urlAcessoAp();
        if (urlAp.length() > 0)
        {
            totalLinhas = adicionarTextoQuebrado(_display, String("URL AP: ") + urlAp, linhas, totalLinhas);
        }

        if (_webAp.staConectada())
        {
            totalLinhas = adicionarTextoQuebrado(_display, String("IP STA: ") + _webAp.ipStaTexto(), linhas, totalLinhas);

            String urlSta = _webAp.urlAcessoSta();
            bool mdnsAtivo = (urlSta.indexOf(".local/") >= 0);

            totalLinhas = adicionarTextoQuebrado(_display,
                                                 String("mDNS: ") + (mdnsAtivo ? "ativo" : "inativo"),
                                                 linhas,
                                                 totalLinhas);

            if (mdnsAtivo)
            {
                totalLinhas = adicionarTextoQuebrado(_display,
                                                     String("Host mDNS: ") + WIFI_MDNS_HOSTNAME + ".local",
                                                     linhas,
                                                     totalLinhas);
            }

            if (urlSta.length() > 0)
            {
                totalLinhas = adicionarTextoQuebrado(_display, String("URL STA: ") + urlSta, linhas, totalLinhas);
            }
        }
        else
        {
            totalLinhas = adicionarTextoQuebrado(_display, "STA: sem conexao", linhas, totalLinhas);
#if WIFI_MDNS_ENABLED
            totalLinhas = adicionarTextoQuebrado(_display, "mDNS: aguardando STA", linhas, totalLinhas);
#endif
        }
    }

    if (totalLinhas <= 0)
    {
        linhas[0] = "Sem informacoes";
        totalLinhas = 1;
    }

    int totalPaginas = (totalLinhas + WEBSERVER_LINHAS_POR_PAGINA - 1) / WEBSERVER_LINHAS_POR_PAGINA;
    int paginaAtual = 0;
    if (totalPaginas > 1)
    {
        paginaAtual = _menu.paginaWebServer() % totalPaginas;
    }

    int inicio = paginaAtual * WEBSERVER_LINHAS_POR_PAGINA;
    for (int i = 0; i < WEBSERVER_LINHAS_POR_PAGINA; i++)
    {
        int indiceLinha = inicio + i;
        if (indiceLinha >= totalLinhas)
        {
            break;
        }

        int y = WEBSERVER_Y_INICIAL + (i * WEBSERVER_ALTURA_LINHA);
        _display.desenharTexto(0, y, linhas[indiceLinha].c_str());
    }

    if (totalPaginas > 1)
    {
        char rodape[26];
        snprintf(rodape, sizeof(rodape), "Pag %d/%d Gire OK volta", paginaAtual + 1, totalPaginas);
        desenharRodapeDica(rodape);
    }
    else
    {
        desenharRodapeDica("OK: voltar");
    }
}

// ============================================================
//  Componentes reutilizáveis
// ============================================================

void DisplayManager::desenharCabecalho(const char *titulo)
{
    _display.desenharTexto(0, 0, titulo);
    _display.desenharLinha(0, 12, OLED_LARGURA - 1, 12);
}

void DisplayManager::desenharRodapeDica(const char *texto)
{
    _display.desenharLinha(0, 54, OLED_LARGURA - 1, 54);
    _display.desenharTextoMini(0, 56, texto);
}

void DisplayManager::desenharConfirmacaoSimNao(int opcaoSelecionada)
{
    const int y = 40;
    const int altura = 12;
    const int simX = 8;
    const int naoX = 72;
    const int largura = 48;

    bool simSelecionado = (opcaoSelecionada == 0);

    if (simSelecionado)
        _display.desenharRetanguloPreenchido(simX, y, largura, altura);
    else
        _display.desenharRetangulo(simX, y, largura, altura);
    if (simSelecionado)
        _display.setCorDesenho(0);
    _display.desenharTexto(simX + 14, y + 2, "SIM");
    if (simSelecionado)
        _display.setCorDesenho(1);

    if (!simSelecionado)
        _display.desenharRetanguloPreenchido(naoX, y, largura, altura);
    else
        _display.desenharRetangulo(naoX, y, largura, altura);
    if (!simSelecionado)
        _display.setCorDesenho(0);
    _display.desenharTexto(naoX + 16, y + 2, "NAO");
    if (!simSelecionado)
        _display.setCorDesenho(1);
}
