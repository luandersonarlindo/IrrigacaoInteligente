// ============================================================
//  web_ap_manager.cpp - Implementacao do WebApManager
// ============================================================

#include "web_ap_manager.h"

#include <stdlib.h>
#include <string.h>

namespace
{
  const char WEB_DASHBOARD_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8"/>
  <meta name="viewport" content="width=device-width, initial-scale=1"/>
  <title>Sistema de Irrigacao</title>
  <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap" rel="stylesheet"/>
  <link type="image/png" sizes="32x32" rel="icon" href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAACXBIWXMAAAsTAAALEwEAmpwYAAACgUlEQVR4nM2WXWiPURzHP4+hVhIr5m3+eSnNGraJlpdcIKEmshtaLky5GilzMfOW91pcuRE3rlyQWqgJF5sbXMxLcaG4WCtpF6IxhX71fep0ep75n+f/YN86dc7v9XvOc57f+cEYxXGgNsd4S4BjIQ67gaocCcwFdoU4TAYizaeUkDj2jRSzaPQBm4GVwFtPtwM4lzK2e7bmuwLYAvSGEChPmRvOAr9SxpmAOKNiHzAPmAG0SdYM1Os4hxKSD0lnNjvl06YY84HWEALtQDVQAE5KthdYpfkp4KY3TGZYLVvkaxdwMXA4hECUMnePc6o3yjPEScUjYAOwHOhP0HclfAKT+XgBNAAbgYcEYA4wASjTEWYlYL7jFGt2CIGtQOUo+mIJxKhUzKJxWcefFwGLdYkc0RVIIBgR0AhM+18EqhV0BLieQOSvE0CFJQ7+UQXmnxKIgKtOgm/OTQ4hMDG0CPnOT5wkw8Aa3eqRPxAwu1sibg1OZhSAL06iQWCm3guXwAXZzwK6HfltoIIScchL1q0K16/1c70HVr4/OXZHyAnjgfceiW1qWgaA6ZoPO/pOlfIq9YSZ70GMgx6Blwq6CKjRt451D6S75siWEbjj9cBSYJJk1tO980hsku1TR/ZTHXWzIztPIGqA73L+AdxVZTQyd5zAdcB+j1SPdv9Ka/sTMmEB8NjbWYee1l5dPMMbj4B1Q+s0/6z7UVJb/cFL0KLvflon5RckI35C8yvkgKNeggF99wrt1tV91fH3aN2UB4HGhF2ule6iJ7fPYXit9cI8CBQSCOyRzn0r"/>
  <link type="image/png" sizes="96x96" rel="icon" href="data:image/png;base64,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"/>

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

    .notif-btn {
      width: 34px;
      height: 34px;
      border: 1px solid var(--border2);
      border-radius: 10px;
      background: #fff;
      display: inline-flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      transition: border-color .15s ease, background .15s ease;
      padding: 0;
    }

    .notif-btn img {
      width: 19px;
      height: 19px;
      display: block;
      opacity: 0.85;
    }

    .notif-btn.active {
      border-color: #f1c27d;
      background: #fff8eb;
    }

