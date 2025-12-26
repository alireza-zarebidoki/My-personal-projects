// وضعیت شطرنج را اینجا مقداردهی و برای تبادل در URL سریال‌سازی می‌کنم

#include "chess.h"

// صفحه را با چیدمان سفارشی (Dragon/Thief/Gryphon) مقداردهی اولیه می‌کنم
void init_board(GameState *game) {
    // خالی کردن صفحه
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            game->board[i][j].type = EMPTY;
            game->board[i][j].color = WHITE;
        }

    // ردیف اول سیاه (مهره‌های سنگین)
    game->board[0][0] = (Piece){GRYPHON, BLACK};  // جایگزین رخ
    game->board[0][1] = (Piece){DRAGON, BLACK};   // جایگزین اسب
    game->board[0][2] = (Piece){THIEF, BLACK};    // جایگزین فیل
    game->board[0][3] = (Piece){QUEEN, BLACK};
    game->board[0][4] = (Piece){KING, BLACK};
    game->board[0][5] = (Piece){THIEF, BLACK};
    game->board[0][6] = (Piece){DRAGON, BLACK};
    game->board[0][7] = (Piece){GRYPHON, BLACK};

    // ردیف دوم سیاه (پیاده‌ها)
    for (int j = 0; j < 8; j++)
        game->board[1][j] = (Piece){PAWN, BLACK};

    // ردیف هفتم سفید (پیاده‌ها)
    for (int j = 0; j < 8; j++)
        game->board[6][j] = (Piece){PAWN, WHITE};

    // ردیف هشتم سفید (مهره‌های سنگین)
    game->board[7][0] = (Piece){GRYPHON, WHITE};
    game->board[7][1] = (Piece){DRAGON, WHITE};
    game->board[7][2] = (Piece){THIEF, WHITE};
    game->board[7][3] = (Piece){QUEEN, WHITE};
    game->board[7][4] = (Piece){KING, WHITE};
    game->board[7][5] = (Piece){THIEF, WHITE};
    game->board[7][6] = (Piece){DRAGON, WHITE};
    game->board[7][7] = (Piece){GRYPHON, WHITE};

    game->current_turn = WHITE;
    game->selected_row = -1;
    game->selected_col = -1;
    game->white_time = 600;  // 10 دقیقه (600 ثانیه)
    game->black_time = 600;  // 10 دقیقه (600 ثانیه)
    game->last_move_time = time(NULL);
    game->game_over = 0;
    game->winner = 0;
    strcpy(game->message, "نوبت سفید");

    update_king_positions(game);
}

// مختصات شاه‌ها را برای هر دو رنگ به‌روزرسانی می‌کنم تا چک/مات سریع‌تر چک شود
void update_king_positions(GameState *game) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (game->board[i][j].type == KING) {
                if (game->board[i][j].color == WHITE) {
                    game->white_king_pos[0] = i;
                    game->white_king_pos[1] = j;
                } else {
                    game->black_king_pos[0] = i;
                    game->black_king_pos[1] = j;
                }
            }
        }
    }
}

// کل صفحه و متادیتا را به رشته فشرده تبدیل می‌کنم تا در query string جا شود
void serialize_state(GameState *game, char *buffer) {
    char temp[2048] = "";

    // ذخیره صفحه (64 خانه، هر کدام 2 رقم: نوع مهره + رنگ)
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char cell[4];
            sprintf(cell, "%d%d", game->board[i][j].type,
                    game->board[i][j].type == EMPTY ? 0 : game->board[i][j].color);
            strcat(temp, cell);
        }
    }

    sprintf(buffer, "%s|%d|%d|%d|%d|%d|%d|%ld",
            temp,
            game->current_turn,
            game->selected_row,
            game->selected_col,
            game->game_over,
            game->white_time,
            game->black_time,
            game->last_move_time);
}

// رشته فشرده را دوباره به ساختار بازی برمی‌گردانم؛ اگر ناقص بود از init استفاده می‌کنم
void deserialize_state(const char *buffer, GameState *game) {
    if (!buffer || strlen(buffer) < 128) {
        init_board(game);
        return;
    }

    char temp[2048];
    strncpy(temp, buffer, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    // پارس کردن داده‌ها
    char *tok = strtok(temp, "|");
    if (!tok) { init_board(game); return; }

    // بازسازی صفحه
    int idx = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (idx + 1 < (int)strlen(tok)) {
                game->board[i][j].type = tok[idx] - '0';
                game->board[i][j].color = tok[idx + 1] - '0';
                idx += 2;
            }
        }
    }

    // بازسازی بقیه داده‌ها
    tok = strtok(NULL, "|");
    game->current_turn = tok ? atoi(tok) : WHITE;

    tok = strtok(NULL, "|");
    game->selected_row = tok ? atoi(tok) : -1;

    tok = strtok(NULL, "|");
    game->selected_col = tok ? atoi(tok) : -1;

    tok = strtok(NULL, "|");
    game->game_over = tok ? atoi(tok) : 0;

    tok = strtok(NULL, "|");
    game->white_time = tok ? atoi(tok) : 600;

    tok = strtok(NULL, "|");
    game->black_time = tok ? atoi(tok) : 600;

    tok = strtok(NULL, "|");
    game->last_move_time = tok ? atol(tok) : time(NULL);

    update_king_positions(game);
}

// یونیکد هر مهره را برمی‌گردانم تا در UI آیکن بگذارم
const char* piece_to_unicode(PieceType type, Color color) {
    if (type == EMPTY) return "";

    // یونیکد مهره‌های سفید
    if (color == WHITE) {
        switch(type) {
            case PAWN: return "♙";
            case DRAGON: return "🐉";  // به جای اسب
            case THIEF: return "🥷";   // به جای فیل
            case GRYPHON: return "🦅"; // به جای رخ
            case QUEEN: return "♕";
            case KING: return "♔";
            default: return "?";
        }
    } else {
        // یونیکد مهره‌های سیاه
        switch(type) {
            case PAWN: return "♟";
            case DRAGON: return "🐲";
            case THIEF: return "🦹";
            case GRYPHON: return "🦜";
            case QUEEN: return "♛";
            case KING: return "♚";
            default: return "?";
        }
    }
}

// نام فارسی هر مهره را برای توضیحات UI برمی‌گردانم
const char* piece_name(PieceType type) {
    switch(type) {
        case PAWN: return "سرباز";
        case DRAGON: return "اژدها";
        case THIEF: return "دزد";
        case GRYPHON: return "گریفون";
        case QUEEN: return "وزیر";
        case KING: return "شاه";
        default: return "";
    }
}
