// این فایل تمام منطق حرکت و قوانین پرش اجباری چکرز تغییر‌یافته را نگه می‌دارد
#include "checkers.h"
#include <stdlib.h>

// بررسی می‌کنم آیا مهره مشخص می‌تواند از خانه فعلی پرش کند یا نه
int can_jump_from(CheckersState *game, int row, int col) {
    Checker piece = game->board[row][col];
    if (piece.type == EMPTY_C) return 0;
    if (piece.type == QUEEN) {
        int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}}; // up, down, left, right
        for (int d = 0; d < 4; d++) {
            int dr = dirs[d][0];
            int dc = dirs[d][1];
            // New rule: QUEEN captures when opponent is two squares away and lands on the 4th square
            // pattern: [Q][empty][opponent][empty][landing(empty)] in same row/col
            int b1_r = row + dr;           // one step (must be empty)
            int mid_r = row + 2*dr;        // opponent at two steps
            int b2_r = row + 3*dr;         // third square (must be empty)
            int land_r = row + 4*dr;       // landing at four steps
            int b1_c = col + dc;
            int mid_c = col + 2*dc;
            int b2_c = col + 3*dc;
            int land_c = col + 4*dc;

            if (b1_r < 0 || b1_r > 7 || b1_c < 0 || b1_c > 7) continue;
            if (mid_r < 0 || mid_r > 7 || mid_c < 0 || mid_c > 7) continue;
            if (b2_r < 0 || b2_r > 7 || b2_c < 0 || b2_c > 7) continue;
            if (land_r < 0 || land_r > 7 || land_c < 0 || land_c > 7) continue;

            Checker b1 = game->board[b1_r][b1_c];
            Checker mid_cell = game->board[mid_r][mid_c];
            Checker b2 = game->board[b2_r][b2_c];
            Checker landing_cell = game->board[land_r][land_c];

            if (b1.type == EMPTY_C && b2.type == EMPTY_C && mid_cell.type != EMPTY_C && mid_cell.color != piece.color && landing_cell.type == EMPTY_C) {
                return 1; // پرشِ ویژه: حریف دو خانه دور و فرود در خانهٔ چهارم
            }
        }
        return 0;
    }

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
    int found = 0;
    int first_r = -1, first_c = -1;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Checker p = game->board[i][j];
            if (p.type != EMPTY_C && p.color == game->current_turn) {
                if (can_jump_from(game, i, j)) {
                    if (found == 0) {
                        first_r = i;
                        first_c = j;
                    }
                    found++;
                }
            }
        }
    }
    if (found) {
        game->must_jump = 1;
        if (found == 1) {
            // only one jumper -> lock jump_from to that piece
            game->jump_from_r = first_r;
            game->jump_from_c = first_c;
        } else {
            game->jump_from_r = -1;
            game->jump_from_c = -1;
        }
        return 1;
    }

    game->must_jump = 0;
    game->jump_from_r = -1;
    game->jump_from_c = -1;
    return 0;
}

