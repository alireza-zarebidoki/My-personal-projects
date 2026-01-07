/**
 * @file src/games/chess/logic.c
 * @project arcade-system
 * @author Alireza Zarebidoki
 * @date_created 2024
 * @date_modified 2024
 * @brief Chess move validation and game logic for modified pieces.
 *
 * @details
 * این فایل تمام منطق حرکت، کیش، و مات شطرنج سفارشی را نگه می‌دارد.
 * هر مهره (شامل مهره‌های سفارشی مثل Dragon و Thief) حرکات خاصی دارد که اینجا پیاده‌سازی شده است.
 *
 * @responsibilities
 * - بررسی صحت حرکت برای هر نوع مهره
 * - بررسی مسیر خالی برای مهره‌های لغزنده (Rook-like)
 * - پیاده‌سازی حرکات خاص Dragon و Thief
 * - شناسایی وضعیت کیش (Checkتهدید شاه)
 * - شناسایی وضعیت مات (بدون حرکت معتبر)
 * - جلوگیری از حرکاتی که شاه را در خطر قرار می‌دهند
 *
 * @interactions
 * - استفاده از chess.h برای تعریف‌های داده‌ای
 * - استفاده از state.c برای دسترسی به state
 * - فراخوانی توسط main.c برای تأیید حرکات
 *
 * @notes
 * - Dragon: ترکیب اسب و شاه (7 خانه اطراف و 2 خانه L شکل)
 * - Thief: فیل با قدرت پرش از 1 مانع (حرکت مورب تا 1 مانع)
 * - Gryphon: ترکیب رخ و 1 خانه مورب
 * - شاه نمی‌تواند به خانه‌ای برود که تهدید باشد
 */

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

