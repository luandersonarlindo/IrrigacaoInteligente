#include "sensor_historico.h"

namespace SensorHistorico
{

void iniciar(Buffer &buf)
{
    buf.total = 0;
    buf.proximoIndice = 0;
}

void inserir(Buffer &buf, float temperaturaC, float umidadePct)
{
    buf.temperaturas[buf.proximoIndice] = temperaturaC;
    buf.umidades[buf.proximoIndice] = umidadePct;
    buf.proximoIndice = (buf.proximoIndice + 1) % CAPACIDADE;

    if (buf.total < CAPACIDADE)
    {
        buf.total++;
    }
}

int total(const Buffer &buf)
{
    return buf.total;
}

void obter(const Buffer &buf, int indice, float &temperaturaC, float &umidadePct)
{
    if (buf.total == 0)
    {
        temperaturaC = 0.0f;
        umidadePct = 0.0f;
        return;
    }

    if (indice < 0)
        indice = 0;
    if (indice > buf.total - 1)
        indice = buf.total - 1;

    int inicio = (buf.total < CAPACIDADE) ? 0 : buf.proximoIndice;
    int posicaoReal = (inicio + indice) % CAPACIDADE;
    temperaturaC = buf.temperaturas[posicaoReal];
    umidadePct = buf.umidades[posicaoReal];
}

void faixaTemperatura(const Buffer &buf, float &minimo, float &maximo)
{
    if (buf.total == 0)
    {
        minimo = 0.0f;
        maximo = 0.0f;
        return;
    }

    float temp, umid;
    obter(buf, 0, temp, umid);
    minimo = temp;
    maximo = temp;

    for (int i = 1; i < buf.total; i++)
    {
        obter(buf, i, temp, umid);
        if (temp < minimo)
            minimo = temp;
        if (temp > maximo)
            maximo = temp;
    }
}

void faixaUmidade(const Buffer &buf, float &minimo, float &maximo)
{
    if (buf.total == 0)
    {
        minimo = 0.0f;
        maximo = 0.0f;
        return;
    }

    float temp, umid;
    obter(buf, 0, temp, umid);
    minimo = umid;
    maximo = umid;

    for (int i = 1; i < buf.total; i++)
    {
        obter(buf, i, temp, umid);
        if (umid < minimo)
            minimo = umid;
        if (umid > maximo)
            maximo = umid;
    }
}

} // namespace SensorHistorico
