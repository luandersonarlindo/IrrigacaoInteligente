// ============================================================
//  web_ap_manager.cpp - Implementacao do WebApManager
// ============================================================

#include "web_ap_manager.h"

#include <stdlib.h>
#include <string.h>

namespace
{
    const char *nomeStatusSta(wl_status_t status)
    {
        switch (status)
        {
        case WL_IDLE_STATUS:
            return "IDLE";
        case WL_NO_SSID_AVAIL:
            return "NO_SSID_AVAIL";
        case WL_SCAN_COMPLETED:
            return "SCAN_COMPLETED";
        case WL_CONNECTED:
            return "CONNECTED";
        case WL_CONNECT_FAILED:
            return "CONNECT_FAILED";
        case WL_CONNECTION_LOST:
            return "CONNECTION_LOST";
        case WL_DISCONNECTED:
            return "DISCONNECTED";
        default:
            return "DESCONHECIDO";
        }
    }

    const char WEB_DASHBOARD_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8"/>
  <meta name="viewport" content="width=device-width, initial-scale=1"/>
  <title>Sistema de Irrigacao</title>
  <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap" rel="stylesheet"/>
  <style>
    :root {
      --bg: #f5f7f5;
      --surface: #ffffff;
      --border: #e0e7e2;
      --border2: #ccd6cf;
      --green: #1a7a3c;
      --green-light: #e8f5ed;
      --green-mid: #2ea05a;
      --red: #c0392b;
      --amber: #b45309;
      --amber-light: #fef3c7;
      --blue: #1565c0;
      --text: #1a2e1f;
      --text2: #4a6652;
      --muted: #7a9482;
      --shadow: 0 1px 3px #0000000d, 0 1px 2px #0000000a;
    }

    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

    body {
      background: var(--bg);
      font-family: 'Inter', sans-serif;
      color: var(--text);
      min-height: 100vh;
      padding-bottom: 64px;
    }

    .topbar {
      background: var(--surface);
      border-bottom: 1px solid var(--border);
      padding: 14px 28px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      box-shadow: var(--shadow);
      position: sticky;
      top: 0;
      z-index: 100;
    }

    .brand { display: flex; align-items: center; gap: 10px; }

    .brand-icon {
      width: 34px; height: 34px;
      background: var(--green-light);
      border-radius: 8px;
      display: flex; align-items: center; justify-content: center;
      font-size: 1.1rem;
    }

    .brand h1 {
      font-size: 0.95rem;
      font-weight: 700;
      color: var(--text);
    }

    .brand p {
      font-size: 0.7rem;
      color: var(--muted);
      margin-top: 1px;
    }

    .topbar-right {
      display: flex;
      align-items: center;
      gap: 16px;
      font-size: 0.75rem;
      color: var(--text2);
    }

    .badge {
      display: inline-flex;
      align-items: center;
      gap: 5px;
      background: var(--green-light);
      color: var(--green);
      border: 1px solid #b6dcc4;
      border-radius: 20px;
      padding: 4px 10px;
      font-size: 0.7rem;
      font-weight: 600;
    }

    .badge-dot {
      width: 6px; height: 6px;
      border-radius: 50%;
      background: var(--green-mid);
    }

    .wrap {
      max-width: 860px;
      margin: 0 auto;
      padding: 28px 20px 0;
    }

    .sensor-grid {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 12px;
      margin-bottom: 20px;
    }

    @media(max-width:600px){ .sensor-grid { grid-template-columns: repeat(2,1fr); } }

    .sensor-card {
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 16px;
      box-shadow: var(--shadow);
    }

    .sensor-icon { font-size: 1.1rem; margin-bottom: 10px; display: block; }

    .sensor-label {
      font-size: 0.65rem;
      font-weight: 600;
      color: var(--muted);
      text-transform: uppercase;
      letter-spacing: 0.08em;
    }

    .sensor-value {
      font-size: 1.6rem;
      font-weight: 700;
      line-height: 1.15;
      margin-top: 4px;
    }

    .sensor-unit {
      font-size: 0.65rem;
      color: var(--muted);
      margin-top: 2px;
    }

    .section-label {
      font-size: 0.7rem;
      font-weight: 600;
      color: var(--muted);
      text-transform: uppercase;
      letter-spacing: 0.1em;
      margin-bottom: 10px;
      padding-bottom: 8px;
      border-bottom: 1px solid var(--border);
    }

