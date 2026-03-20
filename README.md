<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>SovLink</title>
<link href="https://fonts.googleapis.com/css2?family=Cinzel+Decorative:wght@700;900&family=Cinzel:wght@400;600&family=IM+Fell+English:ital@0;1&display=swap" rel="stylesheet">
<style>
  :root {
    --red: #8b0000;
    --red-mid: #a31515;
    --red-light: #c0392b;
    --gold: #c9a84c;
    --gold-light: #f0d080;
    --dark: #0c0000;
    --text: #e8d5a3;
    --text-dim: #a08060;
  }
 
  * { margin: 0; padding: 0; box-sizing: border-box; }
 
  body {
    background-color: #080000;
    color: var(--text);
    font-family: 'Cinzel', serif;
    min-height: 100vh;
    overflow-x: hidden;
    position: relative;
  }
 
  /* ═══════════════════════════════
     SOVIET BACKGROUND
  ═══════════════════════════════ */
 
  /* Grunge texture base */
  body::before {
    content: '';
    position: fixed;
    inset: 0;
    background:
      repeating-linear-gradient(
        0deg,
        transparent 0px,
        transparent 3px,
        rgba(139,0,0,0.03) 3px,
        rgba(139,0,0,0.03) 4px
      ),
      repeating-linear-gradient(
        90deg,
        transparent 0px,
        transparent 60px,
        rgba(201,168,76,0.015) 60px,
        rgba(201,168,76,0.015) 61px
      ),
      radial-gradient(ellipse 80% 60% at 50% 0%, rgba(139,0,0,0.35) 0%, transparent 70%),
      radial-gradient(ellipse 60% 40% at 50% 100%, rgba(80,0,0,0.2) 0%, transparent 70%),
      linear-gradient(180deg, #0c0000 0%, #100000 50%, #0a0000 100%);
    pointer-events: none;
    z-index: 0;
  }
 
  /* Big decorative stars — Soviet style */
  body::after {
    content: '★                    ★';
    position: fixed;
    top: 20px;
    left: 0; right: 0;
    text-align: center;
    font-size: 1.2rem;
    color: rgba(139,0,0,0.25);
    letter-spacing: 60vw;
    pointer-events: none;
    z-index: 0;
  }
 
  /* SVG pattern overlay — hammer & sickle watermark grid */
  .sov-bg {
    position: fixed;
    inset: 0;
    z-index: 0;
    pointer-events: none;
    overflow: hidden;
  }
 
  /* Diagonal propaganda stripes */
  .sov-stripes {
    position: fixed;
    inset: 0;
    background: repeating-linear-gradient(
      -45deg,
      transparent 0px,
      transparent 40px,
      rgba(139,0,0,0.04) 40px,
      rgba(139,0,0,0.04) 41px
    );
    z-index: 0;
    pointer-events: none;
  }
 
  /* Corner ornaments */
  .corner {
    position: fixed;
    width: 120px;
    height: 120px;
    z-index: 0;
    pointer-events: none;
    opacity: 0.18;
  }
  .corner svg { width: 100%; height: 100%; }
  .corner-tl { top: 0; left: 0; }
  .corner-tr { top: 0; right: 0; transform: scaleX(-1); }
  .corner-bl { bottom: 0; left: 0; transform: scaleY(-1); }
  .corner-br { bottom: 0; right: 0; transform: scale(-1,-1); }
 
  /* Red banner strips top & bottom */
  .banner-top, .banner-bot {
    position: fixed;
    left: 0; right: 0;
    height: 6px;
    z-index: 1;
    pointer-events: none;
  }
  .banner-top {
    top: 0;
    background: linear-gradient(90deg, var(--red) 0%, var(--gold) 50%, var(--red) 100%);
  }
  .banner-bot {
    bottom: 0;
    background: linear-gradient(90deg, var(--red) 0%, var(--gold) 50%, var(--red) 100%);
  }
 
  /* ═══════════════════════════════
     LAYOUT
  ═══════════════════════════════ */
  .page { position: relative; z-index: 2; }
 
  /* ── HERO ── */
  .hero {
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 70px 20px 40px;
    position: relative;
  }
 
  /* ── SPINNING LOGO — rotates counterclockwise ── */
  .logo-wrap {
    position: relative;
    width: 200px; height: 200px;
    margin-bottom: 44px;
  }
 
  /* Glow halo behind logo */
  .logo-glow {
    position: absolute;
    inset: -20px;
    border-radius: 50%;
    background: radial-gradient(circle, rgba(139,0,0,0.5) 0%, rgba(201,168,76,0.15) 40%, transparent 70%);
    animation: pulse 3s ease-in-out infinite;
    filter: blur(8px);
  }
 
  @keyframes pulse {
    0%, 100% { opacity: 0.6; transform: scale(0.95); }
    50%       { opacity: 1;   transform: scale(1.05); }
  }
 
  /* The image itself spins counterclockwise */
  .logo-img {
    width: 100%;
    height: 100%;
    object-fit: contain;
    animation: spinLeft 10s linear infinite;
    filter: drop-shadow(0 0 16px rgba(201,168,76,0.5))
            drop-shadow(0 0 40px rgba(139,0,0,0.7));
    position: relative;
    z-index: 2;
  }
 
  @keyframes spinLeft {
    from { transform: rotate(0deg); }
    to   { transform: rotate(-360deg); }
  }
 
  /* ── TITLE ── */
  .title {
    font-family: 'Cinzel Decorative', serif;
    font-size: clamp(2.4rem, 5vw, 3.8rem);
    font-weight: 900;
    letter-spacing: 0.25em;
    text-transform: uppercase;
    background: linear-gradient(135deg, var(--gold-light) 0%, var(--gold) 45%, #7a5010 80%, var(--gold) 100%);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    background-clip: text;
    margin-bottom: 10px;
  }
 
  .subtitle {
    font-family: 'IM Fell English', serif;
    font-style: italic;
    color: rgba(201,168,76,0.5);
    font-size: 1rem;
    letter-spacing: 0.15em;
    margin-bottom: 28px;
    text-transform: uppercase;
  }
 
  /* Soviet motto band */
  .motto {
    display: flex;
    align-items: center;
    gap: 20px;
    margin-bottom: 40px;
    color: rgba(139,0,0,0.7);
    font-size: 0.75rem;
    letter-spacing: 0.25em;
    text-transform: uppercase;
  }
  .motto-line { width: 60px; height: 1px; background: rgba(139,0,0,0.5); }
 
  /* ── DIVIDER ── */
  .divider {
    display: flex;
    align-items: center;
    gap: 16px;
    width: 100%;
    max-width: 720px;
    margin: 0 auto 52px;
  }
  .divider-line { flex: 1; height: 1px; background: linear-gradient(90deg, transparent, rgba(201,168,76,0.4), transparent); }
  .divider-star { color: var(--red-light); font-size: 1rem; }
 
  /* ── SECTIONS ── */
  .container {
    max-width: 860px;
    margin: 0 auto;
    padding: 0 24px 80px;
  }
 
  .section {
    margin-bottom: 56px;
    opacity: 0;
    transform: translateY(24px);
    animation: fadeUp 0.7s ease forwards;
  }
  .section:nth-child(1) { animation-delay: 0.15s; }
  .section:nth-child(2) { animation-delay: 0.3s; }
  .section:nth-child(3) { animation-delay: 0.45s; }
  .section:nth-child(4) { animation-delay: 0.6s; }
 
  @keyframes fadeUp {
    to { opacity: 1; transform: translateY(0); }
  }
 
  .section-label {
    display: flex;
    align-items: center;
    gap: 14px;
    margin-bottom: 22px;
  }
  .section-label h2 {
    font-family: 'Cinzel Decorative', serif;
    font-size: 0.82rem;
    font-weight: 700;
    letter-spacing: 0.3em;
    text-transform: uppercase;
    color: var(--gold);
    white-space: nowrap;
  }
  .section-label .bar {
    flex: 1;
    height: 1px;
    background: linear-gradient(90deg, rgba(201,168,76,0.5), transparent);
  }
  .section-label .num {
    font-size: 0.7rem;
    color: var(--red-light);
    letter-spacing: 0.2em;
    font-weight: 600;
  }
 
  /* ── INTRO CARD ── */
  .intro-card {
    background: linear-gradient(135deg, rgba(100,0,0,0.15), rgba(0,0,0,0.4));
    border: 1px solid rgba(201,168,76,0.15);
    border-left: 3px solid var(--red-light);
    padding: 28px 32px;
    font-family: 'IM Fell English', serif;
    font-size: 1.05rem;
    line-height: 1.9;
    color: #d4b896;
    position: relative;
  }
  .intro-card::before {
    content: '« ';
    color: rgba(139,0,0,0.4);
    font-size: 2rem;
    position: absolute;
    top: 12px; left: 14px;
    font-family: serif;
    line-height: 1;
  }
 
  /* ── FEATURE GRID ── */
  .feat-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(195px, 1fr));
    gap: 14px;
  }
 
  .feat-card {
    background: rgba(8,0,0,0.7);
    border: 1px solid rgba(201,168,76,0.12);
    border-top: 2px solid var(--red-mid);
    padding: 22px 18px;
    transition: all 0.3s ease;
  }
  .feat-card:hover {
    border-top-color: var(--gold);
    background: rgba(100,0,0,0.12);
    transform: translateY(-4px);
    box-shadow: 0 10px 30px rgba(0,0,0,0.5);
  }
  .feat-card .icon { font-size: 1.4rem; margin-bottom: 10px; display: block; }
  .feat-card .feat-title {
    font-family: 'Cinzel', serif;
    font-size: 0.78rem;
    font-weight: 600;
    letter-spacing: 0.12em;
    color: var(--gold);
    text-transform: uppercase;
    margin-bottom: 8px;
  }
  .feat-card .feat-desc {
    font-family: 'IM Fell English', serif;
    font-size: 0.93rem;
    color: var(--text-dim);
    line-height: 1.6;
  }
 
  /* ── SECURITY LIST ── */
  .sec-list { display: flex; flex-direction: column; gap: 12px; }
  .sec-item {
    display: flex;
    align-items: flex-start;
    gap: 16px;
    padding: 18px 22px;
    background: rgba(8,0,0,0.6);
    border: 1px solid rgba(139,0,0,0.25);
    border-left: 3px solid rgba(139,0,0,0.6);
    transition: border-color 0.3s;
  }
  .sec-item:hover { border-left-color: var(--gold); }
  .sec-item .star { color: var(--red-light); font-size: 1rem; margin-top: 2px; flex-shrink: 0; }
  .sec-item .sec-content h3 {
    font-family: 'Cinzel', serif;
    font-size: 0.82rem;
    font-weight: 600;
    color: #e8c070;
    letter-spacing: 0.1em;
    margin-bottom: 5px;
  }
  .sec-item .sec-content p {
    font-family: 'IM Fell English', serif;
    font-size: 0.95rem;
    color: var(--text-dim);
    line-height: 1.5;
  }
 
  /* ── TECH ── */
  .tech-row { display: flex; flex-wrap: wrap; gap: 10px; }
  .tech-pill {
    padding: 8px 16px;
    border: 1px solid rgba(139,0,0,0.4);
    font-family: 'Cinzel', serif;
    font-size: 0.7rem;
    font-weight: 600;
    letter-spacing: 0.12em;
    color: rgba(201,168,76,0.8);
    background: rgba(80,0,0,0.15);
    text-transform: uppercase;
    transition: all 0.2s;
  }
  .tech-pill:hover { background: rgba(139,0,0,0.25); border-color: var(--gold); color: var(--gold-light); }
 
  /* ── BADGE ROW ── */
  .badge-row { display: flex; justify-content: center; gap: 10px; flex-wrap: wrap; margin-bottom: 30px; }
  .badge {
    padding: 4px 14px;
    border: 1px solid rgba(139,0,0,0.5);
    border-radius: 999px;
    font-family: 'Cinzel', serif;
    font-size: 0.62rem;
    font-weight: 600;
    letter-spacing: 0.15em;
    color: rgba(201,168,76,0.7);
    text-transform: uppercase;
    background: rgba(80,0,0,0.2);
  }
 
  /* ── FOOTER ── */
  .footer {
    text-align: center;
    padding: 28px 20px 40px;
    border-top: 1px solid rgba(139,0,0,0.3);
    font-family: 'IM Fell English', serif;
    font-style: italic;
    color: rgba(139,0,0,0.5);
    font-size: 0.85rem;
    letter-spacing: 0.1em;
  }
  .footer-star { color: rgba(139,0,0,0.6); font-size: 1.2rem; display: block; margin-bottom: 8px; }
