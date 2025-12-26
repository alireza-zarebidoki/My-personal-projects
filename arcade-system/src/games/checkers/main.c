#include "checkers.h"

// رنگ خانه‌ها را اینجا برمی‌گردانم تا ظاهر صفحه قابل تغییر باشد
const char* checker_cell_bg(int row, int col) {
    return ((row + col) % 2 == 0) ? "#e8e8e8" : "#8b4513";
}

// کل رندر تخته را در این تابع نگه داشتم تا هم هایلایت‌ها هم لینک‌ها را یکجا بسازم
void render_checkers_board(CheckersState *game, int highlight[8][8]) {
    printf("<div class='checkers-board'>");

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Checker piece = game->board[i][j];
            char bg[100];

            // این بخش رنگ پس‌زمینه هر خانه را با توجه به انتخاب و پرش‌ها مشخص می‌کند
            if (game->selected_row == i && game->selected_col == j) {
                sprintf(bg, "#f1c40f"); // انتخاب شده
            } else if (highlight && highlight[i][j]) {
                if (abs((game->selected_row - i)) == 2) {
                    sprintf(bg, "#e74c3c"); // پرش (قرمز)
                } else {
                    sprintf(bg, "#2ecc71"); // حرکت عادی (سبز)
                }
            } else if (game->must_jump && game->jump_from_r == i && game->jump_from_c == j) {
                sprintf(bg, "#ff6b6b"); // پرش اجباری (قرمز روشن)
            } else {
                sprintf(bg, "%s", checker_cell_bg(i, j));
            }

            // لینک هر خانه را با وضعیت فعلی سریال می‌کنم تا کلیک بعدی بدانم چه خبر است
            char state_str[2048];
            serialize_checkers_state(game, state_str);

            printf("<a href='checkers.cgi?state=%s&click=%d%d' class='checkers-cell' style='background:%s;'>", state_str, i, j, bg);

            if (piece.type != EMPTY_C) {
                const char *icon = "";
                const char *color = (piece.color == RED) ? "#e74c3c" : "#3498db";

                switch(piece.type) {
                    case MAN: icon = "fas fa-circle"; break;
                    case KING: icon = "fas fa-crown"; break;
                    case QUEEN: icon = "fas fa-chess-queen"; break;
                    case EMPTY_C: default: icon = ""; break; // default avoids -Wswitch warning
                }

                printf("<i class='%s' style='color:%s; filter:drop-shadow(0 0 2px #000);'></i>", icon, color);
            }

            printf("</a>");
        }
    }

    printf("</div>");
}

