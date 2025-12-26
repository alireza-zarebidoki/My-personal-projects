# 🎮 بازی‌ها (Games)

این پوشه حاوی تمام بازی‌های CGI سیستم آرکید است. هر بازی یک فایل C مستقل است که به صورت CGI executable کامپایل می‌شود.

---

## 📋 فهرست بازی‌ها

| بازی | فایل | قیمت | سطح دشواری | حالت‌ها |
|------|------|------|-------------|---------|
| حدس عدد | `guess_number.c` | 3 سکه | آسان/متوسط/سخت | Single Player |
| دوز محدود | `tictactoe_limited.c` | 5 سکه | متوسط | PvP |
| سنگ کاغذ قیچی++ | `rps_5move.c` | 4 سکه | متوسط | PvP / vs Bot |
| حلقه آویز | `hangman_battle.c` | 6 سکه | متوسط | PvP / vs Bot |
| فیبوناچی 2048 | `fibonacci_2048.c` | 7 سکه | سخت | Single Player |
| شطرنج تغییریافته | `chess/` | 10 سکه | سخت | PvP |
| چکرز | `checkers/` | 8 سکه | متوسط | PvP |

---

## 🏗️ معماری بازی‌ها

### ساختار کلی هر بازی

هر بازی CGI دارای این ساختار است:

```c
#include "../common/c_utils.h"

int main() {
    // 1. پارس پارامترهای URL
    char *param = get_param("param_name");

    // 2. منطق بازی
    // محاسبات، بررسی state، تصمیم‌گیری

    // 3. تولید HTML
    print_header();
    printf("<html>...</html>");

    return 0;
}
```

### جریان اجرا

```
کاربر → انتخاب بازی
    → Frontend (script.js) → بررسی سکه
    → کسر سکه
    → ساخت iframe با URL: cgi-bin/game.cgi?params
    → Python HTTP Server
    → اجرای CGI executable
    → تولید HTML
    → نمایش در iframe
    → کاربر تعامل می‌کند
    → submit form یا click link
    → CGI دوباره اجرا می‌شود با state جدید در URL
```

### Stateless Design

✅ **تمام بازی‌ها stateless هستند**:
- هیچ state روی server ذخیره نمی‌شود
- تمام state در URL parameters است
- هر request مستقل است
- CGI process بعد از هر request terminate می‌شود

**مثال**:
```
/cgi-bin/tictactoe_limited.cgi?p1=0,4,8&p2=1,5&turn=1&move=2
```
در این URL:
- `p1=0,4,8`: مهره‌های بازیکن 1 در خانه‌های 0، 4، 8
- `p2=1,5`: مهره‌های بازیکن 2 در خانه‌های 1، 5
- `turn=1`: نوبت بازیکن 1
- `move=2`: بازیکن می‌خواهد خانه 2 را بازی کند

---

## 🎯 1. حدس عدد (Guess Number)

### توضیح
بازیکن باید عدد تصادفی بین 1 تا 100 را حدس بزند. در هر تلاش، راهنمایی داده می‌شود.

### فایل
- `guess_number.c` (257 خط)

### سطوح دشواری

#### آسان (Easy)
- 10 تلاش
- راهنمایی دقیق: "عدد بین 30 تا 40 است"

#### متوسط (Medium)
- 7 تلاش
- راهنمایی کلی: "عدد بین 1 تا 50 است"

#### سخت (Hard)
- 5 تلاش
- راهنمایی مبهم: فقط "بالاتر" یا "پایین‌تر"

### پارامترهای URL

```
?mode=init                          # شروع بازی جدید (انتخاب سطح)
?mode=start&diff=1                  # شروع بازی با سطح 1 (easy)
?mode=guess&target=67&diff=1&tries=10&guess=50  # حدس زدن
```

### منطق اصلی

```c
// تولید عدد تصادفی
srand(time(NULL) + getpid());
int target = (rand() % 100) + 1;

// بررسی حدس
if (guess == target) {
    printf("برنده شدی! +5 سکه");
} else if (tries == 0) {
    printf("باختی! عدد %d بود", target);
} else {
    // راهنمایی بده
    give_hint(guess, target, difficulty);
}
```