</style>
</head>
<body>
 
<!-- Soviet background layers -->
<div class="banner-top"></div>
<div class="banner-bot"></div>
<div class="sov-stripes"></div>
 
<!-- Corner ornaments SVG -->
<div class="corner corner-tl">
  <svg viewBox="0 0 120 120" fill="none" xmlns="http://www.w3.org/2000/svg">
    <path d="M0 0 L120 0 L0 120 Z" fill="rgba(139,0,0,0.4)"/>
    <path d="M0 0 L80 0 L0 80 Z" fill="rgba(139,0,0,0.2)"/>
    <line x1="0" y1="0" x2="120" y2="0" stroke="#c9a84c" stroke-width="2" opacity="0.6"/>
    <line x1="0" y1="0" x2="0" y2="120" stroke="#c9a84c" stroke-width="2" opacity="0.6"/>
    <text x="14" y="28" fill="#c9a84c" font-size="20" opacity="0.7">★</text>
  </svg>
</div>
<div class="corner corner-tr">
  <svg viewBox="0 0 120 120" fill="none" xmlns="http://www.w3.org/2000/svg">
    <path d="M0 0 L120 0 L0 120 Z" fill="rgba(139,0,0,0.4)"/>
    <path d="M0 0 L80 0 L0 80 Z" fill="rgba(139,0,0,0.2)"/>
    <line x1="0" y1="0" x2="120" y2="0" stroke="#c9a84c" stroke-width="2" opacity="0.6"/>
    <line x1="0" y1="0" x2="0" y2="120" stroke="#c9a84c" stroke-width="2" opacity="0.6"/>
    <text x="14" y="28" fill="#c9a84c" font-size="20" opacity="0.7">★</text>
  </svg>
