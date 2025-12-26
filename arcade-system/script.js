/*
 * ═══════════════════════════════════════════════════════════════════════════
 *                       فایل اصلی JavaScript پروژه Arcade System
 *                                  script.js
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * این فایل را من خودم از صفر نوشتم تا تمام منطق سمت کلاینت کابینت آرکید
 * را مدیریت کند. این فایل قلب تپنده رابط کاربری است.
 *
 * مسئولیت‌های این فایل:
 * - مدیریت منوی چرخشی (carousel) بازی‌ها
 * - کنترل احراز هویت (login/register/logout)
 * - مدیریت سکه‌ها و اقتصاد بازی
 * - بارگذاری و اجرای بازی‌ها در iframe
 * - مدیریت پنل کاربری و ادمین
 * - رابط راهنما (help modal)
 * - انیمیشن ذرات پس‌زمینه
 *
 * ساختار کلی:
 * 1. راه‌اندازی particles.js برای پس‌زمینه
 * 2. تعریف متغیرهای سراسری و ثابت‌ها
 * 3. توابع کمکی (helper functions)
 * 4. توابع احراز هویت
 * 5. توابع منوی بازی
 * 6. توابع پنل ادمین
 * 7. Event listeners
 * 8. مقداردهی اولیه در window.onload
 *
 * وابستگی‌ها:
 * - auth.js: ماژول احراز هویت که قبل از این فایل لود می‌شود
 * - particles.js: کتابخانه ذرات پس‌زمینه (اختیاری)
 *
 * یادآوری برای خودم:
 * - این فایل با auth.js ارتباط نزدیک دارد
 * - تغییرات در اینجا باید با سرور CGI هماهنگ باشد
 * - قیمت بازی‌ها باید با سرور همگام بماند
 */

// ═══════════════════════════════════════════════════════════════════════════
//                          بخش ۱: راه‌اندازی Particles.js
// ═══════════════════════════════════════════════════════════════════════════

/*
 * راه‌اندازی انیمیشن ذرات پس‌زمینه
 *
 * این بخش را اضافه کردم تا حال و هوای retro و آرکید قدیمی را به صفحه بدهم.
 * ذرات سبز نئونی که به هم وصل می‌شوند و با موس تعامل دارند.
 *
 * چرا particles.js:
 * - کتابخانه سبک و سریع است
 * - خیلی زیبا و حرفه‌ای به نظر می‌رسد
 * - با تم نئونی کابینت هماهنگ است
 * - تعاملی است (با موس کار می‌کند)
 *
 * تنظیمات:
 * - تعداد ذرات: 80 عدد (نه خیلی شلوغ، نه خیلی خالی)
 * - رنگ: #10b981 (سبز نئونی تم ما)
 * - شکل: دایره
 * - opacity: 0.3 (نیمه‌شفاف تا حواس را پرت نکند)
 * - اندازه: 3 پیکسل (تصادفی)
 * - خطوط اتصال: فعال، فاصله 150 پیکسل
 * - حرکت: سرعت 1 (آرام و ملایم)
 * - تعامل: grab mode هنگام hover
 *
 * اگر particles.js لود نشده باشد:
 * - typeof برای جلوگیری از خطا چک می‌کند
 * - بدون این کتابخانه، صفحه عادی کار می‌کند
 *
 * نکته: این کد باید قبل از DOMContentLoaded اجرا شود تا قبل از render صفحه آماده باشد
 */
if (typeof particlesJS !== 'undefined') {
    particlesJS("particles-js", {
        "particles": {
            "number": { "value": 80 },  // تعداد ذرات را به 80 محدود کردم تا سیستم کند نشود
            "color": { "value": "#10b981" },  // رنگ سبز نئونی که با تم کابینت هماهنگ است
            "shape": { "type": "circle" },  // شکل دایره‌ای برای سادگی و زیبایی
            "opacity": { "value": 0.3 },  // نیمه‌شفاف تا پشت آن محتوا قابل رویت باشد
            "size": { "value": 3, "random": true },  // اندازه کوچک و تصادفی برای طبیعی بودن
            "line_linked": {
                "enable": true,  // خطوط اتصال را فعال کردم برای جلوه بصری بهتر
                "distance": 150,  // حداکثر فاصله اتصال
                "color": "#10b981",  // رنگ خطوط هم سبز نئونی
                "opacity": 0.15,  // خطوط کمرنگ‌تر از خود ذرات
                "width": 1  // ضخامت خط 1 پیکسل
            },
            "move": {
                "enable": true,  // حرکت ذرات را فعال کردم
                "speed": 1  // سرعت پایین برای آرامش بصری
            }
        },
        "interactivity": {
            "events": {
                "onhover": {
                    "enable": true,  // تعامل با موس را فعال کردم
                    "mode": "grab"  // حالت grab: ذرات به موس جذب می‌شوند
                }
            }
        }
    });
}

// ═══════════════════════════════════════════════════════════════════════════
//              بخش ۲: تعریف متغیرهای سراسری و ثابت‌ها
// ═══════════════════════════════════════════════════════════════════════════

/*
 * متغیرهای مربوط به منوی چرخشی (Carousel)
 *
 * این متغیرها وضعیت انتخاب بازی در carousel را نگه می‌دارند.
 * carousel من یک منوی افقی است که با کلیدهای چپ/راست کنترل می‌شود.
 */

// selectedIndex: ایندکس بازی انتخاب شده فعلی (0 تا games.length-1)
// این عدد مشخص می‌کند کدام بازی در مرکز carousel قرار دارد و هایلایت است
let selectedIndex = 0;

// games: لیست تمام بازی‌ها در DOM
// هر بازی یک div با class="game-item" است که شامل آیکون، نام و قیمت است
const games = document.querySelectorAll('.game-item');

// carousel: کانتینر اصلی که تمام بازی‌ها در آن هستند
// این المان را translate می‌کنم تا carousel حرکت کند
const carousel = document.querySelector('.game-carousel');

/*
 * رفرنس‌های DOM - دروازه‌های دسترسی به UI
 *
 * این رفرنس‌ها را یکبار در ابتدا می‌گیرم و در تمام برنامه استفاده می‌کنم.
 * هر getElementById یا querySelector یک عملیات جستجوی DOM است، پس
 * برای بهینه‌سازی همه را در ابتدا cache می‌کنم.
 *
 * دسته‌بندی رفرنس‌ها:
 * ۱. سیستم احراز هویت (Auth System)
 * ۲. پنل حساب کاربری (Account Panel)
 * ۳. پنل ادمین (Admin Panel)
 * ۴. پنجره راهنما (Help Modal)
 */

// ─────────────────────────────────────────────────────────────────────────
// گروه ۱: سیستم احراز هویت
// ─────────────────────────────────────────────────────────────────────────

// authOverlay: پنجره modal ورود/ثبت‌نام که روی صفحه ظاهر می‌شود
const authOverlay = document.getElementById('auth-overlay');

// openAuthBtn: دکمه "ورود" در نوار بالا که modal را باز می‌کند
const openAuthBtn = document.getElementById('open-auth');

// closeAuthBtn: دکمه × برای بستن modal احراز هویت
const closeAuthBtn = document.getElementById('close-auth');

// logoutBtn: دکمه خروج در نوار بالا (فقط برای کاربر login شده نمایان است)
const logoutBtn = document.getElementById('logout-btn');

