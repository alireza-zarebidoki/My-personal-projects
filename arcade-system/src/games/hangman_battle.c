/*
 * ═══════════════════════════════════════════════════════════════════════════
 * فایل: hangman_battle.c
 * نویسنده: علیرضا زارع
 * توضیح: بازی نبرد کلمات (Hangman Battle) - نسخه دو نفره و با بات
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * این بازی یک نسخه پیشرفته Hangman (دار آویز) است که دو بازیکن می‌توانند
 * همزمان با هم رقابت کنند.
 *
 * ویژگی‌ها:
 * ─────────────────────────────────────────────────────────────────────────
 *
 * - دو حالت بازی: دو نفره (PvP) و تک‌نفره (vs Bot)
 * - هر بازیکن کلمه‌ای برای حریف تعیین می‌کند
 * - بازیکنان نوبتی حروف را حدس می‌زنند
 * - اولین نفری که کلمه را کامل کند یا حریف را بکشد برنده است
 * - Bot هوشمند با الگوریتم frequency-based
 * - رابط کاربری تمیز با نمایش ASCII art دار
 * - امکان انتخاب کلمه دستی یا تصادفی
 *
 * معماری:
 * ─────────────────────────────────────────────────────────────────────────
 *
 * - State machine با 5 حالت: setup → word_menu → p1_setup → [p2_setup] → play
 * - تمام state در URL (stateless CGI)
 * - هر حرکت یک request جدید
 * - Bot با meta refresh خودکار حرکت می‌کند
 *
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "../common/c_utils.h"
#include <ctype.h>
#include <time.h>

// ───────────────────────────────────────────────────────────────────────────
// ثابت‌ها (Constants)
// ───────────────────────────────────────────────────────────────────────────

/*
 * MAX_WORD_LEN: حداکثر طول کلمه مجاز
 *
 * چرا 20؟
 * - اکثر کلمات انگلیسی کمتر از 20 حرف هستند
 * - طولانی‌تر از این UI را شلوغ می‌کند
 * - buffer safety برای URL parameters
 */
#define MAX_WORD_LEN 20

/*
 * MAX_LIVES: تعداد حداکثر خطاهای مجاز
 *
 * چرا 6؟
 * - استاندارد کلاسیک بازی Hangman
 * - 6 قسمت دار: سر، بدن، دست چپ، دست راست، پای چپ، پای راست
 * - تعادل بین چالش و قابل بازی بودن
 */
#define MAX_LIVES 6

/*
 * RANDOM_WORDS: لیست کلمات تصادفی
 *
 * این لیست برای حالت "انتخاب تصادفی" استفاده می‌شود.
 *
 * دلایل انتخاب این کلمات:
 * - همه مرتبط با کامپیوتر و برنامه‌نویسی (تماتیک arcade)
 * - طول متوسط (7-10 حرف)
 * - نه خیلی آسان، نه خیلی سخت
 * - همه با حروف بزرگ (uppercase) برای یکنواختی
 *
 * تعداد: 15 کلمه
 */
const char *RANDOM_WORDS[] = {
    "COMPUTER", "ALGORITHM", "PROGRAM", "DATABASE", "NETWORK",
    "SECURITY", "VARIABLE", "POINTER", "COMPILER", "STRUCTURE",
    "FUNCTION", "ARCADE", "SYSTEM", "HARDWARE", "SOFTWARE"
};

// ───────────────────────────────────────────────────────────────────────────
// توابع کمکی (Helper Functions)
// ───────────────────────────────────────────────────────────────────────────