// برای رخ مسیر را خانه‌به‌خانه می‌روم و به محض برخورد به مهره متوقف می‌شوم
static int is_rook_path_clear(GameState *game, int from_r, int from_c, int to_r, int to_c) {
    if (!(from_r == to_r || from_c == to_c)) return 0;

    int dr = (to_r > from_r) ? 1 : ((to_r < from_r) ? -1 : 0);
    int dc = (to_c > from_c) ? 1 : ((to_c < from_c) ? -1 : 0);

    int r = from_r + dr;
    int c = from_c + dc;
    int blocked = 0;

    while (r != to_r || c != to_c) {
        if (game->board[r][c].type != EMPTY) {
            blocked = 1;
            break; // به محض رسیدن به مانع مسیر را متوقف می‌کنم
        }
        r += dr;
        c += dc;
    }

    return blocked ? 0 : 1;
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

// بررسی می‌کنم یک مهره خاص می‌تواند خانه مقصد را تهدید کند یا نه (صرفاً از منظر حمله)
static int can_attack_square(GameState *game, int from_r, int from_c, int to_r, int to_c) {
    Piece piece = game->board[from_r][from_c];
    int dr = abs(to_r - from_r);
    int dc = abs(to_c - from_c);

    switch (piece.type) {
        case PAWN: {
            int direction = (piece.color == WHITE) ? -1 : 1;
            return (to_r == from_r + direction && dc == 1);
        }

        case DRAGON: {
            if ((dr == 2 && dc == 1) || (dr == 1 && dc == 2)) return 1;
            if (dr <= 1 && dc <= 1 && (dr + dc > 0)) return 1;
            return 0;
        }

        case THIEF: {
            if (dr != dc || dr == 0) return 0;
            int obstacles = count_obstacles_diagonal(game, from_r, from_c, to_r, to_c);
            return (obstacles <= 1);
        }

        case GRYPHON: {
            int dirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

            for (int i = 0; i < 4; i++) {
                int mid_r = from_r + dirs[i][0];
                int mid_c = from_c + dirs[i][1];

                if (mid_r < 0 || mid_r > 7 || mid_c < 0 || mid_c > 7) continue;

                Piece mid_piece = game->board[mid_r][mid_c];

                // اگر خانه مورب پر از مهره همرنگ باشد، این جهت مسدود است
                if (mid_piece.type != EMPTY && mid_piece.color == piece.color) continue;

                // اگر روی همان خانه مورب فرود بیاییم، فقط وقتی مجاز است که خالی باشد یا مهره حریف باشد
                if (to_r == mid_r && to_c == mid_c) return 1;

                // ادامه مثل رخ فقط وقتی خانه مورب خالی باشد
                if (mid_piece.type == EMPTY) {
                    // چهار جهت رخ از خانه مورب را قدم‌به‌قدم بررسی می‌کنم
                    int rook_dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
                    for (int k = 0; k < 4; k++) {
                        int r = mid_r + rook_dirs[k][0];
                        int c = mid_c + rook_dirs[k][1];

                        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
                            if (game->board[r][c].type != EMPTY) {
                                if (r == to_r && c == to_c && game->board[r][c].color != piece.color)
                                    return 1; // اولین مهره حریف قابل capture است
                                break; // مانع اول مسیر را می‌بندد
                            }

                            if (r == to_r && c == to_c) return 1; // خانه خالی در مسیر رخ

                            r += rook_dirs[k][0];
                            c += rook_dirs[k][1];
                        }
                    }
                }
            }
            return 0;
        }

        case ROOK: {
            if (dr == 0 || dc == 0) {
                return is_rook_path_clear(game, from_r, from_c, to_r, to_c);
            }
            return 0;
        }

        case QUEEN: {
            if (dr == 0 || dc == 0 || dr == dc) {
                return is_path_clear(game, from_r, from_c, to_r, to_c);
            }
            if ((dr == 2 && dc == 1) || (dr == 1 && dc == 2)) return 1;
            return 0;
        }

        case KING: {
            if (dr == 0 && dc == 0) return 0;
            if (dr > 2 || dc > 2) return 0;
            if (!(dr == 0 || dc == 0 || dr == dc)) return 0;

            if (dr <= 1 && dc <= 1) return 1;

            return is_path_clear(game, from_r, from_c, to_r, to_c);
        }

        default:
            return 0;
    }
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
            // اول یک خانه مورب اجباری، سپس در صورت خالی بودن آن خانه ادامه حرکت رخ
            int dirs[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

            for (int i = 0; i < 4; i++) {
                int mid_r = from_r + dirs[i][0];
                int mid_c = from_c + dirs[i][1];

                if (mid_r < 0 || mid_r > 7 || mid_c < 0 || mid_c > 7) continue;

                Piece mid_piece = game->board[mid_r][mid_c];

                if (mid_piece.type != EMPTY && mid_piece.color == piece.color) continue; // همرنگ مانع است

                // حرکت فقط به خانه مورب (توقف یا زدن)
                if (to_r == mid_r && to_c == mid_c) return 1;

                // اگر خانه مورب خالی بود می‌توان مثل رخ ادامه داد
                if (mid_piece.type == EMPTY) {
                    int rook_dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
                    for (int k = 0; k < 4; k++) {
                        int r = mid_r + rook_dirs[k][0];
                        int c = mid_c + rook_dirs[k][1];

                        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
                            if (game->board[r][c].type != EMPTY) {
                                if (r == to_r && c == to_c && game->board[r][c].color != piece.color)
                                    return 1; // اولین مهره حریف قابل capture است
                                break; // مانع اول مسیر را می‌بندد
                            }

                            if (r == to_r && c == to_c) return 1; // خانه خالی در مسیر رخ

                            r += rook_dirs[k][0];
                            c += rook_dirs[k][1];
                        }
                    }
                }
            }
            return 0;
        }

        case ROOK: {
            if (dr == 0 || dc == 0) {
                return is_rook_path_clear(game, from_r, from_c, to_r, to_c);
            }
            return 0;
        }

        case QUEEN: {
            // حرکت افقی، عمودی یا مورب معمولی وزیر را هم اینجا می‌سنجم
            if (dr == 0 || dc == 0 || dr == dc) {
                return is_path_clear(game, from_r, from_c, to_r, to_c);
            }
            // Adding knight-style jump logic for the queen
            if ((dr == 2 && dc == 1) || (dr == 1 && dc == 2)) return 1;

            return 0;
        }

        case KING: {
            // شاه می‌تواند یک یا دو خانه در هر جهت (افقی، عمودی، مورب) برود بدون پرش
            if (dr == 0 && dc == 0) return 0;
            if (dr > 2 || dc > 2) return 0;
            if (!(dr == 0 || dc == 0 || dr == dc)) return 0;

            if (dr <= 1 && dc <= 1) return 1;

            return is_path_clear(game, from_r, from_c, to_r, to_c);
        }

        default:
            return 0;
    }
}

// بررسی می‌کنم شاه رنگ موردنظر زیر ضرب است یا نه
int is_in_check(GameState *game, Color color) {
    int king_r, king_c;

    Color attacker = (color == WHITE) ? BLACK : WHITE;

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
            if (p.type != EMPTY && p.color == attacker) {
                if (can_attack_square(game, i, j, king_r, king_c)) {
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