    .valve-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 10px;
      margin-bottom: 20px;
    }

    @media(max-width:480px){ .valve-grid { grid-template-columns: 1fr; } }

    .valve-card {
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 14px 16px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
      cursor: pointer;
      box-shadow: var(--shadow);
    }

    .valve-card.active {
      border-color: #a8d5b8;
      background: var(--green-light);
    }

    .valve-num {
      font-size: 0.6rem;
      font-weight: 600;
      color: var(--muted);
      text-transform: uppercase;
      letter-spacing: 0.1em;
    }

    .valve-name {
      font-size: 0.9rem;
      font-weight: 600;
      margin-top: 2px;
    }

    .valve-status {
      font-size: 0.68rem;
      font-weight: 500;
      margin-top: 3px;
      color: var(--muted);
    }

    .valve-card.active .valve-status { color: var(--green); }

    .toggle {
      flex-shrink: 0;
      width: 44px; height: 24px;
      border-radius: 24px;
      border: none;
      cursor: pointer;
      position: relative;
      outline: none;
    }

    .toggle.off { background: var(--border2); }
    .toggle.on { background: var(--green-mid); }

    .knob {
      position: absolute;
      top: 3px;
      width: 18px; height: 18px;
      border-radius: 50%;
      background: #fff;
      box-shadow: 0 1px 3px #00000030;
      pointer-events: none;
      transition: left .15s ease;
    }

    .toggle.off .knob { left: 3px; }
    .toggle.on .knob { left: 23px; }

    .force-btn {
      width: 100%;
      padding: 14px;
      background: var(--surface);
      border: 1px solid #f5c6a0;
      border-radius: 12px;
      color: var(--amber);
      font-family: 'Inter', sans-serif;
      font-size: 0.85rem;
      font-weight: 600;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
      box-shadow: var(--shadow);
      margin-bottom: 20px;
    }

    .force-btn:hover { background: var(--amber-light); }

    .schedule-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
    }

    .schedule-card {
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 16px 18px;
      box-shadow: var(--shadow);
    }

    .schedule-label {
      font-size: 0.65rem;
      font-weight: 600;
      color: var(--muted);
      text-transform: uppercase;
      letter-spacing: 0.08em;
      margin-bottom: 6px;
    }

    .schedule-time {
      font-size: 1.5rem;
      font-weight: 700;
      color: var(--text);
    }

    .schedule-status {
      margin-top: 6px;
      font-size: 0.7rem;
      font-weight: 500;
    }

    .done { color: var(--green); }
    .pending { color: var(--muted); }

    .network-info {
      margin-top: 16px;
      background: var(--surface);
      border: 1px dashed var(--border2);
      border-radius: 12px;
      padding: 12px 14px;
      font-size: 0.72rem;
      color: var(--text2);
    }

    .editor-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 10px;
      margin-top: 10px;
    }

    @media(max-width:700px){ .editor-grid { grid-template-columns: 1fr; } }

    .editor-card {
      background: var(--surface);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 12px;
      box-shadow: var(--shadow);
    }

    .editor-head {
      font-size: 0.75rem;
      font-weight: 700;
      color: var(--text2);
      margin-bottom: 8px;
      text-transform: uppercase;
      letter-spacing: 0.08em;
    }

    .editor-row {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 8px;
      margin-bottom: 8px;
    }

    .editor-row label {
      font-size: 0.68rem;
      color: var(--text2);
      margin-bottom: 3px;
      display: block;
    }

    .editor-row input[type="time"],
    .editor-row input[type="number"] {
      width: 100%;
      border: 1px solid var(--border2);
      border-radius: 8px;
      padding: 6px 8px;
      font-size: 0.78rem;
      font-family: 'Inter', sans-serif;
      background: #fff;
      color: var(--text);
    }

    .chips {
      display: flex;
      flex-wrap: wrap;
      gap: 6px;
      margin-bottom: 8px;
    }

    .chip {
      border: 1px solid var(--border2);
      border-radius: 18px;
      padding: 3px 7px;
      font-size: 0.66rem;
      color: var(--text2);
      background: #fff;
      display: inline-flex;
      align-items: center;
      gap: 4px;
    }

    .tiny-actions {
      display: flex;
      gap: 8px;
      margin-top: 6px;
    }

    .tiny-btn {
      flex: 1;
      border: 1px solid var(--border2);
      border-radius: 8px;
      padding: 7px 8px;
      font-size: 0.72rem;
      font-weight: 600;
      background: #fff;
      color: var(--text2);
      cursor: pointer;
      font-family: 'Inter', sans-serif;
    }

    .tiny-btn.primary {
      border-color: #a8d5b8;
      color: var(--green);
      background: var(--green-light);
    }

    .toast {
      position: fixed;
      bottom: 24px;
      left: 50%;
      transform: translateX(-50%) translateY(60px);
      background: var(--text);
      color: #fff;
      font-size: 0.75rem;
      font-weight: 500;
      padding: 9px 20px;
      border-radius: 30px;
      opacity: 0;
      pointer-events: none;
      transition: transform .3s, opacity .3s;
      white-space: nowrap;
    }

    .toast.show {
      transform: translateX(-50%) translateY(0);
      opacity: 1;
    }
  </style>
