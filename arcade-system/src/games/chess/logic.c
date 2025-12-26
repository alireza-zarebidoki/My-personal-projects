// این فایل را گذاشتم تا تمام منطق حرکت، کیش و مات شطرنج سفارشی همین‌جا متمرکز باشد

#include "chess.h"
#include <stdlib.h>

// مسیر بین مبدا و مقصد را قدم‌به‌قدم چک می‌کنم تا مطمئن شوم مهره‌های لغزنده از روی چیزی نمی‌پرند
int is_path_clear(GameState *game, int from_r, int from_c, int to_r, int to_c) {
    int dr = (to_r > from_r) ? 1 : ((to_r < from_r) ? -1 : 0);
    int dc = (to_c > from_c) ? 1 : ((to_c < from_c) ? -1 : 0);

    int r = from_r + dr;
    int c = from_c + dc;

    while (r != to_r || c != to_c) {
        if (game->board[r][c].type != EMPTY) return 0;
        r += dr;
        c += dc;
    }
    return 1;
}

// برای مهره Thief می‌شمارم چند مانع در مسیر مورب وجود دارد تا محدودیت یک مانع را اعمال کنم
int count_obstacles_diagonal(GameState *game, int from_r, int from_c, int to_r, int to_c) {
    int dr = (to_r > from_r) ? 1 : -1;
    int dc = (to_c > from_c) ? 1 : -1;
    int count = 0;

    int r = from_r + dr;
    int c = from_c + dc;

    while (r != to_r || c != to_c) {
        if (game->board[r][c].type != EMPTY) count++;
        r += dr;
        c += dc;
    }
    return count;
}

// منطق خام اعتبار حرکت هر مهره را اینجا به صورت قطعه‌ای پیاده کردم
int is_valid_move_logic(GameState *game, int from_r, int from_c, int to_r, int to_c) {
    Piece piece = game->board[from_r][from_c];
    Piece target = game->board[to_r][to_c];

    // نمی‌توان مهره خودی را بخورد
    if (target.type != EMPTY && target.color == piece.color) return 0;

    int dr = abs(to_r - from_r);
    int dc = abs(to_c - from_c);

    switch (piece.type) {
        case PAWN: {
            int direction = (piece.color == WHITE) ? -1 : 1;
            // حرکت یک خانه به جلو
            if (to_c == from_c && to_r == from_r + direction && target.type == EMPTY)
                return 1;
            // حرکت دو خانه از موقعیت اولیه
            int start_row = (piece.color == WHITE) ? 6 : 1;
            if (from_r == start_row && to_c == from_c && to_r == from_r + 2 * direction &&
                target.type == EMPTY && game->board[from_r + direction][from_c].type == EMPTY)
                return 1;
            // خوردن مهره حریف
            if (abs(to_c - from_c) == 1 && to_r == from_r + direction &&
                target.type != EMPTY && target.color != piece.color)
                return 1;
            return 0;
        }

        case DRAGON: {
            // این مهره را ترکیبی از اسب و شاه طراحی کردم
            // حرکت اسب (L-shape)
            if ((dr == 2 && dc == 1) || (dr == 1 && dc == 2)) return 1;
            // حرکت شاه (یک خانه به هر جهت)
            if (dr <= 1 && dc <= 1 && (dr + dc > 0)) return 1;
            return 0;
        }

        case THIEF: {
            // مثل فیل حرکت می‌کنم اما اجازه یک مانع در مسیر می‌دهم
            if (dr != dc || dr == 0) return 0;
            int obstacles = count_obstacles_diagonal(game, from_r, from_c, to_r, to_c);
            return (obstacles <= 1);
        }

        case GRYPHON: {
            // ابتدا یک خانه مورب می‌روم و بعد مثل رخ ادامه می‌دهم؛ مقصد باید روی همان خط افقی/عمودیِ خانه مورب باشد
            int diag_positions[4][2] = {
                {from_r - 1, from_c - 1}, {from_r - 1, from_c + 1},
                {from_r + 1, from_c - 1}, {from_r + 1, from_c + 1}
            };

            for (int i = 0; i < 4; i++) {
                int mid_r = diag_positions[i][0];
                int mid_c = diag_positions[i][1];

                if (mid_r < 0 || mid_r > 7 || mid_c < 0 || mid_c > 7) continue;

                // بررسی اینکه مقصد در راستای افقی یا عمودی این خانه مورب است
                if ((to_r == mid_r && to_c != mid_c) || (to_c == mid_c && to_r != mid_r)) {
                    // بررسی خالی بودن مسیر از خانه مورب تا مقصد
                    if (is_path_clear(game, mid_r, mid_c, to_r, to_c)) return 1;
                }
            }
            return 0;
        }

        case QUEEN: {
            // حرکت افقی، عمودی یا مورب معمولی وزیر را هم اینجا می‌سنجم
            if (dr == 0 || dc == 0 || dr == dc) {
                return is_path_clear(game, from_r, from_c, to_r, to_c);
            }
            return 0;
        }

        case KING: {
            // شاه فقط یک خانه به هر جهت حرکت می‌کند
            return (dr <= 1 && dc <= 1 && (dr + dc > 0));
        }

        default:
            return 0;
    }
}

