# 🎨 Frontend Documentation

این مستند توضیحات کاملی درباره بخش Frontend سیستم آرکید (HTML، CSS، JavaScript) ارائه می‌دهد.

🌐 **نسخه زنده**: [https://baziland.zarebidoki.com/](https://baziland.zarebidoki.com/)
**Current Version:** 1.2.0 — Chess rule enforcement fixes (Rook و Gryphon) و بهبود پایداری اعتبارسنجی/هایلایت حرکت.

---

## 📋 فهرست مطالب

- [معرفی](#معرفی)
- [معماری Frontend](#معماری-frontend)
- [فایل‌ها](#فایلها)
- [HTML Structure](#html-structure)
- [CSS Architecture](#css-architecture)
- [JavaScript Modules](#javascript-modules)
- [جریان کاربر](#جریان-کاربر)
- [استایل‌ها و تم](#استایلها-و-تم)
- [نکات توسعه](#نکات-توسعه)

---

## 🎯 معرفی

### چرا Vanilla JavaScript؟

من می‌توانستم از React، Vue، یا Angular استفاده کنم، اما تصمیم گرفتم:

✅ **مزایا**:
- **یادگیری عمیق**: درک کامل DOM و event handling
- **Performance**: بدون overhead framework
- **سبک**: bundle size کوچک
- **کنترل کامل**: هر خط کد را خودم نوشتم
- **سازگاری**: با CGI backend به راحتی کار می‌کند

❌ **معایب**:
- کد بیشتر برای state management
- بدون reactivity خودکار
- DOM manipulation دستی

### Stack Technology

```
Frontend Stack:
├── HTML5          (ساختار)
├── CSS3           (استایل + انیمیشن)
├── Vanilla JS     (منطق)
├── Particles.js   (افکت پس‌زمینه)
└── Font Awesome   (آیکون‌ها)
```

---

## 🏗️ معماری Frontend

### ساختار کلی

```
Frontend Architecture:

┌─────────────────────────────────────┐
│         index.html                  │
│  (ساختار HTML + کابینت آرکید)      │
└─────────────┬───────────────────────┘
              │
       ┌──────┴──────┐
       │             │
  ┌────▼─────┐  ┌───▼────┐
  │ style.css│  │script.js│
  │(1128 line)│ │(1353 line)│
  └──────────┘  └────┬────┘
                     │
              ┌──────┴──────┐
              │             │
         ┌────▼─────┐  ┌───▼────────┐
         │ auth.js  │  │Particles.js│
         │(328 line)│  │ (Library)  │
         └──────────┘  └────────────┘
```

### تقسیم مسئولیت

| فایل | مسئولیت | تعداد خط |
|------|---------|----------|
| `index.html` | ساختار، modal ها، کابینت | 584 |
| `style.css` | استایل، انیمیشن، تم، 3D | 1128 |
| `script.js` | کاروسل، رویدادها، UI | 1353 |
| `auth.js` | احراز هویت، session، API | 328 |

---

## 📁 فایل‌ها

### 1. index.html (584 خط)

**نقش**: ساختار اصلی صفحه

**بخش‌های اصلی**:
```html
<!DOCTYPE html>
<html dir="rtl" lang="fa">
<head>
    <!-- Meta tags, fonts, libraries -->
</head>
<body>
    <!-- 1. Auth Strip -->
    <div class="auth-strip">...</div>

    <!-- 2. Help Modal -->
    <div id="help-modal">...</div>

    <!-- 3. Account Panel -->
    <div id="account-panel">...</div>

    <!-- 4. Admin Panel -->
    <div id="admin-panel">...</div>

    <!-- 5. Auth Overlay (Login/Register) -->
    <div id="auth-overlay">...</div>

    <!-- 6. Arcade Container (کابینت) -->
    <div class="arcade-container">
        <div class="cabinet-body">
            <div class="marquee">BAZI LAND</div>
            <div class="monitor-bezel">
                <div class="crt-screen">
                    <div id="game-display">
                        <div class="menu-overlay">INSERT COIN</div>
                        <div class="game-carousel">
                            <!-- بازی‌ها -->
                        </div>
                    </div>
                </div>
            </div>
            <div class="control-deck">
                <!-- جویستیک و دکمه‌ها -->
            </div>
        </div>
    </div>

    <!-- Particles Background -->
    <div id="particles-js"></div>

    <!-- Scripts -->
    <script src="auth.js"></script>
    <script src="script.js"></script>
</body>
</html>
```

**ویژگی‌های خاص**:
- `dir="rtl"`: راست به چپ برای فارسی
- `lang="fa"`: زبان فارسی
- Semantic HTML5 tags
- ARIA labels برای accessibility

---

### 2. style.css (1128 خط)

**نقش**: تمام استایل‌ها، انیمیشن‌ها، و افکت‌های بصری

**بخش‌های اصلی**:

#### بخش 1: CSS Variables (خطوط 1-30)
```css
:root {
    --bg-dark: #0a0a0a;
    --neon-green: #00ff41;
    --cabinet-body: #2a2a2a;
    --cabinet-border: #1a1a1a;
    --text-light: #e0e0e0;
    --glow: 0 0 10px var(--neon-green);
    /* ... */
}
```

#### بخش 2: Base Styles (خطوط 30-100)
- Reset CSS
- Body background
- Font definitions

#### بخش 3: Auth UI (خطوط 100-250)
- Auth strip (نوار بالا)
- User pill
- Login/Register buttons
- Help modal
- Account panel dropdown

#### بخش 4: Admin Panel (خطوط 250-350)
- User management table
- Coin management
- Activity logs

#### بخش 5: Auth Overlay (خطوط 350-450)
- Login/Register forms
- Form inputs
- Gradient backgrounds

#### بخش 6: Arcade Cabinet (خطوط 450-815)
- **Cabinet body**: gradient، shadows
- **Marquee**: نئون سبز با glow
- **Monitor bezel**: frame
- **CRT screen**: border-radius بیضوی
- **Scanlines**: افکت خطوط CRT
- **Reflection**: شفافیت شیشه

#### بخش 7: Game Display (خطوط 815-950)
- Game carousel positioning
- Game items (کارت‌های بازی)
- Price tags
- Active state effects

#### بخش 8: Control Deck (خطوط 950-1128)
- **3D perspective**: rotateX
- **Joystick**: base + stick + ball
- **Arcade buttons**: دکمه‌های بزرگ
- **Small buttons**: COIN، SELECT، START
- **Coin HUD**: نمایش سکه

**تکنیک‌های CSS پیشرفته**:
- CSS Variables برای theming
- 3D transforms (`perspective`، `rotateX`)
- Multiple text-shadows برای neon glow
- Radial gradients برای افکت‌های نورانی
- Keyframe animations
- Custom scrollbar
- Backdrop-filter برای blur

---

### 3. script.js (1353 خط)

**نقش**: منطق اصلی frontend، مدیریت UI، و ارتباط با backend

**بخش‌های اصلی**:

#### بخش 1: Particles.js Setup (خطوط 1-100)
```javascript
particlesJS('particles-js', {
    particles: {
        number: { value: 100 },
        color: { value: '#00ff41' },
        // ...
    }
});
```

#### بخش 2: Game Data (خطوط 100-200)
```javascript
const games = [
    {
        id: 'guess_number',
        title: 'حدس عدد',
        price: 3,
        path: 'cgi-bin/guess_number.cgi'
    },
    // ...
];
```

#### بخش 3: Game Carousel (خطوط 200-400)
```javascript
let currentIndex = 0;

function updateCarousel() {
    const offset = -(currentIndex * (itemWidth + gap));
    carousel.style.transform = `translateX(${offset}px)`;
    updateActiveGame();
}

function nextGame() {
    currentIndex = (currentIndex + 1) % gameItems.length;
    updateCarousel();
}

function prevGame() {
    currentIndex = (currentIndex - 1 + gameItems.length) % gameItems.length;
    updateCarousel();
}
```

**ریاضیات Carousel**:
```
Position = -(index × (itemWidth + gap)) + offset

مثال (itemWidth = 120px، gap = 20px):
index 0: -(0 × 140) = 0px
index 1: -(1 × 140) = -140px
index 2: -(2 × 140) = -280px
```

#### بخش 4: Game Loading (خطوط 400-550)
```javascript
function loadGame(gameId) {
    const game = games.find(g => g.id === gameId);

    // بررسی احراز هویت
    if (!Auth.isLoggedIn()) {
        showAuthOverlay();
        return;
    }

    // بررسی سکه
    const user = Auth.getUser();
    if (user.coins < game.price) {
        alert('سکه کافی نداری!');
        return;
    }

    // کسر سکه
    Auth.subtractCoins(game.price);

    // بارگذاری iframe
    const iframe = document.createElement('iframe');
    iframe.src = game.path;
    gameDisplay.innerHTML = '';
    gameDisplay.appendChild(iframe);
}
```

#### بخش 5: Keyboard Controls (خطوط 550-650)
```javascript
document.addEventListener('keydown', (e) => {
    if (e.key === 'ArrowLeft') {
        prevGame();
    } else if (e.key === 'ArrowRight') {
        nextGame();
    } else if (e.key === 'Enter') {
        loadGame(games[currentIndex].id);
    } else if (e.key === 'Escape') {
        closeGame();
    }
});
```

#### بخش 6: Auth UI Integration (خطوط 650-850)
```javascript
function updateAuthUI() {
    const user = Auth.getUser();
    if (user) {
        usernameSpan.textContent = user.username;
        coinsSpan.textContent = user.coins;
        loginBtn.style.display = 'none';
        registerBtn.style.display = 'none';
        userPill.style.display = 'flex';
    } else {
        // حالت logout
    }
}
```

#### بخش 7: Admin Panel (خطوط 850-1200)
```javascript
function loadAdminPanel() {
    if (!Auth.isAdmin()) {
        alert('دسترسی ندارید!');
        return;
    }

    // دریافت داده‌ها از API
    fetch('/cgi-bin/auth_api.cgi?action=get_all_users')
        .then(response => response.json())
        .then(users => {
            renderUsersTable(users);
        });
}

function addCoins(username, amount) {
    Auth.adminAddCoins(username, amount);
    loadAdminPanel();  // refresh
}
```

#### بخش 8: Event Listeners (خطوط 1200-1353)
```javascript
// Init on page load
document.addEventListener('DOMContentLoaded', () => {
    updateCarousel();
    updateAuthUI();

    // Auto-update UI every second
    setInterval(() => {
        if (Auth.isLoggedIn()) {
            updateAuthUI();
        }
    }, 1000);
});
```

**الگوهای طراحی**:
- Event-driven architecture
- Functional programming (pure functions)
- Module pattern (با Auth.js)
- Observer pattern (event listeners)

---

### 4. auth.js (328 خط)

**نقش**: ماژول احراز هویت (IIFE pattern)

**ساختار**:
```javascript
const Auth = (function() {
    // Private variables
    const API_BASE = '/cgi-bin/auth_api.cgi';

    // Private functions
    function saveSession(user) {
        localStorage.setItem('user', JSON.stringify(user));
        localStorage.setItem('sessionToken', user.sessionToken);
    }

    function loadSession() {
        const userStr = localStorage.getItem('user');
        return userStr ? JSON.parse(userStr) : null;
    }

    // Public API
    return {
        login: async function(username, password) {
            const response = await fetch(`${API_BASE}?action=login`, {
                method: 'POST',
                body: JSON.stringify({ username, password })
            });
            const data = await response.json();
            if (data.success) {
                saveSession(data.user);
                return { success: true };
            }
            return { success: false, error: data.error };
        },

        register: async function(username, password) {
            // ...
        },

        logout: function() {
            localStorage.removeItem('user');
            localStorage.removeItem('sessionToken');
        },

        isLoggedIn: function() {
            return loadSession() !== null;
        },

        getUser: function() {
            return loadSession();
        },

        isAdmin: function() {
            const user = loadSession();
            return user && user.role === 'admin';
        },

        subtractCoins: async function(amount) {
            // ...
        },

        addCoins: async function(amount) {
            // ...
        },

        adminAddCoins: async function(username, amount) {
            // ...
        }
    };
})();
```

**چرا IIFE?**
- ✅ Encapsulation (private variables)
- ✅ Namespace pollution prevention
- ✅ Singleton pattern
- ✅ Clean API

---

## 🎨 CSS Architecture

### CSS Variables System

من از CSS Variables برای theming استفاده کردم:

```css
:root {
    /* Colors */
    --bg-dark: #0a0a0a;
    --neon-green: #00ff41;
    --cabinet-body: #2a2a2a;

    /* Effects */
    --glow: 0 0 10px var(--neon-green);
    --glow-strong: 0 0 20px var(--neon-green);

    /* Spacing */
    --spacing-sm: 8px;
    --spacing-md: 16px;

    /* Transitions */
    --transition: all 0.3s ease;
}
```

**مزایا**:
- تغییر تم به راحتی
- consistency در رنگ‌ها
- کد تمیزتر

### 3D Effects

برای شبیه‌سازی کابینت آرکید:

```css
/* Perspective container */
.control-deck {
    perspective: 500px;
}

/* 3D surface */
.deck-surface {
    transform: rotateX(20deg);
    transform-style: preserve-3d;
}
```

### Neon Glow Effect

```css
.marquee {
    color: var(--neon-green);
    text-shadow:
        0 0 5px #fff,              /* هاله سفید نزدیک */
        0 0 10px var(--neon-green), /* گلو سبز */
        0 0 20px var(--neon-green), /* گلو دورتر */
        0 0 40px var(--neon-green); /* هاله بیرونی */
}
```

### Animations

```css
@keyframes blink {
    0%, 49% { opacity: 1; }
    50%, 100% { opacity: 0; }
}

.menu-overlay {
    animation: blink 1.5s infinite;
}

@keyframes slideIn {
    from { transform: translateY(-100%); }
    to { transform: translateY(0); }
}
```

---

## 🔄 جریان کاربر (User Flow)

### 1. ورود به سایت

```
کاربر → index.html بارگذاری می‌شود
    → Particles.js اجرا می‌شود (پس‌زمینه)
    → script.js کاروسل را می‌سازد
    → auth.js session را بررسی می‌کند
    → اگر logged in:
        └→ نمایش user pill + coins
    → اگر logged out:
        └→ نمایش Login/Register buttons
```

### 2. ثبت‌نام / ورود

```
کلیک Login button
    → نمایش auth overlay
    → کاربر username/password وارد می‌کند
    → submit form
    → auth.js.login() فراخوانی می‌شود
    → fetch به auth_api.cgi
    → CGI بررسی می‌کند users.json
    → اگر موفق:
        └→ session در localStorage ذخیره می‌شود
        └→ UI آپدیت می‌شود
        └→ overlay بسته می‌شود
```

### 3. انتخاب بازی

```
کاربر بازی را انتخاب می‌کند (کلیک یا Enter)
    → loadGame(gameId) فراخوانی می‌شود
    → بررسی login:
        └→ اگر نه → نمایش auth overlay
    → بررسی سکه:
        └→ اگر کافی نیست → alert
    → کسر سکه:
        └→ Auth.subtractCoins(price)
        └→ fetch به auth_api.cgi
    → بارگذاری بازی:
        └→ ساخت iframe
        └→ iframe.src = game.path
        └→ نمایش در game-display
```

### 4. بازی کردن

```
iframe → cgi-bin/game.cgi?params
    → Python server اجرا می‌کند CGI
    → CGI HTML تولید می‌کند
    → HTML در iframe نمایش داده می‌شود
    → کاربر تعامل می‌کند:
        └→ کلیک روی link
        └→ submit form
        └→ URL جدید → CGI دوباره اجرا می‌شود
```

### 5. برد / باخت

```
اگر برد:
    → CGI پاداش سکه را محاسبه می‌کند
    → JavaScript پیام برد را نمایش می‌دهد
    → Auth.addCoins(reward)
    → UI آپدیت می‌شود

اگر باخت:
    → پیام باخت نمایش داده می‌شود
    → بازگشت به منو
```

---

## 🎮 Features

### Keyboard Controls

| کلید | عملکرد |
|------|--------|
| `←` | بازی قبلی |
| `→` | بازی بعدی |
| `Enter` | بارگذاری بازی |
| `Escape` | بستن بازی |

### Responsive Design

```css
/* Desktop (default) */
.arcade-container {
    transform: scale(0.85);
}

/* Tablet */
@media (max-width: 1024px) {
    .arcade-container {
        transform: scale(0.7);
    }
}

/* Mobile */
@media (max-width: 768px) {
    .arcade-container {
        transform: scale(0.5);
    }
    .control-deck {
        display: none; /* جویستیک مخفی */
    }
}
```

### Dark Theme

تمام صفحه با تم تاریک طراحی شده:
- Background: `#0a0a0a` (تقریباً سیاه)
- Text: `#e0e0e0` (خاکستری روشن)
- Accent: `#00ff41` (نئون سبز)

---

## 🛠️ نکات توسعه

### اضافه کردن بازی جدید

1. **به `script.js` اضافه کنید**:
```javascript
const games = [
    // ...
    {
        id: 'new_game',
        title: 'بازی جدید',
        price: 5,
        path: 'cgi-bin/new_game.cgi',
        description: 'توضیح بازی'
    }
];
```

2. **در `index.html` card اضافه کنید**:
```html
<div class="game-item" data-game="new_game">
    <img src="path/to/icon.png" alt="بازی جدید">
    <h3>بازی جدید</h3>
    <span class="game-price">
        <i class="fas fa-coins"></i> 5
    </span>
</div>
```

3. **CGI بازی را بسازید**:
```bash
make new_game.cgi
```

### تغییر تم رنگی

در `style.css`:
```css
:root {
    --neon-green: #ff00ff;  /* بنفش به جای سبز */
    --cabinet-body: #1a1a2e; /* آبی تیره */
}
```

### اضافه کردن Modal جدید

1. **HTML**:
```html
<div id="my-modal" class="modal hidden">
    <div class="modal-content">
        <h2>عنوان</h2>
        <button class="close-btn">✕</button>
    </div>
</div>
```

2. **CSS**:
```css
#my-modal {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: rgba(0,0,0,0.8);
    z-index: 1000;
}

#my-modal.hidden {
    display: none;
}
```

3. **JavaScript**:
```javascript
function showMyModal() {
    document.getElementById('my-modal').classList.remove('hidden');
}

function hideMyModal() {
    document.getElementById('my-modal').classList.add('hidden');
}
```

---

## 🐛 Debugging

### دیباگ JavaScript

```javascript
// Console logging
console.log('Current game index:', currentIndex);

// Breakpoints در browser DevTools (F12)

// Error handling
try {
    loadGame(gameId);
} catch (error) {
    console.error('Error loading game:', error);
    alert('خطا در بارگذاری بازی');
}
```

### دیباگ CSS

```css
/* نمایش border برای دیباگ layout */
* {
    outline: 1px solid red;
}

/* بررسی z-index */
.element {
    background: rgba(255, 0, 0, 0.3);
}
```

### مشکلات رایج

**1. بازی بارگذاری نمی‌شود**
```javascript
// بررسی console
console.log('Game path:', game.path);
console.log('User coins:', user.coins);
```

**2. استایل اعمال نمی‌شود**
```bash
# Cache browser را پاک کنید
Ctrl + Shift + R (hard refresh)

# بررسی کنید CSS لود شده
# در Network tab DevTools
```

**3. Session از دست می‌رود**
```javascript
// بررسی localStorage
console.log(localStorage.getItem('user'));
console.log(localStorage.getItem('sessionToken'));
```

---

## 🔮 بهبودهای آینده

### UI/UX
- [ ] Smooth transitions بین بازی‌ها
- [ ] Loading states برای fetch requests
- [ ] Toast notifications به جای alert
- [ ] Sound effects برای دکمه‌ها
- [ ] Full-screen mode
- [ ] Virtual joystick برای موبایل

### Features
- [ ] Dark/Light theme toggle
- [ ] Custom themes (قرمز، آبی، بنفش)
- [ ] Leaderboard UI
- [ ] Achievement badges
- [ ] Friend list
- [ ] Chat system

### Technical
- [ ] Service Worker (PWA)
- [ ] Offline support
- [ ] WebSocket برای real-time
- [ ] Lazy loading images
- [ ] Code splitting
- [ ] TypeScript migration

---

**Frontend با ❤️ و Vanilla JavaScript ساخته شده است**