</head>
<body>

<div class="topbar">
  <div class="brand">
    <div class="brand-icon">IO</div>
    <div>
      <h1>Sistema de Irrigacao</h1>
      <p>ESP32 - Recife - 8 valvulas</p>
    </div>
  </div>
  <div class="topbar-right">
    <span class="badge"><span class="badge-dot"></span>Online</span>
    <span id="clock">--:--:--</span>
  </div>
</div>

<div class="wrap">
  <div class="section-label">Controle de Valvulas</div>
  <div class="valve-grid" id="valveGrid"></div>

  <div class="section-label">Editor de Agendas</div>
  <div class="editor-grid" id="editorAgendas"></div>

  <button class="force-btn" onclick="limparAgendasVisual()">
    Limpar todas as agendas
  </button>

  <div class="network-info" id="networkInfo">Rede carregando...</div>
</div>

<div class="toast" id="toast"></div>

<script>
const toastEl = document.getElementById('toast');

function showToast(msg) {
  toastEl.textContent = msg;
  toastEl.classList.add('show');
  setTimeout(() => toastEl.classList.remove('show'), 2200);
}

async function requestJson(url, options) {
  const response = await fetch(url, options);
  const data = await response.json().catch(() => ({}));
  if (!response.ok) {
    throw new Error(data.erro || ('HTTP ' + response.status));
  }
  return data;
}

function pad2(v) {
  return String(v).padStart(2, '0');
}

function renderValvulas(valvulas) {
  const grid = document.getElementById('valveGrid');
  grid.innerHTML = valvulas.map(v => {
    const activeCls = v.ativa ? 'active' : '';
    const toggleCls = v.ativa ? 'on' : 'off';
    return `
      <div class="valve-card ${activeCls}" onclick="alternarValvula(${v.id})">
        <div>
          <div class="valve-num">Valvula ${pad2(v.id)} - GPIO ${v.gpio}</div>
          <div class="valve-name">Setor ${v.id}</div>
          <div class="valve-status">${v.status} (${v.origem})</div>
        </div>
        <button class="toggle ${toggleCls}" onclick="event.stopPropagation(); alternarValvula(${v.id});">
          <span class="knob"></span>
        </button>
      </div>
    `;
  }).join('');
}

function atualizarRede(rede) {
  const info = document.getElementById('networkInfo');
  const ap = rede.ap;
  const sta = rede.sta;
  let linhaSta = 'STA desativado';
  if (sta.configurado) {
    linhaSta = sta.conectado
      ? ('STA conectado em ' + sta.ssid + ' (' + sta.ip + ')')
      : ('STA tentando conectar em ' + sta.ssid);
  }
  info.textContent = 'AP: ' + ap.ssid + ' (' + ap.ip + ') | ' + linhaSta;
}

function maskAtivo(mask, bit) {
  return (Number(mask || 0) & (1 << bit)) !== 0;
}

function montarEditorAgendas(slots) {
  const alvo = document.getElementById('editorAgendas');
  const diasSigla = ['DOM', 'SEG', 'TER', 'QUA', 'QUI', 'SEX', 'SAB'];

  alvo.innerHTML = slots.map(s => {
    const hora = pad2(s.hora || 0) + ':' + pad2(s.minuto || 0);

    let diasHtml = '';
    for (let i = 0; i < 7; i++) {
      const checked = maskAtivo(s.diasMask, i) ? 'checked' : '';
      diasHtml += `<label class="chip"><input type="checkbox" id="dia-${s.slot}-${i}" ${checked} onchange="marcarEdicaoAgenda()"/>${diasSigla[i]}</label>`;
    }

    let setoresHtml = '';
    for (let i = 0; i < 8; i++) {
      const checked = maskAtivo(s.setoresMask, i) ? 'checked' : '';
      setoresHtml += `<label class="chip"><input type="checkbox" id="set-${s.slot}-${i}" ${checked} onchange="marcarEdicaoAgenda()"/>S${i + 1}</label>`;
    }

    return `
      <div class="editor-card">
        <div class="editor-head">Agenda ${s.slot}</div>
        <label class="chip"><input type="checkbox" id="ativo-${s.slot}" ${s.ativa ? 'checked' : ''} onchange="marcarEdicaoAgenda()"/>Ativa</label>
        <div class="editor-row" style="margin-top:8px;">
          <div>
            <label for="hora-${s.slot}">Horario</label>
            <input id="hora-${s.slot}" type="time" value="${hora}" onchange="marcarEdicaoAgenda()"/>
          </div>
          <div>
            <label for="dur-${s.slot}">Duracao (min)</label>
            <input id="dur-${s.slot}" type="number" min="1" max="240" step="1" value="${Number(s.duracaoMin || 10)}" onchange="marcarEdicaoAgenda()"/>
          </div>
        </div>
        <label style="font-size:0.68rem;color:#4a6652;display:block;margin:6px 0 3px;">Dias</label>
        <div class="chips">${diasHtml}</div>
        <label style="font-size:0.68rem;color:#4a6652;display:block;margin:6px 0 3px;">Setores</label>
        <div class="chips">${setoresHtml}</div>
        <div class="tiny-actions">
          <button class="tiny-btn primary" onclick="salvarAgendaVisual(${s.slot})">Salvar</button>
          <button class="tiny-btn" onclick="excluirAgendaVisual(${s.slot})">Excluir</button>
        </div>
      </div>
    `;
  }).join('');
}