</div>
<div class="corner corner-bl">
  <svg viewBox="0 0 120 120" fill="none" xmlns="http://www.w3.org/2000/svg">
    <path d="M0 0 L120 0 L0 120 Z" fill="rgba(139,0,0,0.4)"/>
    <path d="M0 0 L80 0 L0 80 Z" fill="rgba(139,0,0,0.2)"/>
    <line x1="0" y1="0" x2="120" y2="0" stroke="#c9a84c" stroke-width="2" opacity="0.6"/>
    <line x1="0" y1="0" x2="0" y2="120" stroke="#c9a84c" stroke-width="2" opacity="0.6"/>
    <text x="14" y="28" fill="#c9a84c" font-size="20" opacity="0.7">★</text>
  </svg>
</div>
<div class="corner corner-br">
  <svg viewBox="0 0 120 120" fill="none" xmlns="http://www.w3.org/2000/svg">
    <path d="M0 0 L120 0 L0 120 Z" fill="rgba(139,0,0,0.4)"/>
    <path d="M0 0 L80 0 L0 80 Z" fill="rgba(139,0,0,0.2)"/>
    <line x1="0" y1="0" x2="120" y2="0" stroke="#c9a84c" stroke-width="2" opacity="0.6"/>
    <line x1="0" y1="0" x2="0" y2="120" stroke="#c9a84c" stroke-width="2" opacity="0.6"/>
    <text x="14" y="28" fill="#c9a84c" font-size="20" opacity="0.7">★</text>
  </svg>