// userDisplay: نمایش نام کاربری در نوار بالا
const userDisplay = document.getElementById('user-display');

// userPill: دکمه pill شکل که نام کاربر و وضعیت آنلاین را نشان می‌دهد
const userPill = document.getElementById('user-pill');

// authFeedback: المان نمایش پیام خطا/موفقیت در modal احراز هویت
const authFeedback = document.getElementById('auth-feedback');

// tabButtons: دکمه‌های تب "ورود" و "ثبت‌نام" در modal
const tabButtons = document.querySelectorAll('.tab-btn');

// loginForm: فرم ورود (username + password)
const loginForm = document.getElementById('login-form');

// registerForm: فرم ثبت‌نام (username + password)
const registerForm = document.getElementById('register-form');

// فیلدهای ورودی فرم ورود
const loginUsername = document.getElementById('login-username');
const loginPassword = document.getElementById('login-password');

// فیلدهای ورودی فرم ثبت‌نام
const regUsername = document.getElementById('reg-username');
const regPassword = document.getElementById('reg-password');

// ─────────────────────────────────────────────────────────────────────────
// گروه ۲: پنل حساب کاربری
// ─────────────────────────────────────────────────────────────────────────

// coinAmountEl: نمایش تعداد سکه‌ها در نوار بالا
const coinAmountEl = document.getElementById('coin-amount');

// accountBtn: دکمه "حساب کاربری" در نوار بالا (بعد از ورود ظاهر می‌شود)
const accountBtn = document.getElementById('account-btn');

// accountPanel: پنل کشویی حساب کاربری که از بالا می‌آید
const accountPanel = document.getElementById('account-panel');

// اطلاعات نمایشی در پنل حساب کاربری
const panelUsername = document.getElementById('panel-username');
const panelRole = document.getElementById('panel-role');
const panelCoins = document.getElementById('panel-coins');

// دکمه‌های عملیاتی پنل حساب
const accountAdminBtn = document.getElementById('account-admin-btn');  // دکمه ورود به پنل ادمین
const accountLogoutBtn = document.getElementById('account-logout-btn');  // دکمه خروج

// ─────────────────────────────────────────────────────────────────────────
// گروه ۳: پنل ادمین
// ─────────────────────────────────────────────────────────────────────────

// adminBtn: دکمه دسترسی به پنل ادمین در نوار بالا (فقط برای admin نمایان است)
const adminBtn = document.getElementById('admin-btn');

// adminOverlay: modal پنل ادمین
const adminOverlay = document.getElementById('admin-overlay');

// adminClose: دکمه بستن modal ادمین
const adminClose = document.getElementById('admin-close');

// adminUsersEl: جدول/لیست کاربران در پنل ادمین
const adminUsersEl = document.getElementById('admin-users');

// adminUserSelect: dropdown انتخاب کاربر برای تغییر سکه
const adminUserSelect = document.getElementById('admin-user-select');

// adminCoinAmount: input مقدار سکه برای اضافه/کسر
const adminCoinAmount = document.getElementById('admin-coin-amount');

// adminCoinForm: فرم مدیریت سکه در پنل ادمین
const adminCoinForm = document.getElementById('admin-coin-form');

// adminFeedback: نمایش پیام خطا/موفقیت در پنل ادمین
const adminFeedback = document.getElementById('admin-feedback');

// adminLogsEl: لیست لاگ‌های اخیر (برای audit trail)
const adminLogsEl = document.getElementById('admin-logs');

// ─────────────────────────────────────────────────────────────────────────
// گروه ۴: پنجره راهنما
// ─────────────────────────────────────────────────────────────────────────

// helpBtn: دکمه "راهنما" در نوار بالا
const helpBtn = document.getElementById('help-btn');

// helpOverlay: modal راهنما
const helpOverlay = document.getElementById('help-overlay');

// helpClose: دکمه بستن modal راهنما
const helpClose = document.getElementById('help-close');

// helpTabs: دکمه‌های تب راهنما (مثلاً: بازی‌ها، قوانین، دستورات کلیدی)
const helpTabs = document.querySelectorAll('.help-tab');

// helpPanes: محتوای هر تب راهنما
const helpPanes = document.querySelectorAll('.help-pane');

// ─────────────────────────────────────────────────────────────────────────
// متغیرهای وضعیت (State Variables)
// ─────────────────────────────────────────────────────────────────────────

/*
 * این متغیرها وضعیت فعلی برنامه را نگه می‌دارند.
 * من از این‌ها برای tracking کردن چه چیزی فعال است استفاده می‌کنم.
 */

// activeAuthTab: تب فعال در modal احراز هویت ('login' یا 'register')
// مقدار پیش‌فرض 'login' است چون اکثر کاربران می‌خواهند وارد شوند نه ثبت‌نام کنند
let activeAuthTab = 'login';

// currentGameUrl: URL بازی‌ای که در حال حاضر در iframe لود شده است
// از این برای جلوگیری از reload مکرر یک بازی استفاده می‌کنم
let currentGameUrl = null;

// isAuthModalOpen: آیا modal احراز هویت باز است؟
// برای کنترل event handler ها (مثلاً ESC برای بستن) استفاده می‌شود
let isAuthModalOpen = false;

// ═══════════════════════════════════════════════════════════════════════════
//                    بخش ۳: ثابت‌های طراحی و محاسبات
// ═══════════════════════════════════════════════════════════════════════════

/*
 * محاسبات Carousel - فیزیک منوی بازی
 *
 * این اعداد را از CSS گرفتم و اینجا نگه داشتم تا JavaScript بتواند
 * carousel را درست حرکت دهد. اگر در CSS تغییر کنم، اینجا هم باید تغییر کنم.
 *
 * چرا این اعداد:
 * - ITEM_WIDTH = 120: هر بازی 120px عرض دارد (آیکون 80px + padding)
 * - GAP = 50: 50px فاصله بین بازی‌ها برای تنفس بصری
 * - TOTAL_UNIT = 170: واحد کامل یک بازی (120 + 50) برای محاسبه offset
 *
 * نحوه استفاده:
 * - offset = -selectedIndex * TOTAL_UNIT
 * - مثلاً اگر selectedIndex=2 باشد: offset = -2 * 170 = -340px
 * - این یعنی carousel باید 340px به چپ برود تا بازی سوم در مرکز باشد
 */
const ITEM_WIDTH = 120;  // عرض هر بازی به پیکسل
const GAP = 50;          // فاصله بین بازی‌ها به پیکسل
const TOTAL_UNIT = ITEM_WIDTH + GAP;  // واحد کامل = 170px

/*
 * قیمت بازی‌ها - اقتصاد Arcade System
 *
 * این object قیمت هر بازی را بر حسب سکه مشخص می‌کند.
 * قیمت‌ها را بر اساس پیچیدگی و مدت زمان بازی تعیین کردم:
 *
 * - guess_number.cgi: 3 سکه (ساده‌ترین، بازی حدس عدد)
 * - tictactoe_limited.cgi: 4 سکه (دوز محدود - سریع و سرگرم‌کننده)
 * - rps_5move.cgi: 5 سکه (سنگ کاغذ قیچی 5 دوره‌ای)
 * - hangman_battle.cgi: 6 سکه (چوبه دار - متوسط)
 * - fibonacci_2048.cgi: 8 سکه (پازل فیبوناچی - نسبتاً سخت)
 * - checkers.cgi: 10 سکه (چکرز کامل - بازی متوسط)
 * - chess_modified.cgi: 15 سکه (شطرنج تعدیل شده - گران‌ترین و پیچیده‌ترین)
 *
 * نحوه استفاده:
 * - getGamePrice(url) این object را می‌خواند و قیمت برمی‌گرداند
 * - اگر بازی جدیدی اضافه کنم، باید در اینجا قیمتش را تعریف کنم
 * - قیمت‌ها باید با سرور هماهنگ باشد (هر چند سرور هم check می‌کند)
 *
 * یادآوری برای خودم:
 * - اگر قیمت‌ها را تغییر دادم، باید در دو جا بروزرسانی کنم:
 *   1. اینجا (script.js)
 *   2. در مستندات یا UI (اگر جایی hard-code شده باشد)
 */