let editorAgendaModificado = false;

function marcarEdicaoAgenda() {
  editorAgendaModificado = true;
}

function coletarMaskDias(slot) {
  let mask = 0;
  for (let i = 0; i < 7; i++) {
    if (document.getElementById(`dia-${slot}-${i}`)?.checked) {
      mask |= (1 << i);
    }
  }
  return mask;
}

function coletarMaskSetores(slot) {
  let mask = 0;
  for (let i = 0; i < 8; i++) {
    if (document.getElementById(`set-${slot}-${i}`)?.checked) {
      mask |= (1 << i);
    }
  }
  return mask;
}

async function salvarAgendaVisual(slot) {
  try {
    const horaTxt = document.getElementById(`hora-${slot}`).value || '06:00';
    const partes = horaTxt.split(':');
    const hora = Number(partes[0] || 0);
    const minuto = Number(partes[1] || 0);

    const duracao = Math.max(1, Math.min(240, Number(document.getElementById(`dur-${slot}`).value || 10)));
    const ativa = document.getElementById(`ativo-${slot}`).checked ? 1 : 0;
    const diasMask = coletarMaskDias(slot);
    const setoresMask = coletarMaskSetores(slot);

    const params = new URLSearchParams({
      slot: String(slot),
      active: String(ativa),
      hour: String(hora),
      minute: String(minuto),
      duration: String(duracao),
      diasMask: String(diasMask),
      setoresMask: String(setoresMask)
    });

    await requestJson('/api/schedule/save?' + params.toString(), { method: 'POST' });
    editorAgendaModificado = false;
    showToast('Agenda ' + slot + ' salva');
    await carregarAgendas(true);
  } catch (err) {
    showToast('Erro ao salvar agenda: ' + err.message);
  }
}

async function excluirAgendaVisual(slot) {
  try {
    await requestJson('/api/schedule/delete?slot=' + slot, { method: 'POST' });
    editorAgendaModificado = false;
    showToast('Agenda ' + slot + ' excluida');
    await carregarAgendas(true);
  } catch (err) {
    showToast('Erro ao excluir agenda: ' + err.message);
  }
}

async function limparAgendasVisual() {
  try {
    await requestJson('/api/schedule/clear', { method: 'POST' });
    editorAgendaModificado = false;
    showToast('Todas as agendas foram limpas');
    await carregarAgendas(true);
  } catch (err) {
    showToast('Erro ao limpar agendas: ' + err.message);
  }
}

async function carregarStatus() {
  const dados = await requestJson('/api/status');
  document.getElementById('clock').textContent = dados.hora || '--:--:--';
  renderValvulas(dados.valvulas || []);
  atualizarRede(dados.rede || { ap: {}, sta: {} });
}

async function carregarAgendas(forcarEditor = false) {
  const dados = await requestJson('/api/schedules');
  if (forcarEditor || !editorAgendaModificado) {
    montarEditorAgendas(dados.slots || []);
  }
}

async function alternarValvula(indice) {
  try {
    await requestJson('/api/valve/toggle?index=' + indice, { method: 'POST' });
    await carregarStatus();
  } catch (err) {
    showToast('Erro ao alternar valvula: ' + err.message);
  }
}

async function refreshAll() {
  try {
    await Promise.all([carregarStatus(), carregarAgendas(true)]);
  } catch (err) {
    showToast('Falha de comunicacao: ' + err.message);
  }
}

refreshAll();
setInterval(carregarStatus, 2000);
setInterval(carregarAgendas, 10000);
</script>

</body>
</html>
)HTML";
} // namespace

WebApManager::WebApManager(IrrigationController &irrigacao,
                           ScheduleManager &schedule,
                           RuntimeConfigManager &config,
                           RtcDriverDs3231 &rtc)
    : _irrigacao(irrigacao),
      _schedule(schedule),
      _config(config),
      _rtc(rtc),
      _server(80),
      _ativo(false),
      _rotasConfiguradas(false),
      _staConfigurada(WIFI_STA_ENABLED && strlen(WIFI_STA_SSID) > 0),
      _ultimoStatusSta(WL_IDLE_STATUS),
      _ultimoRetryStaMs(0)
{
}

