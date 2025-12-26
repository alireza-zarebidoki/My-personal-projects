// اینجا تعریف‌های پایه چکرز تغییر‌یافته را نگه می‌دارم تا منطق و رندر روی یک قرارداد مشترک کار کنند

#ifndef CHECKERS_H
#define CHECKERS_H

#include "../../common/c_utils.h"

// نوع مهره را ثابت کردم تا هم‌زمان Man/King/Queen را پشتیبانی کنم
typedef enum {
    EMPTY_C = 0,
    MAN = 1,      // مهره عادی
    KING = 2,     // King (حرکت مورب آزاد)
    QUEEN = 3     // Queen (حرکت عمودی/افقی خاص)
} CheckerType;

// رنگ مهره را دو مقداری گذاشتم تا منطق ساده بماند
typedef enum {
    RED = 1,
    BLACK_C = 2
} CheckerColor;

// هر مهره نوع و رنگ می‌گیرد
typedef struct {
    CheckerType type;
    CheckerColor color;
} Checker;

// وضعیت کامل صفحه و متای بازی را نگه می‌دارم تا بین CGI ریکوئست‌ها منتقل شود
typedef struct {
    Checker board[8][8];
    CheckerColor current_turn;
    int selected_row;
    int selected_col;
    int must_jump;        // پرش اجباری فعال است
    int jump_from_r;      // مهره‌ای که باید پرش کند
    int jump_from_c;
    int multi_jump;       // آیا در حال پرش چندگانه هستیم؟
    int game_over;
    int winner;           // 0: none, 1: red, 2: black
    char message[256];
} CheckersState;

// توابع state.c: راه‌اندازی و سریال‌سازی را اینجا اعلان می‌کنم
void init_checkers_board(CheckersState *game);
void serialize_checkers_state(CheckersState *game, char *buffer);
void deserialize_checkers_state(const char *buffer, CheckersState *game);

// توابع logic.c: اعتبارسنجی و اعمال حرکت‌ها
int is_valid_checkers_move(CheckersState *game, int from_r, int from_c, int to_r, int to_c);
int can_jump_from(CheckersState *game, int row, int col);
int check_global_forced_jump(CheckersState *game);
void make_checkers_move(CheckersState *game, int from_r, int from_c, int to_r, int to_c);
void get_valid_checkers_moves(CheckersState *game, int row, int col, int valid[8][8]);
int count_pieces(CheckersState *game, CheckerColor color);

// نمایش یونیکد مهره‌ها برای UI
const char* checker_to_unicode(CheckerType type, CheckerColor color);

#endif
