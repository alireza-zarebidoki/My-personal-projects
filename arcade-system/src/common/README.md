# 🛠️ Common Utilities (توابع مشترک)

این پوشه حاوی توابع کمکی و utility هایی است که توسط تمام بازی‌های CGI استفاده می‌شوند.

---

## 📋 فهرست مطالب

- [معرفی](#معرفی)
- [فایل‌ها](#فایلها)
- [توابع موجود](#توابع-موجود)
- [نحوه استفاده](#نحوه-استفاده)
- [جزئیات پیاده‌سازی](#جزئیات-پیادهسازی)
- [مثال‌ها](#مثالها)

---

## 🎯 معرفی

### چرا c_utils؟

در هر بازی CGI، کارهای مشترکی وجود دارد که باید انجام شود:
- چاپ HTTP header
- خواندن پارامترهای URL
- decode کردن URL encoding
- escape کردن HTML entities
- مدیریت QUERY_STRING

به جای کپی/پیست کردن این کدها در هر بازی، من یک ماژول مشترک ساختم که:
✅ **DRY** (Don't Repeat Yourself): کد تکراری کاهش می‌یابد
✅ **Maintainable**: تغییرات در یک جا اعمال می‌شود
✅ **Tested**: یک بار تست می‌شود، همه جا استفاده می‌شود
✅ **Clean**: کد بازی‌ها تمیزتر می‌شود

---

## 📁 فایل‌ها

```
src/common/
├── c_utils.h       # Header file (توابع، ماکروها، تعاریف)
└── c_utils.c       # Implementation (پیاده‌سازی توابع)
```

### c_utils.h
**خط‌ها**: ~50 خط
**محتوا**:
- `#include` های استاندارد
- تعریف ماکروها
- function prototypes
- ثابت‌های عمومی

### c_utils.c
**خط‌ها**: ~200 خط
**محتوا**:
- پیاده‌سازی توابع
- مدیریت حافظه
- error handling

---

## 🔧 توابع موجود

### 1. `print_header()`

**امضا**:
```c
void print_header();
```

**توضیح**:
چاپ HTTP header که برای تمام CGI ها لازم است.

**خروجی**:
```
Content-Type: text/html; charset=UTF-8

```

**چرا لازم است؟**:
- CGI باید قبل از هر چیز HTTP header بفرستد
- بدون این header، سرور 500 error می‌دهد
- charset=UTF-8 برای پشتیبانی فارسی

**مثال**:
```c
int main() {
    print_header();  // اولین خط
    printf("<html>...</html>");
    return 0;
}
```

---

### 2. `get_param(const char* name)`

**امضا**:
```c
char* get_param(const char* name);
```

**توضیح**:
دریافت مقدار یک پارامتر از QUERY_STRING.

**پارامترها**:
- `name`: نام پارامتر (مثلاً "mode"، "player"، "turn")

**خروجی**:
- اگر پارامتر وجود داشته باشد: مقدار decoded شده
- اگر وجود نداشته باشد: `NULL`

**نکات**:
- مقدار برگشتی از heap تخصیص می‌یابد
- باید با `free()` آزاد شود (اگر نیاز به استفاده طولانی مدت داری)
- برای استفاده موقت نیازی به free نیست (CGI بعد از اتمام خودش آزاد می‌کند)

**مثال**:
```c
char *mode = get_param("mode");
if (mode && strcmp(mode, "start") == 0) {
    // حالت start
}

char *player = get_param("player");
if (player) {
    int player_num = atoi(player);
}
```

**نحوه کار**:
```
URL: /game.cgi?mode=start&level=2&player=Ali

QUERY_STRING environment variable:
"mode=start&level=2&player=Ali"

get_param("mode")   → "start"
get_param("level")  → "2"
get_param("player") → "Ali"
get_param("score")  → NULL (وجود ندارد)
```

---

### 3. `url_decode(char* dst, const char* src)`

**امضا**:
```c
void url_decode(char* dst, const char* src);
```

**توضیح**:
تبدیل URL-encoded string به string عادی.

**پارامترها**:
- `dst`: مقصد (رشته خروجی)
- `src`: منبع (رشته URL-encoded)

**نحوه کار URL Encoding**:
```
Space    → %20 یا +
!        → %21
"        → %22
%        → %25
فارسی    → %D8%A7%D8%B2... (UTF-8 bytes)
```

**مثال**:
```c
char input[] = "Hello+World%21";
char output[100];

url_decode(output, input);
// output = "Hello World!"

char input2[] = "%D8%B3%D9%84%D8%A7%D9%85";
url_decode(output, input2);
// output = "سلام"
```

**کاربرد**:
- در `get_param()` استفاده می‌شود
- برای decode کردن form data
- برای پارامترهایی که شامل کاراکترهای خاص هستند

**جزئیات پیاده‌سازی**:
```c
void url_decode(char* dst, const char* src) {
    while (*src) {
        if (*src == '%') {
            // دو رقم hex بعد از %
            int value;
            sscanf(src + 1, "%2x", &value);
            *dst++ = (char)value;
            src += 3;  // جلو بروی %XX
        } else if (*src == '+') {
            *dst++ = ' ';  // + یعنی space
            src++;
        } else {
            *dst++ = *src++;  // عادی
        }
    }
    *dst = '\0';  // null terminator
}
```

---

### 4. `html_escape(char* str)`

**امضا**:
```c
void html_escape(char* str);
```

**توضیح**:
تبدیل کاراکترهای خاص HTML به entity ها (برای جلوگیری از XSS).

**کاراکترهای تبدیل شونده**:
```
<   → &lt;
>   → &gt;
"   → &quot;
'   → &#39;
&   → &amp;
```

**چرا لازم است؟**:
- **امنیت**: جلوگیری از XSS attacks
- **صحت**: نمایش صحیح متن در HTML

**مثال**:
```c
char input[] = "<script>alert('XSS')</script>";
html_escape(input);
// output: "&lt;script&gt;alert(&#39;XSS&#39;)&lt;/script&gt;"

char name[] = "Ali & Reza";
html_escape(name);
// output: "Ali &amp; Reza"
```

**کاربرد**:
```c
char *user_input = get_param("name");
if (user_input) {
    html_escape(user_input);  // امن سازی
    printf("<p>سلام %s!</p>", user_input);
}
```

---

### 5. ماکروهای کمکی

#### `MAX_QUERY_LEN`
```c
#define MAX_QUERY_LEN 4096
```
حداکثر طول QUERY_STRING.

#### `MAX_PARAM_LEN`
```c
#define MAX_PARAM_LEN 256
```
حداکثر طول یک پارامتر.

---

## 📚 نحوه استفاده

### مرحله 1: Include کردن

```c
#include "../common/c_utils.h"
```

### مرحله 2: استفاده از توابع

```c
int main() {
    // 1. چاپ header
    print_header();

    // 2. دریافت پارامترها
    char *mode = get_param("mode");
    char *level = get_param("level");
    char *name = get_param("name");

    // 3. escape برای امنیت
    if (name) {
        html_escape(name);
    }

    // 4. منطق بازی
    printf("<!DOCTYPE html>\n");
    printf("<html>\n");

    if (!mode || strcmp(mode, "init") == 0) {
        printf("<h1>شروع بازی</h1>\n");
    } else if (strcmp(mode, "play") == 0) {
        printf("<h1>بازی کن، %s</h1>\n", name ? name : "مهمان");
    }

    printf("</html>\n");
    return 0;
}
```

### مرحله 3: کامپایل

```bash
gcc -o cgi-bin/game.cgi \
    src/games/game.c \
    src/common/c_utils.c \
    -I src/common
```

یا با Makefile:
```makefile
game.cgi: src/games/game.c src/common/c_utils.c
	$(CC) $(CFLAGS) $^ -o cgi-bin/$@ $(INCLUDES)
```

---

## 🔍 جزئیات پیاده‌سازی

### مدیریت حافظه

```c
// در get_param()
char* get_param(const char* name) {
    char *result = malloc(MAX_PARAM_LEN);
    if (!result) {
        return NULL;  // out of memory
    }

    // پردازش...

    return result;  // caller مسئول free کردن است
}
```

**نکته**: در CGI، چون process سریع terminate می‌شود، معمولاً نیازی به `free()` نیست. سیستم عامل خودش تمام حافظه را آزاد می‌کند.

### پارس کردن QUERY_STRING

```c
char *query = getenv("QUERY_STRING");
if (!query) {
    return NULL;  // هیچ پارامتری نیست
}

// مثال: "mode=start&level=2&player=Ali"

// جدا کردن با &
char *token = strtok(query, "&");
while (token) {
    // جدا کردن key=value
    char *eq = strchr(token, '=');
    if (eq) {
        *eq = '\0';  // جدا کردن key و value
        char *key = token;
        char *value = eq + 1;

        if (strcmp(key, name) == 0) {
            url_decode(result, value);
            return result;
        }
    }
    token = strtok(NULL, "&");
}
```

### امنیت

**مشکلات احتمالی**:
1. **Buffer Overflow**: اگر پارامتر خیلی بزرگ باشد
2. **XSS**: اگر پارامتر شامل `<script>` باشد
3. **SQL Injection**: اگر پارامتر به database برود (ما JSON داریم)

**راه حل‌ها**:
```c
// 1. محدود کردن طول
if (strlen(param) > MAX_PARAM_LEN) {
    return NULL;  // خیلی بزرگ
}

// 2. HTML escape
html_escape(user_input);

// 3. Validation
if (!is_valid_input(param)) {
    return NULL;
}
```

---

## 💡 مثال‌های کاربردی

### مثال 1: بازی ساده

```c
#include "../common/c_utils.h"

int main() {
    print_header();

    char *name = get_param("name");
    char *score = get_param("score");

    printf("<!DOCTYPE html>\n");
    printf("<html dir='rtl'>\n");
    printf("<head><meta charset='UTF-8'></head>\n");
    printf("<body>\n");

    if (name) {
        html_escape(name);
        printf("<h1>سلام %s</h1>\n", name);
    }

    if (score) {
        printf("<p>امتیاز: %s</p>\n", score);
    }

    printf("</body>\n");
    printf("</html>\n");

    return 0;
}
```

**استفاده**:
```
/cgi-bin/game.cgi?name=Ali&score=100
```

**خروجی**:
```html
<!DOCTYPE html>
<html dir='rtl'>
<head><meta charset='UTF-8'></head>
<body>
<h1>سلام Ali</h1>
<p>امتیاز: 100</p>
</body>
</html>
```

---

### مثال 2: Form با متن فارسی

```c
#include "../common/c_utils.h"

int main() {
    print_header();

    char *message = get_param("msg");

    printf("<!DOCTYPE html>\n");
    printf("<html dir='rtl'>\n");
    printf("<body>\n");

    if (message) {
        html_escape(message);
        printf("<p>پیام: %s</p>\n", message);
    } else {
        printf("<form method='GET'>\n");
        printf("<input name='msg' placeholder='پیام خود را بنویسید'>\n");
        printf("<button type='submit'>ارسال</button>\n");
        printf("</form>\n");
    }

    printf("</body>\n");
    printf("</html>\n");

    return 0;
}
```

---

### مثال 3: بررسی چند پارامتر

```c
#include "../common/c_utils.h"

int main() {
    print_header();

    char *mode = get_param("mode");
    char *p1 = get_param("p1");
    char *p2 = get_param("p2");
    char *turn = get_param("turn");

    printf("<!DOCTYPE html>\n");
    printf("<html>\n");
    printf("<body>\n");

    if (!mode || strcmp(mode, "init") == 0) {
        printf("<h1>بازی جدید</h1>\n");
        printf("<a href='?mode=start&p1=Ali&p2=Reza&turn=1'>شروع</a>\n");
    }
    else if (strcmp(mode, "start") == 0) {
        printf("<h1>بازیکن 1: %s</h1>\n", p1 ? p1 : "ناشناس");
        printf("<h1>بازیکن 2: %s</h1>\n", p2 ? p2 : "ناشناس");
        printf("<p>نوبت: %s</p>\n", turn ? turn : "1");
    }

    printf("</body>\n");
    printf("</html>\n");

    return 0;
}
```

---

## 🧪 تست

### تست مستقیم

```bash
# بدون پارامتر
./cgi-bin/game.cgi

# با پارامتر
QUERY_STRING="mode=start&level=2" ./cgi-bin/game.cgi
```

### تست URL Decode

```c
// test_url_decode.c
#include "c_utils.h"

int main() {
    char input[] = "Hello+World%21";
    char output[100];

    url_decode(output, input);
    printf("Decoded: %s\n", output);  // "Hello World!"

    return 0;
}
```

```bash
gcc -o test_url_decode test_url_decode.c src/common/c_utils.c
./test_url_decode
```

### تست HTML Escape

```c
// test_html_escape.c
#include "c_utils.h"

int main() {
    char input[] = "<script>alert('XSS')</script>";
    html_escape(input);
    printf("Escaped: %s\n", input);

    return 0;
}
```

---

## 🔮 بهبودهای آینده

- [ ] `get_post_data()`: خواندن POST data
- [ ] `set_cookie()`: تنظیم cookie
- [ ] `get_cookie()`: خواندن cookie
- [ ] `json_encode()`: تبدیل struct به JSON
- [ ] `json_decode()`: پارس JSON
- [ ] `validate_input()`: validation عمومی
- [ ] `log_error()`: لاگ error ها
- [ ] `redirect()`: HTTP redirect
- [ ] `parse_json()`: parser ساده JSON

---

## 📝 نکات مهم

### Performance
- توابع سبک و سریع هستند
- بدون allocation اضافی
- برای CGI بهینه شده

### امنیت
⚠️ **هشدارهای امنیتی**:
- همیشه `html_escape()` را روی user input اجرا کنید
- محدودیت طول ورودی را چک کنید
- فرض نکنید پارامتر همیشه وجود دارد (check for NULL)

### خطاهای رایج

❌ **اشتباه**:
```c
char *name = get_param("name");
printf("<p>%s</p>", name);  // اگر name = NULL، segfault!
```

✅ **درست**:
```c
char *name = get_param("name");
if (name) {
    html_escape(name);
    printf("<p>%s</p>", name);
} else {
    printf("<p>بدون نام</p>");
}
```

---

**این ماژول با ❤️ و دقت نوشته شده است**
