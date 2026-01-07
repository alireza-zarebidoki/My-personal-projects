/**
 * @file src/games/fibonacci_2048.c
 * @project arcade-system
 * @author Alireza Zarebidoki
 * @date_created 2024
 * @date_modified 2024
 * @brief Fibonacci-based tile merging game backend (CGI).
 *
 * @details
 * این فایل منطق بازی «فیبوناچی 2048» را پیاده کرده‌ام؛ یک نسخه تغییر‌یافته
 * بازی 2048 که بجای توان‌های 2، دنباله‌ی فیبوناچی استفاده می‌کند.
 * این بازی بیشتر ترکیبیاتی است و استراتژی متفاوتی نسبت به 2048 معمولی نیاز دارد.
 *
 * @responsibilities
 * - مدیریت تخته بازی (grid) و حرکات کاشی‌ها
 * - پیاده‌سازی منطق ادغام کاشی‌ها بر اساس قوانین فیبوناچی
 * - محاسبه score و تشخیص حالات برد/باخت
 * - رندر HTML تخته با رنگ‌های مختلف برای هر مقدار
 * - مدیریت state بازی و انتقال آن از طریق URL
 *
 * @interactions
 * - دریافت پارامترهای action و grid از CGI
 * - استفاده از توابع c_utils برای HTML wrapper
 * - رندر تخته با لینک‌های کلیک‌پذیر برای حرکات
 *
 * @notes
 * - دنباله فیبوناچی برای 6×6 grid کافی است
 * - استثنای 1 و 1 → 2 را جدا مدیریت کردم
 * - heuristics برای AI ممکن است پیچیده‌تر باشد
 */

#include "../common/c_utils.h"

/*
 * این فایل منطق بازی «فیبوناچی 2048» را پیاده کرده‌ام؛ هر بار که برگردم یادم باشد چرا این همه شرط و دکمه گذاشتم.
 * اگر این فایل را دستکاری کنم همه چیزی که روی دنباله فیبوناچی بنا شده به هم می‌ریزد.
 */

#define MAX_SIZE 6   // این سقف را گذاشتم تا کنترل UI و بافرها از دستم خارج نشود
#define FIB_COUNT 20 // تعداد اعضایی که برای چک ادغام لازم دارم؛ بیشتر از این در تخته فعلی کاربرد ندارد

// این آرایه دنباله فیبوناچی است که نگه داشتم تا هر بار برای ادغام، به جای محاسبه مجدد، از مقدار آماده استفاده کنم
const int FIB_SEQ[FIB_COUNT] = {
    1, 1, 2, 3, 5, 8, 13, 21, 34, 55,
    89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765
};

/*
 * get_color: برای هر مقدار کاشی، رنگ سفارشی برمی‌گردانم تا تخته زنده و قابل تشخیص بماند.
 * num: مقداری که روی کاشی است؛ اگر صفر باشد رنگ پس‌زمینه تیره می‌دهم تا خالی بودن دیده شود.
 * اگر این نگاشت رنگ را حذف کنم صفحه یکنواخت و کسل‌کننده می‌شود و بازیکن به سختی اعداد را تشخیص می‌دهد.
 */
char* get_color(int num) {
    if (num == 0) return "#1e293b";
    if (num == 1) return "#f1c40f";
    if (num == 2) return "#e67e22";
    if (num == 3) return "#e74c3c";
    if (num == 5) return "#9b59b6";
    if (num == 8) return "#3498db";
    if (num == 13) return "#2ecc71";
    if (num == 21) return "#1abc9c";
    return "#95a5a6"; // رنگ پیش‌فرض برای مقادیر بزرگ‌تر که کم پیش می‌آید
}

/*
 * check_merge: بررسی می‌کنم دو عدد کنار هم مجاز به ادغام فیبوناچی هستند یا نه.
 * a, b: مقادیر دو کاشی که باید چک شوند؛ اگر یکی صفر باشد اصلاً ادغامی نداریم.
 * خروجی: اگر قابل ادغام باشند مجموعشان را برمی‌گردانم، در غیر این صورت صفر می‌دهم تا process_line بفهمد خبری نیست.
 * اگر این تابع اشتباه کار کند کل منطق امتیازدهی و حرکت به هم می‌ریزد.
 */