</div>
 
<div class="page">
 
  <!-- HERO -->
  <div class="hero">
 
    <div class="logo-wrap">
      <div class="logo-glow"></div>
      <img class="logo-img" src="data:image/png;base64,/9j/4AAQSkZJRgABAQAAAQABAAD/4gHYSUNDX1BST0ZJTEUAAQEAAAHIAAAAAAQwAABtbnRyUkdCIFhZWiAH4AABAAEAAAAAAABhY3NwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAA9tYAAQAAAADTLQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAlkZXNjAAAA8AAAACRyWFlaAAABFAAAABRnWFlaAAABKAAAABRiWFlaAAABPAAAABR3dHB0AAABUAAAABRyVFJDAAABZAAAAChnVFJDAAABZAAAAChiVFJDAAABZAAAAChjcHJ0AAABjAAAADxtbHVjAAAAAAAAAAEAAAAMZW5VUwAAAAgAAAAcAHMAUgBHAEJYWVogAAAAAAAAb6IAADj1AAADkFhZWiAAAAAAAABimQAAt4UAABjaWFlaIAAAAAAAACSgAAAPhAAAts9YWVogAAAAAAAA9tYAAQAAAADTLXBhcmEAAAAAAAQAAAACZmYAAPKnAAANWQAAE9AAAApbAAAAAAAAAABtbHVjAAAAAAAAAAEAAAAMZW5VUwAAACAAAAAcAEcAbwBvAGcAbABlACAASQBuAGMALgAgADIAMAAxADb/2wBDAAUDBAQEAwUEBAQFBQUGBwwIBwcHBw8LCwkMEQ8SEhEPERETFhwXExQaFRERGCEYGh0dHx8fExciJCIeJBweHx7/2wBDAQUFBQcGBw4ICA4eFBEUHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh7/wAARCAGPAk4DASIAAhEBAxEB/8QAHQABAAEFAQEBAAAAAAAAAAAAAAQDBQYHCAIBCf/EAEwQAAEDAgMFBAYFCQUHBAMAAAEAAgMEEQUSIQYxQVFhBxMicRQyQoGRoQgjUrHwFTM1YnKywdHhJHN0gpIWFyU0Q2PxRFSiwidTk//EABsBAQACAwEBAAAAAAAAAAAAAAADBAIFBgEH/8QAQBEAAgECAwQGBwYGAgEFAAAAAAECAxEEITEFEkHwUWFxgZGxEyIyM6HB0RQ0csLS4QYjQlKSohWCFiVisuLx/9oADAMBAAIRAxEAPwDjJERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEVaigdVVkNM1wa6WRrATuBJsthYR2V1dfUOjiq6ipDSQ51JRukAI4E7h71HOrGn7TJIUpTTa0Nbot44Z2EVstjUUOM2t60ndQtHnm1CvtN2F4fC7LUxUDCHWzTYu1w87MVWW0aEeJPHBzfFeflc5xRdRUvZVspSuY2ep2eYPacxss7m+4gXV2ptk9jaSQMbiED27iafB2NdfoST8VXlteitOfMnjs2bV8/8AF/NI5KZFJIbMje48g26nU2BY1Um1PhFfL+xTvP8ABdeUlDsxTRODajFXeLRzWRMGXyDbq4Q0+yx1ho8V8Qt9ZXaOPuHyUMttQWi8/oSR2XNq+7J/4rzkci0uw219SLw7O4g7zit96nU/ZjtxMRfApYgeMsjGj5ldew0+z7C3/gET3A6mSeQgjyzKbTDDWfm8Bws3ObxwCQ6D9Yn4KvLbttF8H9USx2TN/wBL72vlc5Ci7I9sni7oKCL9utjH8VKpuxvaeVwa+twiInnUl33Bdgs7kRxZMOoIyB4SylYOeu5XGDE8SAaMzW5SSA1jQRpbkopbdnw8v3JFsmV/YVvxP9ByFTdg20EuW+NYYS7cIo5pPuYpkf0f8TzuZLj8Qc0XdloZjbzuBZdaGsxSQ61EvhLbZfDa27yRn5Q73MTKHvFnuDj4t6je3Kj4vwQ/4uafsx/yl9Ecu0v0cqx78j8eqHuIuGxYW8m3PUqe36M1WJmwvrsaMjhmAGFHd55l0rOzFJsjjNUZdMt5DvVKopq5r2maaTKARd8vHgFC9t1no38PoTx2ZBJ727/t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X8Oqy3Yzs8w7BKWKs2ohjLzZ1Pg0Txck7nTuH7vx5LYTxFGEPSap6EqxMqj3E1fq18OHNjEuz/s5r8WYMVmnbBSg2kxOpHhv9iFu9x+S2zhUNDhmHHCsApTRROF55nnNUVB+09w3DoNF9r6jEMTqxF4co0ip4m2ZEAOA3C3NYttTtpRYMXUGBObiGIWyvmveGE8bfbcD7lqqlati5bq57R6KMPWm+xcP3fWy+Y7i2E7P4aJ8VsZX/mYIrGWcdB7IvvJ0WqdrNparFYRUYrUR0OGQuvDSt/NtPlvkfbifdZY9tNtCykq5ajEqh9ficvic0uub8Mx4DoFgGN4vV4lUd9VyZiNGMGjYxyAW4wOy1H1n4/Q1mM2io5cej6l22i2nmrGPpaLNTUbtHcHy/tHgOixeSS+g3Ly95cd68LfU6cYKyOeq151Zb0mERFIQhERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQHpriFdcCxmswqo72kk0PrxO1ZIOoVoX0EjcsZRUlZmcJuDujbuy+OxVtQyuwepkoMUhF+6a6z7ccv229Pkto7N7aw4qW0WLCGixKTRkrhaGV3v9Rx5HQ81yvTzOY9r2Oc17TdrmmxB5grOcB2shrQ2kx94D7WZV5d/ISAfvfFajGbNjUV1+5vcHtPhPJnTeDSYpRPqHNn9GEt7NtfMN4NrfBUcQdRUFHLimOVrKaHNq9wJdM6/qgb3OseC11sxtliuzlM6CSOPFaPuz6KJXEiI20IcPWZ0+CxjanH5qioOK7Q17pHOv3beQ+zGzcB+CVo4bOqTnZvLq5yNtKvCF5pWb1f78TJ9rttqvFo5qHC2OwzCD+cDnASTDnI4eqP1R77rVWO7UxwtdTYPobZXVNv3B/FWfaLaGpxQ9zbuKNpu2Bp39XHifkrBJISbA6Lo8LgIUlpz8zQ4vaUpu0PEqTzOc4kuLnE3JJuSVQJJ3r4i2KVjUN3CIi9PAiIgCIiAIiIAiIgCIiAIiIAiIgCIiAIiIAiIgCIiAIiIAiIgCIiAIiIAiIgCIiAIiIAiIgCIiAIiIAiIgCIiAIiIAiIgCIiAIiIAiIgCIiAIiIAiIgCIiAL2x9l4RAnYyHAdpq/CoH08TmT07vEIprlrHfaby8txVtxCuqKyodU1U7pZXcXcOgHAKAiwVOKd7EjrTcd1vI9OcSV5RFmRhERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREAREQBERAEREB//2Q==" alt="SovLink Logo">
    </div>
 
    <h1 class="title">SovLink</h1>
    <p class="subtitle">Messagerie · Chiffrée · Sans Trace</p>
 
    <div class="motto">
      <div class="motto-line"></div>
      ★ &nbsp; SÉCURITÉ · ANONYMAT · LIBERTÉ &nbsp; ★
      <div class="motto-line"></div>
    </div>
 
    <div class="badge-row">
      <span class="badge">C++</span>
      <span class="badge">Libsodium</span>
      <span class="badge">Boost Asio</span>
      <span class="badge">Qt</span>
      <span class="badge">TCP</span>
    </div>
 
    <div class="divider">
      <div class="divider-line"></div>
      <span class="divider-star">★</span>
      <div class="divider-line"></div>
    </div>
  </div>
 
  <!-- CONTENT -->
  <div class="container">
 
    <div class="section">
      <div class="section-label">
        <span class="num">I</span>
        <h2>Objectifs du Projet</h2>
        <div class="bar"></div>
      </div>
      <div class="intro-card">
        SovLink est une application de messagerie en ligne permettant à plusieurs utilisateurs de communiquer en temps réel via une connexion sécurisée à un serveur TCP. L'objectif central est de fournir un moyen <em>simple, sécurisé et anonyme</em> d'échanger des messages — sans compte, sans historique, sans compromis.<br><br>
        Chaque session génère automatiquement une paire de clés cryptographiques unique. La clé publique devient l'identifiant temporaire de l'utilisateur ; à chaque reconnexion, un nouvel identifiant est créé, garantissant un anonymat renforcé. Aucun message n'est conservé après transmission.
      </div>
    </div>
 
    <div class="section">
      <div class="section-label">
        <span class="num">II</span>
        <h2>Fonctionnalités Principales</h2>
        <div class="bar"></div>
      </div>
      <div class="feat-grid">
        <div class="feat-card">
          <span class="icon">⚡</span>
          <div class="feat-title">Messagerie temps réel</div>
          <div class="feat-desc">Envoi et réception de messages instantanés via un serveur TCP multi-clients.</div>
        </div>
        <div class="feat-card">
          <span class="icon">🔗</span>
          <div class="feat-title">Connexion serveur</div>
          <div class="feat-desc">Connexion directe au serveur pour accéder au service de communication sécurisé.</div>
        </div>
        <div class="feat-card">
          <span class="icon">👥</span>
          <div class="feat-title">Multi-clients</div>
          <div class="feat-desc">Gestion simultanée de plusieurs connexions pour une communication fluide.</div>
        </div>
        <div class="feat-card">
          <span class="icon">🔑</span>
          <div class="feat-title">Sans compte</div>
          <div class="feat-desc">Aucune inscription requise. La clé publique générée sert d'identifiant temporaire.</div>
        </div>
        <div class="feat-card">
          <span class="icon">📤</span>
          <div class="feat-title">Partage de clé</div>
          <div class="feat-desc">Partagez votre clé publique via l'application de votre choix pour initier une conversation.</div>
        </div>
      </div>
    </div>
 
    <div class="section">
      <div class="section-label">
        <span class="num">III</span>
        <h2>Sécurité & Confidentialité</h2>
        <div class="bar"></div>
      </div>
      <div class="sec-list">
        <div class="sec-item">
          <span class="star">★</span>
          <div class="sec-content">
            <h3>Chiffrement de bout en bout</h3>
            <p>Les messages sont chiffrés avant transmission grâce aux paires de clés publique / privée via Libsodium. Seul le destinataire peut lire le contenu.</p>
          </div>
        </div>
        <div class="sec-item">
          <span class="star">★</span>
          <div class="sec-content">
            <h3>Zéro historique</h3>
            <p>Aucun message n'est stocké par le système. Les échanges disparaissent automatiquement à la fin de la communication.</p>
          </div>
        </div>
        <div class="sec-item">
          <span class="star">★</span>
          <div class="sec-content">
            <h3>Anonymat par rotation de clés</h3>
            <p>L'identifiant (clé publique) est régénéré à chaque nouvelle connexion, assurant un anonymat structurel sans effort.</p>
          </div>
        </div>
        <div class="sec-item">
          <span class="star">★</span>
          <div class="sec-content">
            <h3>Suppression automatique</h3>
            <p>Les messages reçus sont automatiquement supprimés après un délai défini, renforçant la confidentialité des échanges.</p>
          </div>
        </div>
      </div>
    </div>
 
    <div class="section">
      <div class="section-label">
        <span class="num">IV</span>
        <h2>Stack Technique</h2>
        <div class="bar"></div>
      </div>
      <div class="tech-row">
        <span class="tech-pill">⚙ C++ — Langage principal</span>
        <span class="tech-pill">🔐 Libsodium — Chiffrement</span>
        <span class="tech-pill">🌐 Boost Asio — Réseau TCP</span>
        <span class="tech-pill">🖥 Qt Designer — Interface</span>
        <span class="tech-pill">🛠 Qt Creator — IDE</span>
      </div>
    </div>
 
  </div>
 
  <div class="footer">
    <span class="footer-star">★ ★ ★</span>
    SovLink · Groupe 3 · Messagerie chiffrée, sans trace, sans compromis
  </div>
 
</div>
</body>
</html>