// بررسی می‌کنم آیا رنگ مشخص حداقل یک حرکت معتبر دارد یا خیر (برای تشخیص بن‌بست)
int has_any_move_for(CheckersState *game, CheckerColor color) {
    int saved_turn = game->current_turn;
    int saved_must = game->must_jump;
    int saved_jr = game->jump_from_r;
    int saved_jc = game->jump_from_c;

    game->current_turn = color;
    check_global_forced_jump(game);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Checker p = game->board[r][c];
            if (p.type != EMPTY_C && p.color == color) {
                for (int tr = 0; tr < 8; tr++) {
                    for (int tc = 0; tc < 8; tc++) {
                        if (is_valid_checkers_move(game, r, c, tr, tc)) {
                            game->current_turn = saved_turn;
                            game->must_jump = saved_must;
                            game->jump_from_r = saved_jr;
                            game->jump_from_c = saved_jc;
                            return 1;
                        }
                    }
                }
            }
        }
    }

    game->current_turn = saved_turn;
    game->must_jump = saved_must;
    game->jump_from_r = saved_jr;
    game->jump_from_c = saved_jc;
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

    // چکرز روی خانه‌های تیره است، پس مقصد باید تیره باشد (به جز Queen که عمودی حرکت می‌کند)
    if (piece.type != QUEEN && (to_r + to_c) % 2 == 0) return 0;

    int dr = to_r - from_r;
    int dc = to_c - from_c;

    // حرکت عمودی یا افقی فقط مخصوص Queen است
    if (piece.type != QUEEN && dc == 0 && dr == 0) return 0; // هیچ حرکتی نیست

    // Queen: یک خانه عمودی/افقی یا پرش یک مهره عمودی/افقی
    if (piece.type == QUEEN) {
        // حرکت باید عمودی یا افقی باشد (نه ثابت و نه مورب)
        if (!((dc == 0 && dr != 0) || (dr == 0 && dc != 0))) return 0;

        // حرکت و پرشِ ویژهٔ QUEEN: پرش فقط وقتی رخ می‌دهد که مهرهٔ حریف دو خانه دور باشد
        // حرکت عادی دوخانه‌ای مجاز است (اگر میانه خالی باشد و پرش اجباری نباشد)
        if ((abs(dr) + abs(dc) == 2) && (abs(dr) == 2 || abs(dc) == 2)) {
            int between_r = from_r + (dr == 0 ? 0 : (dr / abs(dr)) * 1);
            int between_c = from_c + (dc == 0 ? 0 : (dc / abs(dc)) * 1);
            Checker between = game->board[between_r][between_c];
            if (between.type == EMPTY_C) {
                if (game->must_jump) return 0;
                return 1; // حرکت عادی دوخانه‌ای
            }
            return 0;
        }

        // پرش چهار خانه‌ای (حریف دو خانه دور و مقصد در خانهٔ چهارم)
        if ((abs(dr) + abs(dc) == 4) && (abs(dr) == 4 || abs(dc) == 4)) {
            int b1_r = from_r + (dr == 0 ? 0 : (dr / abs(dr)) * 1);
            int mid_r = from_r + (dr == 0 ? 0 : (dr / abs(dr)) * 2);
            int b2_r = from_r + (dr == 0 ? 0 : (dr / abs(dr)) * 3);
            int b1_c = from_c + (dc == 0 ? 0 : (dc / abs(dc)) * 1);
            int mid_c = from_c + (dc == 0 ? 0 : (dc / abs(dc)) * 2);
            int b2_c = from_c + (dc == 0 ? 0 : (dc / abs(dc)) * 3);

            Checker b1 = game->board[b1_r][b1_c];
            Checker mid = game->board[mid_r][mid_c];
            Checker b2 = game->board[b2_r][b2_c];

            if (b1.type == EMPTY_C && b2.type == EMPTY_C && mid.type != EMPTY_C && mid.color != piece.color) {
                return 1; // پرش چهارخانه‌ای معتبر
            }
        }

        return 0;
    }

    // حرکت عادی یک خانه مورب (فقط Man/King)
    if (abs(dr) == 1 && abs(dc) == 1) {
        // اگر پرش اجباری فعال است، حرکت عادی را ممنوع می‌کنم
        if (game->must_jump) return 0;

        if (piece.type == MAN) {
            if (piece.color == RED && dr > 0) return 0;  // قرمز فقط بالا
            if (piece.color == BLACK_C && dr < 0) return 0; // سیاه فقط پایین
        }

        return 1;
    }

    // حرکت پرشی دو خانه مورب، با وجود مهره حریف در میانه (Queen مجاز نیست)
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
    // QUEEN special capture: opponent two squares away -> capture by landing on fourth square
    if (moving.type == QUEEN && (abs(dr) == 4 || abs(dc) == 4) && (dr == 0 || dc == 0)) {
        int signr = (dr == 0) ? 0 : (dr > 0 ? 1 : -1);
        int signc = (dc == 0) ? 0 : (dc > 0 ? 1 : -1);
        int midr = from_r + signr * 2;
        int midc = from_c + signc * 2;
        Checker mid = game->board[midr][midc];
        if (mid.type != EMPTY_C && mid.color != moving.color) {
            game->board[midr][midc] = (Checker){EMPTY_C, RED};
            is_jump = 1;
        }
    } else if (moving.type == QUEEN && (abs(dr) == 2 || abs(dc) == 2) && (dr == 0 || dc == 0)) {
        // two-square non-capture move -> do not remove anything
        is_jump = 0;
    } else if (abs(dr) == 2 && abs(dc) == 2) {
        int mr = from_r + dr / 2;
        int mc = from_c + dc / 2;
        Checker mid = game->board[mr][mc];
        if (mid.type != EMPTY_C && mid.color != moving.color) {
            game->board[mr][mc] = (Checker){EMPTY_C, RED};
            is_jump = 1;
        }
    }

    game->board[to_r][to_c] = moving;
    game->board[from_r][from_c] = (Checker){EMPTY_C, RED};

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
        strcpy(game->message, "سفید برنده شد!");
    } else if (!has_any_move_for(game, game->current_turn)) {
        game->game_over = 1;
        game->winner = (game->current_turn == RED) ? BLACK_C : RED;
        sprintf(game->message, "%s برنده شد (%s حرکتی ندارد)",
                (game->winner == RED) ? "سفید" : "سیاه",
                (game->current_turn == RED) ? "سفید" : "سیاه");
    } else {
        sprintf(game->message, "نوبت %s%s",
                (game->current_turn == RED) ? "سفید" : "سیاه",
                game->must_jump ? " (پرش اجباری!)" : "");
    }
}

// ماتریس حرکات مجاز را برای هایلایت UI محاسبه می‌کنم
void get_valid_checkers_moves(CheckersState *game, int row, int col, int valid[8][8]) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            valid[i][j] = 0;

    if (game->board[row][col].type == EMPTY_C) return;

    // در حالت پرش زنجیره‌ای فقط همان مهرهٔ انتخاب‌شده اجازه حرکت دارد
    if (game->multi_jump) {
        if (game->jump_from_r != row || game->jump_from_c != col) return;
    }

    // حالا برای هر مقصد ممکن، دو حالت را بررسی می‌کنیم:
    //  - 1 : حرکت واقعاً مجاز است (نمایش سبز روشن)
    //  - 2 : اگر پرش اجباری غیرفعال بود، این حرکت مجاز می‌بود (نمایش سبزِ مرده‌تر)
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            // ابتدا همچون قبل حرکتِ واقعی را چک کن
            if (is_valid_checkers_move(game, row, col, i, j)) {
                valid[i][j] = 1;
                continue;
            }

            // اگر پرش اجباری فعال است، بررسی کن که آیا همین حرکت در حالت بدون پرشِ اجباری مجاز می‌بود
            if (game->must_jump) {
                int saved_must = game->must_jump;
                int saved_jr = game->jump_from_r;
                int saved_jc = game->jump_from_c;

                game->must_jump = 0;
                game->jump_from_r = -1;
                game->jump_from_c = -1;

                if (is_valid_checkers_move(game, row, col, i, j)) {
                    valid[i][j] = 2; // نمایشِ مرده‌تر
                }

                game->must_jump = saved_must;
                game->jump_from_r = saved_jr;
                game->jump_from_c = saved_jc;
            }
        }
    }
}