int check_merge(int a, int b) {
    if (a == 0 || b == 0) return 0;

    // استثنای ۱ و ۱ را جدا گذاشتم چون تنها جفت تکراری مجاز است
    if (a == 1 && b == 1) return 2;

    // سایر اعداد را با نگاه به مجاورت در دنباله فیبوناچی بررسی می‌کنم
    for (int i = 1; i < FIB_COUNT - 1; i++) {
        if ((a == FIB_SEQ[i] && b == FIB_SEQ[i + 1]) ||
            (b == FIB_SEQ[i] && a == FIB_SEQ[i + 1])) {
            return a + b;
        }
    }
    return 0;
}

/*
 * spawn_tile: کاشی جدید را روی یکی از خانه‌های خالی می‌گذارم تا بازی پیش برود.
 * board: ماتریس وضعیت تخته که درجا تغییر می‌دهم.
 * size: اندازه تخته (۳ تا ۶)؛ اگر بزرگ‌تر شود این آرایه empty_cells جواب نمی‌دهد.
 */
void spawn_tile(int board[MAX_SIZE][MAX_SIZE], int size) {
    int empty_cells[MAX_SIZE * MAX_SIZE][2]; // لیست مختصات خانه‌های خالی
    int count = 0; // تعداد خانه‌های خالی پیدا شده

    // تمام خانه‌های خالی را جمع می‌کنم تا بعداً یکی را تصادفی انتخاب کنم
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (board[i][j] == 0) {
                empty_cells[count][0] = i;
                empty_cells[count][1] = j;
                count++;
            }

    // اگر خانه خالی وجود داشته باشد یکی را تصادفی انتخاب می‌کنم و ۱ یا ۲ می‌گذارم تا بازیکن حرکت بعدی داشته باشد
    if (count > 0) {
        int r = rand() % count;
        board[empty_cells[r][0]][empty_cells[r][1]] =
            (rand() % 2) ? 1 : 2;
    }
}

/*
 * process_line: یک خط از تخته را فشرده و سپس بر اساس قوانین فیبوناچی ادغام می‌کنم.
 * line: آرایه خط/ستون که مستقیم تغییر می‌دهم؛ ورودی و خروجی یکسان است.
 * size: طول خط (همان اندازه تخته).
 * changed: فلگ خروجی که اگر خط نسبت به ورودی تغییر کرد، ۱ می‌شود تا بدانم باید کاشی جدید اسپان کنم.
 * خروجی: امتیاز اضافه شده در اثر ادغام‌ها.
 * اگر این تابع خطا داشته باشد یا changed درست ست نشود، بازی دیگر کاشی جدید ایجاد نمی‌کند یا امتیاز غلط می‌شود.
 */
int process_line(int line[], int size, int *changed) {
    int score_gain = 0;          // امتیاز حاصل از ادغام همین خط
    int original[MAX_SIZE];      // نسخه اولیه خط برای تشخیص تغییر

    for (int i = 0; i < size; i++)
        original[i] = line[i];

    // ابتدا صفرها را فیلتر می‌کنم تا کاشی‌ها کنار هم بیایند و آماده ادغام شوند
    int temp[MAX_SIZE] = {0}; // خط فشرده موقت
    int t = 0;                // شاخص آخرین خانه پر شده در temp
    for (int i = 0; i < size; i++)
        if (line[i] != 0)
            temp[t++] = line[i];

    // ادغام انجام می‌دهم و مراقبم هر کاشی فقط یک بار ترکیب شود (با i++)
    for (int i = 0; i < t - 1; i++) {
        int m = check_merge(temp[i], temp[i + 1]);
        if (m) {
            temp[i] = m;
            temp[i + 1] = 0;
            score_gain += m;
            i++; // این i++ را می‌گذارم تا کاشی تازه ادغام‌شده دوباره در همین پاس ترکیب نشود
        }
    }

    // دوباره خط را فشرده می‌کنم تا صفرهای بینابینی حذف شود
    int idx = 0; // موقعیت نوشتن در line نهایی
    for (int i = 0; i < size; i++)
        if (temp[i] != 0)
            line[idx++] = temp[i];

    while (idx < size)
        line[idx++] = 0;

    // تشخیص می‌دهم آیا چیزی نسبت به نسخه اولیه تغییر کرده یا نه تا بیرون بدانم کاشی جدید لازم است
    for (int i = 0; i < size; i++)
        if (line[i] != original[i])
            *changed = 1;

    return score_gain;
}