/*
 * has_guessed(): بررسی حدس قبلی
 *
 * پارامترها:
 * - letter: حرفی که می‌خواهیم چک کنیم
 * - guesses: رشته تمام حدس‌های قبلی
 *
 * خروجی:
 * - 1: اگر این حرف قبلاً حدس زده شده
 * - 0: اگر حرف جدید است
 *
 * منطق:
 * ─────
 *
 * از strchr() استفاده می‌کنم که حرف را در رشته جستجو می‌کند.
 * هم lowercase و هم uppercase را چک می‌کنم تا case-insensitive باشد.
 *
 * مثال:
 * - guesses = "abc"
 * - has_guessed('A', guesses) → 1 (چون 'a' وجود دارد)
 * - has_guessed('d', guesses) → 0 (وجود ندارد)
 */
int has_guessed(char letter, const char *guesses) {
    // بررسی null pointer برای امنیت
    if (!letter || !guesses) return 0;

    // جستجوی case-insensitive
    return (strchr(guesses, tolower(letter)) != NULL ||
            strchr(guesses, toupper(letter)) != NULL);
}

/*
 * print_masked_word(): چاپ کلمه ماسک‌شده
 *
 * پارامترها:
 * - word: کلمه اصلی (مثلاً "COMPUTER")
 * - guesses: حدس‌های انجام شده (مثلاً "cop")
 *
 * خروجی:
 * - حروف حدس زده شده: نمایش داده می‌شوند
 * - حروف باقیمانده: با "_" نمایش داده می‌شوند
 *
 * مثال:
 * word = "COMPUTER", guesses = "cop"
 * → خروجی: "C O _ P _ _ _ _ "
 *
 * چرا فاصله بین حروف؟
 * - خوانایی بهتر
 * - جلوگیری از چسبیدن حروف به هم
 */
void print_masked_word(const char *word, const char *guesses) {
    for (int i = 0; word[i]; i++) {
        if (has_guessed(word[i], guesses)) {
            // حرف حدس زده شده → نمایش بده
            printf("%c ", word[i]);
        } else {
            // حرف نامعلوم → خط تیره
            printf("_ ");
        }
    }
}

/*
 * count_mistakes(): شمارش تعداد خطاها
 *
 * پارامترها:
 * - word: کلمه اصلی که باید حدس زده شود
 * - guesses: تمام حدس‌های انجام شده
 *
 * خروجی:
 * - تعداد حدس‌های اشتباه
 *
 * منطق:
 * ─────
 *
 * برای هر حرف در guesses:
 * 1. چک کن آیا در word وجود دارد؟
 * 2. اگر نه → mistakes++
 *
 * مثال:
 * word = "CAT", guesses = "abxc"
 * - 'a' → در word نیست → mistake
 * - 'b' → در word نیست → mistake
 * - 'x' → در word نیست → mistake
 * - 'c' → در word هست → OK
 * → mistakes = 3
 */
int count_mistakes(const char *word, const char *guesses) {
    int mistakes = 0;

    // برای هر حرف حدس زده شده
    for (int i = 0; guesses[i]; i++) {
        int found = 0;

        // جستجو در کلمه اصلی
        for (int j = 0; word[j]; j++) {
            if (tolower(word[j]) == tolower(guesses[i])) {
                found = 1;
                break;
            }
        }

        // اگر در کلمه نبود → خطا
        if (!found) mistakes++;
    }

    return mistakes;
}

/*
 * print_hangman(): رسم دار (ASCII Art)
 *
 * پارامتر:
 * - mistakes: تعداد خطاها (0 تا 6)
 *
 * خروجی:
 * - نمایش ASCII art دار بر اساس تعداد خطاها
 *
 * مراحل دار (هر خطا یک قسمت اضافه می‌شود):
 * ────────────────────────────────────────────────────────────
 *
 * 0 خطا:     1 خطا:     2 خطا:     3 خطا:
 *   +---+      +---+      +---+      +---+
 *   |   |      |   |      |   |      |   |
 *       |      O   |      O   |      O   |
 *       |          |      |   |     /|   |
 *       |          |          |          |
 *       |          |          |          |
 * =========  =========  =========  =========
 *
 * 4 خطا:     5 خطا:     6 خطا (مرگ):
 *   +---+      +---+      +---+
 *   |   |      |   |      |   |
 *   O   |      O   |      O   |
 *  /|\  |     /|\  |     /|\  |
 *       |     \    |     \ /  |
 *       |          |          |
 * =========  =========  =========
 *
 * استایل:
 * - رنگ قرمز (#e74c3c) برای نمایش خطر
 * - monospace font برای ASCII art صحیح
 * - پس‌زمینه تیره شفاف
 */
