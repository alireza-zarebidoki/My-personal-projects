// قرارداد داده‌ای شطرنج تغییر‌یافته را اینجا نگه می‌دارم تا منطق و CGI روی یک تعریف مشترک کار کنند

#ifndef CHESS_H
#define CHESS_H

#include "../../common/c_utils.h"

// نوع مهره‌ها را همراه با جایگزین‌های سفارشی خودم تعریف کردم
typedef enum {
    EMPTY = 0,
    PAWN = 1,      // سرباز
    KNIGHT = 2,    // اسب (حذف شده، جایگزین: Dragon)
    BISHOP = 3,    // فیل (حذف شده، جایگزین: Thief)
    ROOK = 4,      // رخ (حذف شده، جایگزین: Gryphon)
    QUEEN = 5,     // وزیر
    KING = 6,      // شاه
    DRAGON = 7,    // اژدها (ترکیب اسب + شاه)
    THIEF = 8,     // دزد (فیل با پرش از ۱ مانع)
    GRYPHON = 9    // گریفون (یک خانه مورب + رخ)
} PieceType;

// رنگ مهره را دو مقدار ساده گذاشتم
typedef enum {
    WHITE = 1,
    BLACK = 2
} Color;

// هر مهره فقط نوع و رنگ دارد تا ساختار سبک بماند
typedef struct {
    PieceType type;
    Color color;
} Piece;

// وضعیت کامل بازی را نگه می‌دارم تا بین درخواست‌های CGI جابه‌جا شود
typedef struct {
    Piece board[8][8];
    Color current_turn;
    int selected_row;
    int selected_col;
    int white_king_pos[2];  // [row, col]
    int black_king_pos[2];
    int white_time;         // زمان باقی‌مانده سفید (ثانیه)
    int black_time;         // زمان باقی‌مانده سیاه (ثانیه)
    long last_move_time;    // زمان آخرین حرکت (timestamp)
    int game_over;
    int winner; // 0: none, 1: white, 2: black, 3: draw
    char message[256];
} GameState;

// state.c: راه‌اندازی، سریال‌سازی و همگام‌سازی موقعیت شاه‌ها
void init_board(GameState *game);
void serialize_state(GameState *game, char *buffer);
void deserialize_state(const char *buffer, GameState *game);
void update_king_positions(GameState *game);

// logic.c: اعتبارسنجی حرکت، کیش/مات و اعمال حرکت
int is_valid_move(GameState *game, int from_r, int from_c, int to_r, int to_c);
int is_path_clear(GameState *game, int from_r, int from_c, int to_r, int to_c);
int is_in_check(GameState *game, Color color);
int can_king_escape(GameState *game, Color color);
void get_valid_moves(GameState *game, int row, int col, int valid[8][8]);
void make_move(GameState *game, int from_r, int from_c, int to_r, int to_c);
int is_checkmate(GameState *game, Color color);
int is_stalemate(GameState *game, Color color);

// توابع کمکی برای نمایش و UI
const char* piece_to_unicode(PieceType type, Color color);
const char* piece_name(PieceType type);

#endif