/*
 * can_move: بررسی می‌کنم حداقل یک حرکت ممکن باقی مانده باشد؛ اگر نه، بازی تمام است.
 * board/size: تخته فعلی؛ هیچ تغییری نمی‌دهم فقط چک می‌کنم.
 * خروجی: ۱ یعنی هنوز حرکتی هست، ۰ یعنی بن‌بست.
 */
int can_move(int board[MAX_SIZE][MAX_SIZE], int size) {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            if (board[i][j] == 0)
                return 1;
            if (i + 1 < size && check_merge(board[i][j], board[i + 1][j]))
                return 1;
            if (j + 1 < size && check_merge(board[i][j], board[i][j + 1]))
                return 1;
        }
    return 0;
}

/*
 * url_decode: نسخه ساده برای پارامتر board؛ فقط %2C را به کاما برمی‌گردانم چون بقیه کاراکترها در این رشته نمی‌آیند.
 * str: همان رشته ورودی board که درجا اصلاح می‌کنم.
 */
void url_decode(char* str) {
    char* pstr = str;
    char* pdst = str;
    while (*pstr) {
        if (*pstr == '%' && *(pstr+1) == '2' && *(pstr+2) == 'C') {
            *pdst = ',';
            pstr += 3;
            pdst++;
        } else {
            *pdst = *pstr;
            pstr++;
            pdst++;
        }
    }
    *pdst = '\0';
}


/*
 * main: نقطه ورود CGI برای بازی؛ ورودی را می‌خوانم، وضعیت را بازسازی می‌کنم، منوی شروع یا صفحه بازی را رندر می‌کنم.
 * اگر این تابع شلوغ به نظر می‌رسد به خاطر این است که می‌خواستم بدون وابستگی سمت کلاینت هم کار کند.
 */
