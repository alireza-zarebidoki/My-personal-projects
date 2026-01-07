# 🤝 راهنمای مشارکت (Contributing Guide)

ممنون که می‌خواهید در پروژه **بازی‌لند (BAZI LAND)** مشارکت کنید! این راهنما به شما کمک می‌کند تا به بهترین شکل در پروژه همکاری کنید.

🌐 **نسخه زنده**: [https://baziland.zarebidoki.com/](https://baziland.zarebidoki.com/)

---

## 📋 فهرست مطالب

- [نحوه مشارکت](#نحوه-مشارکت)
- [محیط توسعه](#محیط-توسعه)
- [ساختار کد](#ساختار-کد)
- [استانداردهای کد](#استانداردهای-کد)
- [فرآیند Pull Request](#فرآیند-pull-request)
- [گزارش باگ](#گزارش-باگ)
- [پیشنهاد فیچر](#پیشنهاد-فیچر)

---

## 🎯 نحوه مشارکت

### راه‌های مشارکت

شما می‌توانید به روش‌های زیر مشارکت کنید:

1. **افزودن بازی جدید**: بازی CGI جدید بسازید
2. **بهبود بازی موجود**: باگ‌فیکس یا فیچر جدید
3. **بهبود UI/UX**: طراحی بهتر، انیمیشن، responsive
4. **بهبود Backend**: بهینه‌سازی C code، امنیت
5. **مستندات**: بهبود README ها، کامنت‌ها
6. **تست**: نوشتن test case ها
7. **گزارش باگ**: پیدا کردن و گزارش مشکلات

---

## 🛠️ محیط توسعه

### پیش‌نیازها

```bash
# نصب ابزارهای لازم
sudo apt-get install gcc make python3

# بررسی نسخه‌ها
gcc --version      # >= 9.0
make --version     # >= 4.0
python3 --version  # >= 3.8
```

### Setup محیط

```bash
# 1. Fork کردن repository
# در GitHub روی Fork کلیک کنید

# 2. Clone کردن
git clone https://github.com/YOUR_USERNAME/arcade-system.git
cd arcade-system

# 3. اضافه کردن upstream
git remote add upstream https://github.com/ORIGINAL_OWNER/arcade-system.git

# 4. کامپایل پروژه
make all

# 5. اجرای سرور
python3 -m http.server --cgi 8080

# 6. باز کردن در مرورگر
# http://localhost:8080
```

### ساختار Branch ها

```bash
# Branch اصلی: main
# Branch توسعه: develop

# ساخت branch جدید برای فیچر
git checkout -b feature/new-game-snake

# ساخت branch برای باگ‌فیکس
git checkout -b bugfix/carousel-navigation

# ساخت branch برای بهبود
git checkout -b improvement/css-performance
```

---

## 📁 ساختار کد

### اضافه کردن بازی جدید

#### مرحله 1: ساخت فایل C

```bash
# در src/games/
touch src/games/snake.c
```

**snake.c**:
```c
#include "../common/c_utils.h"

int main() {
    print_header();

    char *mode = get_param("mode");

    printf("<!DOCTYPE html>\n");
    printf("<html dir='rtl'>\n");
    printf("<head>\n");
    printf("<meta charset='UTF-8'>\n");
    printf("<title>بازی مار (Snake)</title>\n");
    printf("<style>\n");
    // استایل‌های بازی
    printf("</style>\n");
    printf("</head>\n");
    printf("<body>\n");

    if (!mode || strcmp(mode, "init") == 0) {
        // صفحه شروع بازی
        printf("<h1>بازی مار</h1>\n");
        printf("<a href='?mode=start'>شروع بازی</a>\n");
    }
    else if (strcmp(mode, "start") == 0) {
        // منطق بازی
        printf("<canvas id='game'></canvas>\n");
        printf("<script>\n");
        // JavaScript بازی
        printf("</script>\n");
    }

    printf("</body>\n");
    printf("</html>\n");

    return 0;
}
```

#### مرحله 2: افزودن به Makefile

```makefile
# در Makefile
snake.cgi: src/games/snake.c src/common/c_utils.c
	$(CC) $(CFLAGS) $^ -o $(CGI_DIR)/$@ $(INCLUDES)

# در target all
all: ... snake.cgi
```

#### مرحله 3: افزودن به Frontend

**در script.js**:
```javascript
const games = [
    // ...
    {
        id: 'snake',
        title: 'مار (Snake)',
        price: 6,
        path: 'cgi-bin/snake.cgi',
        description: 'بازی کلاسیک مار با قوانین آرکیدی'
    }
];
```

**در index.html**:
```html
<div class="game-item" data-game="snake">
    <img src="icons/snake.png" alt="بازی مار">
    <h3>مار</h3>
    <span class="game-price">
        <i class="fas fa-coins"></i> 6
    </span>
</div>
```

#### مرحله 4: کامپایل و تست

```bash
# کامپایل
make snake.cgi

# اجازه اجرا
chmod +x cgi-bin/snake.cgi

# تست مستقیم
./cgi-bin/snake.cgi

# تست در مرورگر
# http://localhost:8080/cgi-bin/snake.cgi
```

---

## 📏 استانداردهای کد

### C Code Style

```c
// ✅ درست
int calculate_score(int moves, int time) {
    if (moves < 0 || time < 0) {
        return 0;
    }

    int base_score = 100;
    int time_bonus = (60 - time) * 2;

    return base_score + time_bonus;
}

// ❌ اشتباه
int calc(int m,int t){if(m<0||t<0)return 0;int b=100;return b+(60-t)*2;}
```

**قواعد**:
- ✅ نام‌گذاری واضح (snake_case)
- ✅ فاصله‌گذاری مناسب
- ✅ کامنت فارسی برای توضیح منطق
- ✅ Error handling
- ✅ حداکثر 80 کاراکتر در هر خط

### JavaScript Code Style

```javascript
// ✅ درست
function loadGame(gameId) {
    const game = games.find(g => g.id === gameId);

    if (!game) {
        console.error('بازی پیدا نشد:', gameId);
        return;
    }

    // بررسی احراز هویت
    if (!Auth.isLoggedIn()) {
        showAuthOverlay();
        return;
    }

    // بارگذاری بازی
    createGameIframe(game);
}

// ❌ اشتباه
function loadGame(id){var g=games.find(x=>x.id==id);if(!g)return;if(!Auth.isLoggedIn())showAuthOverlay();else createGameIframe(g);}
```

**قواعد**:
- ✅ camelCase برای نام‌گذاری
- ✅ const/let به جای var
- ✅ Arrow functions جایی که مناسب است
- ✅ کامنت فارسی
- ✅ semicolon در پایان خطوط

### CSS Code Style

```css
/* ✅ درست */
.game-item {
    width: 120px;
    height: 120px;
    background: var(--cabinet-body);
    border-radius: 10px;
    transition: var(--transition);
}

.game-item:hover {
    transform: scale(1.1);
    box-shadow: var(--glow);
}

/* ❌ اشتباه */
.game-item{width:120px;height:120px;background:var(--cabinet-body);border-radius:10px;transition:var(--transition);}
.game-item:hover{transform:scale(1.1);box-shadow:var(--glow);}
```

**قواعد**:
- ✅ یک property در هر خط
- ✅ فاصله بعد از `:`
- ✅ استفاده از CSS Variables
- ✅ Mobile-first approach
- ✅ کامنت فارسی برای توضیح

### کامنت‌ها

**همه کامنت‌ها باید به فارسی باشند**:

```c
// ✅ درست
// این تابع امتیاز بازیکن را بر اساس تعداد حرکات و زمان محاسبه می‌کند
// هر چه سریع‌تر تمام کند، امتیاز بیشتری می‌گیرد
int calculate_score(int moves, int time) {
    // ...
}

// ❌ اشتباه
// This function calculates player score based on moves and time
int calculate_score(int moves, int time) {
    // ...
}
```

---

## 🔄 فرآیند Pull Request

### مراحل

#### 1. Sync کردن با upstream

```bash
git fetch upstream
git checkout main
git merge upstream/main
```

#### 2. ساخت branch جدید

```bash
git checkout -b feature/my-new-feature
```

#### 3. توسعه

```bash
# کد بزنید، تست کنید
make all
python3 -m http.server --cgi 8080

# commit های کوچک و منظم
git add src/games/new_game.c
git commit -m "افزودن بازی جدید: Snake

- پیاده‌سازی منطق بازی
- افزودن به کاروسل
- تست شده روی Chrome و Firefox
"
```

#### 4. Push کردن

```bash
git push origin feature/my-new-feature
```

#### 5. ساخت Pull Request

در GitHub:
1. روی "Compare & pull request" کلیک کنید
2. عنوان واضح بنویسید (فارسی یا انگلیسی)
3. توضیحات کامل بدهید
4. Screenshot اضافه کنید (اگر تغییر UI است)
5. "Create pull request" کلیک کنید

### Template Pull Request

```markdown
## توضیحات
این PR بازی جدید Snake را اضافه می‌کند.

## تغییرات
- ✅ فایل `src/games/snake.c` اضافه شد
- ✅ به Makefile اضافه شد
- ✅ به `script.js` اضافه شد
- ✅ به `index.html` اضافه شد
- ✅ تست شد

## Screenshots
![Snake Game](screenshots/snake.png)

## Checklist
- [x] کد کامپایل می‌شود بدون warning
- [x] بازی در مرورگر کار می‌کند
- [x] کامنت‌ها به فارسی هستند
- [x] استایل‌ها responsive هستند
- [x] بدون باگ مشخص

## مسائل مرتبط
Closes #42
```

---

## 🐛 گزارش باگ

### قبل از گزارش

1. **جستجو کنید**: شاید قبلاً گزارش شده
2. **تست کنید**: روی مرورگرهای مختلف
3. **کنسول را بررسی کنید**: خطاهای JavaScript
4. **لاگ سرور را ببینید**: خطاهای CGI

### Template Issue

```markdown
## توضیحات باگ
بازی دوز محدود وقتی بازیکن برنده می‌شود، سکه اضافه نمی‌شود.

## مراحل بازسازی
1. وارد شوید
2. بازی دوز محدود را انتخاب کنید
3. بازی کنید و برنده شوید
4. بررسی کنید موجودی سکه

## رفتار مورد انتظار
بعد از برد، 8 سکه باید اضافه شود.

## رفتار واقعی
سکه اضافه نمی‌شود.

## Screenshots
![Bug Screenshot](screenshots/bug.png)

## محیط
- OS: Ubuntu 22.04
- Browser: Chrome 120
- Python: 3.10
- GCC: 11.0

## لاگ‌های مرتبط
```
127.0.0.1 - - [26/Dec/2025 15:00:00] "GET /cgi-bin/tictactoe_limited.cgi?... HTTP/1.1" 200 -
```

## اطلاعات اضافی
کنسول مرورگر error نشان نمی‌دهد.
```

---

## 💡 پیشنهاد فیچر

### Template

```markdown
## توضیحات فیچر
اضافه کردن Leaderboard برای نمایش بهترین بازیکنان.

## انگیزه
کاربران می‌خواهند ببینند در رتبه‌بندی کجا هستند و این باعث رقابت می‌شود.

## راه‌حل پیشنهادی
1. جدول `scores` در database
2. API endpoint برای دریافت top 10
3. UI برای نمایش leaderboard در صفحه اصلی

## جایگزین‌ها
- نمایش فقط در پنل کاربری
- نمایش per-game leaderboard

## مثال UI
[Mockup یا sketch]

## پیاده‌سازی
آیا می‌خواهید این را خودتان پیاده کنید؟
- [ ] بله، می‌خواهم PR بدهم
- [x] نه، فقط پیشنهاد است
```

---

## ✅ Checklist قبل از PR

### کد

- [ ] کد کامپایل می‌شود بدون error و warning
- [ ] تست شده در مرورگرهای مختلف (Chrome, Firefox, Edge)
- [ ] تست شده در Desktop و Mobile
- [ ] بدون console error
- [ ] کامنت‌ها به فارسی
- [ ] استانداردهای کد رعایت شده

### مستندات

- [ ] README آپدیت شده (اگر لازم است)
- [ ] کامنت‌های کافی در کد
- [ ] CHANGELOG آپدیت شده

### تست

- [ ] بازی بدون باگ کار می‌کند
- [ ] احراز هویت درست کار می‌کند
- [ ] سیستم سکه صحیح عمل می‌کند
- [ ] UI responsive است

### Git

- [ ] Commit message های واضح
- [ ] Branch از main ساخته شده
- [ ] Sync با upstream
- [ ] بدون merge conflict

---

## 🎓 منابع یادگیری

### C Programming
- [C Programming Tutorial](https://www.learn-c.org/)
- [CGI Programming](https://www.tutorialspoint.com/cgidoc/index.htm)

### Web Development
- [MDN Web Docs](https://developer.mozilla.org/)
- [JavaScript.info](https://javascript.info/)
- [CSS Tricks](https://css-tricks.com/)

### Git & GitHub
- [Git Tutorial](https://git-scm.com/docs/gittutorial)
- [GitHub Guides](https://guides.github.com/)

---

## 🤔 سوالات متداول

### چطور بازی جدید اضافه کنم؟
[نحوه مشارکت](#ساختار-کد) را ببینید.

### کد من کامپایل نمی‌شود؟
```bash
# بررسی syntax
gcc -Wall -Wextra -fsyntax-only src/games/game.c

# بررسی کنید c_utils.h را include کرده‌اید
#include "../common/c_utils.h"
```

### CGI من 500 error می‌دهد؟
```bash
# بررسی لاگ سرور
# معمولاً مشکل از:
# 1. بدون print_header()
# 2. بدون executable permission (chmod +x)
# 3. syntax error در HTML خروجی
```

### PR من reject شد؟
- کد را refactor کنید
- کامنت‌های reviewer را بخوانید
- تغییرات را اعمال کنید
- دوباره push کنید (همان branch)

---

## 📞 تماس با ما

- **GitHub Issues**: برای باگ و فیچر
- **Discussions**: برای سوالات عمومی
- **Email**: alireza@zarebidoki.com

---

**با تشکر از مشارکت شما! 🙏**
