// ============================================================
//  web_ap_manager.cpp - Implementação do WebApManager
// ============================================================

#include "web_ap_manager.h"

#include <ESPmDNS.h>
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
  <title>Sistema de Irrigação</title>
  <link type="image/png" sizes="32x32" rel="icon" href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAACXBIWXMAAAsTAAALEwEAmpwYAAACgUlEQVR4nM2WXWiPURzHP4+hVhIr5m3+eSnNGraJlpdcIKEmshtaLky5GilzMfOW91pcuRE3rlyQWqgJF5sbXMxLcaG4WCtpF6IxhX71fep0ep75n+f/YN86dc7v9XvOc57f+cEYxXGgNsd4S4BjIQ67gaocCcwFdoU4TAYizaeUkDj2jRSzaPQBm4GVwFtPtwM4lzK2e7bmuwLYAvSGEChPmRvOAr9SxpmAOKNiHzAPmAG0SdYM1Os4hxKSD0lnNjvl06YY84HWEALtQDVQAE5KthdYpfkp4KY3TGZYLVvkaxdwMXA4hECUMnePc6o3yjPEScUjYAOwHOhP0HclfAKT+XgBNAAbgYcEYA4wASjTEWYlYL7jFGt2CIGtQOUo+mIJxKhUzKJxWcefFwGLdYkc0RVIIBgR0AhM+18EqhV0BLieQOSvE0CFJQ7+UQXmnxKIgKtOgm/OTQ4hMDG0CPnOT5wkw8Aa3eqRPxAwu1sibg1OZhSAL06iQWCm3guXwAXZzwK6HfltoIIScchL1q0K16/1c70HVr4/OXZHyAnjgfceiW1qWgaA6ZoPO/pOlfIq9YSZ70GMgx6Blwq6CKjRt451D6S75siWEbjj9cBSYJJk1tO980hsku1TR/ZTHXWzIztPIGqA73L+AdxVZTQyd5zAdcB+j1SPdv9Ka/sTMmEB8NjbWYee1l5dPMMbj4B1Q+s0/6z7UVJb/cFL0KLvflon5RckI35C8yvkgKNeggF99wrt1tV91fH3aN2UB4HGhF2ule6iJ7fPYXit9cI8CBQSCOyRzn0r"/>
  <style>
    :root {
      --bg: #f3f6f0;
      --surface: #ffffff;
      --surface-2: #eaf0e5;
      --border: #dbe3d3;
      --border-strong: #c2cfb7;
      --text: #202b1c;
      --text-2: #52604a;
      --text-3: #7c8a71;
      --leaf-700: #2c6b3e;
      --leaf-600: #3c8a52;
      --leaf-100: #e1f0e1;
      --leaf-050: #eef7ec;
      --leaf-border: #aed3ba;
      --water-700: #235777;
      --water-600: #2f7099;
      --water-100: #e1eef6;
      --water-border: #b9d4e7;
      --clay-700: #8a4a1d;
      --clay-100: #f7e9da;
      --clay-border: #e6cba8;
      --brick-700: #8f2e26;
      --brick-600: #ad3b30;
      --brick-100: #fbe4e1;
      --brick-border: #efc4bf;
      --shadow: 0 1px 2px rgba(20,30,15,.06), 0 4px 14px rgba(20,30,15,.06);
      --r-sm: 8px;
      --r-md: 12px;
      --r-lg: 18px;
      color-scheme: light;
    }

    @media (prefers-color-scheme: dark) {
      :root {
        --bg: #10150e;
        --surface: #171e15;
        --surface-2: #1e2619;
        --border: #262f21;
        --border-strong: #3b4634;
        --text: #e8ece1;
        --text-2: #a7b49d;
        --text-3: #7c8a71;
        --leaf-700: #86d59a;
        --leaf-600: #59ab6f;
        --leaf-100: #1c3322;
        --leaf-050: #182a1d;
        --leaf-border: #2f5a3c;
        --water-700: #94c7e8;
        --water-600: #5fa0cc;
        --water-100: #172a3a;
        --water-border: #2a4a63;
        --clay-700: #e6ac71;
        --clay-100: #3a2a17;
        --clay-border: #5f4426;
        --brick-700: #f0988e;
        --brick-600: #da675c;
        --brick-100: #3a1c18;
        --brick-border: #5f2d26;
        --shadow: 0 1px 2px rgba(0,0,0,.35), 0 6px 18px rgba(0,0,0,.3);
        color-scheme: dark;
      }
    }

    *, *::before, *::after { box-sizing: border-box; }
    html, body { margin: 0; padding: 0; }

    body {
      background: var(--bg);
      color: var(--text);
      font-family: system-ui, -apple-system, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
      font-size: 15px;
      line-height: 1.45;
      -webkit-font-smoothing: antialiased;
      padding-bottom: 48px;
      overflow-x: hidden;
    }

    h1, h2 { margin: 0; font-weight: 700; }
    button { font-family: inherit; }

    .mono {
      font-family: ui-monospace, "SF Mono", Menlo, Consolas, "Liberation Mono", monospace;
      font-variant-numeric: tabular-nums;
    }

    .sr-only {
      position: absolute; width: 1px; height: 1px; padding: 0; margin: -1px;
      overflow: hidden; clip: rect(0,0,0,0); white-space: nowrap; border: 0;
    }

    :focus-visible { outline: 2px solid var(--leaf-600); outline-offset: 2px; border-radius: 4px; }

    .icon {
      width: 1em; height: 1em;
      stroke: currentColor; stroke-width: 1.7; fill: none;
      stroke-linecap: round; stroke-linejoin: round;
      display: block; flex-shrink: 0;
    }

    @media (prefers-reduced-motion: reduce) {
      *, *::before, *::after { transition-duration: .001ms !important; animation-duration: .001ms !important; }
    }

    /* ---- Topbar ---- */
    .topbar {
      position: sticky; top: 0; z-index: 40;
      display: flex; align-items: center; justify-content: space-between; gap: 12px;
      padding: 12px 20px;
      background: var(--surface);
      border-bottom: 1px solid var(--border);
      box-shadow: var(--shadow);
    }

    .brand { display: flex; align-items: center; gap: 10px; min-width: 0; }

    .brand-mark {
      width: 36px; height: 36px; border-radius: var(--r-sm);
      background: var(--leaf-100); color: var(--leaf-700);
      display: flex; align-items: center; justify-content: center; flex-shrink: 0;
    }

    .brand-mark .icon { width: 20px; height: 20px; stroke-width: 1.9; }
    .brand-text h1 { font-size: .95rem; letter-spacing: -.01em; }

    .brand-text p {
      margin: 1px 0 0; font-size: .72rem; color: var(--text-3);
      white-space: nowrap; overflow: hidden; text-overflow: ellipsis;
    }

    .topbar-actions { display: flex; align-items: center; gap: 10px; flex-shrink: 0; }

    .ws-pill {
      display: inline-flex; align-items: center; gap: 6px;
      border-radius: 999px; padding: 6px 11px;
      font-size: .68rem; font-weight: 600;
      border: 1px solid var(--border-strong); color: var(--text-2);
      white-space: nowrap;
    }

    .ws-pill .dot { width: 6px; height: 6px; border-radius: 50%; background: var(--text-3); flex-shrink: 0; }
    .ws-pill.connected { border-color: var(--leaf-border); background: var(--leaf-050); color: var(--leaf-700); }
    .ws-pill.connected .dot { background: var(--leaf-600); }
    .ws-pill.reconnecting { border-color: var(--clay-border); background: var(--clay-100); color: var(--clay-700); }
    .ws-pill.reconnecting .dot { background: var(--clay-700); }
    .ws-pill.fallback { border-color: var(--water-border); background: var(--water-100); color: var(--water-700); }
    .ws-pill.fallback .dot { background: var(--water-600); }

    .icon-btn {
      position: relative;
      width: 36px; height: 36px; border-radius: var(--r-sm);
      border: 1px solid var(--border-strong); background: var(--surface);
      color: var(--text-2);
      display: inline-flex; align-items: center; justify-content: center;
      cursor: pointer;
    }

    .icon-btn:hover { background: var(--surface-2); }
    .icon-btn .icon { width: 18px; height: 18px; }

    .notif-count {
      position: absolute; top: -6px; right: -6px;
      min-width: 17px; height: 17px; padding: 0 4px;
      border-radius: 999px;
      background: var(--brick-600); color: #fff;
      font-size: .62rem; font-weight: 700;
      display: flex; align-items: center; justify-content: center;
      border: 2px solid var(--surface);
    }

    .notif-count[hidden] { display: none; }

    /* ---- Layout ---- */
    .wrap {
      max-width: 980px; margin: 0 auto; padding: 20px 20px 0;
      display: flex; flex-direction: column; gap: 28px;
    }

    .panel-title {
      display: flex; align-items: baseline; gap: 10px;
      font-size: .78rem; font-weight: 700;
      text-transform: uppercase; letter-spacing: .08em;
      color: var(--text-2);
      padding-bottom: 10px; border-bottom: 1px solid var(--border);
      margin-bottom: 14px;
    }

    .panel-sub {
      text-transform: none; letter-spacing: 0;
      font-weight: 500; font-size: .72rem; color: var(--text-3);
    }

    /* ---- Faixa de status ---- */
    .status-strip { display: grid; grid-template-columns: repeat(3, 1fr); gap: 12px; }

    .stat-card {
      background: var(--surface); border: 1px solid var(--border);
      border-radius: var(--r-lg); padding: 16px 18px;
      box-shadow: var(--shadow);
      display: flex; flex-direction: column; gap: 6px; min-width: 0;
    }

    .stat-label {
      display: flex; align-items: center; gap: 6px;
      font-size: .68rem; font-weight: 700;
      text-transform: uppercase; letter-spacing: .06em;
      color: var(--text-3);
    }

    .stat-label .icon { width: 13px; height: 13px; }
    .stat-value { font-weight: 600; font-size: 1.7rem; color: var(--text); line-height: 1.1; }
    .stat-value-sub { font-size: 1rem; color: var(--text-3); font-weight: 500; }
    .stat-sub { font-size: .74rem; color: var(--text-2); }
    .stat-chips { display: flex; flex-wrap: wrap; gap: 6px; margin-top: 2px; }

    .chip {
      display: inline-flex; align-items: center; gap: 4px;
      font-size: .68rem; font-weight: 600; padding: 3px 8px;
      border-radius: 999px;
      border: 1px solid var(--border-strong);
      color: var(--text-2); background: var(--surface-2);
    }

    .chip.origin-manual { color: var(--water-700); background: var(--water-100); border-color: var(--water-border); }
    .chip.origin-agenda { color: var(--leaf-700); background: var(--leaf-050); border-color: var(--leaf-border); }

    .action-card { justify-content: space-between; }

    .btn {
      font-weight: 700; font-size: .8rem;
      border-radius: var(--r-sm); padding: 9px 14px;
      border: 1px solid transparent; cursor: pointer;
      display: inline-flex; align-items: center; justify-content: center; gap: 6px;
      white-space: nowrap;
    }

    .btn:disabled { opacity: .5; cursor: not-allowed; }
    .btn-emergency { background: var(--brick-100); color: var(--brick-700); border-color: var(--brick-border); }
    .btn-ghost { background: var(--surface); color: var(--text-2); border-color: var(--border-strong); }
    .btn-ghost:hover { background: var(--surface-2); }
    .btn-primary { background: var(--leaf-600); color: #fff; }
    .btn-primary:hover { background: var(--leaf-700); }
    .btn-sm { font-size: .72rem; padding: 6px 10px; }

    .confirm-row {
      display: flex; flex-wrap: wrap; align-items: center; gap: 8px;
      font-size: .74rem; color: var(--brick-700);
      background: var(--brick-100);
      border: 1px solid var(--brick-border);
      border-radius: var(--r-sm); padding: 8px 10px; margin-top: 4px;
    }

    .confirm-row[hidden] { display: none; }

    /* ---- Sensores ---- */
    .sensor-row { display: grid; grid-template-columns: repeat(auto-fit, minmax(170px, 230px)); gap: 12px; }

    .sensor-card {
      background: var(--surface); border: 1px solid var(--border);
      border-radius: var(--r-lg); padding: 16px 18px;
      box-shadow: var(--shadow);
      display: flex; flex-direction: column; gap: 8px;
    }

    .sensor-icon {
      width: 32px; height: 32px; border-radius: var(--r-sm);
      display: flex; align-items: center; justify-content: center;
    }

    .sensor-icon .icon { width: 17px; height: 17px; }
    .sensor-icon.temp { background: var(--clay-100); color: var(--clay-700); }
    .sensor-icon.hum { background: var(--water-100); color: var(--water-700); }

    .sensor-label {
      font-size: .68rem; font-weight: 700;
      text-transform: uppercase; letter-spacing: .06em; color: var(--text-3);
    }

    .sensor-value { font-weight: 600; font-size: 1.6rem; color: var(--text); }
    .sensor-value span { font-size: 1rem; color: var(--text-3); font-weight: 500; }
    .sensor-age { font-size: .7rem; color: var(--text-3); }

    /* ---- Válvulas ---- */
    .valve-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(160px, 1fr)); gap: 10px; }

    .valve-card {
      background: var(--surface); border: 1px solid var(--border);
      border-radius: var(--r-md); padding: 12px 13px;
      box-shadow: var(--shadow);
      display: flex; flex-direction: column; gap: 9px;
    }

    .valve-card.open { border-color: var(--leaf-border); background: var(--leaf-050); }
    .valve-card.pending { border-style: dashed; border-color: var(--border-strong); }

    .valve-head { display: flex; align-items: baseline; justify-content: space-between; gap: 6px; }
    .valve-id { font-weight: 700; font-size: .82rem; }
    .valve-gpio { font-size: .66rem; color: var(--text-3); }
    .valve-status-row { display: flex; align-items: center; justify-content: space-between; gap: 6px; flex-wrap: wrap; }
    .valve-foot { display: flex; align-items: center; justify-content: space-between; gap: 6px; min-height: 22px; }
    .valve-timer { font-size: .72rem; font-weight: 600; color: var(--text-2); }
    .valve-hint { font-size: .66rem; color: var(--text-3); }

    .toggle {
      width: 40px; height: 22px; border-radius: 999px; border: none;
      background: var(--border-strong); position: relative; cursor: pointer; flex-shrink: 0;
      padding: 0;
    }

    .toggle .knob {
      position: absolute; top: 2px; left: 2px;
      width: 18px; height: 18px; border-radius: 50%;
      background: #fff; box-shadow: 0 1px 2px rgba(0,0,0,.25);
      transition: transform .15s ease;
    }

    .toggle.on { background: var(--leaf-600); }
    .toggle.on .knob { transform: translateX(18px); }

    .pill {
      display: inline-flex; align-items: center; gap: 4px;
      font-size: .64rem; font-weight: 700;
      text-transform: uppercase; letter-spacing: .03em;
      padding: 3px 7px; border-radius: 999px;
      border: 1px solid var(--border-strong);
      color: var(--text-2); background: var(--surface-2);
    }

    .pill-manual { color: var(--water-700); background: var(--water-100); border-color: var(--water-border); }
    .pill-agenda { color: var(--leaf-700); background: var(--leaf-050); border-color: var(--leaf-border); }
    .pill-pending { color: var(--clay-700); background: var(--clay-100); border-color: var(--clay-border); }
    .pill-off { color: var(--text-3); }

    /* ---- Agendas ---- */
    .schedule-list { display: flex; flex-direction: column; gap: 8px; }

    .schedule-item {
      background: var(--surface); border: 1px solid var(--border);
      border-radius: var(--r-md); box-shadow: var(--shadow);
      overflow: hidden;
    }

    .schedule-summary {
      width: 100%; background: none; border: none;
      display: flex; align-items: center; gap: 12px;
      padding: 12px 14px; cursor: pointer;
      color: var(--text); text-align: left;
    }

    .schedule-slot-badge {
      width: 30px; height: 30px; border-radius: var(--r-sm);
      background: var(--surface-2); color: var(--text-2);
      display: flex; align-items: center; justify-content: center;
      font-weight: 700; font-size: .78rem; flex-shrink: 0;
    }

    .schedule-item.active .schedule-slot-badge { background: var(--leaf-100); color: var(--leaf-700); }
    .schedule-summary-text { flex: 1; min-width: 0; }
    .schedule-summary-title { display: block; font-weight: 700; font-size: .84rem; }

    .schedule-summary-detail {
      display: block; font-size: .74rem; color: var(--text-3);
      white-space: nowrap; overflow: hidden; text-overflow: ellipsis;
    }

    .schedule-summary .icon.chev { color: var(--text-3); transition: transform .15s ease; flex-shrink: 0; }
    .schedule-item.expanded .schedule-summary .icon.chev { transform: rotate(180deg); }
    .schedule-body { display: none; padding: 4px 16px 16px; border-top: 1px solid var(--border); }
    .schedule-item.expanded .schedule-body { display: block; }

    .field-row { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin: 12px 0; }

    .field-label {
      display: block; font-size: .66rem; font-weight: 700;
      text-transform: uppercase; letter-spacing: .05em;
      color: var(--text-3); margin: 0 0 4px;
    }

    input[type="time"], input[type="number"], input[type="date"] {
      width: 100%;
      font-family: ui-monospace, Menlo, Consolas, monospace;
      font-size: .82rem; color: var(--text);
      background: var(--surface-2);
      border: 1px solid var(--border-strong);
      border-radius: var(--r-sm); padding: 7px 9px;
    }

    input:focus-visible { background: var(--surface); }
    .field-help { font-size: .66rem; color: var(--text-3); margin-top: 3px; }

    .chips-row { display: flex; flex-wrap: wrap; gap: 6px; margin-bottom: 12px; }

    .bit-chip {
      font-family: ui-monospace, Menlo, Consolas, monospace;
      font-size: .7rem; font-weight: 600;
      padding: 5px 9px; border-radius: 999px;
      border: 1px solid var(--border-strong);
      background: var(--surface-2); color: var(--text-2);
      cursor: pointer;
    }

    .bit-chip.on { background: var(--leaf-600); border-color: var(--leaf-600); color: #fff; }

    .batch-note {
      display: flex; gap: 8px; align-items: flex-start;
      font-size: .74rem; color: var(--water-700);
      background: var(--water-100);
      border: 1px solid var(--water-border);
      border-radius: var(--r-sm); padding: 9px 11px; margin-bottom: 12px;
    }

    .batch-note[hidden] { display: none; }
    .batch-note .icon { width: 15px; height: 15px; margin-top: 1px; }

    .schedule-actions { display: flex; gap: 8px; align-items: center; flex-wrap: wrap; }
    .schedule-empty { font-size: .8rem; color: var(--text-3); padding: 4px 0 8px; }
    .schedule-clear { margin-top: 10px; }

    /* ---- Sistema ---- */
    .system-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; }

    .system-card {
      background: var(--surface); border: 1px solid var(--border);
      border-radius: var(--r-lg); box-shadow: var(--shadow);
      padding: 16px 18px;
    }

    .system-card-head { display: flex; align-items: center; gap: 8px; margin-bottom: 4px; }
    .system-card-head .icon { width: 15px; height: 15px; color: var(--leaf-700); }
    .system-card-title { font-weight: 700; font-size: .82rem; }
    .system-card-sub { font-size: .72rem; color: var(--text-3); margin: 0 0 14px; }
    .rtc-readout { font-weight: 600; font-size: 1.9rem; color: var(--text); line-height: 1.1; }
    .rtc-date { font-size: .76rem; color: var(--text-2); margin: 3px 0 14px; }

    /* ---- Notificações ---- */
    .notif-drawer {
      max-width: 980px; margin: 0 auto; padding: 0 20px;
      max-height: 0; overflow: hidden;
      transition: max-height .2s ease;
    }

    .notif-drawer.open { max-height: 600px; }

    .notif-panel {
      margin-top: 12px;
      background: var(--surface); border: 1px solid var(--border);
      border-radius: var(--r-lg); box-shadow: var(--shadow);
      padding: 14px;
    }

    .notif-head { display: flex; align-items: center; justify-content: space-between; gap: 10px; margin-bottom: 10px; }
    .notif-head-title { font-weight: 700; font-size: .9rem; }
    .notif-head-sub { font-size: .7rem; color: var(--text-3); }

    .notif-tabs {
      display: grid; grid-template-columns: 1fr 1fr; gap: 6px;
      background: var(--surface-2); border-radius: var(--r-sm);
      padding: 4px; margin-bottom: 12px;
    }

    .notif-tab {
      border: none; border-radius: 7px; padding: 7px 10px;
      font-size: .74rem; font-weight: 700;
      color: var(--text-3); background: transparent; cursor: pointer;
    }

    .notif-tab.active { background: var(--surface); color: var(--text); box-shadow: 0 1px 3px rgba(0,0,0,.08); }
    .notif-tab-panel { display: none; flex-direction: column; gap: 8px; max-height: 340px; overflow-y: auto; }
    .notif-tab-panel.active { display: flex; }

    .notif-group-label {
      font-size: .64rem; font-weight: 700; color: var(--text-3);
      text-transform: uppercase; letter-spacing: .08em;
      margin: 6px 2px 0;
    }

    .notif-row {
      display: flex; align-items: flex-start; gap: 10px;
      padding: 10px;
      border: 1px solid var(--border); border-radius: var(--r-sm);
    }

    .notif-icon {
      width: 28px; height: 28px; border-radius: 50%;
      display: flex; align-items: center; justify-content: center; flex-shrink: 0;
      background: var(--water-100); color: var(--water-700);
    }

    .notif-icon.warning { background: var(--clay-100); color: var(--clay-700); }
    .notif-icon.critical { background: var(--brick-100); color: var(--brick-700); }
    .notif-icon .icon { width: 13px; height: 13px; }
    .notif-msg { font-size: .78rem; color: var(--text); font-weight: 600; }

    .notif-meta {
      font-size: .68rem; color: var(--text-3); margin-top: 2px;
      text-transform: uppercase; letter-spacing: .04em;
    }

    .notif-empty {
      border: 1px dashed var(--border-strong); border-radius: var(--r-sm);
      padding: 12px; font-size: .74rem; color: var(--text-3);
    }

    /* ---- Rodapé / toast ---- */
    .network-footer {
      max-width: 980px; margin: 4px auto 0;
      padding: 12px 20px 28px;
      font-size: .72rem; color: var(--text-3);
      display: flex; flex-wrap: wrap; gap: 6px 14px;
    }

    .network-footer strong { color: var(--text-2); }

    .toast {
      position: fixed; bottom: 24px; left: 50%;
      transform: translateX(-50%) translateY(60px);
      background: var(--text); color: var(--bg);
      font-size: .75rem; font-weight: 600;
      padding: 9px 20px; border-radius: 30px;
      opacity: 0; pointer-events: none;
      transition: transform .3s, opacity .3s;
      white-space: nowrap; z-index: 60;
    }

    .toast.show { transform: translateX(-50%) translateY(0); opacity: 1; }

    @media (max-width: 720px) {
      .status-strip { grid-template-columns: 1fr; }
      .system-grid { grid-template-columns: 1fr; }
      .field-row { grid-template-columns: 1fr; }
      .brand-text p { display: none; }
      .ws-pill span.ws-txt { display: none; }
    }
  </style>
