#pragma once

// Buffer circular de leituras temperatura/umidade, sem dependência de
// hardware (Arduino/DHT), para permitir teste em host e reuso pelo
// Dht11Driver e pelo DisplayManager (mini-gráfico da tela Sensores).
namespace SensorHistorico
{

constexpr int CAPACIDADE = 24;

struct Buffer
{
    float temperaturas[CAPACIDADE];
    float umidades[CAPACIDADE];
    int total;
    int proximoIndice;
};

void iniciar(Buffer &buf);

// Insere uma amostra. Se o buffer estiver cheio, descarta a mais antiga.
void inserir(Buffer &buf, float temperaturaC, float umidadePct);

int total(const Buffer &buf);

// Lê a amostra de índice `indice` (0 = mais antiga, total-1 = mais recente).
void obter(const Buffer &buf, int indice, float &temperaturaC, float &umidadePct);

// Menor/maior temperatura no buffer. Buffer vazio retorna 0/0.
void faixaTemperatura(const Buffer &buf, float &minimo, float &maximo);

// Menor/maior umidade no buffer. Buffer vazio retorna 0/0.
void faixaUmidade(const Buffer &buf, float &minimo, float &maximo);

} // namespace SensorHistorico