// بررسی می‌کنم شاه رنگ موردنظر زیر ضرب است یا نه
int is_in_check(GameState *game, Color color) {
    int king_r, king_c;

    if (color == WHITE) {
        king_r = game->white_king_pos[0];
        king_c = game->white_king_pos[1];
    } else {
        king_r = game->black_king_pos[0];
        king_c = game->black_king_pos[1];
    }

    // بررسی تمام مهره‌های حریف
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece p = game->board[i][j];
            if (p.type != EMPTY && p.color != color) {
                if (is_valid_move_logic(game, i, j, king_r, king_c)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

// با شبیه‌سازی حرکت مطمئن می‌شوم خودکیش اتفاق نیفتد
int is_valid_move(GameState *game, int from_r, int from_c, int to_r, int to_c) {
    // بررسی منطق اولیه
    if (!is_valid_move_logic(game, from_r, from_c, to_r, to_c)) return 0;

    // شبیه‌سازی حرکت
    Piece temp_target = game->board[to_r][to_c];
    Piece moving = game->board[from_r][from_c];

    game->board[to_r][to_c] = moving;
    game->board[from_r][from_c] = (Piece){EMPTY, WHITE};

    // به‌روزرسانی موقعیت شاه در صورت لزوم
    int old_king_pos[2];
    if (moving.type == KING) {
        if (moving.color == WHITE) {
            old_king_pos[0] = game->white_king_pos[0];
            old_king_pos[1] = game->white_king_pos[1];
            game->white_king_pos[0] = to_r;
            game->white_king_pos[1] = to_c;
        } else {
            old_king_pos[0] = game->black_king_pos[0];
            old_king_pos[1] = game->black_king_pos[1];
            game->black_king_pos[0] = to_r;
            game->black_king_pos[1] = to_c;
        }
    }

    // بعد از شبیه‌سازی، وضعیت کیش را ارزیابی می‌کنم
    int in_check = is_in_check(game, moving.color);

    // بازگرداندن وضعیت
    game->board[from_r][from_c] = moving;
    game->board[to_r][to_c] = temp_target;

    if (moving.type == KING) {
        if (moving.color == WHITE) {
            game->white_king_pos[0] = old_king_pos[0];
            game->white_king_pos[1] = old_king_pos[1];
        } else {
            game->black_king_pos[0] = old_king_pos[0];
            game->black_king_pos[1] = old_king_pos[1];
        }
    }

    return !in_check;
}

// ماتریس حرکات مجاز برای هایلایت کردن در UI را پر می‌کنم
void get_valid_moves(GameState *game, int row, int col, int valid[8][8]) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            valid[i][j] = 0;

    if (game->board[row][col].type == EMPTY) return;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (is_valid_move(game, row, col, i, j)) {
                valid[i][j] = 1;
            }
        }
    }
}

// حرکت را اعمال می‌کنم و هم‌زمان زمان بازیکن فعلی و ارتقاء پیاده را مدیریت می‌کنم
void make_move(GameState *game, int from_r, int from_c, int to_r, int to_c) {
    Piece moving = game->board[from_r][from_c];

    // زمان سپری‌شده از آخرین حرکت را حساب می‌کنم تا تایمر رقابتی باقی بماند
    long current_time = time(NULL);
    long elapsed = current_time - game->last_move_time;

    // کاهش زمان بازیکن فعلی
    if (game->current_turn == WHITE) {
        game->white_time -= elapsed;
        if (game->white_time < 0) game->white_time = 0;
    } else {
        game->black_time -= elapsed;
        if (game->black_time < 0) game->black_time = 0;
    }

    game->last_move_time = current_time;

    // اگر پیاده به انتهای زمین برسد همین‌جا به وزیر تبدیلش می‌کنم (انتخاب نوع در main.c قابل توسعه است)
    if (moving.type == PAWN) {
        int promotion_row = (moving.color == WHITE) ? 0 : 7;
        if (to_r == promotion_row) {
            // ارتقاء به وزیر (می‌توان در main.c انتخاب کاربر را اضافه کرد)
            moving.type = QUEEN;
        }
    }

    game->board[to_r][to_c] = moving;
    game->board[from_r][from_c] = (Piece){EMPTY, WHITE};

    update_king_positions(game);

    // در پایان نوبت را عوض می‌کنم
    game->current_turn = (game->current_turn == WHITE) ? BLACK : WHITE;
}

// کیش‌مات را وقتی اعلام می‌کنم که رنگ در کیش باشد و هیچ حرکت معتبری برایش باقی نماند
int is_checkmate(GameState *game, Color color) {
    if (!is_in_check(game, color)) return 0;

    // بررسی تمام حرکات ممکن
    for (int fr = 0; fr < 8; fr++) {
        for (int fc = 0; fc < 8; fc++) {
            Piece p = game->board[fr][fc];
            if (p.type != EMPTY && p.color == color) {
                for (int tr = 0; tr < 8; tr++) {
                    for (int tc = 0; tc < 8; tc++) {
                        if (is_valid_move(game, fr, fc, tr, tc)) {
                            return 0; // حداقل یک حرکت معتبر وجود دارد
                        }
                    }
                }
            }
        }
    }
    return 1; // کیش‌مات
}

// پات را زمانی گزارش می‌کنم که شاه در امان باشد اما هیچ حرکت معتبری وجود نداشته باشد
int is_stalemate(GameState *game, Color color) {
    if (is_in_check(game, color)) return 0; // اگر در کیش باشد، پات نیست

    // بررسی تمام حرکات ممکن
    for (int fr = 0; fr < 8; fr++) {
        for (int fc = 0; fc < 8; fc++) {
            Piece p = game->board[fr][fc];
            if (p.type != EMPTY && p.color == color) {
                for (int tr = 0; tr < 8; tr++) {
                    for (int tc = 0; tc < 8; tc++) {
                        if (is_valid_move(game, fr, fc, tr, tc)) {
                            return 0; // حرکت معتبر پیدا شد
                        }
                    }
                }
            }
        }
    }
    return 1; // پات: هیچ حرکت معتبری وجود ندارد
}
