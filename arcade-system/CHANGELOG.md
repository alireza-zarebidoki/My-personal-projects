# 📝 تاریخچه تغییرات (Changelog)

تمام تغییرات مهم این پروژه در این فایل ثبت می‌شود.

فرمت بر اساس [Keep a Changelog](https://keepachangelog.com/fa/1.0.0/) است.

---

## [Unreleased]

### برنامه‌ریزی شده
- افزودن Leaderboard
- افزودن بازی Snake
- افزودن بازی Tetris
- Migration به SQLite
- افزودن Sound Effects

---

## [1.0.0] - 2025-12-26

### 🎉 نسخه اول (Initial Release)

#### افزودن شده (Added)

**بازی‌ها**:
- ✅ حدس عدد (Guess Number) با 3 سطح دشواری
- ✅ دوز محدود (Tic-Tac-Toe Limited) با قانون FIFO
- ✅ سنگ کاغذ قیچی++ (RPS 5-Move) با Lizard و Spock
- ✅ حلقه آویز (Hangman Battle) PvP و Bot
- ✅ فیبوناچی 2048 با قوانین Fibonacci
- ✅ شطرنج تغییریافته با مهره‌های Dragon، Thief، Gryphon
- ✅ چکرز (Checkers) کلاسیک

**Frontend**:
- ✅ UI آرکید کابینت با CSS 3D
- ✅ کاروسل بازی‌ها با انیمیشن
- ✅ تم نئونی سبز با glow effects
- ✅ Particles.js برای پس‌زمینه
- ✅ طراحی Responsive (Desktop و Mobile)
- ✅ فونت فارسی Vazirmatn

**سیستم احراز هویت**:
- ✅ ثبت‌نام و ورود کاربران
- ✅ نقش‌ها: User و Admin
- ✅ Session management (localStorage)
- ✅ پنل ادمین برای مدیریت کاربران

**سیستم اقتصادی**:
- ✅ سیستم سکه (Coin system)
- ✅ قیمت برای هر بازی
- ✅ پاداش برای برد
- ✅ لاگ تراکنش‌های سکه
- ✅ ادمین می‌تواند سکه اضافه/کم کند

**Backend**:
- ✅ CGI با C
- ✅ توابع کمکی مشترک (c_utils.h/c)
- ✅ Stateless architecture
- ✅ JSON files برای data storage

**Build System**:
- ✅ Makefile برای کامپایل خودکار
- ✅ Target های جداگانه برای هر بازی
- ✅ Clean و rebuild

**مستندات**:
- ✅ README.md اصلی (جامع)
- ✅ README.md برای بازی‌ها
- ✅ README.md برای common utilities
- ✅ README.md برای data layer
- ✅ FRONTEND.md برای مستندات frontend
- ✅ CONTRIBUTING.md برای راهنمای مشارکت
- ✅ README.md برای chess
- ✅ README.md برای checkers
- ✅ LICENSE (MIT)
- ✅ کامنت‌های کامل فارسی در تمام فایل‌ها (3000+ خط)

**UI Components**:
- ✅ Auth strip (نوار بالا)
- ✅ User pill (نمایش اطلاعات کاربر)
- ✅ Help modal (راهنما)
- ✅ Account panel (پنل کاربری)
- ✅ Admin panel (پنل مدیریت)
- ✅ Auth overlay (فرم‌های ورود/ثبت‌نام)
- ✅ Game display (نمایش بازی در iframe)
- ✅ Coin HUD (نمایش سکه)

**افکت‌های بصری**:
- ✅ Neon glow با multiple text-shadows
- ✅ 3D perspective برای control deck
- ✅ CRT scanlines effect
- ✅ Reflection effect (شیشه مانیتور)
- ✅ Joystick با radial gradients
- ✅ Arcade buttons با press animation
- ✅ Smooth transitions و animations
- ✅ Particles در پس‌زمینه

**کنترل‌ها**:
- ✅ Keyboard navigation (Arrow keys)
- ✅ Enter برای بارگذاری بازی
- ✅ Escape برای بستن بازی
- ✅ Mouse/Touch support

#### بهبود یافته (Improved)
- بهینه‌سازی CSS با variables
- بهینه‌سازی JavaScript با module pattern
- Error handling در CGI ها
- Input validation

#### امنیت (Security)
- HTML escape برای جلوگیری از XSS
- URL decode صحیح
- Session validation
- ⚠️ **هشدار**: پسوردها plain text هستند (برای production نیست!)

---

## نسخه‌های آینده (Roadmap)

### [1.1.0] - برنامه‌ریزی نشده

#### افزودن شده
- [ ] Leaderboard system
- [ ] Achievement badges
- [ ] Friend list
- [ ] Sound effects
- [ ] بازی Snake
- [ ] بازی Tetris

#### بهبود
- [ ] Migration به SQLite
- [ ] Hash کردن passwords (bcrypt)
- [ ] HTTPS support
- [ ] CSRF protection
- [ ] Rate limiting
- [ ] Caching (Redis)

#### رفع باگ
- [ ] رفع race condition در JSON writes
- [ ] بهبود error handling
- [ ] بهبود validation

---

### [2.0.0] - برنامه‌ریزی نشده (Major Update)

#### افزودن شده
- [ ] WebSocket برای multiplayer real-time
- [ ] PWA (Progressive Web App)
- [ ] Offline support
- [ ] Chat system
- [ ] Tournament mode
- [ ] Video replay system

#### تغییرات شکننده (Breaking Changes)
- [ ] Migration از JSON به PostgreSQL
- [ ] تغییر API structure
- [ ] تغییر session management (JWT)

---

## نکات نسخه‌سازی (Versioning)

این پروژه از [Semantic Versioning](https://semver.org/lang/fa/) استفاده می‌کند:

```
MAJOR.MINOR.PATCH

MAJOR: تغییرات شکننده (breaking changes)
MINOR: فیچرهای جدید (backward compatible)
PATCH: باگ‌فیکس (backward compatible)
```

**مثال**:
- `1.0.0` → `1.0.1`: رفع باگ کوچک
- `1.0.0` → `1.1.0`: افزودن فیچر جدید
- `1.0.0` → `2.0.0`: تغییر معماری (breaking)

---

## دسته‌بندی تغییرات

### افزودن شده (Added)
فیچرهای جدید که به پروژه اضافه شده‌اند.

### تغییر یافته (Changed)
تغییرات در فیچرهای موجود.

### منسوخ شده (Deprecated)
فیچرهایی که به زودی حذف می‌شوند.

### حذف شده (Removed)
فیچرهای حذف شده.

### رفع شده (Fixed)
باگ‌هایی که رفع شده‌اند.

### امنیت (Security)
تغییرات مربوط به امنیت.

---

## یادداشت‌های مهم

### نسخه 1.0.0

**چرا CGI?**
- یادگیری fundamentals
- سبک و سریع برای پروژه کوچک
- بدون وابستگی

**چرا JSON files?**
- Setup ساده
- مناسب برای تعداد کم کاربر
- Portable

**محدودیت‌های فعلی**:
- ⚠️ بدون encryption
- ⚠️ بدون file locking
- ⚠️ بدون transaction
- ⚠️ Plain text passwords

**این محدودیت‌ها در نسخه‌های بعدی رفع خواهند شد.**

---

## سابقه Commits مهم

```bash
# نسخه 1.0.0
commit abc123... (2025-12-26)
    feat: افزودن تمام بازی‌ها

commit def456... (2025-12-25)
    feat: پیاده‌سازی سیستم احراز هویت

commit ghi789... (2025-12-24)
    feat: طراحی UI آرکید کابینت

commit jkl012... (2025-12-23)
    feat: ساخت CGI utils

commit mno345... (2025-12-22)
    init: شروع پروژه
```

---

## مشارکت‌کنندگان (Contributors)

### نسخه 1.0.0
- **علیرضا زارع** (@alirezazare) - توسعه‌دهنده اصلی

**تشکر ویژه**:
- Font Awesome برای آیکون‌ها
- Particles.js برای افکت پس‌زمینه
- Vazirmatn Font برای فونت فارسی

---

## لینک‌ها

- [Repository](https://github.com/username/arcade-system)
- [Issues](https://github.com/username/arcade-system/issues)
- [Pull Requests](https://github.com/username/arcade-system/pulls)

---

**این Changelog به روز نگه داشته می‌شود 📝**