void WebApManager::begin()
{
    if (DEBUG_SERIAL)
    {
        Serial.println("[WebAP] Inicializando rede e servidor...");
    }

    iniciarApServidor();
}

void WebApManager::atualizar()
{
    if (!_ativo)
    {
        iniciarApServidor();
        return;
    }

    _server.handleClient();
    tentarConexaoSta();
}

bool WebApManager::iniciarApServidor()
{
    if (_ativo)
    {
        return true;
    }

    WiFi.persistent(false);
    WiFi.setSleep(false);

    WiFi.mode(_staConfigurada ? WIFI_AP_STA : WIFI_AP);

    bool okAp = WiFi.softAP(WIFI_AP_SSID,
                            WIFI_AP_PASSWORD,
                            WIFI_AP_CHANNEL,
                            false,
                            WIFI_AP_MAX_CONNECTIONS);
    if (!okAp)
    {
        if (DEBUG_SERIAL)
        {
            Serial.println("[WebAP] Falha ao iniciar AP.");
        }
        return false;
    }

    if (_staConfigurada)
    {
        WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASSWORD);
        _ultimoRetryStaMs = millis();
        _ultimoStatusSta = WiFi.status();

        if (DEBUG_SERIAL)
        {
            Serial.print("[WebAP][STA] SSID alvo: ");
            Serial.println(WIFI_STA_SSID);
            Serial.println("[WebAP][STA] Aguardando conexao ao roteador...");
        }
    }

    if (!_rotasConfiguradas)
    {
        configurarRotas();
        _rotasConfiguradas = true;
    }

    _server.begin();
    _ativo = true;

    if (DEBUG_SERIAL)
    {
        Serial.print("[WebAP] AP ativo em: ");
        Serial.println(urlAcessoAp());
        if (_staConfigurada)
        {
            Serial.println("[WebAP][STA] Dica: para acesso pela rede do roteador, use URL STA quando conectar.");
        }
    }

    return true;
}

void WebApManager::pararApServidor()
{
    if (!_ativo)
    {
        return;
    }

    _server.stop();
    WiFi.softAPdisconnect(true);
    if (_staConfigurada)
    {
        WiFi.disconnect(true, true);
    }
    WiFi.mode(WIFI_OFF);
    _ativo = false;

    if (DEBUG_SERIAL)
    {
        Serial.println("[WebAP] AP e servidor encerrados.");
    }
}

bool WebApManager::ativo() const
{
    return _ativo;
}

const char *WebApManager::ssid() const
{
    return WIFI_AP_SSID;
}

const char *WebApManager::senha() const
{
    return WIFI_AP_PASSWORD;
}

String WebApManager::ipTexto() const
{
    return WiFi.softAPIP().toString();
}

String WebApManager::ipStaTexto() const
{
    if (!staConectada())
    {
        return String("0.0.0.0");
    }

    return WiFi.localIP().toString();
}

bool WebApManager::staConectada() const
{
    return _staConfigurada && (WiFi.status() == WL_CONNECTED);
}

String WebApManager::urlAcessoAp() const
{
    return String("http://") + ipTexto() + "/";
}

String WebApManager::urlAcessoSta() const
{
    if (!staConectada())
    {
        return String("");
    }

    return String("http://") + ipStaTexto() + "/";
}

String WebApManager::urlAcesso() const
{
    if (staConectada())
    {
        return urlAcessoSta();
    }

    return urlAcessoAp();
}