</head>
<body>

<svg style="display:none" aria-hidden="true">
  <symbol id="i-leaf" viewBox="0 0 24 24"><path d="M4 20C4 11 10 4 20 4c0 9-6 16-16 16Z"/><path d="M4 20 14 10"/></symbol>
  <symbol id="i-drop" viewBox="0 0 24 24"><path d="M12 3c-3.5 5-6.5 8.8-6.5 12.4a6.5 6.5 0 0 0 13 0C18.5 11.8 15.5 8 12 3Z"/></symbol>
  <symbol id="i-therm" viewBox="0 0 24 24"><path d="M11 14.6V6a2 2 0 1 1 4 0v8.6a4 4 0 1 1-4 0Z"/><path d="M13 8h2.4"/></symbol>
  <symbol id="i-valve" viewBox="0 0 24 24"><path d="M3 9h9"/><circle cx="16" cy="9" r="3.2"/><path d="M16 5.8V3M16 12.2V15"/></symbol>
  <symbol id="i-clock" viewBox="0 0 24 24"><circle cx="12" cy="12" r="8.4"/><path d="M12 7.6V12l3.2 2"/></symbol>
  <symbol id="i-bell" viewBox="0 0 24 24"><path d="M6.2 16c0-1 .6-1.6 1-2.2.7-1 1-2.3 1-4.3a3.8 3.8 0 0 1 7.6 0c0 2 .3 3.3 1 4.3.4.6 1 1.2 1 2.2Z"/><path d="M10.2 18.4a1.9 1.9 0 0 0 3.6 0"/></symbol>
  <symbol id="i-power" viewBox="0 0 24 24"><path d="M12 3v8"/><path d="M7 6.4a8 8 0 1 0 10 0"/></symbol>
  <symbol id="i-sliders" viewBox="0 0 24 24"><path d="M4 7h9M17.4 7H20M4 17h2.6M11 17H20"/><circle cx="13" cy="7" r="2.2"/><circle cx="9" cy="17" r="2.2"/></symbol>
  <symbol id="i-info" viewBox="0 0 24 24"><circle cx="12" cy="12" r="8.4"/><path d="M12 11v5.2"/><circle cx="12" cy="8" r="0.4" fill="currentColor" stroke="none"/></symbol>
  <symbol id="i-alert" viewBox="0 0 24 24"><path d="M12 4 3 19h18Z"/><path d="M12 10v4.4"/><circle cx="12" cy="16.8" r="0.4" fill="currentColor" stroke="none"/></symbol>
  <symbol id="i-chevron" viewBox="0 0 24 24"><path d="M6 9l6 6 6-6"/></symbol>