---

## 🎲 2. دوز محدود (Tic-Tac-Toe Limited)

### توضیح
دوز کلاسیک اما با محدودیت 3 مهره برای هر بازیکن. وقتی مهره چهارم گذاشته می‌شود، قدیمی‌ترین مهره به صورت خودکار حذف می‌شود (FIFO queue).

### فایل
- `tictactoe_limited.c`

### قوانین خاص

1. **محدودیت 3 مهره**: هر بازیکن حداکثر 3 مهره روی صفحه دارد
2. **FIFO**: قدیمی‌ترین مهره وقتی مهره چهارم گذاشته می‌شود، حذف می‌شود
3. **برنده شدن**: 3 مهره در یک خط (افقی، عمودی، مورب)

### پارامترهای URL

```
?p1=0,4,8&p2=1,5&turn=1&move=2
```

- `p1`: لیست مهره‌های بازیکن 1 (به ترتیب زمانی)
- `p2`: لیست مهره‌های بازیکن 2
- `turn`: نوبت (1 یا 2)
- `move`: خانه انتخاب شده (0-8)

### منطق FIFO

```c
// اگر بازیکن بیش از 3 مهره دارد
if (count(player_moves) >= 3) {
    remove_oldest_move(player_moves);  // حذف اولین مهره
}
add_new_move(player_moves, move);      // اضافه کردن مهره جدید
```

### بررسی برنده

```c
int winning_patterns[8][3] = {
    {0,1,2}, {3,4,5}, {6,7,8},  // افقی
    {0,3,6}, {1,4,7}, {2,5,8},  // عمودی
    {0,4,8}, {2,4,6}             // مورب
};

for (each pattern) {
    if (all_three_in_pattern_belong_to_player) {
        return WIN;
    }
}
```

---

## ✊ 3. سنگ کاغذ قیچی لیزارد اسپاک (RPS 5-Move)

### توضیح
نسخه گسترش‌یافته سنگ کاغذ قیچی با 5 حرکت از سریال Big Bang Theory. بهترین از 5 راند.

### فایل
- `rps_5move.c` (190 خط)

### حرکات و قوانین

```
Rock      > Scissors, Lizard
Paper     > Rock, Spock
Scissors  > Paper, Lizard
Lizard    > Paper, Spock
Spock     > Rock, Scissors
```

**یادآوری**:
- Rock crushes Scissors
- Rock crushes Lizard
- Paper covers Rock
- Paper disproves Spock
- Scissors cuts Paper
- Scissors decapitates Lizard
- Lizard eats Paper
- Lizard poisons Spock
- Spock vaporizes Rock
- Spock smashes Scissors

### حالت‌ها

#### 1. Player vs Player (PvP)
- بازیکن 1 حرکت خود را انتخاب می‌کند
- بازیکن 2 حرکت خود را انتخاب می‌کند
- نتیجه نمایش داده می‌شود

#### 2. Player vs Bot
- بازیکن حرکت را انتخاب می‌کند
- Bot با الگوریتم counter-strategy بازی می‌کند

### Bot AI

```c
// الگوریتم ساده: counter strategy
int bot_move(int player_last_move, int history[]) {
    // بررسی الگوی بازیکن
    int most_frequent = get_most_frequent_move(history);

    // انتخاب حرکتی که آن را شکست می‌دهد
    return counter_move(most_frequent);
}
```

### پارامترهای URL

```
?mode=p1_select&type=1                    # بازیکن 1 انتخاب PvP (1) یا Bot (2)
?mode=p1_move&type=1&p1=0                # بازیکن 1 حرکت Rock (0) را انتخاب کرد
?mode=p2_move&type=1&p1=0&p2=2          # بازیکن 2 حرکت Scissors (2) را انتخاب کرد
```

---

## 🎯 4. حلقه آویز (Hangman Battle)

### توضیح
دو بازیکن (یا بازیکن و Bot) هم‌زمان در دو بازی Hangman شرکت می‌کنند. هر کدام باید کلمه خود را سریع‌تر حدس بزنند.

### فایل
- `hangman_battle.c` (184 خط)