void WebApManager::configurarRotas()
{
    _server.on("/", HTTP_GET, [this]()
               { enviarPaginaPrincipal(); });

    _server.on("/status", HTTP_GET, [this]()
               { enviarStatusSistema(); });

    _server.on("/api/status", HTTP_GET, [this]()
               { enviarStatusSistema(); });

    _server.on("/api/schedules", HTTP_GET, [this]()
               { enviarListaAgendas(); });

    _server.on("/api/valve/toggle", HTTP_POST, [this]()
               {
        int indice = lerIndiceValvula();
        if (indice < 0)
        {
            enviarErroJson(400, "indice de valvula invalido");
            return;
        }

        _irrigacao.toggleValvula(indice);
        enviarStatusSistema(); });

    _server.on("/api/valve/set", HTTP_POST, [this]()
               {
        int indice = lerIndiceValvula();
        if (indice < 0)
        {
            enviarErroJson(400, "indice de valvula invalido");
            return;
        }

        if (!_server.hasArg("state"))
        {
            enviarErroJson(400, "parametro state ausente");
            return;
        }

        String state = _server.arg("state");
        state.trim();
        String lower = state;
        lower.toLowerCase();

        bool valorValido = (lower == "1" || lower == "0" ||
                            lower == "true" || lower == "false" ||
                            lower == "on" || lower == "off" ||
                            lower == "ligado" || lower == "desligado");
        if (!valorValido)
        {
            enviarErroJson(400, "state invalido");
            return;
        }

        bool ligar = textoParaBool(lower);
        bool aberta = (_irrigacao.estadoValvula(indice) == EstadoValvula::ABERTA);

        if (ligar && !aberta)
        {
            _irrigacao.toggleValvula(indice);
        }
        else if (!ligar && aberta)
        {
            _irrigacao.fecharValvula(indice);
        }
        enviarStatusSistema(); });

    _server.on("/api/valves/off-all", HTTP_POST, [this]()
               {
        _irrigacao.fecharTodas();
        enviarStatusSistema(); });

    _server.on("/api/schedule/save", HTTP_POST, [this]()
               {
        int slot = lerSlotAgenda();
        if (slot < 0)
        {
            enviarErroJson(400, "slot de agenda invalido");
            return;
        }

        AgendaSetor agenda;
        if (!_schedule.obterAgenda(slot, agenda))
        {
            enviarErroJson(500, "falha ao obter agenda");
            return;
        }

        if (_server.hasArg("active"))
            agenda.ativa = textoParaBool(_server.arg("active"));
        else
            agenda.ativa = true;

        if (_server.hasArg("hour"))
            agenda.hora = (uint8_t)_server.arg("hour").toInt();
        if (_server.hasArg("minute"))
            agenda.minuto = (uint8_t)_server.arg("minute").toInt();
        if (_server.hasArg("duration"))
            agenda.duracaoMin = (uint16_t)_server.arg("duration").toInt();
        if (_server.hasArg("diasMask"))
            agenda.diasMask = (uint8_t)_server.arg("diasMask").toInt();
        if (_server.hasArg("setoresMask"))
            agenda.setoresMask = (uint8_t)_server.arg("setoresMask").toInt();

        String erro;
        if (!_schedule.salvarAgenda(slot, agenda, erro))
        {
            enviarErroJson(400, erro.c_str());
            return;
        }

        enviarListaAgendas(); });

    _server.on("/api/schedule/delete", HTTP_POST, [this]()
               {
        int slot = lerSlotAgenda();
        if (slot < 0)
        {
            enviarErroJson(400, "slot de agenda invalido");
            return;
        }

        if (!_schedule.removerAgenda(slot))
        {
            enviarErroJson(500, "falha ao remover agenda");
            return;
        }

        enviarListaAgendas(); });

    _server.on("/api/schedule/clear", HTTP_POST, [this]()
               {
        if (!_schedule.limparTodasAgendas())
        {
            enviarErroJson(500, "falha ao limpar agendas");
            return;
        }

        enviarListaAgendas(); });

    _server.on("/api/config/runtime", HTTP_POST, [this]()
               {
        uint32_t timeoutMin = _config.timeoutManualMs() / 60000UL;
        uint16_t duracaoMin = _config.duracaoPadraoMin();

        if (_server.hasArg("timeoutMin"))
            timeoutMin = (uint32_t)_server.arg("timeoutMin").toInt();
        if (_server.hasArg("duracaoMin"))
            duracaoMin = (uint16_t)_server.arg("duracaoMin").toInt();

        bool okTimeout = _config.salvarTimeoutManualMs(timeoutMin * 60000UL);
        bool okDuracao = _config.salvarDuracaoPadraoMin(duracaoMin);

        if (!okTimeout || !okDuracao)
        {
            enviarErroJson(500, "falha ao salvar configuracao runtime");
            return;
        }

        enviarStatusSistema(); });

    _server.on("/api/rtc/set", HTTP_POST, [this]()
               {
        DateTime atual = _rtc.agora();
        int ano = atual.year();
        int mes = atual.month();
        int dia = atual.day();
        int hora = atual.hour();
        int minuto = atual.minute();

        if (_server.hasArg("year"))
            ano = _server.arg("year").toInt();
        if (_server.hasArg("month"))
            mes = _server.arg("month").toInt();
        if (_server.hasArg("day"))
            dia = _server.arg("day").toInt();
        if (_server.hasArg("hour"))
            hora = _server.arg("hour").toInt();
        if (_server.hasArg("minute"))
            minuto = _server.arg("minute").toInt();

        if (ano < 2000 || ano > 2099 ||
            mes < 1 || mes > 12 ||
            dia < 1 || dia > 31 ||
            hora < 0 || hora > 23 ||
            minuto < 0 || minuto > 59)
        {
            enviarErroJson(400, "data/hora invalida");
            return;
        }

        _rtc.ajustarHora(DateTime(ano, mes, dia, hora, minuto, 0));
        enviarStatusSistema(); });

    _server.onNotFound([this]()
                       {
        if (_server.uri().startsWith("/api/"))
        {
            enviarErroJson(404, "rota nao encontrada");
            return;
        }
        _server.send(404, "text/plain; charset=utf-8", "Rota nao encontrada"); });
}

