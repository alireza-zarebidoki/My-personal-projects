# 💾 Data Layer (لایه داده)

این پوشه حاوی فایل‌های JSON است که به عنوان "دیتابیس" ساده پروژه عمل می‌کنند.

---

## 📋 فهرست فایل‌ها

```
data/
├── users.json          # اطلاعات کاربران (username, password, coins, role)
├── sessions.json       # session های فعال کاربران
├── admin.json          # تنظیمات ادمین
└── admin_logs.json     # لاگ تراکنش‌های سکه
```

---

## 🗂️ ساختار فایل‌ها

### 1. users.json

**نقش**: ذخیره اطلاعات کاربران

**ساختار**:
```json
{
    "users": [
        {
            "username": "admin",
            "password": "hashed_password_here",
            "coins": 1000,
            "role": "admin",
            "created_at": "2025-12-26T10:30:00Z",
            "last_login": "2025-12-26T15:20:00Z"
        },
        {
            "username": "ali",
            "password": "hashed_password_here",
            "coins": 50,
            "role": "user",
            "created_at": "2025-12-26T11:00:00Z",
            "last_login": "2025-12-26T14:45:00Z"
        }
    ]
}
```

**فیلدها**:
- `username`: نام کاربری (unique)
- `password`: رمز عبور (هش شده - **توجه**: در نسخه فعلی plain text است!)
- `coins`: تعداد سکه‌های کاربر
- `role`: نقش (`"user"` یا `"admin"`)
- `created_at`: تاریخ ثبت‌نام
- `last_login`: آخرین ورود

---

### 2. sessions.json

**نقش**: ذخیره session های فعال

**ساختار**:
```json
{
    "sessions": [
        {
            "sessionToken": "abc123def456...",
            "username": "ali",
            "created_at": "2025-12-26T15:00:00Z",
            "expires_at": "2025-12-27T15:00:00Z"
        }
    ]
}
```

**فیلدها**:
- `sessionToken`: توکن منحصر به فرد session (UUID یا random string)
- `username`: نام کاربری مربوط به session
- `created_at`: زمان ایجاد session
- `expires_at`: زمان انقضای session (معمولاً 24 ساعت)

**نحوه کار**:
1. کاربر login می‌کند
2. سرور یک `sessionToken` تولید می‌کند
3. token در `sessions.json` ذخیره می‌شود
4. token به کلاینت برگردانده می‌شود
5. کلاینت token را در localStorage ذخیره می‌کند
6. در هر request، token ارسال می‌شود
7. سرور token را validate می‌کند

---

### 3. admin.json

**نقش**: تنظیمات عمومی ادمین

**ساختار**:
```json
{
    "settings": {
        "default_coins": 20,
        "coin_reward_multiplier": 1.5,
        "max_concurrent_sessions": 5,
        "session_timeout_hours": 24
    },
    "game_prices": {
        "guess_number": 3,
        "tictactoe_limited": 5,
        "rps_5move": 4,
        "hangman_battle": 6,
        "fibonacci_2048": 7,
        "chess_modified": 10,
        "checkers": 8
    },
    "game_rewards": {
        "guess_number": 5,
        "tictactoe_limited": 8,
        "rps_5move": 6,
        "hangman_battle": 10,
        "fibonacci_2048": 12,
        "chess_modified": 20,
        "checkers": 15
    }
}
```

**فیلدها**:
- `settings`: تنظیمات عمومی
  - `default_coins`: سکه پیش‌فرض برای کاربر جدید
  - `coin_reward_multiplier`: ضریب پاداش
  - `max_concurrent_sessions`: حداکثر session همزمان
  - `session_timeout_hours`: مدت زمان اعتبار session
- `game_prices`: قیمت هر بازی
- `game_rewards`: پاداش برد در هر بازی

---

### 4. admin_logs.json

**نقش**: ثبت تمام تراکنش‌های سکه (برای audit trail)

