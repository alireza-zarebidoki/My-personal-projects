// این فایل تمام منطق حرکت و قوانین پرش اجباری چکرز تغییر‌یافته را نگه می‌دارد
#include "checkers.h"
#include <stdlib.h>

// بررسی می‌کنم آیا مهره مشخص می‌تواند از خانه فعلی پرش کند یا نه
int can_jump_from(CheckersState *game, int row, int col) {
    Checker piece = game->board[row][col];
    if (piece.type == EMPTY_C) return 0;

    // چهار جهت ممکن برای پرش مورب را یکجا فهرست کردم تا حلقه ساده شود
    int dirs[4][2] = {{-2, -2}, {-2, 2}, {2, -2}, {2, 2}};

    for (int d = 0; d < 4; d++) {
        int nr = row + dirs[d][0];
        int nc = col + dirs[d][1];
        int mr = row + dirs[d][0] / 2; // خانه میانی که باید حریف باشد
        int mc = col + dirs[d][1] / 2;

        // اگر مقصد بیرون صفحه باشد از همان جهت عبور می‌کنم
        if (nr < 0 || nr > 7 || nc < 0 || nc > 7) continue;

        // مهره عادی فقط در جهت مجاز خودش می‌تواند بپرد
        if (piece.type == MAN) {
            if (piece.color == RED && dirs[d][0] > 0) continue; // قرمز فقط رو به بالا
            if (piece.color == BLACK_C && dirs[d][0] < 0) continue; // سیاه فقط رو به پایین
        }

        Checker mid = game->board[mr][mc];
        Checker target = game->board[nr][nc];

        // برای پرش، خانه میانی باید حریف و مقصد باید خالی باشد
        if (mid.type != EMPTY_C && mid.color != piece.color && target.type == EMPTY_C) {
            return 1;
        }
    }

    return 0;
}

// تمام صفحه را می‌گردم تا ببینم آیا در این نوبت پرش اجباری وجود دارد یا نه
int check_global_forced_jump(CheckersState *game) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Checker p = game->board[i][j];
            if (p.type != EMPTY_C && p.color == game->current_turn) {
                if (can_jump_from(game, i, j)) {
                    game->must_jump = 1;
                    game->jump_from_r = i;
                    game->jump_from_c = j;
                    return 1;
                }
            }
        }
    }

    game->must_jump = 0;
    return 0;
}

// بررسی می‌کنم حرکت از مبدا به مقصد با قوانین من تطابق دارد یا نه
int is_valid_checkers_move(CheckersState *game, int from_r, int from_c, int to_r, int to_c) {
    // اگر مقصد خارج از صفحه است بلافاصله مردود می‌کنم
    if (to_r < 0 || to_r > 7 || to_c < 0 || to_c > 7) return 0;

    Checker piece = game->board[from_r][from_c];
    Checker target = game->board[to_r][to_c];

    // فقط به خانه خالی اجازه می‌دهم
    if (target.type != EMPTY_C) return 0;

    // چکرز روی خانه‌های تیره است، پس مقصد باید تیره باشد
    if ((to_r + to_c) % 2 == 0) return 0;

    int dr = to_r - from_r;
    int dc = to_c - from_c;

    // حرکت عادی یک خانه مورب
    if (abs(dr) == 1 && abs(dc) == 1) {
        // اگر پرش اجباری فعال است، حرکت عادی را ممنوع می‌کنم
        if (game->must_jump) return 0;

        if (piece.type == MAN) {
            if (piece.color == RED && dr > 0) return 0;  // قرمز فقط بالا
            if (piece.color == BLACK_C && dr < 0) return 0; // سیاه فقط پایین
        }

        return 1;
    }

    // حرکت Queen یک خانه عمودی یا افقی مجاز است
    if (piece.type == QUEEN) {
        if (dr == 0 && abs(dc) == 1) return 1;
        if (dc == 0 && abs(dr) == 1) return 1;
    }

    // حرکت پرشی دو خانه مورب، با وجود مهره حریف در میانه
    if (abs(dr) == 2 && abs(dc) == 2) {
        int mr = from_r + dr / 2;
        int mc = from_c + dc / 2;
        Checker mid = game->board[mr][mc];

        if (mid.type == EMPTY_C || mid.color == piece.color) return 0;

        if (piece.type == MAN) {
            if (piece.color == RED && dr > 0) return 0;
            if (piece.color == BLACK_C && dr < 0) return 0;
        }

        return 1;
    }

    return 0;
}