</svg>

<div class="topbar">
  <div class="brand">
    <div class="brand-mark"><svg class="icon"><use href="#i-leaf"/></svg></div>
    <div class="brand-text">
      <h1>Sistema de Irrigação</h1>
      <p id="brandSub">ESP32 · 8 válvulas</p>
    </div>
  </div>
  <div class="topbar-actions">
    <div id="wsBadge" class="ws-pill reconnecting" title="Conexão em tempo real">
      <span class="dot"></span>
      <span class="ws-txt" id="wsBadgeText">Conectando</span>
    </div>
    <button class="icon-btn" id="bellBtn" aria-expanded="false" aria-controls="notifDrawer" title="Notificações">
      <svg class="icon"><use href="#i-bell"/></svg>
      <span class="notif-count" id="notifCount" hidden>0</span>
    </button>
  </div>
</div>

<div class="notif-drawer" id="notifDrawer">
  <div class="notif-panel">
    <div class="notif-head">
      <div>
        <div class="notif-head-title">Notificações</div>
        <div class="notif-head-sub" id="alertSummary">Verificando…</div>
      </div>
      <button class="btn btn-ghost btn-sm" id="btnMarcarLidas">Marcar tudo como lido</button>
    </div>
    <div class="notif-tabs">
      <button class="notif-tab active" data-tab="alertas" id="notifTabAlertas">Avisos</button>
      <button class="notif-tab" data-tab="historico">Histórico</button>
    </div>
    <div class="notif-tab-panel active" data-panel="alertas" id="alertBox"></div>
    <div class="notif-tab-panel" data-panel="historico" id="historyList"></div>
  </div>
</div>

<main class="wrap">

  <section class="status-strip">
    <div class="stat-card">
      <span class="stat-label"><svg class="icon"><use href="#i-valve"/></svg>Válvulas abertas</span>
      <span class="stat-value mono" id="statValvulas">–/8</span>
      <div class="stat-chips" id="statValvulasChips"></div>
    </div>
    <div class="stat-card">
      <span class="stat-label"><svg class="icon"><use href="#i-clock"/></svg>Próxima agenda</span>
      <span class="stat-value mono" id="statProxima">—</span>
      <span class="stat-sub" id="statProximaSub">Nenhuma agenda ativa</span>
    </div>
    <div class="stat-card action-card">
      <span class="stat-label"><svg class="icon"><use href="#i-power"/></svg>Ação rápida</span>
      <button class="btn btn-emergency" id="offAllBtn">
        <svg class="icon"><use href="#i-power"/></svg>
        Desligar todas as válvulas
      </button>
      <div class="confirm-row" id="offAllConfirm" hidden>
        <span id="offAllConfirmMsg">Confirma desligar as válvulas abertas?</span>
        <button class="btn btn-emergency btn-sm" id="offAllYes">Sim, desligar</button>
        <button class="btn btn-ghost btn-sm" id="offAllNo">Cancelar</button>
      </div>
    </div>
  </section>

  <section>
    <h2 class="panel-title">Sensores</h2>
    <div class="sensor-row">
      <div class="sensor-card">
        <span class="sensor-icon temp"><svg class="icon"><use href="#i-therm"/></svg></span>
        <span class="sensor-label">Temperatura</span>
        <span class="sensor-value mono" id="sensorTemp">--<span>°C</span></span>
        <span class="sensor-age" id="sensorTempAge">aguardando leitura</span>
      </div>
      <div class="sensor-card">
        <span class="sensor-icon hum"><svg class="icon"><use href="#i-drop"/></svg></span>
        <span class="sensor-label">Umidade do ar</span>
        <span class="sensor-value mono" id="sensorHum">--<span>%</span></span>
        <span class="sensor-age" id="sensorHumAge">aguardando leitura</span>
      </div>
    </div>
  </section>

  <section>
    <h2 class="panel-title">Válvulas <span class="panel-sub">8 setores · 2 relés de 4 canais</span></h2>
    <div class="valve-grid" id="valveGrid"></div>
  </section>

  <section>
    <h2 class="panel-title">Agendas <span class="panel-sub">toque para editar · até 4 agendas</span></h2>
    <div class="schedule-list" id="scheduleList"></div>
    <div class="schedule-clear">
      <button class="btn btn-ghost btn-sm" id="clearAllBtn">Limpar todas as agendas</button>
      <div class="confirm-row" id="clearAllConfirm" hidden>
        <span>Apagar as 4 agendas? Ciclos automáticos em andamento serão interrompidos.</span>
        <button class="btn btn-emergency btn-sm" id="clearAllYes">Sim, apagar tudo</button>
        <button class="btn btn-ghost btn-sm" id="clearAllNo">Cancelar</button>
      </div>
    </div>
  </section>

  <section>
    <h2 class="panel-title">Sistema</h2>
    <div class="system-grid">
      <div class="system-card">
        <div class="system-card-head"><svg class="icon"><use href="#i-clock"/></svg><span class="system-card-title">Relógio do sistema (RTC)</span></div>
        <p class="system-card-sub">DS3231 · base de horário para todas as agendas</p>
        <div class="rtc-readout mono" id="rtcClock">--:--:--</div>
        <div class="rtc-date" id="rtcDate">—</div>
        <div class="field-row">
          <div><label class="field-label" for="rtcDataInput">Data</label><input type="date" id="rtcDataInput"/></div>
          <div><label class="field-label" for="rtcHoraInput">Hora</label><input type="time" id="rtcHoraInput"/></div>
        </div>
        <button class="btn btn-primary btn-sm" id="rtcSalvarBtn">Ajustar horário</button>
      </div>
      <div class="system-card">
        <div class="system-card-head"><svg class="icon"><use href="#i-sliders"/></svg><span class="system-card-title">Configuração de runtime</span></div>
        <p class="system-card-sub">Limites de segurança aplicados a todos os setores</p>
        <div class="field-row">
          <div>
            <label class="field-label" for="rtTimeout">Timeout manual (min)</label>
            <input type="number" id="rtTimeout" min="1" max="120" step="1"/>
            <div class="field-help">1–120 min · fecha válvula manual esquecida</div>
          </div>
          <div>
            <label class="field-label" for="rtDuracao">Duração padrão (min)</label>
            <input type="number" id="rtDuracao" min="1" max="240" step="1"/>
            <div class="field-help">1–240 min · usada em novas agendas</div>
          </div>
        </div>
        <button class="btn btn-primary btn-sm" id="rtSalvarBtn">Salvar configuração</button>
      </div>
    </div>
  </section>

  <footer class="network-footer" id="netFooter">Carregando informações de rede…</footer>
</main>

<div class="toast" id="toast"></div>