    .notif-btn.active img {
      opacity: 1;
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

    .notif-panel {
      max-width: 860px;
      margin: 10px auto 0;
      padding: 0 20px;
      display: none;
    }

    .notif-panel.open {
      display: block;
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

    .alerts-wrap {
      background: #f4f6fb;
      border: 1px solid #e3e7f0;
      border-radius: 16px;
      padding: 14px;
      box-shadow: var(--shadow);
      margin-bottom: 16px;
    }

    .alerts-head {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 10px;
      margin-bottom: 12px;
    }

    .alerts-title-wrap {
      display: flex;
      flex-direction: column;
      gap: 2px;
    }

    .alerts-title {
      font-size: 1.05rem;
      color: #222b45;
      font-weight: 700;
      letter-spacing: 0.01em;
    }

    .alerts-subtitle {
      font-size: 0.68rem;
      color: #7d8799;
      font-weight: 600;
    }

    .notif-actions {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      min-width: 0;
    }

    .notif-tabs {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 6px;
      background: #e9ecf3;
      border-radius: 12px;
      padding: 4px;
      margin-bottom: 10px;
    }

    .notif-tab {
      border: none;
      border-radius: 9px;
      padding: 8px 10px;
      font-size: 0.75rem;
      font-weight: 700;
      font-family: 'Inter', sans-serif;
      color: #6f7891;
      background: transparent;
      cursor: pointer;
    }

    .notif-tab.active {
      background: #fff;
      color: #29314a;
      box-shadow: 0 1px 2px #00000014;
    }

    .notif-tab-content {
      display: block;
    }

    .notif-tab-content.hidden {
      display: none;
    }

    .notif-list-wrap {
      max-height: 360px;
      overflow-y: auto;
      padding-right: 3px;
    }

    .notif-group-label {
      font-size: 0.66rem;
      font-weight: 700;
      color: #7f889f;
      text-transform: uppercase;
      letter-spacing: 0.08em;
      margin: 8px 2px;
    }

    @media(max-width:640px){
      .alerts-head {
        flex-direction: column;
        align-items: stretch;
      }

      .notif-actions {
        width: 100%;
        min-width: 0;
      }

      .notif-actions .tiny-btn {
        width: 100%;
      }
    }

    .alert-list {
      display: block;
      margin: 0;
    }

    .history-list {
      display: block;
      margin: 0;
    }

    .notif-row {
      display: flex;
      align-items: flex-start;
      gap: 10px;
      padding: 11px 8px;
      background: #fff;
      border: 1px solid #e6ebf5;
      border-radius: 12px;
      margin-bottom: 8px;
    }

    .notif-icon {
      width: 34px;
      height: 34px;
      border-radius: 50%;
      display: inline-flex;
      align-items: center;
      justify-content: center;
      font-size: 0.78rem;
      font-weight: 700;
      color: #3f61d8;
      background: #e7ecff;
      border: 1px solid #d6dfff;
      flex-shrink: 0;
    }

    .notif-icon.warning {
      color: #a56a13;
      background: #fff3df;
      border-color: #f6d39d;
    }

    .notif-icon.critical {
      color: #b23a33;
      background: #ffe8e6;
      border-color: #f6c2be;
    }

    .notif-text {
      min-width: 0;
      flex: 1;
    }

    .notif-msg {
      font-size: 0.79rem;
      color: #2e3448;
      line-height: 1.35;
      font-weight: 600;
    }

    .notif-meta {
      margin-top: 3px;
      font-size: 0.66rem;
      color: #818aa2;
      text-transform: uppercase;
      letter-spacing: 0.06em;
    }

    .empty-alert {
      border: 1px dashed #cfd6e6;
      border-radius: 12px;
      padding: 12px;
      font-size: 0.72rem;
      color: #74809a;
      background: #fff;
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
      <h1>Sistema de Irrigação</h1>
      <p>ESP32 - 8 Válvulas</p>
    </div>
  </div>
  <div class="topbar-right">
    <button id="notifButton" class="notif-btn" onclick="alternarPainelNotificacoes()" aria-label="Notificacoes" title="Notificacoes">
      <img id="notifIcon" alt="Notificacoes" src=""/>
    </button>
    <span class="badge"><span class="badge-dot"></span>Online</span>
    <span id="clock">--:--:--</span>
  </div>
</div>

<div id="notifPanel" class="notif-panel">
  <div class="alerts-wrap">
    <div class="alerts-head">
      <div class="alerts-title-wrap">
        <span class="alerts-title">Notificacoes</span>
        <span class="alerts-subtitle" id="alertSummary">Verificando notificacoes...</span>
      </div>
      <div class="notif-actions">
        <button class="tiny-btn" onclick="marcarNotificacoesLidas()">Marcar tudo como lido</button>
      </div>
    </div>

    <div class="notif-tabs">
      <button id="notifTabAlertas" class="notif-tab active" onclick="selecionarAbaNotificacoes('alertas')">Avisos</button>
      <button id="notifTabHistorico" class="notif-tab" onclick="selecionarAbaNotificacoes('historico')">Historico</button>
    </div>

    <div id="notifTabContentAlertas" class="notif-tab-content">
      <div class="alert-list notif-list-wrap" id="alertBox"></div>
    </div>

    <div id="notifTabContentHistorico" class="notif-tab-content hidden">
      <div class="history-list notif-list-wrap" id="historyList"></div>
    </div>
  </div>
</div>

<div class="wrap">
  <div class="section-label">Controle de Válvulas</div>
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
const notifButtonEl = document.getElementById('notifButton');
const notifIconEl = document.getElementById('notifIcon');
const notifPanelEl = document.getElementById('notifPanel');
const notifTabAlertasEl = document.getElementById('notifTabAlertas');
const notifTabHistoricoEl = document.getElementById('notifTabHistorico');
const notifTabContentAlertasEl = document.getElementById('notifTabContentAlertas');
const notifTabContentHistoricoEl = document.getElementById('notifTabContentHistorico');

const BELL_ICON_INACTIVE = 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiIGNsYXNzPSJsdWNpZGUgbHVjaWRlLWJlbGwtaWNvbiBsdWNpZGUtYmVsbCI+PHBhdGggZD0iTTEwLjI2OCAyMWEyIDIgMCAwIDAgMy40NjQgMCIvPjxwYXRoIGQ9Ik0zLjI2MiAxNS4zMjZBMSAxIDAgMCAwIDQgMTdoMTZhMSAxIDAgMCAwIC43NC0xLjY3M0MxOS40MSAxMy45NTYgMTggMTIuNDk5IDE4IDhBNiA2IDAgMCAwIDYgOGMwIDQuNDk5LTEuNDExIDUuOTU2LTIuNzM4IDcuMzI2Ii8+PC9zdmc+';
const BELL_ICON_ACTIVE = 'data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiIGNsYXNzPSJsdWNpZGUgbHVjaWRlLWJlbGwtZG90LWljb24gbHVjaWRlLWJlbGwtZG90Ij48cGF0aCBkPSJNMTAuMjY4IDIxYTIgMiAwIDAgMCAzLjQ2NCAwIi8+PHBhdGggZD0iTTExLjY4IDIuMDA5QTYgNiAwIDAgMCA2IDhjMCA0LjQ5OS0xLjQxMSA1Ljk1Ni0yLjczOCA3LjMyNkExIDEgMCAwIDAgNCAxN2gxNmExIDEgMCAwIDAgLjc0LTEuNjczYy0uODI0LS44NS0xLjY3OC0xLjczMS0yLjIxLTMuMzQ4Ii8+PGNpcmNsZSBjeD0iMTgiIGN5PSI1IiByPSIzIi8+PC9zdmc+';

const STORAGE_NOTIFICACOES_HIST = 'irrigacao.notificacoes.historico';
const STORAGE_NOTIFICACOES_LIDAS_ATE = 'irrigacao.notificacoes.lidasAte';

let cacheAlertasAtivos = [];
let cacheHistoricoNotificacoes = [];
let abaNotificacaoAtual = 'alertas';

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

function carregarHistoricoLocalStorage() {
  try {
    const bruto = localStorage.getItem(STORAGE_NOTIFICACOES_HIST);
    if (!bruto) return [];
    const dados = JSON.parse(bruto);
    return Array.isArray(dados) ? dados : [];
  } catch (_err) {
    return [];
  }
}

function salvarHistoricoLocalStorage(historico) {
  try {
    localStorage.setItem(STORAGE_NOTIFICACOES_HIST, JSON.stringify(Array.isArray(historico) ? historico : []));
  } catch (_err) {
  }
}

function obterMaiorIdNotificacao(historico) {
  if (!Array.isArray(historico) || historico.length === 0) return 0;
  return historico.reduce((acc, item) => {
    const id = Number(item?.id || 0);
    return id > acc ? id : acc;
  }, 0);
}

function obterIdLidoAte() {
  try {
    const valor = Number(localStorage.getItem(STORAGE_NOTIFICACOES_LIDAS_ATE) || 0);
    return Number.isFinite(valor) ? valor : 0;
  } catch (_err) {
    return 0;
  }
}

function definirIdLidoAte(id) {
  try {
    localStorage.setItem(STORAGE_NOTIFICACOES_LIDAS_ATE, String(Math.max(0, Number(id || 0))));
  } catch (_err) {
  }
}

function existeNotificacaoNaoLida(historico) {
  const ultimoLido = obterIdLidoAte();
  return Array.isArray(historico) && historico.some(item => Number(item?.id || 0) > ultimoLido);
}

function atualizarIndicadorNotificacoes() {
  const temAlertaRelevante = Array.isArray(cacheAlertasAtivos) &&
    cacheAlertasAtivos.some(a => String(a?.nivel || 'info').toLowerCase() !== 'info');
  const ativo = temAlertaRelevante || existeNotificacaoNaoLida(cacheHistoricoNotificacoes);
  notifIconEl.src = ativo ? BELL_ICON_ACTIVE : BELL_ICON_INACTIVE;
  notifButtonEl.classList.toggle('active', ativo);
}

function marcarNotificacoesLidas() {
  const maiorId = obterMaiorIdNotificacao(cacheHistoricoNotificacoes);
  definirIdLidoAte(maiorId);
  atualizarIndicadorNotificacoes();
  showToast('Notificacoes marcadas como lidas');
}

function selecionarAbaNotificacoes(aba) {
  abaNotificacaoAtual = aba === 'historico' ? 'historico' : 'alertas';

  const emAlertas = abaNotificacaoAtual === 'alertas';

  if (notifTabAlertasEl) {
    notifTabAlertasEl.classList.toggle('active', emAlertas);
  }
  if (notifTabHistoricoEl) {
    notifTabHistoricoEl.classList.toggle('active', !emAlertas);
  }
  if (notifTabContentAlertasEl) {
    notifTabContentAlertasEl.classList.toggle('hidden', !emAlertas);
  }
  if (notifTabContentHistoricoEl) {
    notifTabContentHistoricoEl.classList.toggle('hidden', emAlertas);
  }
}

function alternarPainelNotificacoes() {
  notifPanelEl.classList.toggle('open');
  if (notifPanelEl.classList.contains('open')) {
    const maiorId = obterMaiorIdNotificacao(cacheHistoricoNotificacoes);
    definirIdLidoAte(maiorId);
    atualizarIndicadorNotificacoes();
    selecionarAbaNotificacoes(abaNotificacaoAtual);
  }
}

function escapeHtml(texto) {
  return String(texto || '')
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#39;');
}

function renderValvulas(valvulas) {
  const grid = document.getElementById('valveGrid');
  grid.innerHTML = valvulas.map(v => {
    const activeCls = v.ativa ? 'active' : '';
    const toggleCls = v.ativa ? 'on' : 'off';
    return `
      <div class="valve-card ${activeCls}" onclick="alternarValvula(${v.id})">
        <div>
          <div class="valve-num">Válvula ${pad2(v.id)} - GPIO ${v.gpio}</div>
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

function classeNivel(nivel) {
  const n = String(nivel || 'info').toLowerCase();
  if (n === 'critical') return 'critical';
  if (n === 'warning') return 'warning';
  return 'info';
}

function rotuloNivelLeigo(nivel) {
  const n = String(nivel || 'info').toLowerCase();
  if (n === 'critical') return 'Urgente';
  if (n === 'warning') return 'Atencao';
  return 'Informacao';
}

function iconeNivelLeigo(nivel) {
  const n = String(nivel || 'info').toLowerCase();
  if (n === 'critical') return '!!';
  if (n === 'warning') return '!';
  return 'i';
}

function traduzirMensagemLeiga(mensagem) {
  const texto = String(mensagem || '').trim();
  if (!texto) return 'Atualizacao do sistema.';

  const manuaisAbertas = texto.match(/(\d+)\s+valvula\(s\)\s+manual\(is\)\s+aberta\(s\)/i);
  if (manuaisAbertas) {
    return manuaisAbertas[1] + ' setor(es) estao ligados manualmente neste momento.';
  }

  if (/STA configurado, mas desconectado/i.test(texto)) {
    return 'O Wi-Fi da casa esta desconectado. O acesso local do equipamento continua funcionando.';
  }

  if (/Conexao STA estabelecida/i.test(texto)) {
    return 'Wi-Fi da casa conectado com sucesso.';
  }

  if (/Conexao STA perdida/i.test(texto)) {
    return 'Wi-Fi da casa foi desconectado.';
  }

  if (/Nenhuma agenda ativa configurada/i.test(texto)) {
    return 'Nenhuma programacao automatica ativa foi configurada.';
  }

  if (/Servidor web iniciado/i.test(texto)) {
    return 'Painel web iniciado e pronto para uso.';
  }

  const setorAtualizado = texto.match(/Setor\s+(\d+)\s+(aberto|fechado)/i);
  if (setorAtualizado) {
    const acao = setorAtualizado[2].toLowerCase() === 'aberto' ? 'ligado' : 'desligado';
    return 'Setor ' + setorAtualizado[1] + ' foi ' + acao + '.';
  }

  const agendaSalva = texto.match(/Agenda\s+(\d+)\s+salva/i);
  if (agendaSalva) {
    return 'Programacao ' + agendaSalva[1] + ' foi salva.';
  }

  const agendaRemovida = texto.match(/Agenda\s+(\d+)\s+removida/i);
  if (agendaRemovida) {
    return 'Programacao ' + agendaRemovida[1] + ' foi removida.';
  }

  return texto;
}

function formatarDataHoraLeiga(data, hora) {
  const dataTxt = String(data || '').trim();
  const horaTxt = String(hora || '').trim();
  if (!dataTxt && !horaTxt) return 'Horario indisponivel';
  if (dataTxt && horaTxt) return dataTxt + ' as ' + horaTxt;
  if (dataTxt) return dataTxt;
  return horaTxt;
}

function normalizarDataBr(data) {
  const texto = String(data || '').trim();
  const partes = texto.match(/^(\d{1,2})\/(\d{1,2})\/(\d{4})$/);
  if (!partes) return '';
  return pad2(Number(partes[1])) + '/' + pad2(Number(partes[2])) + '/' + partes[3];
}

function dataBrComOffset(dias) {
  const data = new Date();
  data.setDate(data.getDate() + dias);
  return pad2(data.getDate()) + '/' + pad2(data.getMonth() + 1) + '/' + data.getFullYear();
}

function agruparHistoricoPorDia(itens) {
  const hoje = dataBrComOffset(0);
  const ontem = dataBrComOffset(-1);
  const grupos = { hoje: [], ontem: [], anteriores: [] };

  (itens || []).forEach(item => {
    const dataItem = normalizarDataBr(item?.data);
    if (dataItem === hoje) {
      grupos.hoje.push(item);
      return;
    }
    if (dataItem === ontem) {
      grupos.ontem.push(item);
      return;
    }
    grupos.anteriores.push(item);
  });

  return grupos;
}

function removerDuplicatasHistoricoVisual(itens) {
  const vistos = new Set();
  return (itens || []).filter(item => {
    const mensagem = traduzirMensagemLeiga(item?.mensagem || 'Sem mensagem');
    const nivel = String(item?.nivel || 'info').toLowerCase();
    const data = normalizarDataBr(item?.data);
    const hora = String(item?.hora || '').trim();
    const chave = mensagem + '|' + nivel + '|' + data + '|' + hora;

    if (vistos.has(chave)) {
      return false;
    }

    vistos.add(chave);
    return true;
  });
}

function montarLinhaNotificacao(mensagem, detalhe, nivel) {
  const classe = classeNivel(nivel);
  const icone = iconeNivelLeigo(nivel);

  return `
    <div class="notif-row">
      <div class="notif-icon ${classe}">${escapeHtml(icone)}</div>
      <div class="notif-text">
        <div class="notif-msg">${escapeHtml(mensagem)}</div>
        <div class="notif-meta">${escapeHtml(detalhe)}</div>
      </div>
    </div>
  `;
}

function renderAlertas(alertas) {
  const alvo = document.getElementById('alertBox');
  const resumo = document.getElementById('alertSummary');

  if (!Array.isArray(alertas) || alertas.length === 0) {
    resumo.textContent = 'Tudo certo no momento.';
    alvo.innerHTML = '<div class="empty-alert">Nenhum aviso importante agora.</div>';
    return;
  }

  const total = alertas.length;
  resumo.textContent = total === 1
    ? 'Voce tem 1 aviso importante.'
    : ('Voce tem ' + total + ' avisos importantes.');

  alvo.innerHTML = alertas.map(a => {
    const nivelLeigo = rotuloNivelLeigo(a.nivel);
    const titulo = nivelLeigo === 'Urgente'
      ? 'Acao recomendada agora'
      : (nivelLeigo === 'Atencao' ? 'Atencao necessaria' : 'Informacao do sistema');

    return montarLinhaNotificacao(
      titulo + ': ' + traduzirMensagemLeiga(a.mensagem || 'Sem descricao'),
      nivelLeigo,
      a.nivel
    );
  }).join('');
}

function renderHistorico(historico) {
  const alvo = document.getElementById('historyList');

  if (!Array.isArray(historico) || historico.length === 0) {
    alvo.innerHTML = '<div class="empty-alert">Sem registros de eventos ate o momento.</div>';
    return;
  }

  const itens = removerDuplicatasHistoricoVisual(historico.slice().reverse()).slice(0, 20);
  const grupos = agruparHistoricoPorDia(itens);

  const montarSecao = (titulo, lista) => {
    if (!Array.isArray(lista) || lista.length === 0) {
      return '';
    }

    const linhas = lista.map(e => {
      const nivelLeigo = rotuloNivelLeigo(e.nivel);
      const dataHora = formatarDataHoraLeiga('', e.hora || '--:--:--');
      const mensagem = traduzirMensagemLeiga(e.mensagem || 'Sem mensagem');
      return montarLinhaNotificacao(mensagem, nivelLeigo + ' | ' + dataHora, e.nivel);
    }).join('');

    return '<div class="notif-group-label">' + escapeHtml(titulo) + '</div>' + linhas;
  };

  const secoes =
    montarSecao('Hoje', grupos.hoje) +
    montarSecao('Ontem', grupos.ontem) +
    montarSecao('Anteriores', grupos.anteriores);

  alvo.innerHTML = secoes || '<div class="empty-alert">Sem registros de eventos ate o momento.</div>';
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

async function carregarAlertasHistorico() {
  const dados = await requestJson('/api/events');
  cacheAlertasAtivos = dados.alertas || [];
  cacheHistoricoNotificacoes = dados.historico || [];

  salvarHistoricoLocalStorage(cacheHistoricoNotificacoes);

  renderAlertas(cacheAlertasAtivos);
  renderHistorico(cacheHistoricoNotificacoes);
  atualizarIndicadorNotificacoes();
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
    await Promise.all([carregarStatus(), carregarAgendas(true), carregarAlertasHistorico()]);
  } catch (err) {
    showToast('Falha de comunicacao: ' + err.message);
  }
}

cacheHistoricoNotificacoes = carregarHistoricoLocalStorage();
renderHistorico(cacheHistoricoNotificacoes);
renderAlertas([]);
atualizarIndicadorNotificacoes();
selecionarAbaNotificacoes('alertas');

refreshAll();
setInterval(carregarStatus, 2000);
setInterval(carregarAgendas, 10000);
setInterval(carregarAlertasHistorico, 4000);
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
      _ultimoRetryStaMs(0),
      _historicoCount(0),
      _historicoHead(0),
      _proximoEventoId(1),
      _monitoramentoInicializado(false),
      _ultimaStaConectada(false),
      _ultimoTotalAgendasAtivas(0)
{
  for (int i = 0; i < NUM_VALVULAS; i++)
  {
    _ultimoEstadoValvulas[i] = false;
    _ultimaOrigemAgenda[i] = false;
  }
}

void WebApManager::begin()
{
  if (!iniciarApServidor())
  {
    return;
  }

  inicializarMonitoramentoEstado();
  registrarEvento("sistema", "info", "Servidor web iniciado");
}

void WebApManager::atualizar()
{
  if (!_ativo)
  {
    if (iniciarApServidor())
    {
      inicializarMonitoramentoEstado();
      registrarEvento("sistema", "info", "Servidor web reiniciado");
    }
    return;
  }

  _server.handleClient();
  tentarConexaoSta();
  atualizarHistoricoEstado();
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
    return false;
  }

  if (_staConfigurada)
  {
    WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASSWORD);
    _ultimoRetryStaMs = millis();
    _ultimoStatusSta = WiFi.status();
  }

  if (!_rotasConfiguradas)
  {
    configurarRotas();
    _rotasConfiguradas = true;
  }

  _server.begin();
  _ativo = true;

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

  _server.on("/api/events", HTTP_GET, [this]()
             { enviarAlertasHistorico(); });

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
        bool abertaAntes = (_irrigacao.estadoValvula(indice) == EstadoValvula::ABERTA);

        if (ligar && !abertaAntes)
        {
            _irrigacao.toggleValvula(indice);
        }
        else if (!ligar && abertaAntes)
        {
            _irrigacao.fecharValvula(indice);
        }

        enviarStatusSistema(); });

  _server.on("/api/valves/off-all", HTTP_POST, [this]()
             {
        int abertasAntes = 0;
        for (int i = 0; i < NUM_VALVULAS; i++)
        {
          if (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA)
          {
            abertasAntes++;
          }
        }

        _irrigacao.fecharTodas();

        if (abertasAntes > 0)
        {
          registrarEvento("irrigacao", "warning", String("Todas as valvulas foram desligadas via API (") + String(abertasAntes) + " abertas)");
        }

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

        registrarEvento("agenda", "info", String("Agenda ") + String(slot + 1) + " salva via dashboard");

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

        registrarEvento("agenda", "warning", String("Agenda ") + String(slot + 1) + " removida via dashboard");

        enviarListaAgendas(); });

  _server.on("/api/schedule/clear", HTTP_POST, [this]()
             {
        if (!_schedule.limparTodasAgendas())
        {
            enviarErroJson(500, "falha ao limpar agendas");
            return;
        }

        registrarEvento("agenda", "critical", "Todas as agendas foram limpas via dashboard");

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

        registrarEvento("config", "info", String("Runtime atualizado: timeout=") + String(timeoutMin) + " min, duracao=" + String(duracaoMin) + " min");

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
        char bufferDataHora[24];
        snprintf(bufferDataHora, sizeof(bufferDataHora), "%02d/%02d/%04d %02d:%02d", dia, mes, ano, hora, minuto);
        registrarEvento("rtc", "info", String("RTC ajustado via dashboard para ") + String(bufferDataHora));
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
}

void WebApManager::inicializarMonitoramentoEstado()
{
  for (int i = 0; i < NUM_VALVULAS; i++)
  {
    _ultimoEstadoValvulas[i] = (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA);
    _ultimaOrigemAgenda[i] = _irrigacao.valvulaEmAgendamento(i);
  }

  _ultimaStaConectada = staConectada();
  _ultimoTotalAgendasAtivas = _schedule.totalAtivas();
  _monitoramentoInicializado = true;
}

void WebApManager::atualizarHistoricoEstado()
{
  if (!_monitoramentoInicializado)
  {
    inicializarMonitoramentoEstado();
    return;
  }

  bool staAtual = staConectada();
  if (_staConfigurada && staAtual != _ultimaStaConectada)
  {
    registrarEvento("rede",
                    staAtual ? "info" : "warning",
                    staAtual ? "Conexao STA estabelecida" : "Conexao STA perdida");
  }
  _ultimaStaConectada = staAtual;

  int totalAtivas = _schedule.totalAtivas();
  if (totalAtivas != _ultimoTotalAgendasAtivas)
  {
    registrarEvento("agenda", "info", String("Total de agendas ativas atualizado para ") + String(totalAtivas));
    _ultimoTotalAgendasAtivas = totalAtivas;
  }

  for (int i = 0; i < NUM_VALVULAS; i++)
  {
    bool aberta = (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA);
    bool origemAgenda = _irrigacao.valvulaEmAgendamento(i);

    if (aberta != _ultimoEstadoValvulas[i])
    {
      if (aberta)
      {
        String msg = String("Setor ") + String(i + 1) +
                     " aberto (" +
                     String(origemAgenda ? "agenda" : "manual") +
                     ")";
        registrarEvento("irrigacao", "info", msg);
      }
      else
      {
        String origemAnterior = _ultimaOrigemAgenda[i] ? "agenda" : "manual";
        String msg = String("Setor ") + String(i + 1) +
                     " fechado (origem anterior: " +
                     origemAnterior + ")";
        registrarEvento("irrigacao", _ultimaOrigemAgenda[i] ? "info" : "warning", msg);
      }
    }
    else if (aberta && origemAgenda != _ultimaOrigemAgenda[i])
    {
      String msg = String("Setor ") + String(i + 1) +
                   " mudou para origem " +
                   String(origemAgenda ? "agenda" : "manual");
      registrarEvento("irrigacao", "info", msg);
    }

    _ultimoEstadoValvulas[i] = aberta;
    _ultimaOrigemAgenda[i] = origemAgenda;
  }
}

void WebApManager::registrarEvento(const char *tipo, const char *nivel, const String &mensagem)
{
  EventoSistema &evento = _historicoEventos[_historicoHead];
  evento.id = _proximoEventoId++;
  evento.uptimeS = millis() / 1000UL;

  DateTime agora = _rtc.agora();
  snprintf(evento.data, sizeof(evento.data), "%02d/%02d/%04d", agora.day(), agora.month(), agora.year());
  snprintf(evento.hora, sizeof(evento.hora), "%02d:%02d:%02d", agora.hour(), agora.minute(), agora.second());
  snprintf(evento.tipo, sizeof(evento.tipo), "%s", (tipo != nullptr) ? tipo : "sistema");
  snprintf(evento.nivel, sizeof(evento.nivel), "%s", (nivel != nullptr) ? nivel : "info");

  size_t tamanho = mensagem.length();
  if (tamanho >= sizeof(evento.mensagem))
  {
    tamanho = sizeof(evento.mensagem) - 1;
  }

  if (tamanho > 0)
  {
    memcpy(evento.mensagem, mensagem.c_str(), tamanho);
  }
  evento.mensagem[tamanho] = '\0';

  _historicoHead = (uint8_t)((_historicoHead + 1) % MAX_HISTORICO_EVENTOS);
  if (_historicoCount < MAX_HISTORICO_EVENTOS)
  {
    _historicoCount++;
  }
}

int WebApManager::contarValvulasManuaisAbertas() const
{
  int total = 0;
  for (int i = 0; i < NUM_VALVULAS; i++)
  {
    bool aberta = (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA);
    if (aberta && !_irrigacao.valvulaEmAgendamento(i))
    {
      total++;
    }
  }
  return total;
}

int WebApManager::contarValvulasAutomaticasAbertas() const
{
  int total = 0;
  for (int i = 0; i < NUM_VALVULAS; i++)
  {
    if (_irrigacao.valvulaEmAgendamento(i))
    {
      total++;
    }
  }
  return total;
}

String WebApManager::escaparJson(const char *texto)
{
  if (texto == nullptr)
  {
    return String("");
  }

  String saida;
  saida.reserve(strlen(texto) + 8);

  for (const char *ptr = texto; *ptr != '\0'; ++ptr)
  {
    char c = *ptr;
    switch (c)
    {
    case '"':
      saida += "\\\"";
      break;
    case '\\':
      saida += "\\\\";
      break;
    case '\n':
      saida += "\\n";
      break;
    case '\r':
      saida += "\\r";
      break;
    case '\t':
      saida += "\\t";
      break;
    default:
      saida += c;
      break;
    }
  }

  return saida;
}

void WebApManager::enviarAlertasHistorico()
{
  int manuaisAbertas = contarValvulasManuaisAbertas();
  int automaticasAbertas = contarValvulasAutomaticasAbertas();
  int agendasAtivas = _schedule.totalAtivas();
  bool staOk = staConectada();
  DateTime agora = _rtc.agora();

  DateTime proximaDataHora;
  AgendaSetor proximaAgenda = {};
  int slotProximo = -1;
  bool temProxima = _schedule.obterProximaExecucao(agora, proximaDataHora, proximaAgenda, slotProximo);

  int setoresProxima = 0;
  for (int i = 0; i < NUM_VALVULAS; i++)
  {
    if (proximaAgenda.setoresMask & (1U << i))
    {
      setoresProxima++;
    }
  }

  String json;
  json.reserve(7800);
  json += "{";
  json += "\"ok\":true,";
  json += "\"resumo\":{";
  json += "\"manuaisAbertas\":" + String(manuaisAbertas) + ",";
  json += "\"automaticasAbertas\":" + String(automaticasAbertas) + ",";
  json += "\"agendasAtivas\":" + String(agendasAtivas) + ",";
  json += "\"staConectada\":" + String(staOk ? "true" : "false");
  json += "},";

  json += "\"alertas\":[";
  bool primeiroAlerta = true;

  if (_staConfigurada && !staOk)
  {
    json += "{\"id\":\"sta_offline\",\"nivel\":\"warning\",\"mensagem\":\"STA configurado, mas desconectado\"}";
    primeiroAlerta = false;
  }

  if (manuaisAbertas > 0)
  {
    if (!primeiroAlerta)
    {
      json += ",";
    }
    json += "{\"id\":\"manual_open\",\"nivel\":\"warning\",\"mensagem\":\"";
    json += String(manuaisAbertas);
    json += " valvula(s) manual(is) aberta(s)\"}";
    primeiroAlerta = false;
  }

  if (agendasAtivas > 0)
  {
    if (!primeiroAlerta)
    {
      json += ",";
    }
    json += "{\"id\":\"agendas_ativas\",\"nivel\":\"info\",\"mensagem\":\"Agendas ativas: ";
    json += String(agendasAtivas);
    json += "\"}";
    primeiroAlerta = false;

    if (automaticasAbertas > 0)
    {
      json += ",";
      json += "{\"id\":\"agenda_execucao\",\"nivel\":\"info\",\"mensagem\":\"Agenda ativa em execucao: AUTO S";
      json += String(automaticasAbertas);
      json += "/";
      json += String(NUM_VALVULAS);
      json += "\"}";
    }

    if (temProxima)
    {
      const char *dias[7] = {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"};
      int dow = proximaDataHora.dayOfTheWeek();
      if (dow < 0 || dow > 6)
      {
        dow = 0;
      }

      json += ",";
      json += "{\"id\":\"proxima_agenda\",\"nivel\":\"info\",\"mensagem\":\"Proxima agenda: ";
      json += String(dias[dow]);
      json += " ";

      char horarioProximo[6];
      snprintf(horarioProximo, sizeof(horarioProximo), "%02d:%02d", proximaDataHora.hour(), proximaDataHora.minute());
      json += String(horarioProximo);

      json += " | Setores: S";
      json += String(setoresProxima);
      json += "/";
      json += String(NUM_VALVULAS);

      if (slotProximo >= 0)
      {
        json += " | Agenda ";
        json += String(slotProximo + 1);
      }

      json += "\"}";
    }
  }
  else
  {
    if (!primeiroAlerta)
    {
      json += ",";
    }
    json += "{\"id\":\"no_schedule\",\"nivel\":\"info\",\"mensagem\":\"Nenhuma agenda ativa configurada\"}";
  }
  json += "],";

  json += "\"historico\":[";
  int inicio = (_historicoHead + MAX_HISTORICO_EVENTOS - _historicoCount) % MAX_HISTORICO_EVENTOS;

  for (int i = 0; i < _historicoCount; i++)
  {
    int idx = (inicio + i) % MAX_HISTORICO_EVENTOS;
    const EventoSistema &evento = _historicoEventos[idx];

    json += "{";
    json += "\"id\":" + String(evento.id) + ",";
    json += "\"uptime_s\":" + String(evento.uptimeS) + ",";
    json += "\"data\":\"" + escaparJson(evento.data) + "\",";
    json += "\"hora\":\"" + escaparJson(evento.hora) + "\",";
    json += "\"tipo\":\"" + escaparJson(evento.tipo) + "\",";
    json += "\"nivel\":\"" + escaparJson(evento.nivel) + "\",";
    json += "\"mensagem\":\"" + escaparJson(evento.mensagem) + "\"";
    json += "}";

    if (i < (_historicoCount - 1))
    {
      json += ",";
    }
  }

  json += "]";
  json += "}";

  enviarRespostaJson(200, json);
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
