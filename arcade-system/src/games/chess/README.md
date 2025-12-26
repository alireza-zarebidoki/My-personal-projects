# ♟️ شطرنج تغییریافته (Chess Modified)

این پوشه حاوی بازی شطرنج با مهره‌های جدید و قوانین اضافی است.

---

## 📋 فایل‌ها

```
chess/
├── chess.h      # Header: struct ها، enum ها، function prototypes
├── main.c       # CGI handler، HTML generation، entry point
├── logic.c      # منطق بازی: move validation، check/checkmate detection
└── state.c      # State management: encoding/decoding URL parameters
```

---

## 🎮 مهره‌های جدید

### 🐉 Dragon (اژدها)
**جایگزین**: Knight (اسب)
**حرکت**: ترکیب اسب + شاه

```
از هر خانه می‌تواند به:
- 8 خانه L-shape (مثل اسب)
- 8 خانه اطراف (مثل شاه)
= جمعاً 16 حرکت ممکن
```

**مثال**:
```
. . . X . X . .     X = حرکت Dragon
. X . . . . X .     D = Dragon
. . . . . . . .
X . . D . . . X
. . . . . . . .
. X . . . . X .
. . . X . X . .
X . . . . . . X
```

---

### 🥷 Thief (دزد)
**جایگزین**: Bishop (فیل)
**حرکت**: فیل با قابلیت پرش از یک مانع

```
- تمام مورب‌ها مثل فیل
- می‌تواند از حداکثر 1 مهره بپرد
```

**مثال**:
```
X . . . . . . T
. X . . . . P .     T = Thief
. . X . . P . .     P = مهره مانع
. . . X P . . .     X = حرکت عادی
. . . P . . . .     O = حرکت با پرش
. . P . . . . .
. P . O . . . .
P . . . O . . .
```

**قانون پرش**:
- فقط از 1 مهره می‌تواند بپرد
- نمی‌تواند مهره را بخورد در حین پرش
- باید در خانه خالی یا مهره حریف فرود آید

---

### 🦅 Gryphon (گریفین)
**جایگزین**: Rook (رخ)
**حرکت**: 1 خانه مورب + سپس حرکت رخ

```
مرحله 1: یک خانه مورب (الزامی)
مرحله 2: حرکت رخ در همان جهت
```

**مثال**:
```
. . . . . . . .
. . . X . . . .     G = Gryphon در (4,4)
. . X . X . . .     1 = خانه اول (مورب)
. X . . . X . .     X = ادامه حرکت (رخ)
. . . . G . . .
. X . . . X . .     حرکت: (4,4) → (3,3) → (2,2) → (1,1)
. . X . X . . .            یا (4,4) → (5,5) → (6,6)
. . . X . . . .
```

---

## 🕐 تایمر

- **زمان کل**: هر بازیکن 10 دقیقه
- **نمایش**: در بالای صفحه
- **اتمام زمان**: باخت خودکار

**پیاده‌سازی**:
- زمان در URL parameter ذخیره می‌شود
- JavaScript هر ثانیه کاهش می‌یابد
- CGI زمان را validate می‌کند

---

## 📝 State Management

تمام state بازی در URL parameters است:

```
/chess_modified.cgi?board=rnbqkbnr/pppppppp/.../PPPPPPPP/RNBQKBNR
                    &turn=white
                    &white_time=600
                    &black_time=600
                    &move_from=e2
                    &move_to=e4
```

### پارامترها

| پارامتر | توضیح | مثال |
|---------|-------|------|
| `board` | وضعیت صفحه (FEN notation) | `rnbq...` |
| `turn` | نوبت | `white` یا `black` |
| `white_time` | زمان باقی‌مانده سفید (ثانیه) | `600` |
| `black_time` | زمان باقی‌مانده سیاه (ثانیه) | `580` |
| `move_from` | مبدأ حرکت | `e2` |
| `move_to` | مقصد حرکت | `e4` |
| `castle` | قابلیت راکو | `KQkq` |
| `en_passant` | خانه en passant | `e3` |

---

## 🧠 منطق بازی (logic.c)

### بررسی حرکت معتبر

