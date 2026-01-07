# 🎮 Arcade System (سیستم آرکید بازی‌لند)

> **English Summary**: A web-based retro arcade system with 7 games (guess number, tic-tac-toe, rock-paper-scissors-lizard-spock, hangman, fibonacci 2048, chess, checkers), built with C CGI backend, vanilla JavaScript frontend, and a coin-based authentication system. Features a nostalgic neon green theme simulating a classic arcade cabinet.

🌐 **Live Demo**: [https://baziland.zarebidoki.com/](https://baziland.zarebidoki.com/)

**Current Version:** 1.2.0 — Chess rule enforcement fixes (Rook و Gryphon) و بهبود پایداری اعتبارسنجی/هایلایت حرکت.

---

## 📋 فهرست مطالب

- [معرفی پروژه](#معرفی-پروژه)
- [ویژگی‌های اصلی](#ویژگیهای-اصلی)
- [ساختار پروژه](#ساختار-پروژه)
- [معماری و جریان کار](#معماری-و-جریان-کار)
- [بازی‌ها](#بازیها)
- [نصب و راه‌اندازی](#نصب-و-راهاندازی)
- [تصمیمات طراحی](#تصمیمات-طراحی)
- [بهبودهای آینده](#بهبودهای-آینده)
- [نکات مهم](#نکات-مهم)

---

## 🎯 معرفی پروژه

### این پروژه دقیقاً چیست؟

**بازی‌لند (BAZI LAND)** یک سیستم آرکید وب-based است که من از صفر طراحی و پیاده‌سازی کردم. هدفم این بوده که تجربه یک کابینت آرکید کلاسیک دهه 80 و 90 را در مرورگر شبیه‌سازی کنم - با تمام جزئیات از جمله:

- 🟢 تم نئونی سبز درخشان
- 🎨 افکت‌های 3D برای کابینت
- 🎮 رابط کاربری با دکمه‌های آرکیدی
- 🪙 سیستم سکه (coin-operated) مثل آرکیدهای واقعی
- 👤 سیستم احراز هویت و مدیریت کاربران
- 📊 سیستم امتیازدهی و پاداش

### چرا ساخته شده؟

از آنجایی که همیشه علاقه‌مند به بازی‌های کلاسیک بودم و می‌خواستم:
1. **تجربه شخصی**: حس nostalgic آرکیدهای قدیمی را زنده کنم
2. **یادگیری تکنیکال**: تجربه کار با CGI، C، و طراحی سیستم stateless
3. **چالش خلاقیت**: ترکیب backend سبک (C) با frontend مدرن (Vanilla JS)
4. **آموزش**: یک پروژه عملی برای یادگیری معماری وب

### چه مشکلی را حل می‌کند؟

- **نیاز به سرگرمی ساده**: بازی‌های سبک و سریع بدون نیاز به نصب
- **تجربه کاربری یکپارچه**: همه بازی‌ها در یک محیط واحد
- **مدیریت دسترسی**: سیستم سکه برای کنترل و gamification
- **یادگیری**: نمونه‌ای عملی از معماری CGI و طراحی responsive

---

## ✨ ویژگی‌های اصلی

### 🎮 بازی‌ها (7 بازی)
1. **حدس عدد** - با 3 سطح دشواری (آسان، متوسط، سخت)
2. **دوز محدود** - Tic-Tac-Toe با محدودیت 3 مهره FIFO
3. **سنگ کاغذ قیچی++ (5 حرکت)** - نسخه Big Bang Theory با Lizard و Spock
4. **حلقه آویز** - Hangman Battle با حالت PvP و Bot
5. **فیبوناچی 2048** - 2048 با قوانین دنباله فیبوناچی
6. **شطرنج تغییریافته** - با مهره‌های جدید (Dragon، Thief، Gryphon)
7. **چکرز** - Checkers کلاسیک

### 🔐 سیستم احراز هویت
- ثبت‌نام و ورود کاربران
- نقش‌ها: User و Admin
- مدیریت session سمت کلاینت
- پنل ادمین برای مدیریت کاربران

### 🪙 سیستم اقتصادی
- هر بازی قیمت دارد (به سکه)
- پاداش برای برد
- ادمین می‌تواند سکه اضافه کند
- لاگ تراکنش‌ها

### 🎨 طراحی UI/UX
- تم نئونی سبز با افکت glow
- شبیه‌سازی کابینت آرکید با CSS 3D
- کاروسل بازی‌ها با انیمیشن smooth
- طراحی responsive برای دسکتاپ و موبایل
- فونت فارسی Vazirmatn

### 🛠️ معماری تکنیکال
- **Backend**: C CGI (stateless، سریع، سبک)
- **Frontend**: Vanilla JavaScript (بدون framework)
- **استایل**: CSS خام با variables و animations
- **سرور**: Python HTTP server با CGI handler
- **دیتابیس**: JSON files (ساده، بدون نیاز به DB server)

---

## 📁 ساختار پروژه

```
test-project/
├── index.html              # صفحه اصلی - کابینت آرکید
├── style.css               # استایل‌های کامل (1128 خط)
├── script.js               # منطق اصلی frontend (1353 خط)
├── auth.js                 # ماژول احراز هویت (328 خط)
├── Makefile                # سیستم build برای کامپایل بازی‌ها
│
├── src/
│   ├── common/
│   │   ├── c_utils.h       # توابع کمکی CGI (header)
│   │   └── c_utils.c       # پیاده‌سازی توابع کمکی
│   │
│   └── games/              # بازی‌های CGI
│       ├── guess_number.c
│       ├── tictactoe_limited.c
│       ├── rps_5move.c
│       ├── hangman_battle.c
│       ├── fibonacci_2048.c
│       ├── chess/          # شطرنج (4 فایل)
│       │   ├── chess.h
│       │   ├── main.c
│       │   ├── logic.c
│       │   └── state.c
│       └── checkers/       # چکرز (4 فایل)
│           ├── checkers.h
│           ├── main.c
│           ├── logic.c
│           └── state.c
│
├── cgi-bin/                # فایل‌های CGI کامپایل شده (.cgi)
│   ├── guess_number.cgi
│   ├── tictactoe_limited.cgi
│   ├── rps_5move.cgi
│   ├── hangman_battle.cgi
│   ├── fibonacci_2048.cgi
│   ├── chess_modified.cgi
│   └── checkers.cgi
│
└── data/                   # فایل‌های JSON
    ├── users.json          # اطلاعات کاربران
    ├── sessions.json       # session های فعال
    ├── admin.json          # تنظیمات ادمین
    └── admin_logs.json     # لاگ تراکنش‌های سکه
```

### نقش هر بخش

#### **Frontend (HTML/CSS/JS)**
- `index.html`: ساختار HTML کابینت آرکید، راهنما، modal های احراز هویت و ادمین
- `style.css`: تمام استایل‌ها شامل افکت‌های 3D، انیمیشن‌ها، تم نئونی
- `script.js`: مدیریت کاروسل، رویدادها، ارتباط با بازی‌ها، کیبورد
- `auth.js`: ماژول IIFE برای احراز هویت، session، API wrapper

#### **Backend (C CGI)**
- `src/common/c_utils.*`: توابع مشترک (print_header، get_param، url_decode، etc.)
- `src/games/*.c`: منطق هر بازی به صورت CGI مستقل
- `cgi-bin/*.cgi`: executable های کامپایل شده

#### **Data Layer**
- فایل‌های JSON برای ذخیره state (بدون نیاز به MySQL یا PostgreSQL)

---

## 🔄 معماری و جریان کار

### جریان کلی

```
کاربر → مرورگر (index.html)
    → کاروسل بازی‌ها
    → انتخاب بازی
    → بررسی احراز هویت (auth.js)
    → بررسی سکه کافی
    → کسر سکه
    → بارگذاری iframe با URL: cgi-bin/game.cgi
    → سرور Python اجرا می‌کند: game.cgi
    → CGI (C) HTML تولید می‌کند
    → مرورگر HTML را در iframe نمایش می‌دهد
    → کاربر بازی می‌کند
    → اگر برد، پاداش سکه
```

### چگونگی کار بخش‌ها با هم

1. **Frontend → Backend**:
   - کاربر بازی را انتخاب می‌کند
   - JavaScript احراز هویت و سکه را چک می‌کند
   - iframe با URL مربوط به CGI ساخته می‌شود
   - درخواست HTTP به سرور می‌رود

2. **Backend (CGI)**:
   - Python HTTP server درخواست را به CGI می‌دهد
   - CGI اجرا می‌شود و HTML تولید می‌کند
   - خروجی به مرورگر برگردانده می‌شود

3. **State Management**:
   - **Frontend**: Session در localStorage
   - **Backend**: Stateless CGI - تمام state در URL parameters

4. **Authentication Flow**:
   ```
   Login → auth_api.cgi → users.json → session → localStorage
   Game Request → Check Session → Check Coins → Subtract → Play
   Win → Reward → Update Balance
   ```

### چرا CGI؟

من می‌توانستم از Node.js، PHP، یا Python استفاده کنم، اما CGI را انتخاب کردم چون:

✅ **مزایا**:
- سبک و سریع (بدون overhead framework)
- هر بازی یک process مستقل است
- یادگیری عمیق‌تر از HTTP و web fundamentals
- کامپایل شده (C) = performance بالا
- بدون وابستگی به runtime های سنگین

❌ **معایب**:
- هر request یک process جدید (overhead)
- مدیریت state سخت‌تر است
- امنیت نیاز به دقت بیشتری دارد
- debug کردن سخت‌تر از زبان‌های interpreted

---

## 🎮 بازی‌ها

### 1. حدس عدد (Guess Number)
- **قیمت**: 3 سکه
- **توضیح**: عدد تصادفی بین 1 تا 100 را حدس بزن
- **سطوح**:
  - آسان: 10 تلاش + راهنمایی دقیق
  - متوسط: 7 تلاش + راهنمایی کلی
  - سخت: 5 تلاش + راهنمایی مبهم
- **فایل**: `src/games/guess_number.c`

### 2. دوز محدود (Tic-Tac-Toe Limited)
- **قیمت**: 5 سکه
- **توضیح**: دوز کلاسیک اما با محدودیت 3 مهره
- **قانون خاص**: وقتی مهره چهارم گذاشته می‌شود، قدیمی‌ترین مهره حذف می‌شود (FIFO)
- **حالت**: 2 نفره (PvP)
- **فایل**: `src/games/tictactoe_limited.c`

### 3. سنگ کاغذ قیچی لیزارد اسپاک (RPS 5-Move)
- **قیمت**: 4 سکه
- **توضیح**: نسخه 5 حرکتی از سریال Big Bang Theory
- **حرکات**: Rock، Paper، Scissors، Lizard، Spock
- **حالت‌ها**: PvP و vs Bot (با هوش مصنوعی counter-strategy)
- **امتیازدهی**: بهترین از 5
- **فایل**: `src/games/rps_5move.c`

### 4. حلقه آویز (Hangman Battle)
- **قیمت**: 6 سکه
- **توضیح**: دو نفر هم‌زمان حلقه آویز بازی می‌کنند
- **حالت‌ها**: PvP و vs Bot
- **Bot AI**: بر اساس فرکانس حروف انگلیسی (ETAOINSHRDLCUMWFGYPBVKJXQZ)
- **فایل**: `src/games/hangman_battle.c`

### 5. فیبوناچی 2048
- **قیمت**: 7 سکه
- **توضیح**: 2048 اما با قوانین دنباله فیبوناچی
- **قانون**: فقط اعداد مجاور در دنباله فیبوناچی ادغام می‌شوند
- **اندازه‌ها**: 4×4، 5×5، 6×6
- **فایل**: `src/games/fibonacci_2048.c`

### 6. شطرنج تغییریافته (Chess Modified)
- **قیمت**: 10 سکه
- **توضیح**: شطرنج با مهره‌های جدید
- **مهره‌های جدید**:
  - 🐉 **Dragon** (جایگزین Knight): ترکیب اسب + شاه
  - 🥷 **Thief** (جایگزین Bishop): فیل با قابلیت پرش از 1 مانع
  - 🦅 **Gryphon** (جایگزین Rook): 1 خانه مورب + حرکت رخ
- **تایمر**: هر بازیکن 10 دقیقه
- **فایل‌ها**: `src/games/chess/`

### 7. چکرز (Checkers)
- **قیمت**: 8 سکه
- **توضیح**: Checkers کلاسیک
- **قوانین**: پرش اجباری، King شدن، پرش زنجیره‌ای
- **فایل‌ها**: `src/games/checkers/`

---

## 🚀 نصب و راه‌اندازی

### پیش‌نیازها

```bash
# GCC برای کامپایل C
gcc --version

# Python 3 برای سرور HTTP
python3 --version

# Make برای build system
make --version
```

### مراحل نصب

#### 1. دانلود پروژه

```bash
git clone <repository-url>
cd test-project
```

#### 2. کامپایل بازی‌ها

```bash
# کامپایل تمام بازی‌ها
make all

# یا کامپایل یک بازی خاص
make guess_number.cgi
make tictactoe_limited.cgi
make rps_5move.cgi
make hangman_battle.cgi
make fibonacci_2048.cgi
make chess_modified.cgi
make checkers.cgi

# پاک کردن فایل‌های کامپایل شده
make clean
```

#### 3. اجازه اجرا به CGI ها

```bash
chmod +x cgi-bin/*.cgi
```

#### 4. راه‌اندازی سرور

```bash
python3 -m http.server --cgi 8080
```

#### 5. باز کردن در مرورگر

```
http://localhost:8080
```

### دستورات مفید

```bash
# بررسی وضعیت CGI ها
ls -lh cgi-bin/

# تست یک CGI به صورت مستقیم
./cgi-bin/guess_number.cgi

# مشاهده لاگ‌های سرور
# در همان ترمینالی که سرور اجرا شده

# rebuild تمام پروژه
make clean && make all
```

---

## 🧠 تصمیمات طراحی

### چرا C برای Backend؟

1. **Performance**: کامپایل شده = سریع
2. **یادگیری**: تجربه کار با memory management و pointers
3. **Challenge**: سخت‌تر از PHP یا Node.js
4. **Lightweight**: بدون نیاز به runtime سنگین

### چرا CGI به جای FastCGI یا modern backend?

- **سادگی**: نیاز به setup کمتری دارد
- **آموزشی**: درک بهتر از HTTP protocol
- **مستقل بودن**: هر بازی یک process مجزا
- **کافی برای این پروژه**: traffic بالا نداریم

### چرا Vanilla JavaScript؟

- **یادگیری**: درک DOM و events بدون abstraction
- **سبک**: بدون overhead React یا Vue
- **کنترل کامل**: هر خط کد را خودم نوشتم
- **Performance**: سریع‌تر برای پروژه کوچک

### چرا JSON files به جای Database?

✅ **مزایا**:
- Setup ساده (بدون نیاز به MySQL، PostgreSQL)
- خواندن/نوشتن آسان از C
- Portable (فقط یک پوشه data/)
- مناسب برای تعداد کم کاربر

❌ **محدودیت‌ها**:
- مقیاس‌پذیری کم (برای 1000+ کاربر مناسب نیست)
- بدون transaction (race condition در write های همزمان)
- بدون indexing (جستجو O(n))

### چرا همه بازی‌ها stateless؟

- **سادگی**: نیاز به session management نداریم
- **مقیاس‌پذیری**: می‌توان multiple server داشت
- **قابلیت share**: URL را می‌توان کپی کرد
- **CGI-friendly**: با ماهیت CGI سازگار است

---

## 🔮 بهبودهای آینده

### فیچرهای جدید

- [ ] **Leaderboard**: جدول امتیازات برتر
- [ ] **More Games**: بازی‌های جدید (Snake، Tetris، Pong)
- [ ] **Multiplayer Real-time**: با WebSocket
- [ ] **Sound Effects**: افکت‌های صوتی آرکیدی
- [ ] **Achievements**: سیستم دستاوردها و بج‌ها
- [ ] **Tournament Mode**: مسابقات زمان‌دار
- [ ] **Friend System**: افزودن دوستان و چالش

### بهبودهای تکنیکال

- [ ] **Database Migration**: جایگزینی JSON با SQLite یا PostgreSQL
- [ ] **API RESTful**: طراحی API استاندارد
- [ ] **WebSocket**: برای multiplayer real-time
- [ ] **Docker**: containerization
- [ ] **CI/CD**: اتوماسیون build و deploy
- [ ] **Testing**: unit tests برای C و JavaScript
- [ ] **Security**: HTTPS، CSRF protection، rate limiting

### بهبودهای UI/UX

- [ ] **Mobile First**: بهینه‌سازی بیشتر برای موبایل
- [ ] **Accessibility**: ARIA labels، keyboard navigation بهتر
- [ ] **Dark/Light Mode**: تم تاریک و روشن
- [ ] **Themes**: تم‌های رنگی مختلف
- [ ] **Animations**: انیمیشن‌های smooth تر
- [ ] **Tutorial**: راهنمای تعاملی برای کاربران جدید

### بهبودهای معماری

- [ ] **Microservices**: جدا کردن auth از games
- [ ] **Caching**: Redis برای session و leaderboard
- [ ] **Load Balancer**: برای مقیاس‌پذیری
- [ ] **Monitoring**: Prometheus + Grafana
- [ ] **Logging**: centralized logging با ELK stack

---

## 📝 نکات مهم

### امنیت

⚠️ **هشدار**: این پروژه برای محیط production آماده نیست!

**مشکلات امنیتی فعلی**:

1. **No HTTPS**: تمام ترافیک plain text است
2. **No CSRF Protection**: حملات CSRF ممکن است
3. **No Rate Limiting**: می‌توان spam کرد
4. **Weak Session**: session در localStorage (نه httpOnly cookie)
5. **No Input Validation**: validation کافی در CGI ها نیست
6. **JSON File Locks**: race condition در write های همزمان

**برای production باید**:
- HTTPS فعال شود
- JWT یا OAuth استفاده شود
- Input validation کامل اضافه شود
- Rate limiting پیاده شود
- Database واقعی به کار رود

### Performance

**نکات**:
- CGI برای traffic پایین مناسب است (< 100 concurrent users)
- برای traffic بالا باید به FastCGI یا REST API مهاجرت کرد
- JSON files برای < 100 کاربر کافی است
- بدون caching، هر request به disk می‌رود

### مرورگرها

**تست شده روی**:
- ✅ Chrome 120+ (Desktop & Mobile)
- ✅ Firefox 120+ (Desktop & Mobile)
- ✅ Edge 120+
- ⚠️ Safari (بعضی افکت‌های CSSممکن است متفاوت باشد)
- ❌ IE (پشتیبانی نمی‌شود)

### محدودیت‌ها

1. **Stateless CGI**: نمی‌توان WebSocket داشت
2. **JSON Storage**: مقیاس‌پذیری محدود
3. **No Transaction**: race condition در concurrent writes
4. **Single Server**: نمی‌توان load balance کرد
5. **Python CGI Handler**: deprecated در Python 3.15+

### دیباگ

```bash
# مشاهده خروجی CGI
./cgi-bin/game.cgi

# بررسی syntax error در C
gcc -Wall -Wextra src/games/game.c

# مشاهده لاگ سرور
# در ترمینال python server

# بررسی JSON files
cat data/users.json | python3 -m json.tool
```

---

## 👨‍💻 توسعه‌دهنده

این پروژه را من (**علیرضا زارع بیدکی**) به صورت شخصی توسعه دادم.

**اهداف یادگیری**:
- کار با C و CGI
- طراحی stateless architecture
- ساخت UI/UX بدون framework
- مدیریت state در frontend
- طراحی سیستم authentication

**تماس**:
- GitHub: alireza-zarebidoki
- Email: alireza@zarebidoki.com

---

## 📄 License

این پروژه آزاد است و می‌توانید از آن استفاده کنید. (اگر license خاصی دارید اینجا قرار دهید)

---

## 🙏 تشکر و قدردانی

در مسیر طراحی و پیاده‌سازی این پروژه، از راهنمایی‌ها و حمایت‌های ارزشمند عزیزان زیر صمیمانه سپاسگزارم:

### 🎓 استاد راهنما: جناب آقای دکتر محمد امین طوسی
که با دانش عمیق و راهنمایی‌های دلسوزانه‌شان، مسیر دشوار پیاده‌سازی معماری CGI را برایم روشن ساختند و با بازخوردهای دقیق، کیفیت نهایی پروژه را ارتقا دادند.

### 👥 دستیاران آموزشی و منتورها
سپاس ویژه از تیم محترم دستیاری آموزشی که در رفع چالش‌های فنی و گره‌های برنامه‎نویسی، با صبوری و دانش فنی خود همراهم بودند.

### 🔍 تیم تست و ارزیابی
قدردانی از دوستان و هم‌کلاسی‌های عزیزی که در فرایند دیباگ، طراحی رابط کاربری و آزمون‌های نهایی، با نگاه دقیق و نقادانه خود به بهبود تجربه کاربری کمک کردند.

### 🛠 منابع و ابزارهای متن‌باز
توسعه این سیستم بدون بهره‌گیری از جامعه متن‌باز (Open Source) ممکن نبود. در این پروژه از ابزارهای زیر استفاده شده است:

- **Font Awesome:** جهت طراحی آیکون‌ها و المان‌های بصری.
- **Particles.js:** جهت ایجاد افکت‌های تعاملی در پس‌زمینه.
- **Vazirmatn Font:** جهت تایپوگرافی استاندارد و خوانای فارسی.
- **Python HTTP Server:** جهت مدیریت درخواست‌های CGI در محیط توسعه.

---

<div align="center">

**ساخته شده با ❤️ و ☕**

</div>
