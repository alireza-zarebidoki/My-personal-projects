# ♟️ چکرز (Checkers / Draughts)

بازی کلاسیک چکرز با قوانین استاندارد و UI زیبا.

---

## 📋 فایل‌ها

```
checkers/
├── checkers.h   # Header: struct ها، enum ها، function prototypes
├── main.c       # CGI handler، HTML generation
├── logic.c      # منطق بازی: move validation، jump detection
└── state.c      # State management: URL encoding/decoding
```

---

## 🎮 قوانین بازی

### صفحه

```
8x8 board با خانه‌های سیاه و سفید
فقط خانه‌های تیره استفاده می‌شوند

  a b c d e f g h
8 . ■ . ■ . ■ . ■  ■ = خانه قابل بازی
7 ■ . ■ . ■ . ■ .  . = خانه غیر قابل بازی
6 . ■ . ■ . ■ . ■
5 ■ . ■ . ■ . ■ .
4 . ■ . ■ . ■ . ■
3 ■ . ■ . ■ . ■ .
2 . ■ . ■ . ■ . ■
1 ■ . ■ . ■ . ■ .
```

### مهره‌ها

**مهره معمولی** (Man):
- حرکت: یک خانه مورب به جلو
- رنگ سفید: از پایین به بالا
- رنگ سیاه: از بالا به پایین

**مهره شاه** (King):
- حرکت: یک خانه مورب به جلو یا عقب
- نماد: تاج روی مهره (👑)
- دریافت: وقتی مهره به آخرین ردیف برسد

---

## 📏 قوانین حرکت

### 1. حرکت معمولی

```
مهره سفید در c3:

  . ■ . ■
  ■ . ■ .
  . W . ■     W = مهره سفید
  ■ . ■ .     X = حرکت ممکن

می‌تواند به: b4 یا d4
```

### 2. پرش (Jump)

اگر مهره حریف در مورب باشد و خانه بعد خالی:

```
قبل از پرش:
  . ■ . ■
  ■ . B .     B = مهره سیاه
  . W . ■     W = مهره سفید
  ■ . ■ .

بعد از پرش:
  . W . ■     W پرید از c3 → b4(jump) → a5
  ■ . ■ .     B خورده شد
  . . . ■
  ■ . ■ .
```

**قوانین پرش**:
- ✅ باید مهره حریف را بخورد
- ✅ خانه بعد باید خالی باشد
- ✅ پرش اجباری است (اگر ممکن باشد)

### 3. پرش زنجیره‌ای (Multiple Jumps)

اگر بعد از یک پرش، پرش دیگری ممکن باشد:

```
قبل:
  . ■ . ■ . ■
  ■ . B . B .     B = مهره‌های سیاه
  . W . ■ . ■     W = مهره سفید
  ■ . ■ . ■ .

بعد:
  . ■ . W . ■     W پرید: c3 → b4 → a5 → b6 → c7
  ■ . . . . .     2 مهره سیاه خورده شد
  . . . ■ . ■
  ■ . ■ . ■ .
```

**قوانین**:
- ✅ باید همه پرش‌های ممکن را انجام دهد
- ✅ نمی‌تواند نصفه متوقف شود
- ✅ در یک نوبت چند مهره می‌تواند بخورد

### 4. King شدن

وقتی مهره به آخرین ردیف برسد:

```
مهره سفید به ردیف 8 رسید:

8 . K . ■ . ■     K = King (شاه شد)
7 ■ . ■ . ■ .
...
1 ■ . ■ . ■ .

حالا می‌تواند به جلو و عقب حرکت کند
```

---

## 🏆 شرایط برد/باخت

### برد

✅ **یکی از این موارد**:
1. تمام مهره‌های حریف خورده شد
2. حریف هیچ حرکت معتبری ندارد (مسدود است)

### مساوی (Draw)

🤝 **در این حالت‌ها**:
1. 40 حرکت بدون خوردن مهره یا پیشروی
2. سه بار تکرار وضعیت یکسان
3. توافق دو بازیکن

---

## 💻 پیاده‌سازی

### State Structure

```c
typedef struct {
    int board[8][8];  // 0=empty, 1=white, 2=black, 3=white_king, 4=black_king
    int turn;         // 1=white, 2=black
    int must_jump;    // آیا پرش اجباری است؟
    int jump_from;    // اگر در حال پرش زنجیره‌ای
} GameState;
```

### Move Validation

```c
bool is_valid_move(GameState *state, Move *move) {
    // 1. بررسی نوبت
    int piece = state->board[move->from_row][move->from_col];
    if (get_color(piece) != state->turn) return false;

    // 2. اگر پرش اجباری است
    if (state->must_jump) {
        if (!is_jump(move)) return false;
        // باید از همان مهره پرش کند
        if (state->jump_from != move->from) return false;
    }

    // 3. بررسی حرکت بر اساس نوع مهره
    if (is_king(piece)) {
        return is_valid_king_move(state, move);
    } else {
        return is_valid_man_move(state, move);
    }
}
```

### Jump Detection

```c
bool can_jump(GameState *state, int row, int col) {
    int piece = state->board[row][col];
    int color = get_color(piece);

    // بررسی 4 جهت مورب
    int directions[4][2] = {{-1,-1}, {-1,1}, {1,-1}, {1,1}};

    for (each direction) {
        int mid_row = row + direction[0];
        int mid_col = col + direction[1];
        int dest_row = row + direction[0] * 2;
        int dest_col = col + direction[1] * 2;

        // آیا مهره حریف در وسط است؟
        if (is_opponent(state->board[mid_row][mid_col], color)) {
            // آیا مقصد خالی است؟
            if (state->board[dest_row][dest_col] == 0) {
                return true;  // پرش ممکن است
            }
        }
    }

    return false;
}
```