void print_hangman(int mistakes) {
    // شروع <pre> با استایل
    printf("<pre style='font-family:monospace; line-height:1.1; color:#e74c3c; background:rgba(0,0,0,0.5); padding:10px; border-radius:5px; display:inline-block; text-align:left; font-size:1rem;'>");

    /*
     * ساختار دار:
     * - خط 1: تیر بالا
     * - خط 2: طناب
     * - خط 3: سر (mistakes >= 1)
     * - خط 4: بدن و دست‌ها (mistakes >= 2,3,4)
     * - خط 5: پاها (mistakes >= 5,6)
     * - خط 6: خالی
     * - خط 7: پایه
     */
    printf("  +---+\n  |   |\n  %s   |\n %s%s%s  |\n %s %s  |\n      |\n=========\n",
        mistakes >= 1 ? "O" : " ",           // سر
        mistakes >= 3 ? "/" : " ",          // دست چپ
        mistakes >= 2 ? "|" : " ",          // بدن
        mistakes >= 4 ? "\\" : " ",         // دست راست
        mistakes >= 5 ? "\\" : " ",         // پای چپ
        mistakes >= 6 ? "/" : " ");         // پای راست

    printf("</pre>");
}

// ───────────────────────────────────────────────────────────────────────────
// تابع اصلی (Main Function)
// ───────────────────────────────────────────────────────────────────────────

