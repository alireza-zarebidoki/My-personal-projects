// وضعیت چکرز را اینجا می‌سازم و بین ریکوئست‌ها کدگذاری/بازکد می‌کنم

#include "checkers.h"

// مقداردهی اولیه صفحه چکرز
void init_checkers_board(CheckersState *game) {
    // اول صفحه را کامل خالی می‌کنم
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            game->board[i][j] = (Checker){EMPTY_C, RED};

    // مهره‌های سیاه را در سه ردیف بالا می‌چینم
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 8; j++) {
            if ((i + j) % 2 == 1) { // فقط خانه‌های تیره
                game->board[i][j] = (Checker){MAN, BLACK_C};
            }
        }
    }

    // مهره‌های قرمز را در سه ردیف پایین می‌چینم
    for (int i = 5; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((i + j) % 2 == 1) {
                game->board[i][j] = (Checker){MAN, RED};
            }
        }
    }

    game->current_turn = RED;
    game->selected_row = -1;
    game->selected_col = -1;
    game->must_jump = 0;
    game->jump_from_r = -1;
    game->jump_from_c = -1;
    game->multi_jump = 0;
    game->game_over = 0;
    game->winner = 0;
    strcpy(game->message, "نوبت قرمز");
}

// تبدیل وضعیت به رشته فشرده برای URL
void serialize_checkers_state(CheckersState *game, char *buffer) {
    char temp[2048] = "";

    // صفحه را خانه‌به‌خانه به رشته تبدیل می‌کنم
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char cell[4];
            sprintf(cell, "%d%d", game->board[i][j].type,
                    game->board[i][j].type == EMPTY_C ? 0 : game->board[i][j].color);
            strcat(temp, cell);
        }
    }

    sprintf(buffer, "%s|%d|%d|%d|%d|%d|%d|%d|%d",
            temp,
            game->current_turn,
            game->selected_row,
            game->selected_col,
            game->must_jump,
            game->jump_from_r,
            game->jump_from_c,
            game->multi_jump,
            game->game_over);
}

// بازسازی وضعیت از رشته کدگذاری‌شده
void deserialize_checkers_state(const char *buffer, CheckersState *game) {
    if (!buffer || strlen(buffer) < 128) {
        init_checkers_board(game);
        return;
    }

    char temp[2048];
    strncpy(temp, buffer, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    char *tok = strtok(temp, "|");
    if (!tok) { init_checkers_board(game); return; }

    // صفحه را از روی رشته دوباره پر می‌کنم
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

    tok = strtok(NULL, "|");
    game->current_turn = tok ? atoi(tok) : RED;

    tok = strtok(NULL, "|");
    game->selected_row = tok ? atoi(tok) : -1;

    tok = strtok(NULL, "|");
    game->selected_col = tok ? atoi(tok) : -1;

    tok = strtok(NULL, "|");
    game->must_jump = tok ? atoi(tok) : 0;

    tok = strtok(NULL, "|");
    game->jump_from_r = tok ? atoi(tok) : -1;

    tok = strtok(NULL, "|");
    game->jump_from_c = tok ? atoi(tok) : -1;

    tok = strtok(NULL, "|");
    game->multi_jump = tok ? atoi(tok) : 0;

    tok = strtok(NULL, "|");
    game->game_over = tok ? atoi(tok) : 0;
}

// یونیکد هر مهره را برای نمایش در UI برمی‌گردانم
const char* checker_to_unicode(CheckerType type, CheckerColor color) {
    if (type == EMPTY_C) return "";

    if (color == RED) {
        switch(type) {
            case MAN: return "🔴";
            case KING: return "👑"; // King قرمز
            case QUEEN: return "💎"; // Queen قرمز
            default: return "?";
        }
    } else {
        switch(type) {
            case MAN: return "⚫";
            case KING: return "♔"; // King سیاه
            case QUEEN: return "♕"; // Queen سیاه
            default: return "?";
        }
    }
}