---

## 🎨 UI Design

### صفحه نمایش

```html
<div class="checkers-board">
    <div class="row" data-row="8">
        <div class="square dark" data-pos="a8"></div>
        <div class="square light" data-pos="b8"></div>
        <!-- ... -->
    </div>
    <!-- rows 7-1 -->
</div>

<div class="game-info">
    <div class="turn-indicator">نوبت: سفید</div>
    <div class="captured">
        <div>خورده شده سفید: <span id="white-captured">0</span></div>
        <div>خورده شده سیاه: <span id="black-captured">0</span></div>
    </div>
</div>
```

### CSS

```css
.checkers-board {
    display: grid;
    grid-template-rows: repeat(8, 60px);
    width: 480px;
    border: 3px solid var(--neon-green);
}

.row {
    display: grid;
    grid-template-columns: repeat(8, 60px);
}

.square {
    width: 60px;
    height: 60px;
    display: flex;
    align-items: center;
    justify-content: center;
}

.square.light { background: #f0d9b5; }
.square.dark { background: #b58863; }

.piece {
    width: 50px;
    height: 50px;
    border-radius: 50%;
    border: 2px solid #333;
}

.piece.white { background: #fff; }
.piece.black { background: #222; }

.piece.king::after {
    content: '👑';
    position: absolute;
}

.square.selected {
    box-shadow: inset 0 0 10px var(--neon-green);
}

.square.possible-move {
    background: rgba(0, 255, 65, 0.3);
}

.square.must-jump {
    animation: pulse 1s infinite;
}

@keyframes pulse {
    0%, 100% { box-shadow: inset 0 0 5px red; }
    50% { box-shadow: inset 0 0 15px red; }
}
```

---

## 🔄 جریان بازی

```
1. شروع بازی
   - 12 مهره سفید در 3 ردیف پایین
   - 12 مهره سیاه در 3 ردیف بالا
   ↓
2. نوبت سفید
   ↓
3. بررسی پرش اجباری
   - اگر پرش ممکن است → highlight مهره‌ها
   ↓
4. کاربر مهره را انتخاب می‌کند
   ↓
5. نمایش حرکات/پرش‌های ممکن
   ↓
6. کاربر مقصد را انتخاب می‌کند
   ↓
7. اعمال حرکت:
   - اگر پرش → حذف مهره خورده شده
   - اگر به آخر رسید → King شدن
   - اگر پرش زنجیره‌ای ممکن → ادامه پرش
   ↓
8. بررسی برد/باخت
   ↓
9. تغییر نوبت
   ↓
10. بازگشت به مرحله 2
```

---

## 🧪 تست

### تست حرکت معمولی

```bash
# مهره سفید در c3 به d4
QUERY_STRING="board=...&from=c3&to=d4" ./checkers.cgi
```

**انتظار**: حرکت معتبر، نوبت به سیاه

### تست پرش ساده

```bash
# سفید در c3، سیاه در d4، پرش به e5
QUERY_STRING="board=...&from=c3&to=e5" ./checkers.cgi
```

**انتظار**: مهره سیاه در d4 حذف شود

### تست پرش زنجیره‌ای

```bash
# پرش: c3 → e5 → g7
QUERY_STRING="board=...&from=c3&to=e5" ./checkers.cgi
# بعد از پرش اول، باید ادامه دهد
QUERY_STRING="board=...&from=e5&to=g7" ./checkers.cgi
```

### تست King شدن

```bash
# مهره سفید از b7 به a8
QUERY_STRING="board=...&from=b7&to=a8" ./checkers.cgi
```

**انتظار**: مهره در a8 تبدیل به King شود (نماد تاج)

---

## 🐛 باگ‌های شناخته شده

- [ ] اگر چند مهره پرش ممکن داشته باشند، باید بازیکن یکی را انتخاب کند
- [ ] پرش اجباری در حالت‌های پیچیده ممکن است اشتباه تشخیص داده شود
- [ ] Draw detection پیاده‌سازی نشده

---

## 🔮 بهبودهای آینده

- [ ] AI bot با Minimax algorithm
- [ ] Hint system (بهترین حرکت)
- [ ] Undo move
- [ ] Timer (زمان محدود برای هر حرکت)
- [ ] Replay system
- [ ] Sound effects (برای پرش، King شدن)
- [ ] Animation (حرکت smooth مهره‌ها)
- [ ] Tournament mode

---

## 📚 قوانین رسمی

این بازی بر اساس قوانین **American Checkers** (English Draughts) پیاده‌سازی شده:

- صفحه 8×8
- 12 مهره برای هر بازیکن
- پرش اجباری
- King فقط یک خانه حرکت می‌کند

**تفاوت با قوانین بین‌المللی**:
- در International Checkers: صفحه 10×10، King می‌تواند چند خانه حرکت کند

---

## 📖 منابع

- [Checkers Rules](https://www.wikihow.com/Play-Checkers)
- [American Checkers Federation](https://www.usacheckers.com/)
- [Minimax Algorithm for Checkers](https://en.wikipedia.org/wiki/Minimax)

---

**چکرز - بازی استراتژی کلاسیک با قوانین ساده، بازی عمیق! ♟️**