### حالت‌ها

#### PvP (Player vs Player)
- هر بازیکن یک کلمه دارد
- به نوبت حرف حدس می‌زنند
- اولین نفری که کلمه را تکمیل کند برنده است

#### vs Bot
- بازیکن و Bot هر دو کلمه جداگانه دارند
- Bot با استراتژی frequency-based حرف انتخاب می‌کند

### Bot Strategy

```c
// ترتیب حروف بر اساس فرکانس در زبان انگلیسی
char frequency[] = "ETAOINSHRDLCUMWFGYPBVKJXQZ";

// Bot از حروف پرتکرار شروع می‌کند
for (each letter in frequency) {
    if (not_guessed_yet(letter)) {
        return letter;
    }
}
```

### پارامترهای URL

```
?mode=init                                      # انتخاب حالت
?mode=start&type=1                             # شروع PvP
?mode=play&w1=HELLO&w2=WORLD&g1=E,L&g2=O,R&turn=1&guess=H
```

- `w1`, `w2`: کلمات دو بازیکن
- `g1`, `g2`: حروف حدس زده شده
- `turn`: نوبت
- `guess`: حرف جدید

---

## 🔢 5. فیبوناچی 2048

### توضیح
نسخه تغییر یافته 2048 که به جای توان‌های 2، از دنباله فیبوناچی استفاده می‌کند.

### فایل
- `fibonacci_2048.c` (359 خط)

### قوانین خاص

**در 2048 معمولی**:
```
2 + 2 = 4
4 + 4 = 8
8 + 8 = 16
```

**در Fibonacci 2048**:
```
1 + 1 = 2
1 + 2 = 3
2 + 3 = 5
3 + 5 = 8
5 + 8 = 13
...
```

✅ **فقط اعداد مجاور در دنباله فیبوناچی می‌توانند ترکیب شوند**

### اندازه‌های صفحه

- 4×4: استاندارد
- 5×5: متوسط
- 6×6: بزرگ (سخت‌تر)

### منطق Merge

```c
int fibonacci[] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, ...};

bool can_merge(int a, int b) {
    // آیا a و b مجاور در دنباله فیبوناچی هستند؟
    int index_a = find_index(a, fibonacci);
    int index_b = find_index(b, fibonacci);

    return (abs(index_a - index_b) == 1);
}

int merge(int a, int b) {
    int index_a = find_index(a, fibonacci);
    int index_b = find_index(b, fibonacci);

    // عدد بزرگ‌تر در دنباله
    return fibonacci[max(index_a, index_b) + 1];
}
```

---

## ♟️ 6. شطرنج تغییریافته (Chess Modified)

### توضیح
شطرنج کلاسیک با 3 مهره جدید که حرکات منحصر به فرد دارند.

### پوشه
- `chess/` (4 فایل: chess.h، main.c، logic.c، state.c)

### مهره‌های جدید

#### 🐉 Dragon (جایگزین Knight)
**حرکت**: ترکیب اسب + شاه
```
- 8 خانه L-shape مثل اسب
- + 8 خانه اطراف مثل شاه
= 16 حرکت ممکن
```

#### 🥷 Thief (جایگزین Bishop)
**حرکت**: فیل با قابلیت پرش
```
- تمام مورب‌ها مثل فیل
- می‌تواند از 1 مهره بپرد (فقط یکی!)
```

#### 🦅 Gryphon (جایگزین Rook)
**حرکت**: یک مورب + رخ
```
- یک خانه مورب (الزامی)
- سپس حرکت رخ در آن جهت
```

### ساختار فایل‌ها

```
chess/
├── chess.h         # تعاریف struct، enum، function prototypes
├── main.c          # CGI handler، HTML generation
├── logic.c         # منطق بازی، move validation، check/checkmate
└── state.c         # مدیریت state، URL encoding/decoding
```

### Timer System

- هر بازیکن 10 دقیقه زمان دارد
- زمان با JavaScript در frontend کاهش می‌یابد
- اگر زمان تمام شود، بازیکن می‌بازد

---

## ♟️ 7. چکرز (Checkers)

### توضیح
بازی Checkers کلاسیک با قوانین استاندارد.

