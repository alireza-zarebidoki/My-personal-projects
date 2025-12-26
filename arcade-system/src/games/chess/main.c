// این فایل CGI اصلی شطرنج تغییر‌یافته است؛ رندر، دریافت کلیک و پایان بازی را خودم مدیریت می‌کنم

#include "chess.h"

// رنگ پس‌زمینه هر خانه را بر اساس مختصات برمی‌گردانم تا الگوی کلاسیک حفظ شود
const char* cell_bg_color(int row, int col) {
    return ((row + col) % 2 == 0) ? "#909090" : "#c0c0c0";
}

// تخته را با هایلایت حرکات مجاز و لینک state رندر می‌کنم
void render_board(GameState *game, int highlight[8][8]) {
    printf("<div style='display:grid; grid-template-columns:repeat(8,50px); gap:0; margin:15px auto; width:fit-content; border:3px solid #333;'>");

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece p = game->board[i][j];
            char bg[100];

            // رنگ پس‌زمینه
            if (game->selected_row == i && game->selected_col == j) {
                sprintf(bg, "#f1c40f"); // خانه انتخاب شده - زرد
            } else if (highlight && highlight[i][j]) {
                sprintf(bg, "#00cc00"); // حرکات معتبر - سبز روشن
            } else if (game->current_turn == WHITE && game->white_king_pos[0] == i && game->white_king_pos[1] == j && is_in_check(game, WHITE)) {
                sprintf(bg, "#e74c3c"); // شاه سفید در کیش
            } else if (game->current_turn == BLACK && game->black_king_pos[0] == i && game->black_king_pos[1] == j && is_in_check(game, BLACK)) {
                sprintf(bg, "#e74c3c"); // شاه سیاه در کیش
            } else {
                sprintf(bg, "%s", cell_bg_color(i, j));
            }

            // لینک برای کلیک
            char state_str[2048];
            serialize_state(game, state_str);

            printf("<a href='chess_modified.cgi?state=%s&click=%d%d' style='text-decoration:none;'>", state_str, i, j);
            printf("<div style='width:50px;height:50px;background:%s;display:flex;align-items:center;justify-content:center;font-size:1.8rem;cursor:pointer;border:1px solid #777;'>", bg);

            if (p.type != EMPTY) {
                const char *icon = "";
                const char *color = (p.color == WHITE) ? "#fff" : "#000";

                switch(p.type) {
                    case PAWN: icon = "fas fa-chess-pawn"; break;
                    case GRYPHON: icon = "fas fa-chess-rook"; break;
                    case DRAGON: icon = "fas fa-dragon"; break;
                    case THIEF: icon = "fas fa-user-secret"; break;
                    case QUEEN: icon = "fas fa-chess-queen"; break;
                    case KING: icon = "fas fa-chess-king"; break;
                    default: icon = "fas fa-circle"; break;
                }

                printf("<i class='%s' style='color:%s; filter:drop-shadow(0 0 2px #000);'></i>", icon, color);
            }

            printf("</div></a>");
        }
    }

    printf("</div>");
}