**ساختار**:
```json
{
    "logs": [
        {
            "id": 1,
            "username": "ali",
            "action": "subtract",
            "amount": 5,
            "reason": "بازی دوز محدود",
            "admin": null,
            "timestamp": "2025-12-26T15:10:00Z",
            "before": 50,
            "after": 45
        },
        {
            "id": 2,
            "username": "ali",
            "action": "add",
            "amount": 8,
            "reason": "پاداش برد در دوز محدود",
            "admin": null,
            "timestamp": "2025-12-26T15:15:00Z",
            "before": 45,
            "after": 53
        },
        {
            "id": 3,
            "username": "reza",
            "action": "admin_add",
            "amount": 100,
            "reason": "هدیه ادمین",
            "admin": "admin",
            "timestamp": "2025-12-26T16:00:00Z",
            "before": 10,
            "after": 110
        }
    ]
}
```

**فیلدها**:
- `id`: شناسه یکتا لاگ
- `username`: نام کاربری که سکه‌اش تغییر کرده
- `action`: نوع عملیات
  - `"add"`: اضافه شدن (پاداش)
  - `"subtract"`: کسر شدن (خرید بازی)
  - `"admin_add"`: اضافه توسط ادمین
  - `"admin_subtract"`: کسر توسط ادمین
- `amount`: مقدار تغییر
- `reason`: دلیل تغییر
- `admin`: نام ادمینی که عملیات را انجام داده (اگر admin_add یا admin_subtract)
- `timestamp`: زمان عملیات
- `before`: موجودی قبل
- `after`: موجودی بعد

---

## 🔧 نحوه استفاده در Backend

### خواندن JSON

```c
// مثال: خواندن users.json
FILE *fp = fopen("data/users.json", "r");
if (!fp) {
    print_error("Cannot open users.json");
    return;
}

// خواندن محتوا
fseek(fp, 0, SEEK_END);
long size = ftell(fp);
rewind(fp);

char *content = malloc(size + 1);
fread(content, 1, size, fp);
content[size] = '\0';
fclose(fp);

// پارس JSON (manual یا با کتابخانه)
// ...
```

### نوشتن JSON

```c
// مثال: آپدیت users.json
FILE *fp = fopen("data/users.json", "w");
if (!fp) {
    print_error("Cannot write users.json");
    return;
}

// ساخت JSON string
char json[4096];
sprintf(json, "{\n  \"users\": [\n");

for (each user) {
    sprintf(json + strlen(json),
        "    {\n"
        "      \"username\": \"%s\",\n"
        "      \"password\": \"%s\",\n"
        "      \"coins\": %d,\n"
        "      \"role\": \"%s\"\n"
        "    }%s\n",
        user.username, user.password, user.coins, user.role,
        (is_last ? "" : ",")
    );
}

sprintf(json + strlen(json), "  ]\n}\n");

// نوشتن به فایل
fwrite(json, 1, strlen(json), fp);
fclose(fp);
```

---

## 🔒 امنیت

### مشکلات امنیتی فعلی

⚠️ **هشدار**: این implementation برای محیط production امن نیست!

**مشکلات**:

1. **Plain Text Passwords**:
   ```json
   "password": "123456"  // ❌ خطرناک!
   ```
   **باید باشد**:
   ```json
   "password": "$2b$10$abc...xyz"  // ✅ هش شده با bcrypt
   ```

2. **No File Locking**:
   - اگر دو CGI همزمان بنویسند، race condition
   - ممکن است داده از دست برود

3. **No Encryption**:
   - فایل‌های JSON plain text هستند
   - هر کسی می‌تواند بخواند

4. **No Backup**:
   - اگر فایل خراب شود، همه داده از دست می‌رود

### بهبودهای امنیتی

برای production باید:

1. **Hash passwords**:
   ```c
   #include <bcrypt.h>
   char hash[BCRYPT_HASHSIZE];
   bcrypt_hashpw(password, hash);
   ```

2. **File locking**:
   ```c
   #include <fcntl.h>
   int fd = open("data/users.json", O_RDWR);
   flock(fd, LOCK_EX);  // exclusive lock
   // ... read/write ...
   flock(fd, LOCK_UN);  // unlock
   ```

3. **Migrate to real database**:
   ```c
   // SQLite
   sqlite3 *db;
   sqlite3_open("data/arcade.db", &db);
   sqlite3_exec(db, "SELECT * FROM users WHERE username = ?", ...);
   ```

4. **Encryption at rest**:
   ```bash
   # فایل‌ها را encrypt کنید
   openssl enc -aes-256-cbc -in users.json -out users.json.enc
   ```

---

## 📊 حجم داده