int main() {
    // ═══════════════════════════════════════════════════════════════════════
    // بخش ۱: Initialization
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * srand(time(NULL)): مقداردهی اولیه random برای:
     * - انتخاب کلمات تصادفی
     * - حرکات Bot
     */
    srand(time(NULL));

    /*
     * print_header(): چاپ HTML header
     */
    print_header("نبرد کلمات (Hangman Battle)");

    /*
     * ═══════════════════════════════════════════════════════════════════════
     * CSS Styles اختصاصی
     * ═══════════════════════════════════════════════════════════════════════
     *
     * چرا inline CSS?
     * - بازی در iframe لود می‌شود
     * - نباید به استایل بیرونی وابسته باشد
     * - تضمین نمایش یکدست
     *
     * کلاس‌های اصلی:
     * ─────────────────
     *
     * .hangman-ui: container اصلی
     * - max-width: 680px (برای خوانایی)
     * - text-align: center (وسط‌چین)
     *
     * .btn: دکمه‌های اصلی
     * - background: #10b981 (سبز neon)
     * - hover effect با brightness
     *
     * .blue, .yellow, .red: variance های رنگی
     * - .blue: #3498db (آبی برای PvP)
     * - .yellow: #f1c40f (زرد برای Bot)
     * - .red: #e74c3c (قرمز برای خطر)
     *
     * .word: نمایش کلمه ماسک‌شده
     * - font-size: 1.4rem (بزرگ برای خوانایی)
     * - letter-spacing: 0.25rem (فاصله بین حروف)
     */
    printf("<style>\n");
    printf("  .game-container { padding-top: 4px !important; }\n");
    printf("  .hangman-ui { max-width: 680px; margin: 0 auto; text-align: center; padding-top: 2px; }\n");
    printf("  .hangman-ui h1, .hangman-ui h2, .hangman-ui h3, .hangman-ui p { color: #e5e7eb; }\n");
    printf("  .hangman-ui h2 { font-size: 1.25rem; margin: 10px 0; }\n");
    printf("  .hangman-ui .btn { background: #10b981; color: #ffffff; border: 2px solid #147f5b; border-radius: 8px; padding: 8px 16px; font-weight: 900; cursor: pointer; }\n");
    printf("  .hangman-ui .btn:hover { filter: brightness(1.1); }\n");
    printf("  .hangman-ui .blue { background:#3498db; color:#ffffff; border-color:#2980b9; }\n");
    printf("  .hangman-ui .yellow { background:#f1c40f; color:#1a1a1a; border-color:#d4ac0d; }\n");
    printf("  .hangman-ui .red { background:#e74c3c; color:#ffffff; border-color:#c0392b; }\n");
    printf("  .hangman-ui input[type=text], .hangman-ui input[type=number] { background:#000; color:#fff; border:1px solid #10b981; padding:8px 10px; border-radius:6px; }\n");
    printf("  .hangman-ui .word { font-size:1.4rem; letter-spacing:0.25rem; color:#ffffff; margin-top: 10px; display:inline-block; }\n");
    printf("  .hangman-ui .panel { margin: 12px auto; padding: 12px; border: 2px solid #10b981; border-radius: 10px; background: rgba(0,0,0,0.6); }\n");
    printf("</style>\n");

    // شروع container اصلی
    printf("<div class='hangman-ui'>\n");

    // ═══════════════════════════════════════════════════════════════════════
    // بخش ۲: State Variables
    // ═══════════════════════════════════════════════════════════════════════

    char buffer[MAX_BUFFER];        // بافر موقت برای خواندن parameters

    /*
     * mode: حالت فعلی بازی
     * - "setup": منوی اصلی (انتخاب نوع بازی)
     * - "word_menu": انتخاب روش تعیین کلمات
     * - "p1_setup": ورود کلمه بازیکن 1
     * - "p2_setup": ورود کلمه بازیکن 2 (فقط PvP)
     * - "play": بازی در حال اجرا
     */
    char mode[50] = "setup";

    /*
     * p1_word, p2_word: کلمات مخفی دو بازیکن
     * - p1_word: کلمه‌ای که بازیکن 2 باید حدس بزند
     * - p2_word: کلمه‌ای که بازیکن 1 باید حدس بزند
     * - اندازه [21]: MAX_WORD_LEN + 1 (برای null terminator)
     */
    char p1_word[21] = "", p2_word[21] = "";

    /*
     * p1_guesses, p2_guesses: حدس‌های انجام شده
     * - هر حرف حدس زده شده به این رشته اضافه می‌شود
     * - مثال: "abcxyz" یعنی این حروف حدس زده شده‌اند
     * - اندازه [30]: فضای کافی برای 26 حرف انگلیسی + buffer
     */
    char p1_guesses[30] = "", p2_guesses[30] = "";

    /*
     * turn: نوبت کیست؟
     * - 1: نوبت بازیکن 1
     * - 2: نوبت بازیکن 2 (یا Bot)
     */
    int turn = 1;

    /*
     * is_bot: آیا حریف Bot است؟
     * - 0: حالت دو نفره (PvP)
     * - 1: حالت تک‌نفره (vs Bot)
     */
    int is_bot = 0;

    // ═══════════════════════════════════════════════════════════════════════
    // بخش ۳: Reading Parameters from URL
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * خواندن mode
     */
    get_param("mode", buffer);
    if (buffer[0]) strncpy(mode, buffer, sizeof(mode)-1);

    /*
     * خواندن کلمات با safe copy
     *
     * strncpy() برای جلوگیری از buffer overflow:
     * - فقط n-1 کاراکتر کپی می‌شود
     * - آخرین کاراکتر را خودمان null می‌کنیم
     *
     * چرا مهم است؟
     * - p1w و p2w از URL می‌آیند (user input)
     * - ممکن است malicious باشند
     * - باید از buffer overflow جلوگیری کنیم
     */
    get_param("p1w", buffer);
    if (buffer[0]) {
        strncpy(p1_word, buffer, sizeof(p1_word)-1);
        p1_word[sizeof(p1_word)-1] = '\0';  // تضمین null termination
    }

    get_param("p2w", buffer);
    if (buffer[0]) {
        strncpy(p2_word, buffer, sizeof(p2_word)-1);
        p2_word[sizeof(p2_word)-1] = '\0';
    }

    get_param("p1g", buffer);
    if (buffer[0]) {
        strncpy(p1_guesses, buffer, sizeof(p1_guesses)-1);
        p1_guesses[sizeof(p1_guesses)-1] = '\0';
    }

    get_param("p2g", buffer);
    if (buffer[0]) {
        strncpy(p2_guesses, buffer, sizeof(p2_guesses)-1);
        p2_guesses[sizeof(p2_guesses)-1] = '\0';
    }

    /*
     * خواندن turn و is_bot (اعداد)
     */
    get_param("turn", buffer);
    if (buffer[0]) turn = atoi(buffer);

    get_param("bot", buffer);
    if (buffer[0]) is_bot = atoi(buffer);

    /*
     * خواندن guess (حدس جدید)
     */
    get_param("guess", buffer);

    // ═══════════════════════════════════════════════════════════════════════
    // بخش ۴: Processing New Guess
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * اگر:
     * - buffer پر است (کاربر حدس جدید فرستاده)
     * - و mode = "play" است
     *
     * آنگاه:
     * - حدس را به لیست حدس‌های بازیکن فعلی اضافه کن
     * - نوبت را عوض کن
     */
    if (buffer[0] && strcmp(mode, "play") == 0) {
        // تبدیل به lowercase برای یکنواختی
        char g = tolower(buffer[0]);

        // انتخاب لیست حدس‌های بازیکن فعلی
        char *cg = (turn == 1) ? p1_guesses : p2_guesses;

        // اگر قبلاً حدس نزده بود
        if (!strchr(cg, g)) {
            // اضافه کردن به انتهای رشته
            int l = strlen(cg);
            cg[l] = g;
            cg[l+1] = '\0';

            // تغییر نوبت
            turn = (turn == 1) ? 2 : 1;
        }
    }

    // --- مدیریت منوها و جریان ورود ---
    if (strcmp(mode, "setup") == 0) {
        printf("<h2>گام ۱: انتخاب نوع نبرد</h2><div style='display:flex; justify-content:center; gap:20px;'>");
        printf("<form action='hangman_battle.cgi'><input type='hidden' name='mode' value='word_menu'><input type='hidden' name='bot' value='0'><button class='btn blue' style='width:160px; height:80px;'>👥 دو نفره (PvP)</button></form>");
        printf("<form action='hangman_battle.cgi'><input type='hidden' name='mode' value='word_menu'><input type='hidden' name='bot' value='1'><button class='btn yellow' style='width:160px; height:80px;'>🤖 حریف بات (Bot)</button></form></div>");
    }
    else if (strcmp(mode, "word_menu") == 0) {
        printf("<h2>گام ۲: تعیین کلمات بازی</h2><div style='display:flex; flex-direction:column; gap:15px; align-items:center;'>");
        printf("<form action='hangman_battle.cgi'><input type='hidden' name='mode' value='p1_setup'><input type='hidden' name='bot' value='%d'><button class='btn blue'>✍️ وارد کردن کلمه به صورت دستی</button></form>", is_bot);
        printf("<form action='hangman_battle.cgi'><input type='hidden' name='mode' value='play'><input type='hidden' name='bot' value='%d'>", is_bot);
        printf("<input type='hidden' name='p1w' value='%s'><input type='hidden' name='p2w' value='%s'><button class='yellow'>🎲 انتخاب کلمه به صورت تصادفی</button></form>", RANDOM_WORDS[rand()%15], RANDOM_WORDS[rand()%15]);
        printf("</div>");
    }
    else if (strcmp(mode, "p1_setup") == 0) {
        printf("<h2 style='color:#facc15;'>نوبت بازیکن ۱</h2><p>کلمه مخفی برای حریف را بنویسید:</p>");
        printf("<form action='hangman_battle.cgi' method='GET'>");
        printf("<input type='hidden' name='mode' value='%s'><input type='hidden' name='bot' value='%d'>", is_bot ? "play" : "p2_setup", is_bot);
        if(is_bot) printf("<input type='hidden' name='p1w' value='%s'>", RANDOM_WORDS[rand()%15]);
        printf("<input class='input' type='text' name='%s' required autofocus autocomplete='off'>", is_bot ? "p2w" : "p1w");
        printf("<br><br><button class='btn'>تایید و ادامه</button></form>");
    }
    else if (strcmp(mode, "p2_setup") == 0) {
        printf("<h2 style='color:#3498db;'>نوبت بازیکن ۲</h2><p>کلمه مخفی برای بازیکن ۱ را بنویسید:</p>");
        printf("<form action='hangman_battle.cgi' method='GET'>");
        printf("<input type='hidden' name='mode' value='play'><input type='hidden' name='bot' value='0'>");
        printf("<input type='hidden' name='p1w' value='%s'>", p1_word); // انتقال کلمه P1 به مرحله نهایی
        printf("<input class='input' type='text' name='p2w' required autofocus autocomplete='off'>");
        printf("<br><br><button class='btn'>شروع نبرد</button></form>");
    }
    else if (strcmp(mode, "play") == 0) {
        if (p1_word[0] == '\0' || p2_word[0] == '\0') {
            printf("<p style='color:red;'>خطا: کلمات بارگذاری نشدند! لطفاً مجدداً شروع کنید.</p><a href='hangman_battle.cgi'><button>بازگشت به منو</button></a>");
        } else {
            int m1 = count_mistakes(p2_word, p1_guesses);
            int m2 = count_mistakes(p1_word, p2_guesses);
            // بررسی می‌کنم آیا یکی از بازیکن‌ها کلمه را کامل کرده یا جانش تمام شده است
            int complete1 = 1; for(int i=0; p2_word[i]; i++) if(!has_guessed(p2_word[i], p1_guesses)) complete1 = 0;
            int complete2 = 1; for(int i=0; p1_word[i]; i++) if(!has_guessed(p1_word[i], p2_guesses)) complete2 = 0;

            if (complete1 || m2 >= MAX_LIVES) {
                printf("<h1>🎉 بازیکن ۱ برنده شد!</h1><p>کلمه حریف: %s</p>", p2_word);
                printf("<div style='display:flex; justify-content:space-around; margin-top:12px;'>");
                printf("<div><h3>Player 1</h3>"); print_hangman(m1); printf("<div class='word'>"); print_masked_word(p2_word, p1_guesses); printf("</div></div>");
                printf("<div><h3>Player 2 %s</h3>", is_bot?"(Bot)":""); print_hangman(m2); printf("<div class='word'>"); print_masked_word(p1_word, p2_guesses); printf("</div></div></div>");
                printf("<div style='margin-top:14px;'><a href='hangman_battle.cgi'><button class='btn yellow'>بازی جدید</button></a></div>");
            } else if (complete2 || m1 >= MAX_LIVES) {
                printf("<h1>🎉 بازیکن ۲ برنده شد!</h1><p>کلمه حریف: %s</p>", p1_word);
                printf("<div style='display:flex; justify-content:space-around; margin-top:12px;'>");
                printf("<div><h3>Player 1</h3>"); print_hangman(m1); printf("<div class='word'>"); print_masked_word(p2_word, p1_guesses); printf("</div></div>");
                printf("<div><h3>Player 2 %s</h3>", is_bot?"(Bot)":""); print_hangman(m2); printf("<div class='word'>"); print_masked_word(p1_word, p2_guesses); printf("</div></div></div>");
                printf("<div style='margin-top:14px;'><a href='hangman_battle.cgi'><button class='btn yellow'>بازی جدید</button></a></div>");
            } else {
                printf("<div style='display:flex; justify-content:space-around;'>");
                printf("<div><h3>Player 1</h3>"); print_hangman(m1); printf("<div class='word'>"); print_masked_word(p2_word, p1_guesses); printf("</div></div>");
                printf("<div><h3>Player 2 %s</h3>", is_bot?"(Bot)":""); print_hangman(m2); printf("<div class='word'>"); print_masked_word(p1_word, p2_guesses); printf("</div></div></div>");

                if (is_bot && turn == 2) {
                    // در حالت بات، از توزیع حروف انگلیسی استفاده می‌کنم تا حدس منطقی داشته باشد
                    const char *bp = "ETAOINSHRDLCUMWFGYPBVKJXQZ"; char bg = 'A';
                    for(int i=0; bp[i]; i++) if(!has_guessed(bp[i], p2_guesses)) { bg = bp[i]; break; }
                    // با اضافه کردن حدس جدید، URL را برای رفرش خودکار می‌سازم
                    char p2_next[64];
                    snprintf(p2_next, sizeof(p2_next), "%s%c", p2_guesses, bg);
                    printf("<meta http-equiv='refresh' content='1;url=hangman_battle.cgi?mode=play&bot=1&p1w=%s&p2w=%s&p1g=%s&p2g=%s&turn=1'>", p1_word, p2_word, p1_guesses, p2_next);
                    printf("<p>حریف بات در حال فکر کردن...</p>");
                } else {
                    printf("<form action='hangman_battle.cgi' method='GET' style='margin-top:20px;'>");
                    printf("<input type='hidden' name='mode' value='play'><input type='hidden' name='turn' value='%d'>", turn);
                    printf("<input type='hidden' name='bot' value='%d'><input type='hidden' name='p1w' value='%s'><input type='hidden' name='p2w' value='%s'>", is_bot, p1_word, p2_word);
                    printf("<input type='hidden' name='p1g' value='%s'><input type='hidden' name='p2g' value='%s'>", p1_guesses, p2_guesses);
                    printf("نوبت %s: <input class='input' name='guess' maxlength='1' size='1' autofocus required autocomplete='off' style='text-align:center;'>", (turn==1)?"P1":"P2");
                    printf(" <button class='btn' type='submit'>حدس بزن</button></form>");
                }
            }
        }
    }

    printf("</div>");

    // ═══════════════════════════════════════════════════════════════════════
    // پایان
    // ═══════════════════════════════════════════════════════════════════════

    print_footer();
    return 0;
}

/*
 * ═══════════════════════════════════════════════════════════════════════════
 * پایان فایل
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * خلاصه جریان بازی:
 * ──────────────────────
 *
 * 1. setup → انتخاب PvP یا Bot
 * 2. word_menu → دستی یا تصادفی؟
 * 3. p1_setup → ورود کلمه بازیکن 1
 * 4. [p2_setup] → ورود کلمه بازیکن 2 (فقط PvP)
 * 5. play → بازی شروع می‌شود
 *
 * در حالت play:
 * ─────────────────
 *
 * - نوبتی حدس می‌زنند
 * - Bot با الگوریتم frequency-based حرکت می‌کند
 * - اولین نفری که کلمه را کامل کند برنده است
 * - یا اگر حریف 6 خطا داشت برنده می‌شود
 *
 * Bot Strategy:
 * ─────────────────
 *
 * از ترتیب frekans حروف انگلیسی استفاده می‌کند:
 * "ETAOINSHRDLCUMWFGYPBVKJXQZ"
 *
 * این ترتیب بر اساس تحقیقات زبانی است و
 * بهترین شانس برای حدس صحیح را می‌دهد.
 */