int main() {
    print_header("شطرنج تغییر یافته");

    // کمی padding بالا را کم می‌کنم تا داخل iframe جای بیشتری داشته باشم
    printf("<style>.game-container{padding-top:4px!important;}</style>");

    GameState game;
    char buffer[MAX_BUFFER];

    // وضعیت ذخیره‌شده در URL را بازخوانی می‌کنم؛ اگر چیزی نبود صفحه را از صفر می‌سازم
    get_param("state", buffer);
    if (buffer[0]) {
        deserialize_state(buffer, &game);

        // به‌روزرسانی زمان بازیکن فعلی
        if (!game.game_over) {
            long current_time = time(NULL);
            long elapsed = current_time - game.last_move_time;

            if (game.current_turn == WHITE) {
                game.white_time -= elapsed;
                if (game.white_time <= 0) {
                    game.white_time = 0;
                    game.game_over = 1;
                    game.winner = BLACK;
                    strcpy(game.message, "زمان سفید تمام شد! سیاه برنده شد!");
                }
            } else {
                game.black_time -= elapsed;
                if (game.black_time <= 0) {
                    game.black_time = 0;
                    game.game_over = 1;
                    game.winner = WHITE;
                    strcpy(game.message, "زمان سیاه تمام شد! سفید برنده شد!");
                }
            }

            game.last_move_time = current_time;
        }
    } else {
        init_board(&game);
    }

    // تایمر هر بازیکن را بالای صفحه نشان می‌دهم تا بازی شبیه مسابقه زمانی شود
    printf("<div style='display:grid; grid-template-columns:1fr 1fr; gap:50px; padding:15px 30px; text-align:center; border-bottom:2px solid #00ff00;'>");
    printf("<div><div style='color:#888; font-size:0.8em;'>Black (P2)</div><div id='black-timer' style='color:#fff; font-size:1.5em; font-weight:bold; font-family:monospace;'>%d:%02d</div></div>",
           game.black_time / 60, game.black_time % 60);
    printf("<div><div style='color:#888; font-size:0.8em;'>White (P1)</div><div id='white-timer' style='color:#fff; font-size:1.5em; font-weight:bold; font-family:monospace;'>%d:%02d</div></div>",
           game.white_time / 60, game.white_time % 60);
    printf("</div>");

    // کلیک کاربر را دریافت می‌کنم؛ اگر مهره‌ای انتخاب شده باشد تلاش به حرکت می‌کنم
    get_param("click", buffer);
    if (buffer[0] && strlen(buffer) == 2 && !game.game_over) {
        int click_r = buffer[0] - '0';
        int click_c = buffer[1] - '0';

        if (click_r >= 0 && click_r < 8 && click_c >= 0 && click_c < 8) {
            Piece clicked = game.board[click_r][click_c];

            // اگر قبلاً مهره‌ای انتخاب شده
            if (game.selected_row != -1) {
                // تلاش برای حرکت
                if (is_valid_move(&game, game.selected_row, game.selected_col, click_r, click_c)) {
                    // اگر پیاده به انتها برسد اینجا منوی ارتقاء را نشان می‌دهم
                    Piece moving = game.board[game.selected_row][game.selected_col];
                    int needs_promotion = 0;

                    if (moving.type == PAWN) {
                        int promotion_row = (moving.color == WHITE) ? 0 : 7;
                        if (click_r == promotion_row) {
                            needs_promotion = 1;
                        }
                    }

                    if (needs_promotion) {
                        // نمایش منوی ارتقاء
                        char state_str[2048];
                        serialize_state(&game, state_str);

                        printf("<h2>ارتقاء پیاده</h2>");
                        printf("<p>مهره جدید را انتخاب کنید:</p>");
                        printf("<div style='display:flex; gap:15px; justify-content:center; margin:20px;'>");

                        PieceType promotions[] = {QUEEN, DRAGON, THIEF, GRYPHON};
                        const char* names[] = {"وزیر", "اژدها", "دزد", "گریفون"};

                        for (int i = 0; i < 4; i++) {
                            printf("<form action='chess_modified.cgi' method='GET'>");
                            printf("<input type='hidden' name='state' value='%s'>", state_str);
                            printf("<input type='hidden' name='promote_from' value='%d%d'>", game.selected_row, game.selected_col);
                            printf("<input type='hidden' name='promote_to' value='%d%d'>", click_r, click_c);
                            printf("<input type='hidden' name='promote_type' value='%d'>", promotions[i]);
                            printf("<button style='font-size:2rem; padding:15px;'>%s<br>%s</button>",
                                   piece_to_unicode(promotions[i], moving.color), names[i]);
                            printf("</form>");
                        }

                        printf("</div>");
                        print_footer();
                        return 0;
                    }

                    make_move(&game, game.selected_row, game.selected_col, click_r, click_c);
                    game.selected_row = -1;
                    game.selected_col = -1;

                    // بررسی پایان بازی
                    if (is_checkmate(&game, game.current_turn)) {
                        game.game_over = 1;
                        game.winner = (game.current_turn == WHITE) ? BLACK : WHITE;
                        sprintf(game.message, "کیش‌مات! %s برنده شد!",
                                (game.winner == WHITE) ? "سفید" : "سیاه");
                    } else if (is_stalemate(&game, game.current_turn)) {
                        game.game_over = 1;
                        game.winner = 0;
                        sprintf(game.message, "پات! بازی مساوی شد.");
                    } else if (is_in_check(&game, game.current_turn)) {
                        sprintf(game.message, "کیش! نوبت %s",
                                (game.current_turn == WHITE) ? "سفید" : "سیاه");
                    } else {
                        sprintf(game.message, "نوبت %s",
                                (game.current_turn == WHITE) ? "سفید" : "سیاه");
                    }
                } else {
                    // اگر حرکت نامعتبر باشد انتخاب فعلی را پاک می‌کنم
                    game.selected_row = -1;
                    game.selected_col = -1;
                }
            }
            // انتخاب مهره جدید
            else if (clicked.type != EMPTY && clicked.color == game.current_turn) {
                game.selected_row = click_r;
                game.selected_col = click_c;
            }
        }
    }

    // اگر پارامترهای ارتقاء در URL باشد، بدون نمایش منو اعمال می‌کنم
    get_param("promote_from", buffer);
    if (buffer[0] && strlen(buffer) == 2) {
        int from_r = buffer[0] - '0';
        int from_c = buffer[1] - '0';

        get_param("promote_to", buffer);
        if (buffer[0] && strlen(buffer) == 2) {
            int to_r = buffer[0] - '0';
            int to_c = buffer[1] - '0';

            get_param("promote_type", buffer);
            if (buffer[0]) {
                PieceType promo_type = atoi(buffer);

                Piece moving = game.board[from_r][from_c];
                moving.type = promo_type;

                game.board[to_r][to_c] = moving;
                game.board[from_r][from_c] = (Piece){EMPTY, WHITE};

                update_king_positions(&game);
                game.current_turn = (game.current_turn == WHITE) ? BLACK : WHITE;
                game.selected_row = -1;
                game.selected_col = -1;

                // بررسی پایان بازی
                if (is_checkmate(&game, game.current_turn)) {
                    game.game_over = 1;
                    game.winner = (game.current_turn == WHITE) ? BLACK : WHITE;
                    sprintf(game.message, "کیش‌مات! %s برنده شد!",
                            (game.winner == WHITE) ? "سفید" : "سیاه");
                } else if (is_stalemate(&game, game.current_turn)) {
                    game.game_over = 1;
                    game.winner = 0;
                    sprintf(game.message, "پات! بازی مساوی شد.");
                } else {
                    sprintf(game.message, "نوبت %s",
                            (game.current_turn == WHITE) ? "سفید" : "سیاه");
                }
            }
        }
    }

    // پیش از رندر، ماتریس حرکات مجاز برای مهره انتخاب‌شده را پر می‌کنم
    int highlight[8][8] = {0};
    if (game.selected_row != -1 && !game.game_over) {
        get_valid_moves(&game, game.selected_row, game.selected_col, highlight);
    }

    render_board(&game, highlight);

    // دکمه شروع مجدد
    printf("<br><a href='chess_modified.cgi' style='margin-top:15px; display:inline-block;'><button style='background:#f1c40f; color:#1a1a1a; padding:10px 25px; font-size:0.95em; border:none; border-radius:6px; cursor:pointer; font-weight:bold;'>Reset Game</button></a>");

    printf("</div></div>");
    print_footer();
    return 0;
}