**تخمین**:
- هر کاربر: ~200 bytes
- هر session: ~150 bytes
- هر لاگ: ~250 bytes

**برای 100 کاربر**:
- users.json: ~20 KB
- sessions.json: ~15 KB (اگر همه online)
- admin_logs.json: ~25 KB (برای 100 تراکنش)

**کل**: حدود 60 KB (خیلی کم!)

---

## 🔄 Backup و Recovery

### Manual Backup

```bash
# Backup
cp -r data/ data_backup_$(date +%Y%m%d_%H%M%S)/

# Restore
cp -r data_backup_20251226_150000/* data/
```

### Automated Backup (Cron)

```bash
# هر روز ساعت 2 صبح backup بگیر
0 2 * * * /path/to/backup_script.sh
```

**backup_script.sh**:
```bash
#!/bin/bash
BACKUP_DIR="/path/to/backups"
DATE=$(date +%Y%m%d_%H%M%S)
cp -r data/ "$BACKUP_DIR/data_$DATE/"

# پاک کردن backup های قدیمی (بیش از 7 روز)
find "$BACKUP_DIR" -type d -mtime +7 -exec rm -rf {} \;
```

---

## 🧪 تست

### بررسی syntax JSON

```bash
# نصب jq (JSON processor)
sudo apt-get install jq

# بررسی syntax
cat data/users.json | jq .

# pretty print
jq . data/users.json > data/users_formatted.json
```

### مثال داده‌های تست

**users.json**:
```json
{
    "users": [
        {
            "username": "test_user",
            "password": "test123",
            "coins": 100,
            "role": "user",
            "created_at": "2025-12-26T00:00:00Z",
            "last_login": "2025-12-26T00:00:00Z"
        },
        {
            "username": "test_admin",
            "password": "admin123",
            "coins": 9999,
            "role": "admin",
            "created_at": "2025-12-26T00:00:00Z",
            "last_login": "2025-12-26T00:00:00Z"
        }
    ]
}
```

---

## 🔮 Migration به Database واقعی

### چرا باید migrate کنیم؟

❌ **مشکلات JSON files**:
- Race condition در concurrent writes
- بدون transaction support
- Performance پایین برای جستجو
- بدون indexing
- بدون relation بین داده‌ها

✅ **مزایای Database**:
- ACID transactions
- Indexing برای جستجوی سریع
- Concurrent access management
- Backup و recovery ابزارهای حرفه‌ای
- Query language (SQL)

### SQLite Schema

```sql
-- users table
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,  -- hashed
    coins INTEGER DEFAULT 20,
    role TEXT DEFAULT 'user',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP
);

-- sessions table
CREATE TABLE sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_token TEXT UNIQUE NOT NULL,
    user_id INTEGER NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expires_at TIMESTAMP NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

-- coin_logs table
CREATE TABLE coin_logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    action TEXT NOT NULL,
    amount INTEGER NOT NULL,
    reason TEXT,
    admin_id INTEGER,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    balance_before INTEGER,
    balance_after INTEGER,
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (admin_id) REFERENCES users(id)
);

-- indexes
CREATE INDEX idx_sessions_token ON sessions(session_token);
CREATE INDEX idx_sessions_user ON sessions(user_id);
CREATE INDEX idx_logs_user ON coin_logs(user_id);
CREATE INDEX idx_logs_timestamp ON coin_logs(timestamp);
```

---

## 📝 نکات مهم

### 1. File Permissions

```bash
# فایل‌های data باید writable باشند
chmod 664 data/*.json

# پوشه data
chmod 775 data/
```

### 2. Git Ignore

در `.gitignore`:
```
# فایل‌های داده را commit نکنید
data/*.json

# اما template را نگه دارید
!data/*.json.template
```

### 3. Init Script

**init_data.sh**:
```bash
#!/bin/bash

# ساخت پوشه data
mkdir -p data

# ایجاد فایل‌های خالی
echo '{"users":[]}' > data/users.json
echo '{"sessions":[]}' > data/sessions.json
echo '{"settings":{},"game_prices":{},"game_rewards":{}}' > data/admin.json
echo '{"logs":[]}' > data/admin_logs.json

echo "Data files initialized!"
```

---

**Data layer با JSON به سادگی مدیریت می‌شود، اما برای production به Database نیاز داریم!**