void WebApManager::tentarConexaoSta()
{
    if (!_staConfigurada)
    {
        return;
    }

    wl_status_t statusAtual = WiFi.status();
    if (statusAtual != _ultimoStatusSta)
    {
        _ultimoStatusSta = statusAtual;

        if (DEBUG_SERIAL)
        {
            Serial.print("[WebAP][STA] Status: ");
            Serial.println(nomeStatusSta(statusAtual));

            if (statusAtual == WL_CONNECTED)
            {
                Serial.print("[WebAP][STA] Conectado no SSID: ");
                Serial.println(WiFi.SSID());
                Serial.print("[WebAP][STA] IP STA: ");
                Serial.println(ipStaTexto());
                Serial.print("[WebAP][STA] URL STA: ");
                Serial.println(urlAcessoSta());
                Serial.print("[WebAP][AP ] URL AP : ");
                Serial.println(urlAcessoAp());
            }
            else if (statusAtual == WL_CONNECT_FAILED)
            {
                Serial.println("[WebAP][STA] Falha de autenticacao. Verifique senha.");
            }
            else if (statusAtual == WL_NO_SSID_AVAIL)
            {
                Serial.println("[WebAP][STA] SSID nao encontrado. Verifique nome/rede 2.4GHz.");
            }
        }
    }

    if (statusAtual == WL_CONNECTED)
    {
        return;
    }

    unsigned long agora = millis();
    if ((unsigned long)(agora - _ultimoRetryStaMs) < WIFI_STA_RETRY_MS)
    {
        return;
    }

    _ultimoRetryStaMs = agora;
    WiFi.disconnect(false, false);
    WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASSWORD);

    if (DEBUG_SERIAL)
    {
        Serial.print("[WebAP][STA] Tentando conectar em ");
        Serial.println(WIFI_STA_SSID);
    }
}

void WebApManager::enviarPaginaPrincipal()
{
    _server.send_P(200, "text/html; charset=utf-8", WEB_DASHBOARD_HTML);
}

void WebApManager::enviarStatusSistema()
{
    DateTime agora = _rtc.agora();

    char hora[10];
    snprintf(hora, sizeof(hora), "%02d:%02d:%02d", agora.hour(), agora.minute(), agora.second());

    char data[16];
    snprintf(data, sizeof(data), "%02d/%02d/%04d", agora.day(), agora.month(), agora.year());

    bool staConectada = this->staConectada();

    String json;
    json.reserve(2000);
    json += "{";
    json += "\"ok\":true,";
    json += "\"hora\":\"" + String(hora) + "\",";
    json += "\"data\":\"" + String(data) + "\",";
    json += "\"uptime_s\":" + String(millis() / 1000UL) + ",";

    json += "\"rede\":{";
    json += "\"ap\":{";
    json += "\"ativo\":" + String(_ativo ? "true" : "false") + ",";
    json += "\"ssid\":\"" + String(ssid()) + "\",";
    json += "\"ip\":\"" + ipTexto() + "\",";
    json += "\"url\":\"" + urlAcessoAp() + "\"";
    json += "},";
    json += "\"sta\":{";
    json += "\"configurado\":" + String(_staConfigurada ? "true" : "false") + ",";
    json += "\"conectado\":" + String(staConectada ? "true" : "false") + ",";
    json += "\"ssid\":\"" + String(_staConfigurada ? WIFI_STA_SSID : "") + "\",";
    json += "\"ip\":\"" + ipStaTexto() + "\",";
    json += "\"url\":\"" + String(staConectada ? urlAcessoSta() : String("")) + "\"";
    json += "}";
    json += "},";

    json += "\"agendas_ativas\":" + String(_schedule.totalAtivas()) + ",";

    json += "\"valvulas\":[";
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
        bool aberta = (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA);
        bool autoAgenda = _irrigacao.valvulaEmAgendamento(i);

        json += "{";
        json += "\"id\":" + String(i + 1) + ",";
        json += "\"gpio\":" + String(gpioValvula(i)) + ",";
        json += "\"ativa\":" + String(aberta ? "true" : "false") + ",";
        json += "\"status\":\"" + String(aberta ? "Ativa" : "Inativa") + "\",";
        json += "\"origem\":\"" + String(!aberta ? "desligada" : (autoAgenda ? "agenda" : "manual")) + "\"";
        json += "}";

        if (i < (NUM_VALVULAS - 1))
        {
            json += ",";
        }
    }
    json += "]";

    json += "}";

    enviarRespostaJson(200, json);
}