### پوشه
- `checkers/` (4 فایل: checkers.h، main.c، logic.c، state.c)

### قوانین

1. **حرکت معمولی**: یک خانه مورب به جلو
2. **پرش**: اگر مهره حریف در مورب باشد و خانه بعد خالی باشد
3. **پرش زنجیره‌ای**: اگر بعد از پرش، پرش دیگری ممکن باشد
4. **King شدن**: وقتی به انتهای صفحه برسد
5. **King**: می‌تواند به جلو و عقب حرکت کند

### منطق پرش

```c
bool can_jump(int from, int to, char board[]) {
    int mid = (from + to) / 2;  // خانه وسط

    // آیا خانه وسط مهره حریف دارد؟
    // آیا خانه مقصد خالی است؟
    return (is_opponent(board[mid]) && is_empty(board[to]));
}
```

---

## 🛠️ توابع مشترک (c_utils)

تمام بازی‌ها از توابع مشترک در `../common/c_utils.h` استفاده می‌کنند.

### توابع اصلی

```c
void print_header();                    // چاپ HTTP header
char* get_param(const char* name);      // دریافت پارامتر از QUERY_STRING
void url_decode(char* dst, char* src);  // decode URL
void html_escape(char* str);            // escape HTML entities
```

### نحوه استفاده

```c
#include "../common/c_utils.h"

int main() {
    print_header();  // Content-Type: text/html

    char *mode = get_param("mode");
    char *player = get_param("player");

    // استفاده از پارامترها
    if (strcmp(mode, "start") == 0) {
        // ...
    }
}
```

---

## 🔧 کامپایل و Build

### دستور کامپایل عمومی

```bash
gcc -o cgi-bin/game.cgi src/games/game.c src/common/c_utils.c -I src/common
```

### استفاده از Makefile

```bash
# کامپایل یک بازی
make guess_number.cgi

# کامپایل تمام بازی‌ها
make all

# پاک کردن
make clean
```

### Compiler Flags

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -O2
INCLUDES = -I src/common
```

---

## 🧪 تست بازی‌ها

### تست مستقیم CGI

```bash
# اجرای مستقیم (بدون پارامتر)
./cgi-bin/guess_number.cgi

# با پارامتر
QUERY_STRING="mode=start&diff=1" ./cgi-bin/guess_number.cgi
```

### تست در مرورگر

```
http://localhost:8080/cgi-bin/guess_number.cgi?mode=start&diff=1
```

### دیباگ

```bash
# بررسی syntax error
gcc -Wall -Wextra -fsyntax-only src/games/game.c

# استفاده از gdb
gdb ./cgi-bin/game.cgi
```

---

## 📝 نکات توسعه

### اضافه کردن بازی جدید

1. فایل C جدید در `src/games/` بسازید
2. از `c_utils.h` استفاده کنید
3. CGI output تولید کنید (HTML)
4. به Makefile اضافه کنید
5. در `script.js` بازی را به کاروسل اضافه کنید

### الگوی پایه

```c
#include "../common/c_utils.h"

int main() {
    print_header();

    char *mode = get_param("mode");

    printf("<!DOCTYPE html>\n");
    printf("<html dir='rtl'>\n");
    printf("<head>\n");
    printf("<meta charset='UTF-8'>\n");
    printf("<title>بازی جدید</title>\n");
    printf("</head>\n");
    printf("<body>\n");

    if (!mode || strcmp(mode, "init") == 0) {
        // صفحه شروع
    } else if (strcmp(mode, "play") == 0) {
        // منطق بازی
    }

    printf("</body>\n");
    printf("</html>\n");

    return 0;
}
```

---

## 🔮 بهبودهای آینده

- [ ] افزودن Snake
- [ ] افزودن Tetris
- [ ] افزودن Pong (multiplayer real-time)
- [ ] افزودن Sudoku
- [ ] افزودن Memory Card Game
- [ ] سیستم save/load برای بازی‌های طولانی
- [ ] Replay system برای شطرنج
- [ ] AI بهتر برای بازی‌ها

---

**این بازی‌ها با ❤️ در C نوشته شده‌اند**