<script>
(function () {
  'use strict';

  var DIAS_CURTOS = ['Dom', 'Seg', 'Ter', 'Qua', 'Qui', 'Sex', 'Sab'];
  var DIAS_LONGOS = ['domingo', 'segunda-feira', 'terça-feira', 'quarta-feira', 'quinta-feira', 'sexta-feira', 'sábado'];
  var NUM_VALVULAS = 8;
  var MAX_AGENDAS = 4;

  var STORAGE_HIST = 'irrigacao.notificacoes.historico';
  var STORAGE_LIDAS_ATE = 'irrigacao.notificacoes.lidasAte';
  var WS_RECONEXAO_MS = 3000;

  // ---- Estado ----
  var statusAtual = null;
  var valvulasRestantes = {};   // id -> segundos restantes (tick local)
  var proximaEmS = null;
  var rtcSegundos = null;
  var agendaCfg = { maxSimultaneos: 2, intervaloLoteS: 10 };
  var slotsAtuais = [];
  var expandidos = {};          // slot -> true
  var editorSujo = false;
  var runtimeSujo = false;
  var rtcSujo = false;
  var cacheAlertas = [];
  var cacheHistorico = [];
  var wsPainel = null;
  var wsConectado = false;
  var wsTimerReconexao = null;
  var wsPathAtual = '/ws';

  // ---- Utilitários ----
  function $(id) { return document.getElementById(id); }
  function p2(v) { return String(v).padStart(2, '0'); }

  function escapeHtml(texto) {
    return String(texto == null ? '' : texto)
      .replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')
      .replace(/"/g, '&quot;').replace(/'/g, '&#39;');
  }

  function fmtMmSs(s) {
    s = Math.max(0, s | 0);
    var h = Math.floor(s / 3600), m = Math.floor((s % 3600) / 60), sec = s % 60;
    if (h > 0) return h + ':' + p2(m) + ':' + p2(sec);
    return m + ':' + p2(sec);
  }

  function fmtLongo(s) {
    if (s == null) return '—';
    s = Math.max(0, s | 0);
    if (s < 60) return '< 1 min';
    var h = Math.floor(s / 3600), m = Math.floor((s % 3600) / 60);
    if (h > 0) return h + 'h ' + p2(m) + 'min';
    return m + ' min';
  }

  function contarBits(mask) {
    var n = 0;
    for (var i = 0; i < NUM_VALVULAS; i++) if (mask & (1 << i)) n++;
    return n;
  }

  function comprimirIndices(mask, limite, nomes) {
    var grupos = [], ini = -1;
    for (var i = 0; i <= limite; i++) {
      var on = i < limite && (mask & (1 << i));
      if (on && ini < 0) ini = i;
      if (!on && ini >= 0) { grupos.push([ini, i - 1]); ini = -1; }
    }
    return grupos.map(function (g) {
      return g[0] === g[1] ? nomes(g[0]) : nomes(g[0]) + '–' + nomes(g[1]);
    }).join(', ');
  }

  function resumoDias(mask) {
    if ((mask & 127) === 127) return 'Todos os dias';
    if ((mask & 127) === 0) return 'Sem dias';
    return comprimirIndices(mask, 7, function (i) { return DIAS_CURTOS[i]; });
  }

  function resumoSetores(mask) {
    if ((mask & 255) === 0) return 'Sem setores';
    return comprimirIndices(mask, NUM_VALVULAS, function (i) { return 'S' + (i + 1); });
  }

  var toastEl = $('toast');
  var toastTimer = null;
  function showToast(msg) {
    toastEl.textContent = msg;
    toastEl.classList.add('show');
    if (toastTimer) clearTimeout(toastTimer);
    toastTimer = setTimeout(function () { toastEl.classList.remove('show'); }, 2400);
  }

  function requestJson(url, options) {
    return fetch(url, options).then(function (response) {
      return response.json().catch(function () { return {}; }).then(function (data) {
        if (!response.ok) throw new Error(data.erro || ('HTTP ' + response.status));
        return data;
      });
    });
  }

  // ---- Notificações: armazenamento local ----
  function carregarHistoricoLocal() {
    try {
      var bruto = localStorage.getItem(STORAGE_HIST);
      if (!bruto) return [];
      var dados = JSON.parse(bruto);
      return Array.isArray(dados) ? dados : [];
    } catch (e) { return []; }
  }

  function salvarHistoricoLocal(historico) {
    try { localStorage.setItem(STORAGE_HIST, JSON.stringify(Array.isArray(historico) ? historico : [])); } catch (e) {}
  }

  function obterMaiorId(historico) {
    var maior = 0;
    (historico || []).forEach(function (item) {
      var id = Number(item && item.id || 0);
      if (id > maior) maior = id;
    });
    return maior;
  }

  function obterIdLidoAte() {
    try {
      var v = Number(localStorage.getItem(STORAGE_LIDAS_ATE) || 0);
      return isFinite(v) ? v : 0;
    } catch (e) { return 0; }
  }

  function definirIdLidoAte(id) {
    try { localStorage.setItem(STORAGE_LIDAS_ATE, String(Math.max(0, Number(id || 0)))); } catch (e) {}
  }

  function atualizarBadgeNotificacoes() {
    var lidoAte = obterIdLidoAte();
    var naoLidas = (cacheHistorico || []).filter(function (item) {
      return Number(item && item.id || 0) > lidoAte;
    }).length;
    var alertasRelevantes = (cacheAlertas || []).filter(function (a) {
      return String(a && a.nivel || 'info').toLowerCase() !== 'info';
    }).length;

    var total = naoLidas + alertasRelevantes;
    var badge = $('notifCount');
    if (total > 0) {
      badge.textContent = total > 99 ? '99+' : String(total);
      badge.hidden = false;
    } else {
      badge.hidden = true;
    }
  }

  function marcarNotificacoesLidas() {
    definirIdLidoAte(obterMaiorId(cacheHistorico));
    atualizarBadgeNotificacoes();
    showToast('Notificações marcadas como lidas');
  }

  // ---- Notificações: tradução leiga ----
  function traduzirMensagem(mensagem) {
    var texto = String(mensagem || '').trim();
    if (!texto) return 'Atualização do sistema.';

    var m = texto.match(/(\d+)\s+valvula\(s\)\s+manual\(is\)\s+aberta\(s\)/i);
    if (m) return m[1] + ' setor(es) ligados manualmente neste momento.';

    if (/STA configurado, mas desconectado/i.test(texto)) return 'O Wi-Fi da casa está desconectado. O acesso local continua funcionando.';
    if (/Conexao STA estabelecida/i.test(texto)) return 'Wi-Fi da casa conectado com sucesso.';
    if (/Conexao STA perdida/i.test(texto)) return 'Wi-Fi da casa foi desconectado.';
    if (/Nenhuma agenda ativa configurada/i.test(texto)) return 'Nenhuma programação automática ativa foi configurada.';
    if (/Servidor web iniciado/i.test(texto)) return 'Painel web iniciado e pronto para uso.';

    m = texto.match(/Setor\s+(\d+)\s+(aberto|fechado)/i);
    if (m) return 'Setor ' + m[1] + ' foi ' + (m[2].toLowerCase() === 'aberto' ? 'ligado' : 'desligado') + '.';

    m = texto.match(/Agenda\s+(\d+)\s+salva/i);
    if (m) return 'Agenda ' + m[1] + ' foi salva.';

    m = texto.match(/Agenda\s+(\d+)\s+removida/i);
    if (m) return 'Agenda ' + m[1] + ' foi removida.';

    return texto;
  }

  function classeNivel(nivel) {
    var n = String(nivel || 'info').toLowerCase();
    if (n === 'critical') return 'critical';
    if (n === 'warning') return 'warning';
    return 'info';
  }

  function rotuloNivel(nivel) {
    var n = classeNivel(nivel);
    if (n === 'critical') return 'Urgente';
    if (n === 'warning') return 'Atenção';
    return 'Informação';
  }

  function linhaNotificacao(mensagem, detalhe, nivel) {
    var classe = classeNivel(nivel);
    var simbolo = classe === 'info' ? 'i-info' : 'i-alert';
    return '<div class="notif-row">' +
      '<span class="notif-icon ' + classe + '"><svg class="icon"><use href="#' + simbolo + '"/></svg></span>' +
      '<div><div class="notif-msg">' + escapeHtml(mensagem) + '</div>' +
      '<div class="notif-meta">' + escapeHtml(detalhe) + '</div></div></div>';
  }

  function normalizarDataBr(data) {
    var m = String(data || '').trim().match(/^(\d{1,2})\/(\d{1,2})\/(\d{4})$/);
    if (!m) return '';
    return p2(Number(m[1])) + '/' + p2(Number(m[2])) + '/' + m[3];
  }

  function dataBrComOffset(dias) {
    var d = new Date();
    d.setDate(d.getDate() + dias);
    return p2(d.getDate()) + '/' + p2(d.getMonth() + 1) + '/' + d.getFullYear();
  }

  function renderAlertas(alertas) {
    var resumo = $('alertSummary');
    var alvo = $('alertBox');

    if (!Array.isArray(alertas) || alertas.length === 0) {
      resumo.textContent = 'Tudo certo no momento.';
      alvo.innerHTML = '<div class="notif-empty">Nenhum aviso importante agora.</div>';
      return;
    }

    resumo.textContent = alertas.length === 1 ? '1 aviso ativo.' : (alertas.length + ' avisos ativos.');
    alvo.innerHTML = alertas.map(function (a) {
      return linhaNotificacao(traduzirMensagem(a.mensagem || 'Sem descrição'), rotuloNivel(a.nivel), a.nivel);
    }).join('');
  }

  function renderHistorico(historico) {
    var alvo = $('historyList');
    if (!Array.isArray(historico) || historico.length === 0) {
      alvo.innerHTML = '<div class="notif-empty">Sem registros de eventos até o momento.</div>';
      return;
    }

    // Mais recentes primeiro, sem duplicatas visuais, limitado a 20.
    var vistos = {};
    var itens = historico.slice().reverse().filter(function (item) {
      var chave = traduzirMensagem(item && item.mensagem) + '|' + classeNivel(item && item.nivel) + '|' +
                  normalizarDataBr(item && item.data) + '|' + String(item && item.hora || '');
      if (vistos[chave]) return false;
      vistos[chave] = true;
      return true;
    }).slice(0, 20);

    var hoje = dataBrComOffset(0), ontem = dataBrComOffset(-1);
    var grupos = { 'Hoje': [], 'Ontem': [], 'Anteriores': [] };
    itens.forEach(function (item) {
      var d = normalizarDataBr(item && item.data);
      if (d === hoje) grupos['Hoje'].push(item);
      else if (d === ontem) grupos['Ontem'].push(item);
      else grupos['Anteriores'].push(item);
    });

    var html = '';
    Object.keys(grupos).forEach(function (titulo) {
      var lista = grupos[titulo];
      if (lista.length === 0) return;
      html += '<div class="notif-group-label">' + titulo + '</div>';
      html += lista.map(function (e) {
        return linhaNotificacao(
          traduzirMensagem(e.mensagem || 'Sem mensagem'),
          rotuloNivel(e.nivel) + ' · ' + String(e.hora || '--:--:--'),
          e.nivel);
      }).join('');
    });

    alvo.innerHTML = html || '<div class="notif-empty">Sem registros de eventos até o momento.</div>';
  }

  // ---- Faixa de status ----
  function renderStrip(dados) {
    var valvulas = dados.valvulas || [];
    var abertas = valvulas.filter(function (v) { return v.ativa; });

    $('statValvulas').textContent = abertas.length + '/' + valvulas.length;

    var chips = abertas.map(function (v) {
      var origem = v.origem === 'agenda' ? 'origin-agenda' : 'origin-manual';
      var rotulo = v.origem === 'agenda' ? 'Agenda' : 'Manual';
      return '<span class="chip ' + origem + '">Setor ' + v.id + ' · ' + rotulo + '</span>';
    });

    var pendentes = valvulas.filter(function (v) { return v.pendente; });
    pendentes.forEach(function (v) {
      chips.push('<span class="chip">Setor ' + v.id + ' · na fila</span>');
    });

    $('statValvulasChips').innerHTML = chips.length
      ? chips.join('')
      : '<span class="chip">Nenhum setor ativo</span>';

    var prox = dados.proxima_agenda;
    if (prox && typeof prox === 'object') {
      proximaEmS = Number(prox.em_s || 0);
      var dow = Number(prox.dow || 0) % 7;
      var quando = proximaEmS < 86400 ? 'hoje' : DIAS_CURTOS[dow];
      $('statProximaSub').textContent = 'Agenda ' + prox.slot + ' · ' + quando + ' às ' +
        p2(prox.hora) + ':' + p2(prox.minuto) + ' · ' + prox.setores + ' setor(es)';
    } else {
      proximaEmS = null;
      $('statProximaSub').textContent = 'Nenhuma agenda ativa';
    }
    $('statProxima').textContent = fmtLongo(proximaEmS);

    $('offAllBtn').disabled = (abertas.length === 0 && pendentes.length === 0);
  }

  // ---- Sensores ----
  function renderSensores(sensores) {
    var ok = Boolean(sensores && sensores.ok);
    var temp = Number(sensores && sensores.temperatura_c);
    var hum = Number(sensores && sensores.umidade_ar_pct);
    var idadeS = Math.round(Number(sensores && sensores.idade_ms || 0) / 1000);

    $('sensorTemp').innerHTML = (ok && isFinite(temp) ? temp.toFixed(1) : '--') + '<span>°C</span>';
    $('sensorHum').innerHTML = (ok && isFinite(hum) ? String(Math.round(hum)) : '--') + '<span>%</span>';

    var idadeTxt = ok ? ('DHT11 · atualizado há ' + idadeS + 's') : 'DHT11 · sem leitura válida';
    $('sensorTempAge').textContent = idadeTxt;
    $('sensorHumAge').textContent = idadeTxt;
  }

  // ---- Válvulas ----
  function renderValvulas(valvulas) {
    valvulasRestantes = {};

    $('valveGrid').innerHTML = (valvulas || []).map(function (v) {
      var classes = 'valve-card' + (v.ativa ? ' open' : (v.pendente ? ' pending' : ''));
      var pill, foot;

      if (v.ativa) {
        pill = v.origem === 'agenda'
          ? '<span class="pill pill-agenda">Agenda</span>'
          : '<span class="pill pill-manual">Manual</span>';
        valvulasRestantes[v.id] = Number(v.restante_s || 0);
        foot = '<span class="valve-timer mono" data-vtimer="' + v.id + '">' + fmtMmSs(v.restante_s || 0) + '</span>' +
          '<button class="toggle on" data-valve="' + v.id + '" aria-pressed="true">' +
          '<span class="knob"></span><span class="sr-only">Alternar Setor ' + v.id + '</span></button>';
      } else if (v.pendente) {
        pill = '<span class="pill pill-pending">Na fila · agenda</span>';
        foot = '<span class="valve-hint">Aguardando próximo lote</span>' +
          '<button class="toggle" data-valve="' + v.id + '" aria-pressed="false">' +
          '<span class="knob"></span><span class="sr-only">Alternar Setor ' + v.id + '</span></button>';
      } else {
        pill = '<span class="pill pill-off">Fechada</span>';
        foot = '<span class="valve-hint">&nbsp;</span>' +
          '<button class="toggle" data-valve="' + v.id + '" aria-pressed="false">' +
          '<span class="knob"></span><span class="sr-only">Alternar Setor ' + v.id + '</span></button>';
      }

      return '<div class="' + classes + '">' +
        '<div class="valve-head"><span class="valve-id">Setor ' + p2(v.id) + '</span>' +
        '<span class="valve-gpio mono">GPIO ' + v.gpio + '</span></div>' +
        '<div class="valve-status-row">' + pill + '</div>' +
        '<div class="valve-foot">' + foot + '</div></div>';
    }).join('');
  }

  // ---- Agendas ----
  function notaLotes(qtdSetores) {
    var maxSim = Math.max(1, Number(agendaCfg.maxSimultaneos || 2));
    var lotes = Math.ceil(qtdSetores / maxSim);
    if (qtdSetores <= 0 || lotes <= 1) return '';
    return qtdSetores + ' setores, limite de ' + maxSim + ' simultâneos → executa em <strong>' + lotes +
      ' lotes</strong> com ' + agendaCfg.intervaloLoteS + 's de intervalo entre eles.';
  }

  function detalheAgenda(s) {
    if (!s.ativa) return 'Toque para configurar';
    var partes = ['Setores ' + resumoSetores(s.setoresMask)];
    var prox = statusAtual && statusAtual.proxima_agenda;
    if (prox && Number(prox.slot) === Number(s.slot)) {
      partes.push('próxima em ' + fmtLongo(Number(prox.em_s || 0)));
    }
    return partes.join(' · ');
  }

  function renderAgendas(slots, forcar) {
    slotsAtuais = slots || [];
    if (editorSujo && !forcar) return;

    $('scheduleList').innerHTML = slotsAtuais.map(function (s) {
      var slot = Number(s.slot);
      var classes = 'schedule-item' + (s.ativa ? ' active' : '') + (expandidos[slot] ? ' expanded' : '');
      var titulo = s.ativa
        ? resumoDias(s.diasMask) + ' · ' + p2(s.hora) + ':' + p2(s.minuto) + ' · ' + s.duracaoMin + ' min'
        : 'Slot vazio';

      var chipsDias = DIAS_CURTOS.map(function (nome, i) {
        var on = (s.diasMask & (1 << i)) !== 0;
        return '<button type="button" class="bit-chip' + (on ? ' on' : '') + '" data-dia="' + i + '">' + nome.toUpperCase() + '</button>';
      }).join('');

      var chipsSetores = '';
      for (var i = 0; i < NUM_VALVULAS; i++) {
        var on = (s.setoresMask & (1 << i)) !== 0;
        chipsSetores += '<button type="button" class="bit-chip' + (on ? ' on' : '') + '" data-setor="' + i + '">S' + (i + 1) + '</button>';
      }

      var nota = notaLotes(contarBits(s.setoresMask));

      return '<div class="' + classes + '" data-slot="' + slot + '">' +
        '<button type="button" class="schedule-summary" data-acao="expandir">' +
        '<span class="schedule-slot-badge mono">' + p2(slot) + '</span>' +
        '<span class="schedule-summary-text">' +
        '<span class="schedule-summary-title">' + escapeHtml(titulo) + '</span>' +
        '<span class="schedule-summary-detail">' + escapeHtml(detalheAgenda(s)) + '</span></span>' +
        '<svg class="icon chev"><use href="#i-chevron"/></svg></button>' +
        '<div class="schedule-body">' +
        (s.ativa ? '' : '<div class="schedule-empty">Nenhuma agenda configurada neste slot.</div>') +
        '<div class="field-row">' +
        '<div><label class="field-label" for="ag-hora-' + slot + '">Horário</label>' +
        '<input type="time" id="ag-hora-' + slot + '" value="' + p2(s.hora) + ':' + p2(s.minuto) + '"/></div>' +
        '<div><label class="field-label" for="ag-dur-' + slot + '">Duração (min)</label>' +
        '<input type="number" id="ag-dur-' + slot + '" min="1" max="240" step="1" value="' + Number(s.duracaoMin || 10) + '"/>' +
        '<div class="field-help">1–240 min</div></div></div>' +
        '<div class="field-label">Dias</div><div class="chips-row" data-chips="dias">' + chipsDias + '</div>' +
        '<div class="field-label">Setores</div><div class="chips-row" data-chips="setores">' + chipsSetores + '</div>' +
        '<div class="batch-note" data-nota' + (nota ? '' : ' hidden') + '>' +
        '<svg class="icon"><use href="#i-info"/></svg><span data-nota-texto>' + nota + '</span></div>' +
        '<div class="schedule-actions">' +
        '<button type="button" class="btn btn-primary btn-sm" data-acao="salvar">' + (s.ativa ? 'Salvar agenda' : 'Criar agenda') + '</button>' +
        (s.ativa ? '<button type="button" class="btn btn-ghost btn-sm" data-acao="excluir">Excluir</button>' : '') +
        '<span class="confirm-row" hidden data-confirm>' +
        '<span>Excluir esta agenda?</span>' +
        '<button type="button" class="btn btn-emergency btn-sm" data-acao="excluir-sim">Sim, excluir</button>' +
        '<button type="button" class="btn btn-ghost btn-sm" data-acao="excluir-nao">Cancelar</button></span>' +
        '</div></div></div>';
    }).join('');
  }

  function atualizarNotaLotes(item) {
    var marcados = item.querySelectorAll('[data-chips="setores"] .bit-chip.on').length;
    var nota = notaLotes(marcados);
    var caixa = item.querySelector('[data-nota]');
    var texto = item.querySelector('[data-nota-texto]');
    if (!caixa || !texto) return;
    if (nota) {
      texto.innerHTML = nota;
      caixa.hidden = false;
    } else {
      caixa.hidden = true;
    }
  }

  function coletarMask(item, seletor) {
    var mask = 0;
    item.querySelectorAll(seletor + ' .bit-chip.on').forEach(function (chip) {
      var bit = Number(chip.getAttribute('data-dia') != null ? chip.getAttribute('data-dia') : chip.getAttribute('data-setor'));
      mask |= (1 << bit);
    });
    return mask;
  }

  function salvarAgenda(slot, item) {
    var horaTxt = item.querySelector('#ag-hora-' + slot).value || '06:00';
    var partes = horaTxt.split(':');
    var duracao = Math.max(1, Math.min(240, Number(item.querySelector('#ag-dur-' + slot).value || 10)));

    var params = new URLSearchParams({
      slot: String(slot),
      active: '1',
      hour: String(Number(partes[0] || 0)),
      minute: String(Number(partes[1] || 0)),
      duration: String(duracao),
      diasMask: String(coletarMask(item, '[data-chips="dias"]')),
      setoresMask: String(coletarMask(item, '[data-chips="setores"]'))
    });

    requestJson('/api/schedule/save?' + params.toString(), { method: 'POST' })
      .then(function () {
        editorSujo = false;
        showToast('Agenda ' + slot + ' salva');
        return carregarAgendas(true);
      })
      .catch(function (err) { showToast('Erro ao salvar: ' + err.message); });
  }

  function excluirAgenda(slot) {
    requestJson('/api/schedule/delete?slot=' + slot, { method: 'POST' })
      .then(function () {
        editorSujo = false;
        showToast('Agenda ' + slot + ' excluída');
        return carregarAgendas(true);
      })
      .catch(function (err) { showToast('Erro ao excluir: ' + err.message); });
  }

  // Delegação de eventos do editor de agendas
  $('scheduleList').addEventListener('click', function (ev) {
    var alvo = ev.target.closest('[data-acao], .bit-chip');
    if (!alvo) return;
    var item = alvo.closest('.schedule-item');
    if (!item) return;
    var slot = Number(item.getAttribute('data-slot'));

    if (alvo.classList.contains('bit-chip')) {
      alvo.classList.toggle('on');
      editorSujo = true;
      atualizarNotaLotes(item);
      return;
    }

    var acao = alvo.getAttribute('data-acao');
    if (acao === 'expandir') {
      var aberto = item.classList.toggle('expanded');
      if (aberto) expandidos[slot] = true; else delete expandidos[slot];
    } else if (acao === 'salvar') {
      salvarAgenda(slot, item);
    } else if (acao === 'excluir') {
      alvo.hidden = true;
      item.querySelector('[data-confirm]').hidden = false;
    } else if (acao === 'excluir-nao') {
      item.querySelector('[data-confirm]').hidden = true;
      var btnExcluir = item.querySelector('[data-acao="excluir"]');
      if (btnExcluir) btnExcluir.hidden = false;
    } else if (acao === 'excluir-sim') {
      excluirAgenda(slot);
    }
  });

  $('scheduleList').addEventListener('input', function () { editorSujo = true; });

  // Limpar todas as agendas
  $('clearAllBtn').addEventListener('click', function () {
    $('clearAllBtn').hidden = true;
    $('clearAllConfirm').hidden = false;
  });
  $('clearAllNo').addEventListener('click', function () {
    $('clearAllConfirm').hidden = true;
    $('clearAllBtn').hidden = false;
  });
  $('clearAllYes').addEventListener('click', function () {
    requestJson('/api/schedule/clear', { method: 'POST' })
      .then(function () {
        editorSujo = false;
        showToast('Todas as agendas foram apagadas');
        return carregarAgendas(true);
      })
      .catch(function (err) { showToast('Erro ao limpar: ' + err.message); })
      .then(function () {
        $('clearAllConfirm').hidden = true;
        $('clearAllBtn').hidden = false;
      });
  });

  // ---- Sistema: RTC ----
  function renderRtc(dados) {
    if (dados.hora) {
      var partes = String(dados.hora).split(':');
      rtcSegundos = (Number(partes[0]) * 3600) + (Number(partes[1]) * 60) + Number(partes[2] || 0);
      pintarRelogio();
    }

    var dataBr = normalizarDataBr(dados.data);
    var dow = Number(dados.dow);
    $('rtcDate').textContent = dataBr
      ? dataBr + (isFinite(dow) ? ' · ' + DIAS_LONGOS[dow % 7] : '')
      : '—';

    if (!rtcSujo && dataBr) {
      var m = dataBr.match(/^(\d{2})\/(\d{2})\/(\d{4})$/);
      if (m) $('rtcDataInput').value = m[3] + '-' + m[2] + '-' + m[1];
      if (dados.hora) $('rtcHoraInput').value = String(dados.hora).slice(0, 5);
    }
  }

  function pintarRelogio() {
    if (rtcSegundos == null) return;
    var h = Math.floor(rtcSegundos / 3600), m = Math.floor((rtcSegundos % 3600) / 60), s = rtcSegundos % 60;
    $('rtcClock').textContent = p2(h) + ':' + p2(m) + ':' + p2(s);
  }

  $('rtcDataInput').addEventListener('input', function () { rtcSujo = true; });
  $('rtcHoraInput').addEventListener('input', function () { rtcSujo = true; });

  $('rtcSalvarBtn').addEventListener('click', function () {
    var data = $('rtcDataInput').value;   // yyyy-mm-dd
    var hora = $('rtcHoraInput').value;   // HH:MM
    if (!data || !hora) {
      showToast('Preencha data e hora');
      return;
    }
    var d = data.split('-'), h = hora.split(':');
    var params = new URLSearchParams({
      year: d[0], month: String(Number(d[1])), day: String(Number(d[2])),
      hour: String(Number(h[0])), minute: String(Number(h[1]))
    });
    requestJson('/api/rtc/set?' + params.toString(), { method: 'POST' })
      .then(function () {
        rtcSujo = false;
        showToast('Relógio ajustado');
      })
      .catch(function (err) { showToast('Erro ao ajustar relógio: ' + err.message); });
  });

  // ---- Sistema: runtime ----
  function renderRuntime(runtime) {
    if (runtimeSujo || !runtime) return;
    $('rtTimeout').value = Number(runtime.timeoutMin || 10);
    $('rtDuracao').value = Number(runtime.duracaoMin || 10);
  }

  $('rtTimeout').addEventListener('input', function () { runtimeSujo = true; });
  $('rtDuracao').addEventListener('input', function () { runtimeSujo = true; });

  $('rtSalvarBtn').addEventListener('click', function () {
    var timeout = Math.max(1, Math.min(120, Number($('rtTimeout').value || 10)));
    var duracao = Math.max(1, Math.min(240, Number($('rtDuracao').value || 10)));
    var params = new URLSearchParams({ timeoutMin: String(timeout), duracaoMin: String(duracao) });
    requestJson('/api/config/runtime?' + params.toString(), { method: 'POST' })
      .then(function () {
        runtimeSujo = false;
        showToast('Configuração salva');
      })
      .catch(function (err) { showToast('Erro ao salvar configuração: ' + err.message); });
  });

  // ---- Desligar todas ----
  $('offAllBtn').addEventListener('click', function () {
    var abertas = statusAtual ? (statusAtual.valvulas || []).filter(function (v) { return v.ativa; }).length : 0;
    $('offAllConfirmMsg').textContent = abertas > 0
      ? 'Confirma desligar ' + abertas + ' válvula(s) aberta(s)?'
      : 'Confirma desligar todas as válvulas?';
    $('offAllConfirm').hidden = false;
    $('offAllBtn').hidden = true;
  });
  $('offAllNo').addEventListener('click', function () {
    $('offAllConfirm').hidden = true;
    $('offAllBtn').hidden = false;
  });
  $('offAllYes').addEventListener('click', function () {
    requestJson('/api/valves/off-all', { method: 'POST' })
      .then(function (dados) {
        showToast('Todas as válvulas foram desligadas');
        if (dados && dados.ok) aplicarStatusSistema(dados);
      })
      .catch(function (err) { showToast('Erro ao desligar: ' + err.message); })
      .then(function () {
        $('offAllConfirm').hidden = true;
        $('offAllBtn').hidden = false;
      });
  });

  // ---- Toggle de válvula ----
  $('valveGrid').addEventListener('click', function (ev) {
    var botao = ev.target.closest('.toggle[data-valve]');
    if (!botao) return;
    var id = Number(botao.getAttribute('data-valve'));
    requestJson('/api/valve/toggle?index=' + id, { method: 'POST' })
      .then(function (dados) {
        if (dados && dados.ok) aplicarStatusSistema(dados);
      })
      .catch(function (err) { showToast('Erro ao alternar válvula: ' + err.message); });
  });

  // ---- Rede / badge WS ----
  function websocketPermitido() {
    var ws = statusAtual && statusAtual.rede && statusAtual.rede.websocket;
    return Boolean(ws && ws.habilitado && ws.biblioteca);
  }

  function atualizarBadgeWs() {
    var badge = $('wsBadge'), texto = $('wsBadgeText');
    badge.classList.remove('connected', 'reconnecting', 'fallback');

    if (!websocketPermitido()) {
      badge.classList.add('fallback');
      badge.title = 'WebSocket indisponível; atualizando por HTTP';
      texto.textContent = 'HTTP';
      return;
    }
    if (wsConectado) {
      badge.classList.add('connected');
      badge.title = 'Atualização em tempo real conectada';
      texto.textContent = 'Tempo real';
      return;
    }
    badge.classList.add('reconnecting');
    badge.title = 'Reconectando canal em tempo real';
    texto.textContent = 'Reconectando';
  }

  function renderRede(rede) {
    var ap = (rede && rede.ap) || {};
    var sta = (rede && rede.sta) || {};
    var ws = (rede && rede.websocket) || {};

    var linhaSta;
    if (!sta.configurado) linhaSta = '<span><strong>STA</strong> desativado</span>';
    else if (sta.conectado) linhaSta = '<span><strong>STA</strong> ' + escapeHtml(sta.ssid) + ' · ' + escapeHtml(sta.ip) + (sta.mdns_ativo ? ' · ' + escapeHtml(sta.mdns_host) : '') + '</span>';
    else linhaSta = '<span><strong>STA</strong> tentando conectar em ' + escapeHtml(sta.ssid) + '</span>';

    var linhaWs = ws.habilitado
      ? '<span><strong>WebSocket</strong> ' + (ws.path || '/ws') + '</span>'
      : '<span><strong>WebSocket</strong> desativado</span>';

    $('netFooter').innerHTML =
      '<span><strong>AP</strong> ' + escapeHtml(ap.ssid || '-') + ' · ' + escapeHtml(ap.ip || '0.0.0.0') + '</span>' +
      linhaSta + linhaWs;

    if (sta.conectado && sta.mdns_ativo) $('brandSub').textContent = 'ESP32 · ' + sta.mdns_host;
    else $('brandSub').textContent = 'ESP32 · ' + (ap.ip || '8 válvulas');
  }

  // ---- Aplicação do payload de status ----
  function aplicarStatusSistema(dados) {
    if (!dados || typeof dados !== 'object') return;
    statusAtual = dados;

    if (dados.agenda_cfg) {
      agendaCfg.maxSimultaneos = Number(dados.agenda_cfg.maxSimultaneos || agendaCfg.maxSimultaneos);
      agendaCfg.intervaloLoteS = Number(dados.agenda_cfg.intervaloLoteS || agendaCfg.intervaloLoteS);
    }

    var wsPath = dados.rede && dados.rede.websocket && dados.rede.websocket.path;
    if (wsPath) wsPathAtual = wsPath;

    renderStrip(dados);
    renderSensores(dados.sensores);
    renderValvulas(dados.valvulas || []);
    renderRede(dados.rede);
    renderRtc(dados);
    renderRuntime(dados.runtime);

    if (!websocketPermitido() && wsPainel) {
      try { wsPainel.close(); } catch (e) {}
      wsPainel = null;
      wsConectado = false;
    }
    atualizarBadgeWs();
  }

  // ---- Carregamentos ----
  function carregarStatus() {
    return requestJson('/api/status').then(aplicarStatusSistema);
  }

  function carregarAgendas(forcar) {
    return requestJson('/api/schedules').then(function (dados) {
      renderAgendas(dados.slots || [], Boolean(forcar));
    });
  }

  function carregarEventos() {
    return requestJson('/api/events').then(function (dados) {
      cacheAlertas = dados.alertas || [];
      cacheHistorico = dados.historico || [];
      salvarHistoricoLocal(cacheHistorico);
      renderAlertas(cacheAlertas);
      renderHistorico(cacheHistorico);
      atualizarBadgeNotificacoes();
    });
  }

  // ---- WebSocket ----
  function agendarReconexaoWs() {
    if (!websocketPermitido() || wsTimerReconexao) return;
    wsTimerReconexao = setTimeout(function () {
      wsTimerReconexao = null;
      conectarWebSocket();
    }, WS_RECONEXAO_MS);
  }

  function conectarWebSocket() {
    if (typeof WebSocket === 'undefined' || !websocketPermitido()) return;
    if (wsPainel && (wsPainel.readyState === WebSocket.OPEN || wsPainel.readyState === WebSocket.CONNECTING)) return;

    var protocolo = location.protocol === 'https:' ? 'wss://' : 'ws://';
    try {
      wsPainel = new WebSocket(protocolo + location.host + wsPathAtual);
    } catch (e) {
      wsConectado = false;
      atualizarBadgeWs();
      agendarReconexaoWs();
      return;
    }

    wsPainel.onopen = function () { wsConectado = true; atualizarBadgeWs(); };
    wsPainel.onmessage = function (evento) {
      var pacote;
      try { pacote = JSON.parse(evento.data || ''); } catch (e) { return; }
      if (pacote && pacote.type === 'status' && pacote.payload) aplicarStatusSistema(pacote.payload);
    };
    wsPainel.onerror = function () { wsConectado = false; atualizarBadgeWs(); };
    wsPainel.onclose = function () {
      wsConectado = false;
      atualizarBadgeWs();
      agendarReconexaoWs();
    };
  }

  // ---- Notificações: interações ----
  $('bellBtn').addEventListener('click', function () {
    var aberto = $('notifDrawer').classList.toggle('open');
    $('bellBtn').setAttribute('aria-expanded', aberto ? 'true' : 'false');
    if (aberto) {
      definirIdLidoAte(obterMaiorId(cacheHistorico));
      atualizarBadgeNotificacoes();
    }
  });

  $('btnMarcarLidas').addEventListener('click', marcarNotificacoesLidas);

  document.querySelectorAll('.notif-tab').forEach(function (tab) {
    tab.addEventListener('click', function () {
      document.querySelectorAll('.notif-tab').forEach(function (t) { t.classList.remove('active'); });
      document.querySelectorAll('.notif-tab-panel').forEach(function (p) { p.classList.remove('active'); });
      tab.classList.add('active');
      document.querySelector('.notif-tab-panel[data-panel="' + tab.getAttribute('data-tab') + '"]').classList.add('active');
    });
  });

  // ---- Tick local de 1s ----
  setInterval(function () {
    Object.keys(valvulasRestantes).forEach(function (id) {
      if (valvulasRestantes[id] > 0) valvulasRestantes[id] -= 1;
      var el = document.querySelector('[data-vtimer="' + id + '"]');
      if (el) el.textContent = fmtMmSs(valvulasRestantes[id]);
    });

    if (proximaEmS != null) {
      if (proximaEmS > 0) proximaEmS -= 1;
      $('statProxima').textContent = fmtLongo(proximaEmS);
    }

    if (rtcSegundos != null) {
      rtcSegundos = (rtcSegundos + 1) % 86400;
      pintarRelogio();
    }
  }, 1000);

  // ---- Inicialização ----
  cacheHistorico = carregarHistoricoLocal();
  renderHistorico(cacheHistorico);
  renderAlertas([]);
  atualizarBadgeNotificacoes();
  atualizarBadgeWs();

  function iniciarWsSePossivel() {
    if (websocketPermitido()) conectarWebSocket();
  }

  Promise.all([carregarStatus(), carregarAgendas(true), carregarEventos()])
    .then(iniciarWsSePossivel)
    .catch(function (err) {
      showToast('Falha de comunicação: ' + err.message);
      iniciarWsSePossivel();
    });

  setInterval(function () {
    if (!wsConectado) carregarStatus().catch(function () {});
  }, 2000);
  setInterval(function () { carregarAgendas(false).catch(function () {}); }, 10000);
  setInterval(function () { carregarEventos().catch(function () {}); }, 4000);
})();
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
      _mdnsAtivo(false),
      _mdnsFalhaLogada(false),
#if WIFI_WEBSOCKET_ENABLED
      _webSocket("/ws"),
      _webSocketIniciado(false),
      _ultimoPushStatusWsMs(0),
#endif
      _historicoCount(0),
      _historicoHead(0),
      _proximoEventoId(1),
      _monitoramentoInicializado(false),
      _ultimaStaConectada(false),
      _ultimoTotalAgendasAtivas(0),
      _sensorTempC(WEB_TEMP_DEFAULT_C),
      _sensorUmidadePct(WEB_HUMIDADE_AR_DEFAULT_PCT),
      _sensorLeituraValida(false),
      _sensorUltimoOkMs(0),
      _agendaPendenteMask(0),
      _agendaAguardandoIntervalo(false)
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

  // AsyncWebServer não precisa de handleClient(): requisições são atendidas
  // em background (task própria), sem bloquear o loop principal.
#if WIFI_WEBSOCKET_ENABLED
  atualizarWebSocket();
#endif
  tentarConexaoSta();
  atualizarMdns();
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

#if WIFI_WEBSOCKET_ENABLED
  iniciarWebSocket();
#endif
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

  _server.end();
#if WIFI_WEBSOCKET_ENABLED
  _webSocketIniciado = false;
#endif
  desativarMdns();
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

#if WIFI_MDNS_ENABLED
  if (_mdnsAtivo)
  {
    return String("http://") + WIFI_MDNS_HOSTNAME + ".local/";
  }
#endif

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

void WebApManager::atualizarLeituraClima(float temperaturaC, float umidadePct, bool leituraValida)
{
  if (leituraValida)
  {
    _sensorTempC = temperaturaC;
    _sensorUmidadePct = umidadePct;
    _sensorLeituraValida = true;
    _sensorUltimoOkMs = millis();
    return;
  }

  _sensorLeituraValida = false;
}

void WebApManager::atualizarEstadoAgendaSequencial(uint16_t setoresPendentesMask, bool aguardandoIntervalo)
{
  _agendaPendenteMask = setoresPendentesMask;
  _agendaAguardandoIntervalo = aguardandoIntervalo;
}

bool WebApManager::sensorClimaValido() const
{
  return _sensorLeituraValida;
}

float WebApManager::temperaturaC() const
{
  return _sensorTempC;
}

float WebApManager::umidadeArPct() const
{
  return _sensorUmidadePct;
}

unsigned long WebApManager::idadeLeituraClimaMs() const
{
  if (!_sensorLeituraValida)
  {
    return 0;
  }

  return (unsigned long)(millis() - _sensorUltimoOkMs);
}

void WebApManager::configurarRotas()
{
  _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
             { enviarPaginaPrincipal(request); });

  _server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request)
             { enviarStatusSistema(request); });

  _server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest *request)
             { enviarStatusSistema(request); });

  _server.on("/api/schedules", HTTP_GET, [this](AsyncWebServerRequest *request)
             { enviarListaAgendas(request); });

  _server.on("/api/events", HTTP_GET, [this](AsyncWebServerRequest *request)
             { enviarAlertasHistorico(request); });

  _server.on("/api/valve/toggle", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
        int indice = lerIndiceValvula(request);
        if (indice < 0)
        {
            enviarErroJson(request, 400, "indice de valvula invalido");
            return;
        }

        _irrigacao.toggleValvula(indice);

        enviarStatusSistema(request); });

  _server.on("/api/valve/set", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
        int indice = lerIndiceValvula(request);
        if (indice < 0)
        {
            enviarErroJson(request, 400, "indice de valvula invalido");
            return;
        }

        if (!temArg(request, "state"))
        {
            enviarErroJson(request, 400, "parametro state ausente");
            return;
        }

        String state = lerArg(request, "state");
        state.trim();
        String lower = state;
        lower.toLowerCase();

        bool valorValido = (lower == "1" || lower == "0" ||
                            lower == "true" || lower == "false" ||
                            lower == "on" || lower == "off" ||
                            lower == "ligado" || lower == "desligado");
        if (!valorValido)
        {
            enviarErroJson(request, 400, "state invalido");
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

        enviarStatusSistema(request); });

  _server.on("/api/valves/off-all", HTTP_POST, [this](AsyncWebServerRequest *request)
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

        enviarStatusSistema(request); });

  _server.on("/api/schedule/save", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
        int slot = lerSlotAgenda(request);
        if (slot < 0)
        {
            enviarErroJson(request, 400, "slot de agenda invalido");
            return;
        }

        AgendaSetor agenda;
        if (!_schedule.obterAgenda(slot, agenda))
        {
            enviarErroJson(request, 500, "falha ao obter agenda");
            return;
        }

        if (temArg(request, "active"))
            agenda.ativa = textoParaBool(lerArg(request, "active"));
        else
            agenda.ativa = true;

        if (temArg(request, "hour"))
            agenda.hora = (uint8_t)lerArg(request, "hour").toInt();
        if (temArg(request, "minute"))
            agenda.minuto = (uint8_t)lerArg(request, "minute").toInt();
        if (temArg(request, "duration"))
            agenda.duracaoMin = (uint16_t)lerArg(request, "duration").toInt();
        if (temArg(request, "diasMask"))
            agenda.diasMask = (uint8_t)lerArg(request, "diasMask").toInt();
        if (temArg(request, "setoresMask"))
            agenda.setoresMask = (uint8_t)lerArg(request, "setoresMask").toInt();

        String erro;
        if (!_schedule.salvarAgenda(slot, agenda, erro))
        {
            enviarErroJson(request, 400, erro.c_str());
            return;
        }

        registrarEvento("agenda", "info", String("Agenda ") + String(slot + 1) + " salva via dashboard");

        enviarListaAgendas(request); });

  _server.on("/api/schedule/delete", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
        int slot = lerSlotAgenda(request);
        if (slot < 0)
        {
            enviarErroJson(request, 400, "slot de agenda invalido");
            return;
        }

        if (!_schedule.removerAgenda(slot))
        {
            enviarErroJson(request, 500, "falha ao remover agenda");
            return;
        }

        registrarEvento("agenda", "warning", String("Agenda ") + String(slot + 1) + " removida via dashboard");

        enviarListaAgendas(request); });

  _server.on("/api/schedule/clear", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
        int agendasAtivasAntes = _schedule.totalAtivas();
        int setoresInterrompidos = 0;

        for (int i = 0; i < NUM_VALVULAS; i++)
        {
          bool aberta = (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA);
          if (aberta && _irrigacao.valvulaEmAgendamento(i))
          {
            _irrigacao.fecharValvula(i);
            setoresInterrompidos++;
          }
        }

        if (!_schedule.limparTodasAgendas())
        {
            enviarErroJson(request, 500, "falha ao limpar agendas");
            return;
        }

        String msg = "Limpeza de agendas selecionada: ";
        if (setoresInterrompidos > 0)
        {
          msg += "ciclo automatico interrompido e agendas apagadas";
        }
        else
        {
          msg += "agendas apagadas";
        }

        msg += " (";
        msg += String(agendasAtivasAntes);
        msg += " agenda(s) ativa(s) antes, ";
        msg += String(setoresInterrompidos);
        msg += " setor(es) interrompido(s))";

        registrarEvento("agenda", "critical", msg);

        enviarListaAgendas(request); });

  _server.on("/api/config/runtime", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
        uint32_t timeoutMin = _config.timeoutManualMs() / 60000UL;
        uint16_t duracaoMin = _config.duracaoPadraoMin();

        if (temArg(request, "timeoutMin"))
            timeoutMin = (uint32_t)lerArg(request, "timeoutMin").toInt();
        if (temArg(request, "duracaoMin"))
            duracaoMin = (uint16_t)lerArg(request, "duracaoMin").toInt();

        bool okTimeout = _config.salvarTimeoutManualMs(timeoutMin * 60000UL);
        bool okDuracao = _config.salvarDuracaoPadraoMin(duracaoMin);

        if (!okTimeout || !okDuracao)
        {
            enviarErroJson(request, 500, "falha ao salvar configuracao runtime");
            return;
        }

        registrarEvento("config", "info", String("Runtime atualizado: timeout=") + String(timeoutMin) + " min, duracao=" + String(duracaoMin) + " min");

        enviarStatusSistema(request); });

  _server.on("/api/rtc/set", HTTP_POST, [this](AsyncWebServerRequest *request)
             {
        DateTime atual = _rtc.agora();
        int ano = atual.year();
        int mes = atual.month();
        int dia = atual.day();
        int hora = atual.hour();
        int minuto = atual.minute();

        if (temArg(request, "year"))
            ano = lerArg(request, "year").toInt();
        if (temArg(request, "month"))
            mes = lerArg(request, "month").toInt();
        if (temArg(request, "day"))
            dia = lerArg(request, "day").toInt();
        if (temArg(request, "hour"))
            hora = lerArg(request, "hour").toInt();
        if (temArg(request, "minute"))
            minuto = lerArg(request, "minute").toInt();

        if (ano < 2000 || ano > 2099 ||
            mes < 1 || mes > 12 ||
            dia < 1 || dia > 31 ||
            hora < 0 || hora > 23 ||
            minuto < 0 || minuto > 59)
        {
            enviarErroJson(request, 400, "data/hora invalida");
            return;
        }

        _rtc.ajustarHora(DateTime(ano, mes, dia, hora, minuto, 0));
        char bufferDataHora[24];
        snprintf(bufferDataHora, sizeof(bufferDataHora), "%02d/%02d/%04d %02d:%02d", dia, mes, ano, hora, minuto);
        registrarEvento("rtc", "info", String("RTC ajustado via dashboard para ") + String(bufferDataHora));
        enviarStatusSistema(request); });

  _server.onNotFound([](AsyncWebServerRequest *request)
                     {
        if (request->url().startsWith("/api/"))
        {
            enviarErroJson(request, 404, "rota nao encontrada");
            return;
        }
        request->send(404, "text/plain; charset=utf-8", "Rota nao encontrada"); });
}