const GAME_PRICES = {
    'chess_modified.cgi': 15,      // شطرنج - پیچیده‌ترین
    'rps_5move.cgi': 5,            // سنگ کاغذ قیچی
    'tictactoe_limited.cgi': 4,   // دوز محدود
    'hangman_battle.cgi': 6,       // چوبه دار
    'fibonacci_2048.cgi': 8,       // پازل فیبوناچی
    'checkers.cgi': 10,            // چکرز
    'guess_number.cgi': 3          // حدس عدد - ساده‌ترین
};

// ═══════════════════════════════════════════════════════════════════════════
//                      بخش ۴: توابع کمکی (Helper Functions)
// ═══════════════════════════════════════════════════════════════════════════

/*
 * تابع toFaDigits - تبدیل اعداد انگلیسی به فارسی
 *
 * چرا این تابع را نوشتم:
 * - UI من کاملاً فارسی است و نمایش اعداد انگلیسی خیلی بد به نظر می‌رسد
 * - می‌خواهم تمام اعداد (سکه‌ها، قیمت‌ها، شماره‌ها) را فارسی نمایش دهم
 * - نمی‌خواهم هر جا از عدد استفاده می‌کنم این کار را تکرار کنم
 *
 * نحوه کار:
 * - یک map از ارقام فارسی می‌سازم (۰۱۲۳۴۵۶۷۸۹)
 * - value را به رشته تبدیل می‌کنم (String(value))
 * - با regex تمام ارقام انگلیسی (0-9) را پیدا می‌کنم
 * - هر رقم را با معادل فارسی‌اش جایگزین می‌کنم
 *
 * مثال‌ها:
 * - toFaDigits(123) → "۱۲۳"
 * - toFaDigits(0) → "۰"
 * - toFaDigits(2024) → "۲۰۲۴"
 *
 * استفاده:
 * - در formatCoinText برای نمایش سکه‌ها
 * - در هر جایی که عددی را در UI نمایش می‌دهم
 *
 * @param {number|string} value - عددی که باید فارسی شود
 * @returns {string} - رشته با ارقام فارسی
 */
function toFaDigits(value) {
    // آرایه‌ای از ارقام فارسی به ترتیب ۰ تا ۹
    const map = ['۰','۱','۲','۳','۴','۵','۶','۷','۸','۹'];

    // value را به string تبدیل کن و هر رقم انگلیسی را با فارسی جایگزین کن
    // d = رقم پیدا شده، map[d] = معادل فارسی آن
    // اگر d یک رقم نباشد (مثلاً نقطه یا منفی)، خودش را برگردان
    return String(value).replace(/\d/g, d => map[d] || d);
}

/*
 * تابع formatCoinText - فرمت نمایش سکه‌ها
 *
 * چرا این تابع جداگانه:
 * - می‌خواهم فرمت نمایش سکه در تمام UI یکسان باشد
 * - اگر بخواهم فرمت را تغییر دهم، فقط در یک جا تغییر می‌دهم
 * - خواندن کد راحت‌تر می‌شود: formatCoinText(50) به جای `${toFaDigits(50)} سکه`
 *
 * فرمت:
 * - "عدد_فارسی سکه"
 * - مثال: "۵۰ سکه"
 *
 * استفاده:
 * - در updateCoinUI برای نمایش سکه در نوار بالا
 * - در updateAccountPanel برای نمایش سکه در پنل حساب
 * - در adminPanel برای نمایش سکه کاربران
 *
 * یادآوری برای خودم:
 * - اگر بخواهم آیکون سکه اضافه کنم، اینجا تغییر می‌دهم
 * - اگر بخواهم جداکننده هزارگان اضافه کنم، اینجا می‌نویسم
 *
 * @param {number} amount - تعداد سکه‌ها
 * @returns {string} - متن فرمت شده برای نمایش
 */
function formatCoinText(amount) {
    // عدد را فارسی کن و " سکه" به انتهایش اضافه کن
    return `${toFaDigits(amount)} سکه`;
}

/*
 * تابع formatRole - فرمت نمایش نقش کاربر
 *
 * چرا این تابع:
 * - سرور role را به انگلیسی برمی‌گرداند ('admin' یا 'user')
 * - می‌خواهم در UI فارسی نمایش دهم
 * - اگر بعداً role های دیگری اضافه کنم، فقط اینجا تغییر می‌دهم
 *
 * نقش‌ها:
 * - 'admin' → 'ادمین'
 * - 'user' → 'کاربر'
 * - هر چیز دیگری → 'کاربر' (پیش‌فرض امن)
 *
 * استفاده:
 * - در updateAccountPanel برای نمایش نقش در پنل حساب
 * - در adminPanel برای نمایش نقش کاربران
 *
 * @param {string} role - نقش کاربر ('admin' یا 'user')
 * @returns {string} - نقش به فارسی
 */
function formatRole(role) {
    // اگر admin باشد، "ادمین" برگردان، وگرنه "کاربر"
    return role === 'admin' ? 'ادمین' : 'کاربر';
}

/*
 * تابع getGamePrice - دریافت قیمت یک بازی
 *
 * چرا این تابع:
 * - نمی‌خواهم مستقیم GAME_PRICES object را از همه جا بخوانم
 * - اگر بازی در لیست نباشد، یک مقدار پیش‌فرض امن (0) برمی‌گرداند
 * - اگر بعداً منطق قیمت‌گذاری پیچیده‌تر شود (تخفیف، قیمت پویا)، فقط اینجا تغییر می‌کنم
 *
 * نحوه کار:
 * - GAME_PRICES[url] را می‌خواند
 * - اگر undefined بود (بازی وجود ندارد)، 0 برمی‌گرداند
 *
 * استفاده:
 * - قبل از load کردن بازی برای چک کردن قیمت
 * - در UI برای نمایش قیمت بازی‌ها
 *
 * @param {string} url - URL بازی (مثلاً 'chess_modified.cgi')
 * @returns {number} - قیمت بازی به سکه (یا 0 اگر بازی وجود نداشته باشد)
 */