int main() {
    print_header("چکرز (Checkers)");

    // استایل اختصاصی را اینجا تزریق می‌کنم تا داخل iframe مستقل باشد
    printf("<style>\n");
    printf("  .game-container { padding-top: 20px !important; }\n");
    printf("  .checkers-ui { max-width: 700px; margin: 0 auto; text-align: center; }\n");
    printf("  .checkers-ui h1, .checkers-ui h2, .checkers-ui h3, .checkers-ui p { color: #e5e7eb; }\n");
    printf("  .checkers-ui h2 { font-size: 1.3rem; margin: 10px 0; }\n");
    printf("  .checkers-board { display:grid; grid-template-columns:repeat(8,60px); gap:0; margin:15px auto; width:fit-content; border:3px solid #333; }\n");
    printf("  .checkers-cell { width:60px; height:60px; display:flex; align-items:center; justify-content:center; font-size:1.8rem; cursor:pointer; border:1px solid #555; text-decoration:none; }\n");
    printf("  .checkers-ui .btn { background: #f39c12; color: #0a0a0a; border: 2px solid #d68910; border-radius: 8px; padding: 10px 25px; font-weight: 900; cursor: pointer; }\n");
    printf("  .checkers-ui .btn:hover { filter: brightness(1.1); }\n");
    printf("  .checkers-ui .panel { margin: 10px auto; padding: 10px; border: 2px solid #10b981; border-radius: 10px; background: rgba(0,0,0,0.6); }\n");
    printf("</style>\n");
    printf("<div class='checkers-ui'>\n");

    CheckersState game;
    char buffer[MAX_BUFFER];

    // وضعیت بازی را اگر در URL باشد می‌خوانم وگرنه صفحه جدید می‌سازم
    get_param("state", buffer);
    if (buffer[0]) {
        deserialize_checkers_state(buffer, &game);
    } else {
        init_checkers_board(&game);
        check_global_forced_jump(&game);
    }

    // اگر کاربر روی خانه‌ای کلیک کرده باشد اینجا حرکت یا انتخاب را مدیریت می‌کنم
    get_param("click", buffer);
    if (buffer[0] && strlen(buffer) == 2 && !game.game_over) {
        int click_r = buffer[0] - '0';
        int click_c = buffer[1] - '0';

        if (click_r >= 0 && click_r < 8 && click_c >= 0 && click_c < 8) {
            Checker clicked = game.board[click_r][click_c];

            // اگر قبلا مهره‌ای را انتخاب کرده بودم، اینجا تلاش می‌کنم حرکت را اعمال کنم
            if (game.selected_row != -1) {
                // تلاش برای حرکت
                if (is_valid_checkers_move(&game, game.selected_row, game.selected_col, click_r, click_c)) {
                    // اگر به انتهای زمین رسیدم شاید نیاز به ارتقاء باشد
                    Checker moving = game.board[game.selected_row][game.selected_col];
                    int needs_promotion = 0;

                    if (moving.type == MAN) {
                        if ((moving.color == RED && click_r == 0) ||
                            (moving.color == BLACK_C && click_r == 7)) {
                            needs_promotion = 1;
                        }
                    }

                    if (needs_promotion) {
                        // اینجا کاربر را مجبور می‌کنم نوع ارتقاء را انتخاب کند
                        char state_str[2048];
                        serialize_checkers_state(&game, state_str);

                        printf("<h2>🎉 ارتقاء مهره!</h2>");
                        printf("<p>نوع جدید را انتخاب کنید:</p>");
                        printf("<div style='display:flex; gap:20px; justify-content:center; margin:30px;'>");

                        // King
                        printf("<form action='checkers.cgi' method='GET'>");
                        printf("<input type='hidden' name='state' value='%s'>", state_str);
                        printf("<input type='hidden' name='promote_from' value='%d%d'>", game.selected_row, game.selected_col);
                        printf("<input type='hidden' name='promote_to' value='%d%d'>", click_r, click_c);
                        printf("<input type='hidden' name='promote_type' value='%d'>", KING);
                        printf("<button style='font-size:2rem; padding:20px; background:#ffd700;'>");
                        printf("👑<br><b>King</b><br><small>حرکت مورب آزاد</small>");
                        printf("</button></form>");

                        // Queen
                        printf("<form action='checkers.cgi' method='GET'>");
                        printf("<input type='hidden' name='state' value='%s'>", state_str);
                        printf("<input type='hidden' name='promote_from' value='%d%d'>", game.selected_row, game.selected_col);
                        printf("<input type='hidden' name='promote_to' value='%d%d'>", click_r, click_c);
                        printf("<input type='hidden' name='promote_type' value='%d'>", QUEEN);
                        printf("<button style='font-size:2rem; padding:20px; background:#9b59b6;'>");
                        printf("💎<br><b>Queen</b><br><small>حرکت عمودی/افقی</small>");
                        printf("</button></form>");

                        printf("</div>");
                        print_footer();
                        return 0;
                    }

                    make_checkers_move(&game, game.selected_row, game.selected_col, click_r, click_c);
                } else {
                    game.selected_row = -1;
                    game.selected_col = -1;
                }
            }
            // اگر مهره‌ای انتخاب نشده، اینجا مهره فعلی را انتخاب می‌کنم
            else if (clicked.type != EMPTY_C && clicked.color == game.current_turn) {
                // وقتی پرش اجباری فعال است فقط همان مهره مجاز است
                if (game.must_jump && (game.jump_from_r != click_r || game.jump_from_c != click_c)) {
                    // عمدا نادیده می‌گیرم تا قانون حفظ شود
                } else {
                    game.selected_row = click_r;
                    game.selected_col = click_c;
                }
            }
        }
    }

    // اگر درخواست ارتقاء آمده باشد اینجا جابجایی را انجام می‌دهم
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
                CheckerType promo_type = atoi(buffer);

                Checker moving = game.board[from_r][from_c];
                int dr = to_r - from_r;
                int dc = to_c - from_c;

                // اگر پرش بود مهره میانی را حذف می‌کنم
                if (abs(dr) == 2 && abs(dc) == 2) {
                    int mr = from_r + dr / 2;
                    int mc = from_c + dc / 2;
                    game.board[mr][mc] = (Checker){EMPTY_C, RED};
                }

                // جابجایی با ارتقاء
                moving.type = promo_type;
                game.board[to_r][to_c] = moving;
                game.board[from_r][from_c] = (Checker){EMPTY_C, RED};

                // تغییر نوبت
                game.selected_row = -1;
                game.selected_col = -1;
                game.multi_jump = 0;
                game.current_turn = (game.current_turn == RED) ? BLACK_C : RED;

                check_global_forced_jump(&game);

                // بررسی پایان بازی
                int red_count = count_pieces(&game, RED);
                int black_count = count_pieces(&game, BLACK_C);

                if (red_count == 0) {
                    game.game_over = 1;
                    game.winner = BLACK_C;
                    strcpy(game.message, "سیاه برنده شد!");
                } else if (black_count == 0) {
                    game.game_over = 1;
                    game.winner = RED;
                    strcpy(game.message, "قرمز برنده شد!");
                } else {
                    sprintf(game.message, "نوبت %s%s",
                            (game.current_turn == RED) ? "قرمز" : "سیاه",
                            game.must_jump ? " (پرش اجباری!)" : "");
                }
            }
        }
    }

    // نمایش رابط
    printf("<h2>🔴 چکرز (Checkers) ⚫</h2>");

    // پیام وضعیت یا برد را اینجا چاپ می‌کنم
    if (game.game_over) {
        printf("<div style='margin:15px; padding:15px; background:rgba(46,204,113,0.3); border:2px solid #2ecc71; border-radius:10px; font-size:1.5rem;'>");
        printf("🎉 %s", game.message);
        printf("</div>");
    } else {
        if (game.must_jump) {
            printf("<div style='margin:10px; padding:10px; background:rgba(231,76,60,0.3); border:2px solid #e74c3c; border-radius:8px; font-size:1.2rem;'>");
            printf("⚠️ پرش اجباری فعال است!");
            printf("</div>");
        }
        printf("<div style='margin:10px; font-size:1.2rem;'>%s</div>", game.message);
    }

    // هایلایت حرکت‌های مجاز را قبل از رندر محاسبه می‌کنم
    int highlight[8][8] = {0};
    if (game.selected_row != -1 && !game.game_over) {
        get_valid_checkers_moves(&game, game.selected_row, game.selected_col, highlight);
    }

    render_checkers_board(&game, highlight);

    // دکمه شروع مجدد
    printf("<br><a href='checkers.cgi'><button class='btn'>🔄 بازی جدید</button></a>");

    printf("</div>");

    print_footer();
    return 0;
}