int main() {
    srand(time(NULL)); // سید تصادفی را اینجا می‌گذارم تا اسپان کاشی‌ها هر بار متفاوت باشد
    print_header("فیبوناچی 2048");

    // استایل مخصوص داخل iframe را تزریق می‌کنم تا از استایل کلی کابینت جدا بماند و ارتفاع اضافی نداشته باشد
    printf("<style>\n");
    printf("  .game-container { padding-top: 4px !important; }\n");
    printf("  .fib2048-ui { max-width: 550px; margin: 0 auto; text-align: center; padding-top: 2px; }\n");
    printf("  .fib2048-ui h1, .fib2048-ui h2, .fib2048-ui h3, .fib2048-ui p { color: #e5e7eb; }\n");
    printf("  .fib2048-ui h2 { font-size: 1.3rem; margin: 10px 0; }\n");
    printf("  .fib2048-ui h3 { font-size: 1.1rem; margin: 8px 0; }\n");
    printf("  .fib2048-board { display:grid; gap:8px; margin: 12px auto; max-width: 100%%; }\n");
    printf("  .fib2048-board.gameover { opacity: 0.92; }\n");
    printf("  .fib2048-cell { height:60px; display:flex; align-items:center; justify-content:center; font-weight:900; border-radius:6px; font-size:1.2rem; color:#0f172a; text-shadow:0 1px 2px rgba(255,255,255,0.35); }\n");
    printf("  .fib2048-ui button { background: #10b981; color: #fff; border: 2px solid #0d8a5f; border-radius: 8px; padding: 10px 20px; font-weight: 900; cursor: pointer; margin: 5px; }\n");
    printf("  .fib2048-ui button:hover { filter: brightness(1.1); }\n");
    printf("  .fib2048-ui .disabled { opacity: 0.55; cursor: not-allowed; }\n");
    printf("</style>\n");
    printf("<div class='fib2048-ui'>\n");

    int board[MAX_SIZE][MAX_SIZE] = {0};   // وضعیت فعلی تخته را اینجا نگه می‌دارم
    int size = 4;                          // اندازه پیش‌فرض تخته
    int score = 0;                         // امتیاز انباشته
    char buffer[MAX_BUFFER];               // بافر عمومی برای خواندن پارامترها
    char mode[20] = "menu";               // حالت فعلی UI (menu/start/play)

    // حالت فعلی را از URL می‌خوانم تا بدانم باید منو نشان دهم یا ادامه بازی را
    get_param("mode", buffer);
    if (buffer[0]) strcpy(mode, buffer);

    // اندازه تخته را اگر کاربر تعیین کرده باشد می‌پذیرم و در محدودۀ امن نگه می‌دارم
    get_param("size", buffer);
    if (buffer[0]) size = atoi(buffer);
    if (size < 3) size = 3;          // کوچکتر از ۳ بازی را بی‌معنا می‌کند
    if (size > MAX_SIZE) size = MAX_SIZE; // بزرگ‌تر از سقف بافر مشکل‌ساز می‌شود

    // امتیاز قبلی را برای ادامه بازی می‌خوانم
    get_param("score", buffer);
    if (buffer[0]) score = atoi(buffer);

    // داده تخته را از URL می‌گیرم تا بتوانم وضعیت را بدون کوکی یا سشن ادامه بدهم
    char board_data[MAX_BUFFER];
    get_param("board", board_data);

    url_decode(board_data); // فقط کاماها را برمی‌گردانم چون رشته فیبوناچی چیز دیگری ندارد

    // اگر داده‌ای وجود داشته باشد آن را به ماتریس برمی‌گردانم
    if (board_data[0]) {
        char *p = board_data; // اشاره‌گر پیمایش متن سریال‌شده
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                board[i][j] = atoi(p); // مقدار خانه فعلی
                while (*p != '\0' && *p != ',') {
                    p++; // جلو می‌روم تا به جداکننده برسم
                }
                if (*p == ',') {
                    p++; // از جداکننده عبور می‌کنم تا به عدد بعدی برسم
                }
            }
        }
    }


    if (strcmp(mode, "start") == 0) {
        // شروع بازی تازه: دو کاشی اولیه را اسپان می‌کنم و حالت را روی play می‌گذارم
        spawn_tile(board, size);
        spawn_tile(board, size);
        strcpy(mode, "play");
    }
    else if (strcmp(mode, "play") == 0) {
        // در حالت بازی، جهت حرکت را می‌خوانم و تخته را در همان جهت پردازش می‌کنم
        char dir_buf[8];
        get_param("dir", dir_buf);

        if (dir_buf[0]) {
            int dir = atoi(dir_buf); // 0 بالا، 1 راست، 2 پایین، 3 چپ
            int changed = 0;         // آیا حرکتی رخ داده؟ برای اسپان کاشی جدید نیاز دارم
            int line[MAX_SIZE];      // بافر موقت برای استخراج هر خط/ستون

            for (int i = 0; i < size; i++) {
                if (dir == 3) { // حرکت به چپ: مستقیماً همان سطر را پردازش می‌کنم
                    score += process_line(board[i], size, &changed);
                }
                else if (dir == 1) { // حرکت به راست: سطر را برعکس می‌کنم که از همان تابع استفاده کنم
                    for (int k = 0; k < size; k++)
                        line[k] = board[i][size - 1 - k];
                    score += process_line(line, size, &changed);
                    for (int k = 0; k < size; k++)
                        board[i][size - 1 - k] = line[k];
                }
                else if (dir == 0) { // حرکت به بالا: ستون i را بیرون می‌کشم
                    for (int k = 0; k < size; k++)
                        line[k] = board[k][i];
                    score += process_line(line, size, &changed);
                    for (int k = 0; k < size; k++)
                        board[k][i] = line[k];
                }
                else if (dir == 2) { // حرکت به پایین: ستون را معکوس می‌کنم و بعد برمی‌گردانم
                    for (int k = 0; k < size; k++)
                        line[k] = board[size - 1 - k][i];
                    score += process_line(line, size, &changed);
                    for (int k = 0; k < size; k++)
                        board[size - 1 - k][i] = line[k];
                }
            }

            // اگر واقعاً تغییری رخ داده باشد، کاشی جدید اسپان می‌کنم؛ در غیر این صورت چیزی اضافه نمی‌کنم
            if (changed)
                spawn_tile(board, size);
        }
    }

    int game_over = !can_move(board, size); // اگر هیچ حرکت یا خانه خالی نباشد بازی تمام است

    if (game_over) {
        printf("<h2 style='color:#f87171'>بازی تمام شد</h2>");
    }

    // دو حالت اصلی UI: منوی انتخاب اندازه یا صفحه بازی فعال
    if (strcmp(mode, "menu") == 0) {
        printf("<h2>فیبوناچی 2048</h2><p>ابعاد را انتخاب کنید:</p>");
        printf("<form action='fibonacci_2048.cgi' style='display:flex; gap:10px; justify-content:center;'>");
        printf("<input type='hidden' name='mode' value='start'>");
        printf("<button name='size' value='4' style='background:%s; color:#000; border:2px solid #333; border-radius:8px; padding:8px 16px; font-weight:900;'>4x4</button> ", get_color(1));
        printf("<button name='size' value='5' style='background:%s; color:#fff; border:2px solid #333; border-radius:8px; padding:8px 16px; font-weight:900;'>5x5</button> ", get_color(8));
        printf("<button name='size' value='6' style='background:%s; color:#fff; border:2px solid #333; border-radius:8px; padding:8px 16px; font-weight:900;'>6x6</button>", get_color(13));
        printf("</form>");
    }
    else {
        // تخته فعلی را دوباره به رشته تبدیل می‌کنم تا در URL بعدی ارسال کنم
        char board_str[MAX_BUFFER];
        board_str[0] = '\0';

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                char tmp[16]; // این بافر کوچک برای هر مقدار کافی است
                snprintf(tmp, sizeof(tmp), "%d,", board[i][j]);

                // از سرریز جلوگیری می‌کنم؛ اگر جا نبود باقی خانه‌ها را رها می‌کنم تا امنیت حفظ شود
                if (strlen(board_str) + strlen(tmp) < MAX_BUFFER - 1)
                    strcat(board_str, tmp);
            }
        }

        printf("<h3>امتیاز: %d</h3>", score);
        printf("<div class='fib2048-board %s' style='grid-template-columns:repeat(%d,1fr);'>", game_over ? "gameover" : "", size);

        // کل شبکه را رندر می‌کنم تا وضعیت فعلی دیده شود
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++) {
                int v = board[i][j];
                printf("<div class='fib2048-cell' style='background:%s;'>", get_color(v));
                if (v) printf("%d", v);
                printf("</div>");
            }

        printf("</div>");

        // فرم حرکات بعدی را می‌سازم تا بدون جاوااسکریپت هم قابل بازی باشد
        printf("<form action='fibonacci_2048.cgi'>");
        printf("<input type='hidden' name='mode' value='play'>");
        printf("<input type='hidden' name='size' value='%d'>", size);
        printf("<input type='hidden' name='score' value='%d'>", score);
        printf("<input type='hidden' name='board' value='%s'>", board_str);

        printf("<div style='margin-top:15px;text-align:center;'>");

        printf("<button name='dir' value='0'%s>⬆️ بالا</button><br>", game_over ? " class='disabled' disabled" : "");
        printf("<button name='dir' value='3' style='margin:0 5px;'%s>➡️ راست</button>", game_over ? " class='disabled' disabled" : "");
        printf("<button name='dir' value='2'%s>⬇️ پایین</button>", game_over ? " class='disabled' disabled" : "");
        printf("<button name='dir' value='1' style='margin:0 5px;'%s>⬅️ چپ</button>", game_over ? " class='disabled' disabled" : "");

        printf("</div>");
        printf("</form>");


    }

    printf("</div>");
    print_footer();
    return 0;
}