```c
bool is_valid_move(Board *board, Move *move) {
    // 1. بررسی خانه مبدأ
    if (!has_piece(board, move->from)) return false;

    // 2. بررسی نوبت
    if (get_piece_color(board, move->from) != board->turn) return false;

    // 3. بررسی حرکت مهره
    switch (get_piece_type(board, move->from)) {
        case PAWN:   return is_valid_pawn_move(board, move);
        case DRAGON: return is_valid_dragon_move(board, move);
        case THIEF:  return is_valid_thief_move(board, move);
        case GRYPHON: return is_valid_gryphon_move(board, move);
        // ...
    }

    // 4. بررسی کیش بعد از حرکت
    apply_move(board, move);
    bool in_check = is_king_in_check(board, board->turn);
    undo_move(board, move);

    return !in_check;
}
```

### تشخیص کیش مات

```c
bool is_checkmate(Board *board) {
    // 1. آیا در حال حاضر کیش است؟
    if (!is_king_in_check(board, board->turn)) {
        return false;
    }

    // 2. آیا حرکتی وجود دارد که کیش را حل کند؟
    for (each piece of current player) {
        for (each possible move) {
            if (is_valid_move(board, move)) {
                return false;  // یک حرکت پیدا شد
            }
        }
    }

    // هیچ حرکتی وجود ندارد
    return true;
}
```

---

## 🎨 UI/UX

### صفحه شطرنج

```html
<div class="chess-board">
    <!-- 8x8 Grid -->
    <div class="square light" data-pos="a8"></div>
    <div class="square dark" data-pos="b8"></div>
    <!-- ... -->
</div>

<div class="info-panel">
    <div class="timer white">⏱️ 10:00</div>
    <div class="timer black">⏱️ 10:00</div>
    <div class="turn-indicator">نوبت: سفید</div>
</div>
```

### CSS

```css
.chess-board {
    display: grid;
    grid-template-columns: repeat(8, 60px);
    grid-template-rows: repeat(8, 60px);
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

.square.selected {
    box-shadow: inset 0 0 10px rgba(0, 255, 65, 0.8);
}

.square.possible-move {
    background: rgba(0, 255, 65, 0.3);
}
```

---

## 🔄 جریان بازی

```
1. شروع بازی
   ↓
2. نمایش صفحه اولیه
   ↓
3. کاربر مهره را کلیک می‌کند
   ↓
4. نمایش حرکات ممکن (highlight)
   ↓
5. کاربر خانه مقصد را کلیک می‌کند
   ↓
6. CGI بررسی می‌کند:
   - آیا حرکت معتبر است؟
   - آیا کیش ایجاد می‌شود؟
   - آیا کیش مات است؟
   ↓
7. اگر معتبر:
   - حرکت اعمال می‌شود
   - نوبت عوض می‌شود
   - زمان کاهش می‌یابد
   ↓
8. نمایش صفحه جدید
   ↓
9. بازگشت به مرحله 3
```

---

## 🧪 تست

### تست حرکت Dragon

```bash
# Dragon در e4 به کدام خانه‌ها می‌رود؟
QUERY_STRING="board=...&move_from=e4&piece=dragon" ./chess_modified.cgi
```

**حرکات مورد انتظار**:
```
c5, c3, d6, d5, d4, d3, d2, e5, e3, f6, f5, f4, f3, g5, g3
(8 L-shape + 7 مجاور)
```

### تست Thief با پرش

```bash
# Thief در a1، مهره در c3، می‌تواند به e5 برود؟
# مسیر: a1 → b2 → c3(jump) → d4 → e5
```

### تست Gryphon

```bash
# Gryphon در d4
# حرکت: d4 → e5 (مورب) → e6, e7, e8 (رخ)
```

---

## 🐛 باگ‌های شناخته شده

- [ ] تشخیص pat (stalemate) پیاده‌سازی نشده
- [ ] en passant با مهره‌های جدید تست نشده
- [ ] ممکن است در حالت‌های نادر، Thief پرش نادرست انجام دهد

---

## 🔮 بهبودهای آینده

- [ ] افزودن AI bot (Minimax algorithm)
- [ ] Replay system (ذخیره و بازپخش بازی)
- [ ] Hint system (پیشنهاد بهترین حرکت)
- [ ] مهره‌های بیشتر (Phoenix، Unicorn)
- [ ] حالت Tournament
- [ ] Undo move
- [ ] Save/Load game

---

## 📚 منابع

- [Chess Programming Wiki](https://www.chessprogramming.org/)
- [FEN Notation](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)
- [Chess Piece Design](https://en.wikipedia.org/wiki/Fairy_chess_piece)

---

**شطرنج تغییریافته - ترکیبی از کلاسیک و خلاقیت! ♟️🐉**