function getGamePrice(url) {
    // قیمت را از GAME_PRICES بخوان، اگر undefined بود 0 برگردان
    return GAME_PRICES[url] || 0;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    بخش ۵: توابع مدیریت UI احراز هویت
// ═══════════════════════════════════════════════════════════════════════════

/*
 * تابع setAuthFeedback - نمایش پیام در modal احراز هویت
 *
 * این تابع را برای نمایش پیام‌های خطا یا موفقیت در modal ورود/ثبت‌نام نوشتم.
 *
 * چرا این تابع:
 * - می‌خواهم تجربه کاربری یکدست داشته باشم در تمام حالات
 * - رنگ پیام بسته به نوع (خطا/موفقیت) متفاوت است
 * - از چند جای مختلف (login، register، خطاهای شبکه) فراخوانی می‌شود
 *
 * رنگ‌بندی:
 * - خطا (isError=true): #fbbf24 (زرد/نارنجی هشداردهنده)
 * - موفقیت (isError=false): #10b981 (سبز نئونی تم ما)
 *
 * نحوه کار:
 * 1. چک می‌کند authFeedback المان وجود داشته باشد (برای جلوگیری از null error)
 * 2. متن پیام را set می‌کند (یا خالی اگر message نداشته باشیم)
 * 3. رنگ را بر اساس isError تنظیم می‌کند
 *
 * استفاده:
 * - setAuthFeedback('نام کاربری یا رمز عبور اشتباه است', true)
 * - setAuthFeedback('ثبت‌نام موفق بود!', false)
 * - setAuthFeedback('') // برای پاک کردن پیام
 *
 * @param {string} message - متن پیام (می‌تواند خالی باشد)
 * @param {boolean} isError - آیا این یک پیام خطا است؟ (پیش‌فرض: true)
 */
function setAuthFeedback(message, isError = true) {
    // اگر المان وجود نداشت، هیچ کاری نکن (defensive programming)
    if (!authFeedback) return;

    // متن پیام را set کن (اگر message نداشتیم، خالی بگذار)
    authFeedback.textContent = message || '';

    // رنگ را بر اساس نوع پیام تنظیم کن
    // خطا = زرد/نارنجی، موفقیت = سبز
    authFeedback.style.color = isError ? '#fbbf24' : '#10b981';
}

/*
 * تابع setAuthTab - تغییر تب فعال در modal احراز هویت
 *
 * این تابع بین تب "ورود" و "ثبت‌نام" سوئیچ می‌کند.
 *
 * چرا این تابع:
 * - منطق تغییر تب در یک جا متمرکز است
 * - باید همزمان چند کار انجام شود: active class، نمایش فرم مناسب
 * - از event handler و هم از کد دیگر قابل فراخوانی است
 *
 * نحوه کار:
 * 1. متغیر سراسری activeAuthTab را به‌روزرسانی می‌کند
 * 2. روی تمام دکمه‌های تب حلقه می‌زند و active class را مدیریت می‌کند
 * 3. فرم مناسب (login یا register) را نمایش می‌دهد و دیگری را مخفی می‌کند
 *
 * تب‌ها:
 * - 'login': فرم ورود
 * - 'register': فرم ثبت‌نام
 *
 * CSS Classes:
 * - .active روی دکمه تب: رنگ سبز و bold
 * - .active روی فرم: display block (فرم‌ها به صورت پیش‌فرض display none هستند)
 *
 * @param {string} tabName - نام تب ('login' یا 'register')
 */
function setAuthTab(tabName) {
    // متغیر سراسری را به‌روزرسانی کن تا بدانیم کدام تب فعال است
    activeAuthTab = tabName;

    // روی تمام دکمه‌های تب حلقه بزن
    tabButtons.forEach(btn => {
        // آیا این دکمه باید active باشد؟
        const active = btn.dataset.tab === tabName;
        // class active را toggle کن (اگر active=true باشد اضافه کن، وگرنه حذف کن)
        btn.classList.toggle('active', active);
    });

    // فرم مناسب را نمایش بده
    // اگر tabName='login' باشد، loginForm را active کن
    loginForm.classList.toggle('active', tabName === 'login');
    // اگر tabName='register' باشد، registerForm را active کن
    registerForm.classList.toggle('active', tabName === 'register');
}

/*
 * تابع openAuth - باز کردن modal احراز هویت
 *
 * این تابع modal ورود/ثبت‌نام را نمایش می‌دهد.
 *
 * چرا این تابع:
 * - وقتی کاربر می‌خواهد بازی کند ولی login نیست، این modal را نشان می‌دهم
 * - وقتی روی دکمه "ورود / ثبت‌نام" کلیک می‌شود
 * - می‌توانم با پیام سفارشی modal را باز کنم (مثلاً "برای بازی باید وارد شوید")
 *
 * نحوه کار:
 * 1. isAuthModalOpen را true می‌کند (برای tracking وضعیت)
 * 2. class hidden را از authOverlay حذف می‌کند → modal ظاهر می‌شود
 * 3. اگر message داده شده بود، آن را در قسمت feedback نمایش می‌دهد
 *
 * استفاده:
 * - openAuth() // باز کردن ساده بدون پیام
 * - openAuth('برای بازی شطرنج باید ۱۵ سکه داشته باشید') // با پیام سفارشی
 *
 * @param {string} message - پیامی که باید نمایش داده شود (اختیاری)
 */
function openAuth(message) {
    // وضعیت modal را به باز تغییر بده
    isAuthModalOpen = true;

    // class hidden را حذف کن تا modal با انیمیشن ظاهر شود
    authOverlay.classList.remove('hidden');

    // اگر پیامی داده شده بود، آن را نمایش بده
    if (message) setAuthFeedback(message, true);
}

/*
 * تابع closeAuth - بستن modal احراز هویت
 *
 * این تابع modal را می‌بندد و همه چیز را ریست می‌کند.
 *
 * چرا این تابع:
 * - وقتی کاربر روی × یا بیرون modal کلیک می‌کند
 * - بعد از login/register موفق
 * - وقتی ESC فشرده می‌شود
 * - می‌خواهم اطمینان حاصل کنم فرم‌ها و پیام‌ها پاک شده‌اند
 *
 * نحوه کار:
 * 1. isAuthModalOpen را false می‌کند
 * 2. class hidden را به authOverlay اضافه می‌کند → modal مخفی می‌شود
 * 3. پیام feedback را پاک می‌کند
 * 4. هر دو فرم (login و register) را reset می‌کند
 *
 * چرا reset فرم‌ها:
 * - وقتی modal بسته می‌شود، نمی‌خواهم اطلاعات قبلی در فیلدها بماند
 * - مخصوصاً password را نمی‌خواهم در حافظه نگه دارم
 * - دفعه بعد که modal باز شود، فرم‌ها خالی هستند
 */
function closeAuth() {
    // وضعیت modal را به بسته تغییر بده
    isAuthModalOpen = false;

    // class hidden را اضافه کن تا modal مخفی شود
    authOverlay.classList.add('hidden');

    // پیام feedback را پاک کن
    setAuthFeedback('');

    // هر دو فرم را ریست کن (فیلدها را خالی کن)
    loginForm.reset();
    registerForm.reset();
}

// ═══════════════════════════════════════════════════════════════════════════
//              بخش ۶: توابع به‌روزرسانی UI بر اساس session
// ═══════════════════════════════════════════════════════════════════════════

/*
 * تابع updateAuthUI - همگام‌سازی کامل UI با وضعیت احراز هویت
 *
 * این تابع مهم‌ترین تابع UI است. هر زمان که session تغییر می‌کند (login، logout،
 * refresh)، این تابع تمام المان‌های وابسته به احراز هویت را به‌روزرسانی می‌کند.
 *
 * چرا این تابع:
 * - وقتی کاربر login می‌کند، باید تمام UI تغییر کند
 * - وقتی کاربر logout می‌کند، باید به حالت مهمان برگردد
 * - وقتی صفحه reload می‌شود، باید session را از سرور بگیرم
 * - می‌خواهم یک single source of truth برای وضعیت UI داشته باشم
 *
 * نحوه کار:
 * 1. از Auth.refreshSession() session فعلی را می‌گیرد
 * 2. اگر session وجود داشت (کاربر login است):
 *    - نام کاربر را در نوار بالا نمایش می‌دهد
 *    - دکمه خروج را نمایش می‌دهد
 *    - متن دکمه‌ها را به "حساب" تغییر می‌دهد
 *    - pill را به حالت "online" می‌برد (دایره سبز)
 *    - سکه‌ها را به‌روزرسانی می‌کند
 *    - پنل حساب را به‌روزرسانی می‌کند
 *    - اگر admin است، دکمه پنل ادمین را نمایش می‌دهد
 * 3. اگر session نبود (کاربر login نیست):
 *    - "مهمان" را نمایش می‌دهد
 *    - دکمه خروج را مخفی می‌کند
 *    - متن دکمه‌ها را به "ورود / ثبت‌نام" تغییر می‌دهد
 *    - pill را به حالت guest می‌برد
 *    - سکه را صفر نمایش می‌دهد
 *    - دکمه پنل ادمین را مخفی می‌کند
 *    - پنل حساب را مخفی می‌کند
 *
 * استفاده:
 * - در window.onload برای مقداردهی اولیه
 * - بعد از login موفق
 * - بعد از logout
 * - بعد از register موفق (اگر خودکار login کند)
 *
 * وابستگی‌ها:
 * - Auth.refreshSession() از auth.js
 * - updateCoinUI() برای به‌روزرسانی سکه‌ها
 * - updateAccountPanel() برای به‌روزرسانی پنل
 *
 * @returns {Promise<void>} - async چون با سرور ارتباط می‌گیرد
 */
async function updateAuthUI() {
    // از سرور session فعلی را بگیر (شامل username، role، coins)
    const session = await Auth.refreshSession();

    if (session) {
        // ─────────────────────────────────────────────────────────────────
        // حالت Login: کاربر وارد شده است
        // ─────────────────────────────────────────────────────────────────

        // نام کاربر را در نوار بالا نمایش بده
        userDisplay.textContent = session.username;

        // دکمه خروج را نمایش بده
        logoutBtn.classList.remove('hidden');

        // متن دکمه ورود را به "حساب" تغییر بده
        openAuthBtn.textContent = 'حساب';
        accountBtn.textContent = 'حساب کاربری';

        // pill را به حالت online ببر (دایره سبز کنار نام)
        userPill.classList.add('online');

        // سکه‌ها را از سرور بگیر و نمایش بده
        await updateCoinUI();

        // پنل حساب کاربری را با اطلاعات جدید به‌روزرسانی کن
        updateAccountPanel();

        // اگر این کاربر admin است، دکمه پنل ادمین را نمایش بده
        if (adminBtn) adminBtn.classList.toggle('hidden', session.role !== 'admin');
        if (accountAdminBtn) accountAdminBtn.classList.toggle('hidden', session.role !== 'admin');

    } else {
        // ─────────────────────────────────────────────────────────────────
        // حالت Guest: کاربر login نیست
        // ─────────────────────────────────────────────────────────────────

        // به جای نام کاربر، "مهمان" نمایش بده
        userDisplay.textContent = 'مهمان';

        // دکمه خروج را مخفی کن (مهمان نمی‌تواند logout کند!)
        logoutBtn.classList.add('hidden');

        // متن دکمه‌ها را به "ورود / ثبت‌نام" برگردان
        openAuthBtn.textContent = 'ورود / ثبت‌نام';
        accountBtn.textContent = 'ورود / ثبت‌نام';

        // pill را از حالت online خارج کن
        userPill.classList.remove('online');

        // سکه را صفر نمایش بده (مهمان سکه ندارد)
        if (coinAmountEl) coinAmountEl.textContent = '۰';

        // پنل حساب را مخفی کن (با پارامتر true)
        updateAccountPanel(true);

        // دکمه پنل ادمین را مخفی کن
        if (adminBtn) adminBtn.classList.add('hidden');
        if (accountAdminBtn) accountAdminBtn.classList.add('hidden');
    }
}

/*
 * تابع updateCoinUI - به‌روزرسانی نمایش سکه در نوار بالا
 *
 * این تابع موجودی سکه را از سرور می‌گیرد و در نوار بالای صفحه نمایش می‌دهد.
 *
 * چرا جداگانه:
 * - می‌خواهم موجودی سکه را بدون reload کل UI به‌روزرسانی کنم
 * - بعد از هر تراکنش (خرید بازی، دریافت پاداش) این تابع صدا زده می‌شود
 * - سبک‌تر از updateAuthUI است چون فقط یک المان را تغییر می‌دهد
 *
 * نحوه کار:
 * 1. چک می‌کند coinAmountEl وجود داشته باشد
 * 2. چک می‌کند کاربر login هست یا نه
 * 3. اگر login بود، موجودی از سرور می‌گیرد
 * 4. اگر login نبود، 0 نمایش می‌دهد
 * 5. عدد را فارسی می‌کند و در المان قرار می‌دهد
 *
 * @returns {Promise<void>}
 */
async function updateCoinUI() {
    // اگر المان نمایش سکه وجود نداشت، هیچ کاری نکن
    if (!coinAmountEl) return;

    // موجودی را بگیر: اگر login بود از سرور بگیر، وگرنه 0
    const balance = (await Auth.isAuthenticated()) ? await Auth.getCoinBalance() : 0;

    // موجودی را فارسی کن و نمایش بده
    coinAmountEl.textContent = toFaDigits(balance);
}

/*
 * تابع updateAccountPanel - به‌روزرسانی پنل حساب کاربری
 *
 * این تابع اطلاعات پنل کشویی حساب کاربری را پر می‌کند.
 *
 * چرا این تابع:
 * - پنل حساب اطلاعات کامل‌تری از کاربر نمایش می‌دهد (نام، نقش، سکه)
 * - وقتی پنل باز می‌شود، باید اطلاعات fresh باشد
 * - اگر کاربر مهمان است، می‌توانیم پنل را مخفی کنیم
 *
 * پارامترها:
 * - hideIfGuest: اگر true باشد و کاربر مهمان باشد، پنل را مخفی کن
 *
 * نحوه کار:
 * 1. session را synchronous می‌گیرد (از cache)
 * 2. اگر session نبود (مهمان):
 *    - فیلدها را خالی می‌کند ('-' یا '۰')
 *    - اگر hideIfGuest=true باشد، پنل را مخفی می‌کند
 * 3. اگر session بود:
 *    - نام کاربر را نمایش می‌دهد
 *    - نقش را فارسی می‌کند و نمایش می‌دهد
 *    - سکه را فارسی می‌کند و نمایش می‌دهد
 *
 * @param {boolean} hideIfGuest - آیا در صورت مهمان بودن، پنل مخفی شود؟
 */
function updateAccountPanel(hideIfGuest = false) {
    // session را sync بگیر (بدون network call)
    const session = Auth.getSessionSync();

    if (!session) {
        // ─────────────────────────────────────────────────────────────────
        // حالت مهمان: اطلاعات خالی
        // ─────────────────────────────────────────────────────────────────

        // نام کاربر را '-' بگذار
        if (panelUsername) panelUsername.textContent = '-';

        // نقش را '-' بگذار
        if (panelRole) panelRole.textContent = '-';

        // سکه را '۰' بگذار
        if (panelCoins) panelCoins.textContent = '۰';

        // اگر hideIfGuest=true بود، پنل را مخفی کن
        if (hideIfGuest && accountPanel) accountPanel.classList.add('hidden');
        return;
    }

    // ─────────────────────────────────────────────────────────────────────
    // حالت Login: اطلاعات واقعی کاربر
    // ─────────────────────────────────────────────────────────────────────

    // نام کاربر را نمایش بده
    if (panelUsername) panelUsername.textContent = session.username;

    // نقش را فارسی کن و نمایش بده
    if (panelRole) panelRole.textContent = formatRole(session.role);

    // سکه را فارسی کن و نمایش بده
    if (panelCoins) panelCoins.textContent = toFaDigits(session.coins || 0);
}

// ═══════════════════════════════════════════════════════════════════════════
//                    بخش ۷: توابع مدیریت دسترسی و بازی
// ═══════════════════════════════════════════════════════════════════════════

/*
 * تابع requireAuth - بررسی احراز هویت قبل از بازی
 *
 * این تابع یک guard function است که قبل از شروع هر بازی صدا زده می‌شود.
 *
 * چرا این تابع:
 * - می‌خواهم مطمئن شوم فقط کاربران login شده می‌توانند بازی کنند
 * - نمی‌خواهم مهمان‌ها بتوانند بازی کنند و سکه خرج کنند
 * - یک نقطه مرکزی برای چک کردن دسترسی
 *
 * نحوه کار:
 * 1. چک می‌کند کاربر authenticated است یا نه
 * 2. اگر نبود، modal ورود را با پیام باز می‌کند
 * 3. چک می‌کند کاربر role مناسب دارد یا نه (user یا admin)
 * 4. اگر role نداشت، پیام خطا می‌دهد
 * 5. اگر همه چیز OK بود، true برمی‌گرداند
 *
 * @returns {Promise<boolean>} - true اگر کاربر مجاز باشد، false اگر نباشد
 */
async function requireAuth() {
    // آیا کاربر login کرده است؟
    if (!await Auth.isAuthenticated()) {
        // نه، پس modal ورود را با پیام باز کن
        openAuth('برای بازی، ابتدا وارد شوید.');
        return false;
    }

    // آیا کاربر role مناسب دارد؟ (user یا admin)
    if (!await Auth.hasRole(['user', 'admin'])) {
        // نه، پس پیام خطای دسترسی بده
        setAuthFeedback('دسترسی کافی ندارید.');
        return false;
    }

    // همه چیز OK است، اجازه بازی بده
    return true;
}

/*
 * تابع showWarning - نمایش پیام هشدار به کاربر
 *
 * این تابع برای نمایش پیام‌های مهم (مثل کمبود سکه، خطا) استفاده می‌شود.
 *
 * چرا این تابع:
 * - می‌خواهم پیام‌های خطا را به صورت بصری چشمگیر نشان دهم
 * - اگر game-display موجود باشد، پیام را در آنجا نمایش می‌دهم
 * - اگر نباشد، به عنوان fallback از alert استفاده می‌کنم
 *
 * نحوه کار:
 * 1. سعی می‌کند game-display را پیدا کند
 * 2. اگر پیدا کرد، یک <h2> با class blink-text در آن می‌گذارد
 * 3. رنگ قرمز (#f87171) برای هشدار
 * 4. اگر پیدا نکرد، alert مرورگر را نمایش می‌دهد
 *
 * استفاده:
 * - showWarning('سکه کافی نیست')
 * - showWarning('خطا در اتصال به سرور')
 *
 * @param {string} message - متن پیام هشدار
 */
function showWarning(message) {
    // game-display را پیدا کن (محل نمایش بازی)
    const display = document.getElementById('game-display');

    if (display) {
        // اگر display موجود بود، پیام را در آنجا نمایش بده
        // با class blink-text چشمک می‌زند، رنگ قرمز برای هشدار
        display.innerHTML = `<h2 class="blink-text" style="color:#f87171; margin-top:0;">${message}</h2>`;
    } else {
        // اگر display نبود، از alert مرورگر استفاده کن (fallback)
        alert(message);
    }
}

/*
 * تابع ensureCoinsForGame - بررسی و کسر سکه قبل از بازی
 *
 * این تابع قلب سیستم اقتصادی Arcade است. قبل از شروع هر بازی،
 * موجودی را چک می‌کند و سکه را کسر می‌کند.
 *
 * چرا این تابع:
 * - هر بازی قیمت دارد و باید قبل از شروع پرداخت شود
 * - می‌خواهم از کمبود سکه جلوگیری کنم (UX بهتر)
 * - سیستم coin-operated arcade واقعی را شبیه‌سازی می‌کنم
 *
 * نحوه کار:
 * 1. قیمت بازی را از GAME_PRICES می‌گیرد
 * 2. اگر قیمت 0 یا منفی بود، بازی رایگان است → true برمی‌گرداند
 * 3. موجودی فعلی کاربر را از سرور می‌گیرد
 * 4. اگر موجودی کمتر از قیمت بود، پیام هشدار می‌دهد → false
 * 5. اگر موجودی کافی بود، سکه را کسر می‌کند
 * 6. UI را به‌روزرسانی می‌کند تا موجودی جدید نمایش داده شود
 * 7. اگر خطایی رخ داد (مثلاً خطای شبکه)، پیام می‌دهد → false
 *
 * @param {string} url - URL بازی (برای گرفتن قیمت)
 * @returns {Promise<boolean>} - true اگر سکه کسر شد، false اگر کافی نبود یا خطا رخ داد
 */
async function ensureCoinsForGame(url) {
    // قیمت بازی را بگیر
    const price = getGamePrice(url);

    // اگر قیمت 0 یا منفی بود، بازی رایگان است
    if (price <= 0) return true;

    // موجودی فعلی کاربر را بگیر
    const balance = await Auth.getCoinBalance();

    // آیا موجودی کافی است؟
    if (balance < price) {
        // نه، پس پیام هشدار بده و false برگردان
        showWarning(`سکه کافی نیست. قیمت بازی: ${formatCoinText(price)} | موجودی: ${formatCoinText(balance)}`);
        return false;
    }

    // موجودی کافی است، حالا سکه را کسر کن
    try {
        // سکه را از حساب کاربر کسر کن
        await Auth.subtractCoin(price);

        // UI را به‌روزرسانی کن تا موجودی جدید نمایش داده شود
        await updateAuthUI();

        // موفق بود، true برگردان
        return true;
    } catch (err) {
        // خطا رخ داد (مثلاً خطای شبکه یا سرور)
        showWarning(err.message || 'خطا در کسر سکه');
        return false;
    }
}

/*
 * تابع rewardForGameWin - پاداش دادن بعد از برد
 *
 * وقتی کاربر یک بازی را ببرد، این تابع پاداش (سکه) را به حسابش اضافه می‌کند.
 *
 * چرا این تابع:
 * - می‌خواهم بازیکنان را تشویق کنم تا بازی کنند
 * - سیستم اقتصادی باید چرخش داشته باشد (خرج و درآمد)
 * - پاداش معمولاً برابر یا بیشتر از قیمت بازی است
 *
 * نحوه کار:
 * 1. قیمت بازی را می‌گیرد (پاداش معمولاً برابر قیمت است)
 * 2. اگر قیمت 0 یا منفی بود، پاداشی نیست
 * 3. Auth.rewardWin() را صدا می‌زند تا سرور سکه را اضافه کند
 * 4. UI را به‌روزرسانی می‌کند تا موجودی جدید نمایش داده شود
 * 5. اگر خطا رخ داد، فقط console.warn می‌کند (نمی‌خواهم UX را خراب کنم)
 *
 * نکته:
 * - خطای پاداش را نادیده می‌گیرم چون کاربر بازی را برده و نمی‌خواهم
 *   تجربه‌اش را خراب کنم. در production باید log شود.
 *
 * @param {string} url - URL بازی (برای گرفتن مقدار پاداش)
 * @returns {Promise<void>}
 */
async function rewardForGameWin(url) {
    // قیمت/پاداش بازی را بگیر
    const price = getGamePrice(url);

    // اگر قیمت 0 یا منفی بود، پاداشی نیست
    if (price <= 0) return;

    try {
        // پاداش را به حساب کاربر اضافه کن
        await Auth.rewardWin(price);

        // UI را به‌روزرسانی کن تا موجودی جدید نمایش داده شود
        await updateAuthUI();
    } catch (err) {
        // خطا رخ داد، فقط console warn کن (UX را خراب نکن)
        // در production باید این خطا log شود
        console.warn('Reward failed:', err.message || err);
    }
}

// نقطه ورودی شروع بازی؛ اول احراز هویت و سکه را چک می‌کنم، بعد iframe را بارگذاری می‌کنم
async function startGame(url) {
    if (!await requireAuth()) return false;
    if (!await ensureCoinsForGame(url)) return false;

    const display = document.getElementById('game-display');
    display.innerHTML = '<h2 class="blink-text" style="color:#facc15; margin-top:0;">LOADING SYSTEM...</h2>';

    const iframe = document.createElement('iframe');
    iframe.title = 'Game Frame';
    iframe.style.width = '100%';
    iframe.style.height = '100%';
    iframe.style.border = 'none';
    iframe.style.background = 'transparent';
    iframe.src = `cgi-bin/${url}`;
    display.innerHTML = '';
    display.appendChild(iframe);

    currentGameUrl = url;
    return true;
}

// این هوک را در iframe صدا می‌زنم تا بعد از برد پاداش اعمال شود
function endGame(result) {
    if (result === 'win' && currentGameUrl) {
        rewardForGameWin(currentGameUrl);
    }
}

window.endGame = endGame;

// سوئیچ بین تب‌های راهنما را اینجا کنترل می‌کنم تا یک کد مرکزی داشته باشم
function switchHelpTab(tabName) {
    helpTabs.forEach(btn => btn.classList.toggle('active', btn.dataset.helpTab === tabName));
    helpPanes.forEach(pane => pane.classList.toggle('active', pane.dataset.helpPane === tabName));
}

// دکمه راهنما را که می‌زنم اینجا نمایش لایه را فعال می‌کنم
function openHelp() {
    if (helpOverlay) helpOverlay.classList.remove('hidden');
    switchHelpTab('overview');
}

// بستن راهنما را هم در همین تابع نگه داشتم
function closeHelp() {
    if (helpOverlay) helpOverlay.classList.add('hidden');
}

// این تابع برچسب قیمت بازی‌ها را در کاروسل مقداردهی می‌کند تا همیشه با جدول قیمت‌ها هم‌سو باشد
function syncGamePricesUI() {
    games.forEach(game => {
        const url = game.getAttribute('data-url');
        const price = getGamePrice(url);
        let priceTag = game.querySelector('.price-tag');
        if (!priceTag) {
            priceTag = document.createElement('div');
            priceTag.className = 'price-tag';
            priceTag.innerHTML = '<span class="coin-icon price-coin"></span><span class="price-value"></span>';
            game.appendChild(priceTag);
        }
        const valueEl = priceTag.querySelector('.price-value');
        if (valueEl) valueEl.textContent = formatCoinText(price);
    });
}

// اینجا لاگین را مدیریت می‌کنم؛ بعد از موفقیت UI را تازه می‌کنم
async function handleLogin(e) {
    e.preventDefault();
    try {
        await Auth.login(loginUsername.value, loginPassword.value);
        setAuthFeedback('با موفقیت وارد شدید.', false);
        await updateAuthUI();
        setTimeout(closeAuth, 350);
    } catch (err) {
        setAuthFeedback(err.message || 'خطا در ورود', true);
    }
}

// ثبت‌نام کاربر جدید را اینجا انجام می‌دهم و بعد از موفقیت واردش می‌کنم
async function handleRegister(e) {
    e.preventDefault();
    try {
        await Auth.register(regUsername.value, regPassword.value);
        setAuthFeedback('حساب ایجاد شد و وارد شدید.', false);
        await updateAuthUI();
    } catch (err) {
        setAuthFeedback(err.message || 'خطا در ثبت‌نام', true);
    }
}

// موقع حرکت در کاروسل این تابع مسئول هایلایت و جابجایی نوار است
function updateSelection() {
    games.forEach((g, index) => {
        if (index === selectedIndex) {
            g.classList.add('active');
        } else {
            g.classList.remove('active');
        }
    });

    const centerOffset = ITEM_WIDTH / 2;
    const translateValue = -(selectedIndex * TOTAL_UNIT) - centerOffset;

    carousel.style.transform = `translateX(${translateValue}px)`;
}

// این تابع فقط برای اطمینان از این است که CGI واقعا HTML برمی‌گرداند
async function fetchGameDocument(url) {
    const response = await fetch(`cgi-bin/${url}`, {
        headers: { Accept: 'text/html,application/xhtml+xml' },
    });
    const contentType = response.headers.get('content-type') || '';
    const bodyText = await response.text();
    const looksHtml = contentType.includes('text/html') || /^<!?html/i.test(bodyText.trim());

    if (!response.ok || !looksHtml) {
        throw new Error('پاسخ CGI به صورت HTML دریافت نشد. سرور باید CGI را اجرا کند.');
    }

    return bodyText;
}

// این تابع وقتی آیتمی انتخاب شد بازی را استارت می‌کند
async function selectGame(e) {
    if (e && typeof e.preventDefault === 'function') {
        e.preventDefault();
    }

    const activeGame = games[selectedIndex];
    const url = activeGame.getAttribute('data-url');

    if (url) {
        await startGame(url);
    } else {
        activeGame.style.animation = "shake 0.5s";
        setTimeout(() => activeGame.style.animation = "", 500);
    }
}

games.forEach((game, index) => {
    game.addEventListener('click', (event) => {
        event.preventDefault();
        selectedIndex = index;
        updateSelection();
        selectGame(event);
    });
});

tabButtons.forEach(btn => {
    btn.addEventListener('click', () => setAuthTab(btn.dataset.tab));
});

async function handleAccountAction() {
    if (!await Auth.isAuthenticated()) {
        accountPanel.classList.add('hidden');
        openAuth();
        return;
    }
    await Auth.refreshSession();
    updateAccountPanel();
    authOverlay.classList.add('hidden');
    accountPanel.classList.toggle('hidden');
}

function handleLogout() {
    Auth.logout();
    accountPanel.classList.add('hidden');
    closeAuth();
    updateAuthUI();
}

openAuthBtn.addEventListener('click', handleAccountAction);
closeAuthBtn.addEventListener('click', closeAuth);
logoutBtn.addEventListener('click', handleLogout);
if (accountLogoutBtn) accountLogoutBtn.addEventListener('click', handleLogout);
loginForm.addEventListener('submit', handleLogin);
registerForm.addEventListener('submit', handleRegister);

if (authOverlay) {
    authOverlay.addEventListener('click', (e) => {
        if (e.target === authOverlay) {
            closeAuth();
        }
    });
}

accountBtn.addEventListener('click', handleAccountAction);

if (helpBtn) helpBtn.addEventListener('click', openHelp);
if (helpClose) helpClose.addEventListener('click', closeHelp);
if (helpOverlay) {
    helpOverlay.addEventListener('click', (e) => {
        if (e.target === helpOverlay) {
            closeHelp();
        }
    });
}

helpTabs.forEach(btn => {
    btn.addEventListener('click', () => switchHelpTab(btn.dataset.helpTab));
});

function renderAdminUsers() {
    if (!adminUsersEl || !adminUserSelect) return;
    Auth.listUsers().then(users => {
        adminUsersEl.innerHTML = '<table><thead><tr><th>کاربر</th><th>نقش</th><th>سکه</th><th>تغییر نقش</th></tr></thead><tbody></tbody></table>';
        const tbody = adminUsersEl.querySelector('tbody');
        users.forEach(u => {
            const tr = document.createElement('tr');
            const roleSelect = document.createElement('select');
            roleSelect.innerHTML = '<option value="user">کاربر</option><option value="admin">ادمین</option>';
            roleSelect.value = u.role === 'admin' ? 'admin' : 'user';
            roleSelect.addEventListener('change', async (ev) => {
                const newRole = ev.target.value;
                if (newRole === u.role) return;
                try {
                    await Auth.adminChangeRole(u.username, newRole);
                    renderAdminUsers();
                    if (adminFeedback) adminFeedback.textContent = 'نقش به‌روزرسانی شد.';
                    await updateAuthUI();
                } catch (err) {
                    ev.target.value = u.role;
                    if (adminFeedback) adminFeedback.textContent = err.message || 'خطا در تغییر نقش';
                }
            });

            const actionTd = document.createElement('td');
            actionTd.className = 'actions';
            actionTd.appendChild(roleSelect);

            tr.innerHTML = `<td>${u.username}</td><td>${formatRole(u.role)}</td><td>${toFaDigits(u.coins)}</td>`;
            tr.appendChild(actionTd);
            tbody.appendChild(tr);
        });

        adminUserSelect.innerHTML = '';
        users.forEach(u => {
            const opt = document.createElement('option');
            opt.value = u.username;
            opt.textContent = `${u.username} (${formatRole(u.role)})`;
            adminUserSelect.appendChild(opt);
        });
    }).catch(err => {
        if (adminFeedback) adminFeedback.textContent = err.message || 'خطا در بارگذاری کاربران';
    });
}

function renderAdminLogs() {
    if (!adminLogsEl) return;
    Auth.getAdminLogs().then(logs => {
        adminLogsEl.innerHTML = '';
        logs.forEach(log => {
            const div = document.createElement('div');
            const date = new Date(log.ts || 0).toLocaleString();
            div.className = 'admin-log-item';
            div.innerHTML = `<span>${log.admin} ➜ ${log.target} (+${toFaDigits(log.delta)})</span><span class="meta">${date}</span>`;
            adminLogsEl.appendChild(div);
        });
    }).catch(() => {
        adminLogsEl.innerHTML = '';
    });
}

function openAdminPanel() {
    Auth.hasRole(['admin']).then(isAdmin => {
        if (!isAdmin) {
            setAuthFeedback('دسترسی ادمین ندارید.');
            return;
        }

        // 🔒 بستن کامل پنل اکانت
        if (accountPanel) {
            accountPanel.classList.add('hidden');
        }

        renderAdminUsers();
        renderAdminLogs();

        if (adminFeedback) adminFeedback.textContent = '';

        adminOverlay.classList.remove('hidden');
    });
}

function closeAdminPanel() {
    adminOverlay.classList.add('hidden');

    // ❌ پنل اکانت را خودکار باز نکن
    // فقط اجازه بده کاربر دوباره دکمه حساب را بزند
}



if (adminBtn) adminBtn.addEventListener('click', openAdminPanel);
if (accountAdminBtn) accountAdminBtn.addEventListener('click', openAdminPanel);
if (adminClose) adminClose.addEventListener('click', closeAdminPanel);
if (adminCoinForm) adminCoinForm.addEventListener('submit', (e) => {
    e.preventDefault();
    Auth.hasRole(['admin']).then(isAdmin => {
        if (!isAdmin) { if (adminFeedback) adminFeedback.textContent = 'دسترسی ادمین ندارید.'; return; }
        const user = adminUserSelect.value;
        const amount = Number(adminCoinAmount.value);
        if (!user) { adminFeedback.textContent = 'کاربر را انتخاب کنید.'; return; }
        if (!Number.isFinite(amount) || amount <= 0) { adminFeedback.textContent = 'مقدار معتبر وارد کنید.'; return; }
        Auth.adminAddCoins(user, amount)
            .then(() => {
                adminFeedback.textContent = 'سکه افزوده شد.';
                renderAdminUsers();
                renderAdminLogs();
                updateAuthUI();
            })
            .catch(err => { adminFeedback.textContent = err.message || 'خطا در افزودن سکه.'; });
    });
});

document.addEventListener('keydown', (e) => {
    const authOpen = !authOverlay.classList.contains('hidden');
    const helpOpen = helpOverlay && !helpOverlay.classList.contains('hidden');
    const typingInAuth = authOpen && authOverlay.contains(document.activeElement);

    // جلوگیری از تداخل کلیدهای بازی با زمان تایپ داخل فرم احراز هویت
    if (typingInAuth) {
        if (e.key === 'Escape') {
            e.preventDefault();
            closeAuth();
        }
        return;
    }

    if(["ArrowRight", "ArrowLeft", "ArrowUp", "ArrowDown"].indexOf(e.code) > -1) {
        e.preventDefault();
    }

    if ((authOpen || helpOpen) && e.key === 'Escape') {
        e.preventDefault();
        if (helpOpen) closeHelp();
        if (authOpen) closeAuth();
        return;
    }

    if (e.key === 'ArrowRight') {
        // راست = آیتم بعدی (چون چیدمان LTR است)
        selectedIndex = (selectedIndex + 1) % games.length;
        updateSelection();
    } else if (e.key === 'ArrowLeft') {
        // چپ = آیتم قبلی
        selectedIndex = (selectedIndex - 1 + games.length) % games.length;
        updateSelection();
    }
    if (e.key === 'Enter' || e.key === 'a' || e.key === 'A') {
        selectGame(); // کلید A یا اینتر برای شروع
    } else if (e.key === 'b' || e.key === 'B' || e.key === 'Escape') {
        location.reload(); // کلید B یا Escape برای خروج و بازگشت به منو
    }
});

window.onload = async () => {
    await updateAuthUI();
    syncGamePricesUI();

    updateSelection();
    if (typeof updateStats === 'function') {
        updateStats();
        playTimeInterval = setInterval(updateStats, 1000);
    }
};