void WebApManager::enviarListaAgendas()
{
    DateTime agora = _rtc.agora();

    String json;
    json.reserve(1400);
    json += "{";
    json += "\"ok\":true,";
    json += "\"total_ativas\":" + String(_schedule.totalAtivas()) + ",";
    json += "\"slots\":[";

    for (int slot = 0; slot < MAX_AGENDAS_TOTAIS; slot++)
    {
        AgendaSetor agenda;
        _schedule.obterAgenda(slot, agenda);

        json += "{";
        json += "\"slot\":" + String(slot + 1) + ",";
        json += "\"ativa\":" + String(agenda.ativa ? "true" : "false") + ",";
        json += "\"hora\":" + String(agenda.hora) + ",";
        json += "\"minuto\":" + String(agenda.minuto) + ",";
        json += "\"duracaoMin\":" + String(agenda.duracaoMin) + ",";
        json += "\"diasMask\":" + String(agenda.diasMask) + ",";
        json += "\"setoresMask\":" + String(agenda.setoresMask);
        json += "}";

        if (slot < (MAX_AGENDAS_TOTAIS - 1))
        {
            json += ",";
        }
    }
    json += "],";

    DateTime proximaData;
    AgendaSetor proximaAgenda;
    int slotProximo = -1;
    bool temProxima = _schedule.obterProximaExecucao(agora, proximaData, proximaAgenda, slotProximo);

    json += "\"proxima\":";
    if (!temProxima)
    {
        json += "null";
    }
    else
    {
        json += "{";
        json += "\"slot\":" + String(slotProximo + 1) + ",";
        json += "\"hora\":" + String(proximaData.hour()) + ",";
        json += "\"minuto\":" + String(proximaData.minute()) + ",";
        json += "\"diasMask\":" + String(proximaAgenda.diasMask) + ",";
        json += "\"setoresMask\":" + String(proximaAgenda.setoresMask);
        json += "}";
    }

    json += "}";

    enviarRespostaJson(200, json);
}

void WebApManager::enviarRespostaJson(int statusCode, const String &json)
{
    _server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
    _server.send(statusCode, "application/json; charset=utf-8", json);
}

void WebApManager::enviarErroJson(int statusCode, const char *mensagem)
{
    String json;
    json.reserve(160);
    json += "{\"ok\":false,\"erro\":\"";
    json += mensagem;
    json += "\"}";

    enviarRespostaJson(statusCode, json);
}

int WebApManager::lerIndiceValvula() const
{
    if (!_server.hasArg("index"))
    {
        return -1;
    }

    String txt = _server.arg("index");
    txt.trim();

    char *endPtr = nullptr;
    long valor = strtol(txt.c_str(), &endPtr, 10);
    if (endPtr == txt.c_str() || *endPtr != '\0')
    {
        return -1;
    }

    if (valor >= 1 && valor <= NUM_VALVULAS)
    {
        return (int)valor - 1;
    }

    if (valor >= 0 && valor < NUM_VALVULAS)
    {
        return (int)valor;
    }

    return -1;
}

int WebApManager::lerSlotAgenda() const
{
    if (!_server.hasArg("slot"))
    {
        return -1;
    }

    String txt = _server.arg("slot");
    txt.trim();

    char *endPtr = nullptr;
    long valor = strtol(txt.c_str(), &endPtr, 10);
    if (endPtr == txt.c_str() || *endPtr != '\0')
    {
        return -1;
    }

    if (valor >= 1 && valor <= MAX_AGENDAS_TOTAIS)
    {
        return (int)valor - 1;
    }

    if (valor >= 0 && valor < MAX_AGENDAS_TOTAIS)
    {
        return (int)valor;
    }

    return -1;
}

uint8_t WebApManager::gpioValvula(int indice)
{
    switch (indice)
    {
    case 0:
        return PIN_RELAY_1;
    case 1:
        return PIN_RELAY_2;
    case 2:
        return PIN_RELAY_3;
    case 3:
        return PIN_RELAY_4;
    case 4:
        return PIN_RELAY_5;
    case 5:
        return PIN_RELAY_6;
    case 6:
        return PIN_RELAY_7;
    case 7:
        return PIN_RELAY_8;
    default:
        return 0;
    }
}

bool WebApManager::textoParaBool(const String &texto)
{
    String valor = texto;
    valor.trim();
    valor.toLowerCase();

    return (valor == "1" || valor == "true" || valor == "on" || valor == "ligado" || valor == "sim");
}