void WebApManager::tentarConexaoSta()
{
  if (!_staConfigurada)
  {
    return;
  }

  wl_status_t statusAtual = WiFi.status();
  bool acabouDeConectar = statusAtual == WL_CONNECTED && _ultimoStatusSta != WL_CONNECTED;
  if (statusAtual != _ultimoStatusSta)
  {
    _ultimoStatusSta = statusAtual;
  }

  if (statusAtual == WL_CONNECTED)
  {
    if (acabouDeConectar)
    {
      // Corrige a hora a cada reconexão (não só no boot) — cobre tanto o RTC
      // "perdido" após queda de energia quanto drift acumulado ao longo do uso.
      sincronizarHoraNtp();
    }
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

void WebApManager::atualizarMdns()
{
#if WIFI_MDNS_ENABLED
  if (!_staConfigurada)
  {
    desativarMdns();
    return;
  }

  if (staConectada())
  {
    if (_mdnsAtivo)
    {
      return;
    }

    if (MDNS.begin(WIFI_MDNS_HOSTNAME))
    {
      MDNS.addService("http", "tcp", 80);
      _mdnsAtivo = true;
      _mdnsFalhaLogada = false;
      registrarEvento("rede", "info", String("mDNS ativo em http://") + WIFI_MDNS_HOSTNAME + ".local/");
      return;
    }

    if (!_mdnsFalhaLogada)
    {
      registrarEvento("rede", "warning", "Falha ao iniciar mDNS");
      _mdnsFalhaLogada = true;
    }
    return;
  }

  if (_mdnsAtivo)
  {
    desativarMdns();
    registrarEvento("rede", "warning", "mDNS desativado (STA desconectada)");
  }
  _mdnsFalhaLogada = false;
#else
  desativarMdns();
#endif
}

void WebApManager::desativarMdns()
{
  if (!_mdnsAtivo)
  {
    return;
  }

  MDNS.end();
  _mdnsAtivo = false;
}

#if WIFI_WEBSOCKET_ENABLED
void WebApManager::iniciarWebSocket()
{
  if (_webSocketIniciado)
  {
    return;
  }

  _webSocket.onEvent([this](AsyncWebSocket *servidor, AsyncWebSocketClient *cliente,
                            AwsEventType tipo, void *arg, uint8_t *dados, size_t len)
                     { aoEventoWebSocket(servidor, cliente, tipo, arg, dados, len); });
  _server.addHandler(&_webSocket);

  _webSocketIniciado = true;
  _ultimoPushStatusWsMs = millis();
}

void WebApManager::atualizarWebSocket()
{
  if (!_webSocketIniciado)
  {
    return;
  }

  _webSocket.cleanupClients();

  unsigned long agora = millis();
  if ((unsigned long)(agora - _ultimoPushStatusWsMs) < WIFI_WEBSOCKET_PUSH_STATUS_MS)
  {
    return;
  }

  _ultimoPushStatusWsMs = agora;
  enviarStatusWebSocket();
}

void WebApManager::aoEventoWebSocket(AsyncWebSocket *servidor, AsyncWebSocketClient *cliente,
                                    AwsEventType tipo, void *arg, uint8_t *dados, size_t len)
{
  (void)servidor;
  (void)arg;
  (void)dados;
  (void)len;

  if (tipo == WS_EVT_CONNECT)
  {
    enviarStatusWebSocket((int)cliente->id());
  }
}

void WebApManager::enviarStatusWebSocket(int clienteId)
{
  if (!_webSocketIniciado)
  {
    return;
  }

  String pacote;
  pacote.reserve(2500);
  pacote += "{\"type\":\"status\",\"payload\":";
  pacote += montarJsonStatusSistema();
  pacote += "}";

  if (clienteId >= 0)
  {
    _webSocket.text((uint32_t)clienteId, pacote);
    return;
  }

  _webSocket.textAll(pacote);
}
#endif

// Chamado a cada transição STA desconectado->conectado (não só no boot), pra
// corrigir tanto um RTC "perdido" após queda de energia quanto drift acumulado
// ao longo do uso. Bloqueia o loop principal por até TIMEOUT_MS enquanto
// espera resposta do servidor NTP — aceitável aqui (irrigação de jardim, não
// é sistema de tempo real crítico), mas mantido curto de propósito.
void WebApManager::sincronizarHoraNtp()
{
  const long GMT_OFFSET_SEC = -3 * 3600; // Horário de Brasília (UTC-3, sem horário de verão desde 2019)
  const int DAYLIGHT_OFFSET_SEC = 0;
  const unsigned long TIMEOUT_MS = 3000;

  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, "a.st1.ntp.br", "pool.ntp.org");

  struct tm horaNtp;
  if (!getLocalTime(&horaNtp, TIMEOUT_MS))
  {
    if (DEBUG_SERIAL)
    {
      Serial.println("[NTP] Falha ao obter hora — RTC mantido como estava.");
    }
    return;
  }

  DateTime novaHora(horaNtp.tm_year + 1900, horaNtp.tm_mon + 1, horaNtp.tm_mday,
                    horaNtp.tm_hour, horaNtp.tm_min, horaNtp.tm_sec);
  _rtc.ajustarHora(novaHora);

  if (DEBUG_SERIAL)
  {
    Serial.println("[NTP] RTC sincronizado com sucesso.");
  }
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
  return _irrigacao.contarAbertasManual();
}

int WebApManager::contarValvulasAutomaticasAbertas() const
{
  return _irrigacao.contarAbertasAutomatico();
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

void WebApManager::enviarAlertasHistorico(AsyncWebServerRequest *request)
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

  enviarRespostaJson(request, 200, json);
}

void WebApManager::enviarPaginaPrincipal(AsyncWebServerRequest *request)
{
  request->send(200, "text/html; charset=utf-8", WEB_DASHBOARD_HTML);
}

String WebApManager::montarJsonStatusSistema()
{
  DateTime agora = _rtc.agora();

  char data[16];
  snprintf(data, sizeof(data), "%02d/%02d/%04d", agora.day(), agora.month(), agora.year());

  char hora[12];
  snprintf(hora, sizeof(hora), "%02d:%02d:%02d", agora.hour(), agora.minute(), agora.second());

  bool staConectada = this->staConectada();
  String urlSta = staConectada ? urlAcessoSta() : String("");
  String mdnsHost = String(WIFI_MDNS_HOSTNAME) + ".local";
  String mdnsUrl = _mdnsAtivo ? (String("http://") + mdnsHost + "/") : String("");

  String json;
  json.reserve(3400);
  json += "{";
  json += "\"ok\":true,";
  json += "\"data\":\"" + String(data) + "\",";
  json += "\"hora\":\"" + String(hora) + "\",";
  json += "\"dow\":" + String(agora.dayOfTheWeek()) + ",";
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
  json += "\"url\":\"" + urlSta + "\",";
  json += "\"mdns_ativo\":" + String(_mdnsAtivo ? "true" : "false") + ",";
  json += "\"mdns_host\":\"" + mdnsHost + "\",";
  json += "\"mdns_url\":\"" + mdnsUrl + "\"";
  json += "}";
#if WIFI_WEBSOCKET_ENABLED
  json += ",";
  json += "\"websocket\":{";
  json += "\"habilitado\":true,";
  json += "\"ativo\":" + String(_webSocketIniciado ? "true" : "false") + ",";
  json += "\"path\":\"/ws\"";
  json += "}";
#else
  json += ",";
  json += "\"websocket\":{";
  json += "\"habilitado\":false,";
  json += "\"ativo\":false,";
  json += "\"path\":\"/ws\"";
  json += "}";
#endif
  json += "},";

  json += "\"agendas_ativas\":" + String(_schedule.totalAtivas()) + ",";

  json += "\"runtime\":{";
  json += "\"timeoutMin\":" + String(_config.timeoutManualMs() / 60000UL) + ",";
  json += "\"duracaoMin\":" + String(_config.duracaoPadraoMin());
  json += "},";

  json += "\"agenda_cfg\":{";
  json += "\"maxSimultaneos\":" + String(MAX_SETOR_SIMULTANEOS_AGENDA) + ",";
  json += "\"intervaloLoteS\":" + String(INTERVALO_LOTE_AGENDA_MS / 1000UL);
  json += "},";

  DateTime proximaData;
  AgendaSetor proximaAgenda = {};
  int slotProximo = -1;
  bool temProxima = _schedule.obterProximaExecucao(agora, proximaData, proximaAgenda, slotProximo);

  json += "\"proxima_agenda\":";
  if (!temProxima)
  {
    json += "null,";
  }
  else
  {
    long emS = (long)(proximaData.unixtime() - agora.unixtime());
    if (emS < 0)
    {
      emS = 0;
    }

    int setoresProxima = 0;
    for (int i = 0; i < NUM_VALVULAS; i++)
    {
      if (proximaAgenda.setoresMask & (1U << i))
      {
        setoresProxima++;
      }
    }

    json += "{";
    json += "\"slot\":" + String(slotProximo + 1) + ",";
    json += "\"dow\":" + String(proximaData.dayOfTheWeek()) + ",";
    json += "\"hora\":" + String(proximaData.hour()) + ",";
    json += "\"minuto\":" + String(proximaData.minute()) + ",";
    json += "\"setores\":" + String(setoresProxima) + ",";
    json += "\"setoresMask\":" + String(proximaAgenda.setoresMask) + ",";
    json += "\"em_s\":" + String(emS);
    json += "},";
  }

  json += "\"sensores\":{";
  json += "\"temperatura_c\":" + String(_sensorTempC, 1) + ",";
  json += "\"umidade_ar_pct\":" + String(_sensorUmidadePct, 0) + ",";
  json += "\"ok\":" + String(sensorClimaValido() ? "true" : "false") + ",";
  json += "\"idade_ms\":" + String(idadeLeituraClimaMs());
  json += "},";

  json += "\"valvulas\":[";
  for (int i = 0; i < NUM_VALVULAS; i++)
  {
    bool aberta = (_irrigacao.estadoValvula(i) == EstadoValvula::ABERTA);
    bool autoAgenda = _irrigacao.valvulaEmAgendamento(i);
    bool pendente = !aberta && ((_agendaPendenteMask & (1U << i)) != 0);

    json += "{";
    json += "\"id\":" + String(i + 1) + ",";
    json += "\"gpio\":" + String(gpioValvula(i)) + ",";
    json += "\"ativa\":" + String(aberta ? "true" : "false") + ",";
    json += "\"status\":\"" + String(aberta ? "Ativa" : "Inativa") + "\",";
    json += "\"origem\":\"" + String(!aberta ? "desligada" : (autoAgenda ? "agenda" : "manual")) + "\",";
    json += "\"pendente\":" + String(pendente ? "true" : "false") + ",";
    json += "\"restante_s\":" + String(_irrigacao.tempoRestanteMs(i) / 1000UL);
    json += "}";

    if (i < (NUM_VALVULAS - 1))
    {
      json += ",";
    }
  }
  json += "]";

  json += "}";

  return json;
}

void WebApManager::enviarStatusSistema(AsyncWebServerRequest *request)
{
  enviarRespostaJson(request, 200, montarJsonStatusSistema());

#if WIFI_WEBSOCKET_ENABLED
  enviarStatusWebSocket();
#endif
}

void WebApManager::enviarListaAgendas(AsyncWebServerRequest *request)
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

  enviarRespostaJson(request, 200, json);
}