// تعداد مهره‌های باقی‌مانده از یک رنگ را می‌شمارم (برای پایان بازی یا پیام وضعیت)
int count_pieces(CheckersState *game, CheckerColor color) {
    int count = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (game->board[i][j].type != EMPTY_C && game->board[i][j].color == color)
                count++;
    return count;
}

// حرکت را اعمال می‌کنم؛ حذف مهره خورده، ارتقاء، و جابه‌جایی نوبت را مدیریت می‌کنم
void make_checkers_move(CheckersState *game, int from_r, int from_c, int to_r, int to_c) {
    Checker moving = game->board[from_r][from_c];
    int dr = to_r - from_r;
    int dc = to_c - from_c;

    int is_jump = 0;
    if (abs(dr) == 2 && abs(dc) == 2) {
        int mr = from_r + dr / 2;
        int mc = from_c + dc / 2;
        game->board[mr][mc] = (Checker){EMPTY_C, RED};
        is_jump = 1;
    }

    game->board[to_r][to_c] = moving;
    game->board[from_r][from_c] = (Checker){EMPTY_C, RED};

    // اگر مهره عادی به انتهای زمین رسید، پیش‌فرض به King ارتقاء می‌دهم (انتخاب UI در main.c مدیریت می‌شود)
    if (moving.type == MAN) {
        if ((moving.color == RED && to_r == 0) || (moving.color == BLACK_C && to_r == 7)) {
            game->board[to_r][to_c].type = KING;
        }
    }

    // اگر هنوز امکان پرش بعدی وجود دارد، در همین نوبت می‌مانم تا پرش زنجیره‌ای انجام شود
    if (is_jump && can_jump_from(game, to_r, to_c)) {
        game->multi_jump = 1;
        game->selected_row = to_r;
        game->selected_col = to_c;
        game->must_jump = 1;
        game->jump_from_r = to_r;
        game->jump_from_c = to_c;
        return;
    }

    // اگر پرش زنجیره‌ای نبود، نوبت را عوض می‌کنم و پیام را تنظیم می‌کنم
    game->multi_jump = 0;
    game->selected_row = -1;
    game->selected_col = -1;
    game->current_turn = (game->current_turn == RED) ? BLACK_C : RED;

    check_global_forced_jump(game);

    int red_count = count_pieces(game, RED);
    int black_count = count_pieces(game, BLACK_C);

    if (red_count == 0) {
        game->game_over = 1;
        game->winner = BLACK_C;
        strcpy(game->message, "سیاه برنده شد!");
    } else if (black_count == 0) {
        game->game_over = 1;
        game->winner = RED;
        strcpy(game->message, "قرمز برنده شد!");
    } else {
        sprintf(game->message, "نوبت %s%s",
                (game->current_turn == RED) ? "قرمز" : "سیاه",
                game->must_jump ? " (پرش اجباری!)" : "");
    }
}

// ماتریس حرکات مجاز را برای هایلایت UI محاسبه می‌کنم
void get_valid_checkers_moves(CheckersState *game, int row, int col, int valid[8][8]) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            valid[i][j] = 0;

    if (game->board[row][col].type == EMPTY_C) return;

    // اگر پرش اجباری فعال است فقط همان مهره اجازه دارد حرکت کند
    if (game->must_jump && (game->jump_from_r != row || game->jump_from_c != col)) {
        return;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (is_valid_checkers_move(game, row, col, i, j)) {
                valid[i][j] = 1;
            }
        }
    }
}