void WebApManager::enviarRespostaJson(AsyncWebServerRequest *request, int statusCode, const String &json)
{
  AsyncWebServerResponse *response = request->beginResponse(statusCode, "application/json; charset=utf-8", json);
  response->addHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  request->send(response);
}

void WebApManager::enviarErroJson(AsyncWebServerRequest *request, int statusCode, const char *mensagem)
{
  String json;
  json.reserve(160);
  json += "{\"ok\":false,\"erro\":\"";
  json += mensagem;
  json += "\"}";

  enviarRespostaJson(request, statusCode, json);
}

bool WebApManager::temArg(AsyncWebServerRequest *request, const char *nome)
{
  return request->hasParam(nome, true) || request->hasParam(nome, false);
}

String WebApManager::lerArg(AsyncWebServerRequest *request, const char *nome)
{
  const AsyncWebParameter *param = request->getParam(nome, true);
  if (param == nullptr)
  {
    param = request->getParam(nome, false);
  }
  return (param != nullptr) ? param->value() : String("");
}

int WebApManager::lerIndiceValvula(AsyncWebServerRequest *request)
{
  if (!temArg(request, "index"))
  {
    return -1;
  }

  String txt = lerArg(request, "index");
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

int WebApManager::lerSlotAgenda(AsyncWebServerRequest *request)
{
  if (!temArg(request, "slot"))
  {
    return -1;
  }

  String txt = lerArg(request, "slot");
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
