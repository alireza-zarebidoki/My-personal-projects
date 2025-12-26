/*
 * ═══════════════════════════════════════════════════════════════════════════
 *                         ماژول احراز هویت Arcade System
 *                                  auth.js
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * این ماژول را من از صفر نوشتم تا تمام عملیات احراز هویت سمت کلاینت را
 * مدیریت کند. این یک IIFE (Immediately Invoked Function Expression) است
 * که interface عمومی Auth را export می‌کند.
 *
 * چرا IIFE:
 * - می‌خواهم متغیرهای داخلی (مثل cachedSession) private باشند
 * - فقط توابع مورد نیاز را expose می‌کنم
 * - از global namespace pollution جلوگیری می‌کنم
 *
 * مسئولیت‌های این ماژول:
 * - ثبت‌نام کاربران جدید (register)
 * - ورود و خروج (login/logout)
 * - مدیریت session و token
 * - مدیریت سکه‌ها (getCoinBalance، subtractCoin، rewardWin)
 * - توابع ادمین (لیست کاربران، تغییر نقش، اضافه کردن سکه)
 * - مدیریت لاگ‌های ادمین
 *
 * ارتباط با سرور:
 * - تمام درخواست‌ها به /cgi-bin/auth_api.cgi می‌روند
 * - از JSON برای انتقال داده استفاده می‌کنم
 * - token در localStorage ذخیره می‌شود
 * - session در حافظه cache می‌شود برای سرعت بیشتر
 *
 * وابستگی‌ها:
 * - auth_api.cgi در سرور
 * - localStorage API مرورگر
 *
 * یادآوری برای خودم:
 * - تغییرات در session structure باید اینجا و در auth_api.cgi همگام باشند
 * - token expiration در سرور مدیریت می‌شود، نه اینجا
 * - همیشه error handling را در نظر بگیرم
 */

const Auth = (() => {
    // ═══════════════════════════════════════════════════════════════════════
    //                          ثابت‌های پیکربندی
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * SESSION_TOKEN_KEY: کلید localStorage برای ذخیره token
     *
     * چرا این نام:
     * - arcade_session_token: واضح است که چیست
     * - _v1: versioning برای آینده (اگر ساختار token تغییر کرد، v2 می‌کنم)
     *
     * یادآوری: اگر این را تغییر دهم، تمام کاربران logout می‌شوند!
     */
    const SESSION_TOKEN_KEY = 'arcade_session_token_v1';

    /*
     * API: آدرس CGI سرور برای تمام عملیات احراز هویت
     *
     * این CGI تمام action های زیر را پشتیبانی می‌کند:
     * - register: ثبت‌نام
     * - login: ورود
     * - logout: خروج
     * - session: دریافت اطلاعات session
     * - coin_balance: دریافت موجودی سکه
     * - subtract_coin: کسر سکه
     * - reward_win: پاداش برد
     * - list_users: لیست کاربران (admin only)
     * - admin_add_coins: اضافه کردن سکه (admin only)
     * - admin_change_role: تغییر نقش (admin only)
     * - admin_logs: دریافت لاگ‌ها (admin only)
     */
    const API = '/cgi-bin/auth_api.cgi';

    /*
     * DEFAULT_REWARD_RATE: نرخ پیش‌فرض پاداش برد
     *
     * این عدد (0.2 = 20%) نرخ افزایش پاداش نسبت به قیمت بازی است.
     * مثلاً اگر بازی 10 سکه قیمت داشته باشد، پاداش 12 سکه است (10 * 1.2).
     *
     * چرا 20%:
     * - می‌خواهم بازیکنان تشویق شوند ولی سیستم تورم نکند
     * - اگر همیشه ببرند، 20% سود می‌کنند
     * - اگر همیشه ببازند، سکه‌هایشان کم می‌شود
     *
     * یادآوری: سرور هم این نرخ را چک می‌کند (trust but verify)
     */
    const DEFAULT_REWARD_RATE = 0.2;

    // ═══════════════════════════════════════════════════════════════════════
    //                          متغیرهای خصوصی
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * cachedSession: cache محلی اطلاعات session
     *
     * چرا cache:
     * - نمی‌خواهم برای هر عملیات به سرور درخواست بزنم
     * - UI بارها به اطلاعات session نیاز دارد (username، coins، role)
     * - با cache کردن، سرعت بالا می‌رود و بار سرور کم می‌شود
     *
     * ساختار session:
     * {
     *   username: string,
     *   role: 'user' | 'admin',
     *   coins: number,
     *   token: string
     * }
     *
     * نکته: این cache فقط تا refresh صفحه معتبر است
     */
    let cachedSession = null;

    /*
     * textEncoder: برای encoding متن (استفاده نشده در حال حاضر)
     *
     * این را برای آینده نگه داشتم. اگر بخواهم hash کردن یا encryption
     * اضافه کنم، به این نیاز دارم.
     */
    const textEncoder = new TextEncoder();

    // ═══════════════════════════════════════════════════════════════════════
    //                          تابع اصلی ارتباط با API
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * تابع api - ارسال درخواست به سرور CGI
     *
     * این تابع core communication layer است. تمام درخواست‌ها از اینجا می‌گذرند.
     *
     * چرا این تابع:
     * - می‌خواهم یک نقطه مرکزی برای ارتباط با سرور داشته باشم
     * - error handling در یک جا متمرکز است
     * - token را به صورت خودکار به تمام درخواست‌ها اضافه می‌کند
     *
     * نحوه کار:
     * 1. action را به query string اضافه می‌کند
     * 2. payload + token را در body به صورت JSON می‌فرستد
     * 3. پاسخ را parse می‌کند
     * 4. اگر خطا داشت، exception پرتاب می‌کند
     * 5. اگر موفق بود، data را برمی‌گرداند
     *
     * پروتکل سرور:
     * - درخواست: POST /cgi-bin/auth_api.cgi?action=ACTION
     * - body: { ...payload, token: "..." }
     * - پاسخ موفق: { ok: true, ... }
     * - پاسخ خطا: { ok: false, error: "message" }
     *
     * error handling:
     * - اگر res.ok === false (HTTP error)
     * - اگر data.ok === false (application error)
     * - اگر JSON parse نشود، {} برمی‌گرداند
     *
     * @param {string} action - نام عملیات (login، register، session، etc.)
     * @param {object} payload - داده‌های ارسالی (پیش‌فرض: {})
     * @returns {Promise<object>} - پاسخ سرور
     * @throws {Error} - در صورت خطا
     */
    async function api(action, payload = {}) {
        // ارسال درخواست POST به CGI
        const res = await fetch(`${API}?action=${encodeURIComponent(action)}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ ...payload, token: getToken() })
        });

        // تلاش برای parse کردن JSON (اگر نشد {} برمی‌گرداند)
        const data = await res.json().catch(() => ({}));

        // اگر HTTP error یا application error بود، exception پرتاب کن
        if (!res.ok || data.ok === false) {
            throw new Error(data.error || 'خطا در ارتباط با سرور');
        }

        // موفق بود، data را برگردان
        return data;
    }

    // ═══════════════════════════════════════════════════════════════════════
    //                    توابع مدیریت Token و Session Cache
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * تابع getToken - دریافت token از localStorage
     *
     * این تابع token فعلی کاربر را از localStorage می‌خواند.
     *
     * چرا این تابع:
     * - token در localStorage ذخیره می‌شود تا بین refresh های صفحه باقی بماند
     * - تمام درخواست‌های API نیاز به token دارند
     * - یک نقطه مرکزی برای خواندن token
     *
     * @returns {string|null} - token اگر وجود داشته باشد، وگرنه null
     */
    function getToken() {
        return localStorage.getItem(SESSION_TOKEN_KEY);
    }

    /*
     * تابع setToken - ذخیره یا حذف token
     *
     * این تابع token را در localStorage ذخیره می‌کند یا حذف می‌کند.
     *
     * چرا این تابع:
     * - بعد از login باید token را ذخیره کنم
     * - بعد از logout باید token را حذف کنم
     * - یک نقطه مرکزی برای مدیریت token
     *
     * نحوه کار:
     * - اگر token داده شده بود، آن را ذخیره می‌کند
     * - اگر token نبود (null/undefined/empty)، کلید را حذف می‌کند
     *
     * @param {string|null} token - token برای ذخیره (یا null برای حذف)
     */
    function setToken(token) {
        if (token) localStorage.setItem(SESSION_TOKEN_KEY, token);
        else localStorage.removeItem(SESSION_TOKEN_KEY);
    }

    /*
     * تابع setCachedSession - تنظیم cache محلی session
     *
     * این تابع session را در متغیر cachedSession ذخیره می‌کند.
     *
     * چرا این تابع:
     * - می‌خواهم session را در حافظه cache کنم
     * - UI بارها به اطلاعات session نیاز دارد
     * - با cache کردن، از درخواست‌های مکرر به سرور جلوگیری می‌شود
     *
     * نحوه کار:
     * - اگر session داده شده بود، یک shallow copy می‌سازد {...session}
     * - اگر session نبود (null)، cache را null می‌کند
     *
     * نکته: shallow copy برای جلوگیری از mutate شدن session اصلی است
     *
     * @param {object|null} session - session برای cache کردن (یا null)
     */
    function setCachedSession(session) {
        cachedSession = session ? { ...session } : null;
    }

    // ═══════════════════════════════════════════════════════════════════════
    //                        توابع احراز هویت اصلی
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * تابع register - ثبت‌نام کاربر جدید
     *
     * این تابع یک کاربر جدید در سیستم ثبت می‌کند و به صورت خودکار login می‌کند.
     *
     * چرا این تابع:
     * - کاربران جدید باید حساب بسازند
     * - بعد از ثبت‌نام، خودکار login می‌شوند (UX بهتر)
     * - token و session به صورت خودکار ذخیره می‌شوند
     *
     * نحوه کار:
     * 1. درخواست register را به سرور می‌فرستد
     * 2. سرور کاربر جدید را می‌سازد و session برمی‌گرداند
     * 3. token را در localStorage ذخیره می‌کند
     * 4. session را cache می‌کند
     * 5. session را برمی‌گرداند
     *
     * خطاهای ممکن:
     * - 'نام کاربری قبلاً استفاده شده'
     * - 'نام کاربری یا رمز عبور معتبر نیست'
     * - 'خطا در ارتباط با سرور'
     *
     * @param {string} username - نام کاربری (باید unique باشد)
     * @param {string} password - رمز عبور
     * @returns {Promise<object>} - session شامل {username, role, coins, token}
     * @throws {Error} - در صورت خطا
     */
    async function register(username, password) {
        // درخواست ثبت‌نام را بفرست
        const data = await api('register', { username, password });

        // token را ذخیره کن
        setToken(data.session.token);

        // session را cache کن
        setCachedSession(data.session);

        // session را برگردان
        return data.session;
    }

    /*
     * تابع login - ورود کاربر
     *
     * این تابع کاربر موجود را login می‌کند.
     *
     * چرا این تابع:
     * - کاربران باید بتوانند با حساب خود login کنند
     * - token و session باید بعد از login ذخیره شوند
     * - session جدید از سرور گرفته می‌شود
     *
     * نحوه کار:
     * 1. درخواست login را به سرور می‌فرستد
     * 2. سرور username/password را چک می‌کند
     * 3. اگر درست بود، session برمی‌گرداند
     * 4. token را در localStorage ذخیره می‌کند
     * 5. session را cache می‌کند
     * 6. session را برمی‌گرداند
     *
     * خطاهای ممکن:
     * - 'نام کاربری یا رمز عبور اشتباه است'
     * - 'خطا در ارتباط با سرور'
     *
     * @param {string} username - نام کاربری
     * @param {string} password - رمز عبور
     * @returns {Promise<object>} - session شامل {username, role, coins, token}
     * @throws {Error} - در صورت خطا
     */
    async function login(username, password) {
        // درخواست login را بفرست
        const data = await api('login', { username, password });

        // token را ذخیره کن
        setToken(data.session.token);

        // session را cache کن
        setCachedSession(data.session);

        // session را برگردان
        return data.session;
    }

    /*
     * تابع refreshSession - به‌روزرسانی session از سرور
     *
     * این تابع session جدید از سرور می‌گیرد و cache را به‌روزرسانی می‌کند.
     *
     * چرا این تابع:
     * - می‌خواهم مطمئن شوم session هنوز معتبر است
     * - موجودی سکه ممکن است تغییر کرده باشد
     * - token ممکن است منقضی شده باشد
     * - بعد از reload صفحه باید session را از سرور بگیرم
     *
     * نحوه کار:
     * 1. token فعلی را از localStorage می‌گیرد
     * 2. اگر token نبود، cache را null می‌کند و null برمی‌گرداند
     * 3. درخواست session را به سرور می‌فرستد
     * 4. session جدید را cache می‌کند
     * 5. session را برمی‌گرداند
     *
     * استفاده:
     * - در window.onload برای بازیابی session
     * - بعد از عملیات مالی (خرید، پاداش)
     * - برای چک کردن اعتبار session
     *
     * @returns {Promise<object|null>} - session اگر معتبر باشد، وگرنه null
     */
    async function refreshSession() {
        // token را بگیر
        const token = getToken();

        // اگر token نبود، کاربر login نیست
        if (!token) {
            setCachedSession(null);
            return null;
        }

        // session جدید را از سرور بگیر
        const data = await api('session', { token });

        // cache را به‌روزرسانی کن
        setCachedSession(data.session);

        // session را برگردان
        return data.session;
    }

    /*
     * تابع getSessionSync - دریافت session از cache (بدون network call)
     *
     * این تابع session cache شده را برمی‌گرداند بدون درخواست به سرور.
     *
     * چرا این تابع:
     * - گاهی فقط می‌خواهم session فعلی را ببینم (بدون refresh)
     * - نمی‌خواهم منتظر async operation بمانم
     * - برای استفاده در توابع sync (مثل updateAccountPanel)
     *
     * نکته: این تابع ممکن است session قدیمی برگرداند!
     *
     * @returns {object|null} - session از cache (یا null)
     */
    function getSessionSync() {
        return cachedSession;
    }

    /*
     * تابع getSession - دریافت session (با fallback به refresh)
     *
     * این تابع session را برمی‌گرداند. اگر در cache بود، همان را می‌دهد،
     * وگرنه از سرور refresh می‌کند.
     *
     * چرا این تابع:
     * - می‌خواهم session را بگیرم ولی نمی‌دانم cache شده است یا نه
     * - اگر cache بود، سریع برمی‌گرداند
     * - اگر نبود، از سرور می‌گیرد
     *
     * استفاده:
     * - بیشتر جاها این تابع را استفاده می‌کنم
     * - برای چک کردن authentication
     * - برای گرفتن اطلاعات کاربر
     *
     * @returns {Promise<object|null>} - session (یا null)
     */
    async function getSession() {
        // اگر cache شده بود، همان را برگردان
        if (cachedSession) return cachedSession;

        // وگرنه از سرور refresh کن
        return refreshSession();
    }

    /*
     * تابع logout - خروج کاربر
     *
     * این تابع کاربر را logout می‌کند و تمام اطلاعات session را پاک می‌کند.
     *
     * چرا این تابع:
     * - کاربر باید بتواند از حساب خود خارج شود
     * - باید token و session را پاک کنم
     * - باید به سرور هم اطلاع دهم (برای invalidate کردن token)
     *
     * نحوه کار:
     * 1. token فعلی را می‌گیرد
     * 2. token را از localStorage حذف می‌کند
     * 3. cache را پاک می‌کند
     * 4. به سرور اطلاع می‌دهد (اگر token بود)
     *
     * نکته: اگر درخواست logout به سرور شکست بخورد، مشکلی نیست!
     * کاربر در client logout شده و token دیگر استفاده نمی‌شود.
     */
    function logout() {
        // token فعلی را بگیر
        const token = getToken();

        // token را حذف کن (logout local)
        setToken(null);

        // cache را پاک کن
        setCachedSession(null);

        // به سرور هم اطلاع بده (اگر token داشتیم)
        // اگر شکست بخورد، مهم نیست (silent fail)
        if (token) api('logout', { token }).catch(() => {});
    }

    // ═══════════════════════════════════════════════════════════════════════
    //                      توابع بررسی دسترسی (Authorization)
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * تابع isAuthenticated - چک کردن وضعیت login
     *
     * این تابع بررسی می‌کند که آیا کاربر login کرده است یا نه.
     *
     * چرا این تابع:
     * - قبل از شروع بازی باید مطمئن شوم کاربر login است
     * - برای نمایش/مخفی کردن دکمه‌ها
     * - برای redirect کردن به صفحه login
     *
     * نحوه کار:
     * 1. session را می‌گیرد (از cache یا سرور)
     * 2. !! برای تبدیل به boolean استفاده می‌شود
     * 3. اگر session بود → true، وگرنه → false
     *
     * استفاده:
     * - if (!await Auth.isAuthenticated()) { openAuth(); }
     *
     * @returns {Promise<boolean>} - true اگر login باشد، false اگر نباشد
     */
    async function isAuthenticated() {
        // session را بگیر
        const session = await getSession();

        // اگر session بود، true برگردان، وگرنه false
        return !!session;
    }

    /*
     * تابع hasRole - بررسی نقش کاربر
     *
     * این تابع چک می‌کند که آیا کاربر یکی از نقش‌های مورد نیاز را دارد یا نه.
     *
     * چرا این تابع:
     * - برخی عملیات فقط برای admin است (مثل پنل مدیریت)
     * - برخی عملیات فقط برای user/admin است (مثل بازی)
     * - می‌خواهم دسترسی را بر اساس role کنترل کنم
     *
     * نحوه کار:
     * 1. session را می‌گیرد
     * 2. اگر session نبود → false (کاربر login نیست)
     * 3. اگر requiredRoles خالی بود → true (بدون محدودیت)
     * 4. وگرنه چک می‌کند session.role در requiredRoles باشد
     *
     * نقش‌ها:
     * - 'user': کاربر عادی
     * - 'admin': مدیر سیستم
     *
     * استفاده:
     * - await Auth.hasRole(['admin']) // فقط admin
     * - await Auth.hasRole(['user', 'admin']) // هر دو
     * - await Auth.hasRole([]) // همه (حتی مهمان!)
     *
     * @param {string[]} requiredRoles - لیست نقش‌های مجاز
     * @returns {Promise<boolean>} - true اگر کاربر یکی از نقش‌ها را داشته باشد
     */
    async function hasRole(requiredRoles) {
        // session را بگیر
        const session = await getSession();

        // اگر session نبود، دسترسی نداریم
        if (!session) return false;

        // اگر هیچ محدودیتی نیست، همه می‌توانند
        if (!requiredRoles || requiredRoles.length === 0) return true;

        // چک کن نقش کاربر در لیست باشد
        return requiredRoles.includes(session.role);
    }

    // ═══════════════════════════════════════════════════════════════════════
    //                        توابع مدیریت سکه (Coin Management)
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * تابع getCoinBalance - دریافت موجودی سکه فعلی
     *
     * این تابع موجودی سکه کاربر را از سرور می‌گیرد.
     *
     * چرا این تابع:
     * - قبل از خرید بازی باید موجودی را چک کنم
     * - برای نمایش موجودی در UI
     * - می‌خواهم مطمئن شوم آخرین موجودی را دارم (refreshSession)
     *
     * نحوه کار:
     * 1. refreshSession را صدا می‌زند (از سرور می‌گیرد)
     * 2. اگر session بود، coins را برمی‌گرداند
     * 3. اگر نبود، 0 برمی‌گرداند
     * 4. Number() برای اطمینان از اینکه عدد است
     *
     * @returns {Promise<number>} - موجودی سکه (همیشه عدد)
     */
    async function getCoinBalance() {
        // session را refresh کن تا آخرین موجودی را داشته باشیم
        const session = await refreshSession();

        // موجودی را برگردان (یا 0 اگر session نبود)
        return session ? Number(session.coins || 0) : 0;
    }

    /*
     * تابع addCoin - اضافه کردن سکه به حساب کاربر
     *
     * این تابع سکه به حساب کاربر فعلی اضافه می‌کند.
     *
     * چرا این تابع:
     * - ادمین ممکن است بخواهد سکه هدیه بدهد
     * - برای تست و debug
     * - در آینده ممکن است سیستم پاداش روزانه داشته باشم
     *
     * نحوه کار:
     * 1. درخواست add_coin را به سرور می‌فرستد
     * 2. سرور سکه را اضافه می‌کند و موجودی جدید برمی‌گرداند
     * 3. cache را به‌روزرسانی می‌کند
     * 4. موجودی جدید را برمی‌گرداند
     *
     * @param {number} amount - مقدار سکه برای اضافه کردن (باید مثبت باشد)
     * @returns {Promise<number>} - موجودی جدید
     */
    async function addCoin(amount) {
        // درخواست اضافه کردن سکه را بفرست
        const data = await api('add_coin', { amount: Number(amount) || 0 });

        // cache را به‌روزرسانی کن
        if (cachedSession) cachedSession.coins = data.coins;

        // موجودی جدید را برگردان
        return data.coins;
    }

    /*
     * تابع subtractCoin - کسر سکه از حساب کاربر
     *
     * این تابع سکه را از حساب کاربر کم می‌کند (معمولاً برای خرید بازی).
     *
     * چرا این تابع:
     * - هر بازی قیمت دارد و باید قبل از شروع پرداخت شود
     * - می‌خواهم اطمینان حاصل کنم موجودی کافی است (سرور چک می‌کند)
     * - سیستم اقتصادی arcade را پیاده‌سازی می‌کنم
     *
     * نحوه کار:
     * 1. درخواست subtract_coin را به سرور می‌فرستد
     * 2. سرور چک می‌کند موجودی کافی هست
     * 3. اگر کافی بود، کسر می‌کند و موجودی جدید برمی‌گرداند
     * 4. اگر کافی نبود، خطا می‌دهد
     * 5. cache را به‌روزرسانی می‌کند
     *
     * خطاهای ممکن:
     * - 'موجودی کافی نیست'
     *
     * @param {number} amount - مقدار سکه برای کسر کردن
     * @returns {Promise<number>} - موجودی جدید
     * @throws {Error} - اگر موجودی کافی نباشد
     */
    async function subtractCoin(amount) {
        // درخواست کسر سکه را بفرست
        const data = await api('subtract_coin', { amount: Number(amount) || 0 });

        // cache را به‌روزرسانی کن
        if (cachedSession) cachedSession.coins = data.coins;

        // موجودی جدید را برگردان
        return data.coins;
    }

    /*
     * تابع rewardWin - پاداش دادن بعد از برد در بازی
     *
     * این تابع بعد از اینکه کاربر بازی را می‌برد، پاداش به حسابش می‌ریزد.
     *
     * چرا این تابع:
     * - می‌خواهم بازیکنان را تشویق کنم که بازی کنند
     * - سیستم اقتصادی باید چرخش داشته باشد (خرج و درآمد)
     * - پاداش معمولاً بیشتر از قیمت بازی است (سود 20%)
     *
     * نحوه کار:
     * 1. basePrice را به سرور می‌فرستد (قیمت بازی)
     * 2. سرور پاداش را محاسبه می‌کند: basePrice * (1 + DEFAULT_REWARD_RATE)
     * 3. پاداش را به حساب کاربر اضافه می‌کند
     * 4. موجودی جدید را برمی‌گرداند
     * 5. cache را به‌روزرسانی می‌کند
     *
     * مثال:
     * - بازی 10 سکه قیمت دارد
     * - کاربر بازی را می‌برد
     * - پاداش: 10 * 1.2 = 12 سکه
     * - سود: 2 سکه
     *
     * @param {number} basePrice - قیمت بازی (برای محاسبه پاداش)
     * @returns {Promise<number>} - موجودی جدید
     */
    async function rewardWin(basePrice) {
        // درخواست پاداش را بفرست
        const data = await api('reward_win', { basePrice: Number(basePrice) || 0 });

        // cache را به‌روزرسانی کن
        if (cachedSession) cachedSession.coins = data.coins;

        // موجودی جدید را برگردان
        return data.coins;
    }

    // ═══════════════════════════════════════════════════════════════════════
    //                        توابع پنل ادمین (Admin Functions)
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * تابع adminAddCoins - اضافه کردن سکه به حساب کاربر دیگر (توسط ادمین)
     *
     * این تابع به ادمین اجازه می‌دهد سکه به حساب هر کاربری اضافه کند.
     *
     * چرا این تابع:
     * - ادمین باید بتواند اقتصاد سیستم را مدیریت کند
     * - برای جبران خطاها یا هدیه دادن
     * - برای تست سیستم
     *
     * نحوه کار:
     * 1. درخواست admin_add_coins را به سرور می‌فرستد
     * 2. سرور چک می‌کند کاربر فعلی admin است
     * 3. سکه را به حساب targetUsername اضافه می‌کند
     * 4. لاگ می‌کند (برای audit trail)
     * 5. موجودی جدید کاربر هدف را برمی‌گرداند
     *
     * محدودیت: فقط admin می‌تواند استفاده کند
     *
     * @param {string} targetUsername - نام کاربری که سکه به او اضافه می‌شود
     * @param {number} amount - مقدار سکه (باید مثبت باشد)
     * @returns {Promise<number>} - موجودی جدید کاربر هدف
     * @throws {Error} - اگر کاربر admin نباشد یا کاربر هدف وجود نداشته باشد
     */
    async function adminAddCoins(targetUsername, amount) {
        // درخواست اضافه کردن سکه را بفرست
        const data = await api('admin_add_coins', {
            username: targetUsername,
            amount: Number(amount) || 0
        });

        // موجودی جدید کاربر هدف را برگردان
        return data.coins;
    }

    /*
     * تابع adminChangeRole - تغییر نقش کاربر (توسط ادمین)
     *
     * این تابع به ادمین اجازه می‌دهد نقش یک کاربر را تغییر دهد (user ↔ admin).
     *
     * چرا این تابع:
     * - ادمین باید بتواند ادمین‌های جدید بسازد
     * - برای تنزل رتبه کاربران (admin → user)
     * - مدیریت دسترسی‌ها
     *
     * نحوه کار:
     * 1. درخواست admin_change_role را به سرور می‌فرستد
     * 2. سرور چک می‌کند کاربر فعلی admin است
     * 3. نقش کاربر هدف را تغییر می‌دهد
     * 4. لاگ می‌کند
     * 5. اطلاعات به‌روزرسانی شده کاربر را برمی‌گرداند
     *
     * نقش‌های ممکن:
     * - 'user': کاربر عادی
     * - 'admin': مدیر سیستم
     *
     * محدودیت: فقط admin می‌تواند استفاده کند
     *
     * @param {string} targetUsername - نام کاربری که نقشش تغییر می‌کند
     * @param {string} role - نقش جدید ('user' یا 'admin')
     * @returns {Promise<object>} - اطلاعات به‌روزرسانی شده کاربر
     * @throws {Error} - اگر کاربر admin نباشد
     */
    async function adminChangeRole(targetUsername, role) {
        // درخواست تغییر نقش را بفرست
        const data = await api('admin_change_role', {
            username: targetUsername,
            role
        });

        // اطلاعات به‌روزرسانی شده کاربر را برگردان
        return data.user;
    }

    /*
     * تابع listUsers - دریافت لیست تمام کاربران (برای پنل ادمین)
     *
     * این تابع لیست تمام کاربران سیستم را برمی‌گرداند.
     *
     * چرا این تابع:
     * - ادمین باید بتواند تمام کاربران را ببیند
     * - برای پنل مدیریت کاربران
     * - برای انتخاب کاربر برای عملیات ادمین
     *
     * نحوه کار:
     * 1. درخواست list_users را به سرور می‌فرستد
     * 2. سرور چک می‌کند کاربر فعلی admin است
     * 3. لیست تمام کاربران را برمی‌گرداند
     *
     * فرمت خروجی:
     * [
     *   {username: 'user1', role: 'user', coins: 100},
     *   {username: 'admin1', role: 'admin', coins: 500},
     *   ...
     * ]
     *
     * محدودیت: فقط admin می‌تواند استفاده کند
     *
     * @returns {Promise<Array>} - آرایه‌ای از اطلاعات کاربران
     * @throws {Error} - اگر کاربر admin نباشد
     */
    async function listUsers() {
        // درخواست لیست کاربران را بفرست
        const data = await api('list_users');

        // لیست کاربران را برگردان (یا آرایه خالی اگر نبود)
        return data.users || [];
    }

    /*
     * تابع getAdminLogs - دریافت لاگ‌های عملیات ادمین
     *
     * این تابع تاریخچه عملیات ادمین‌ها را برمی‌گرداند (audit trail).
     *
     * چرا این تابع:
     * - باید بدانیم چه کسی چه کاری کرده است
     * - برای شفافیت و امنیت
     * - برای پیگیری تغییرات سکه
     *
     * نحوه کار:
     * 1. درخواست logs را به سرور می‌فرستد
     * 2. سرور چک می‌کند کاربر فعلی admin است
     * 3. لیست لاگ‌ها را برمی‌گرداند
     *
     * فرمت خروجی:
     * [
     *   {admin: 'admin1', target: 'user1', delta: 50, ts: 1234567890},
     *   ...
     * ]
     *
     * محدودیت: فقط admin می‌تواند استفاده کند
     *
     * @returns {Promise<Array>} - آرایه‌ای از لاگ‌ها
     * @throws {Error} - اگر کاربر admin نباشد
     */
    async function getAdminLogs() {
        // درخواست لاگ‌ها را بفرست
        const data = await api('logs');

        // لیست لاگ‌ها را برگردان (یا آرایه خالی اگر نبود)
        return data.logs || [];
    }

    // ═══════════════════════════════════════════════════════════════════════
    //                        توابع کمکی (Helper Functions)
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * تابع getRewardRate - دریافت نرخ پاداش
     *
     * این تابع نرخ پاداش برد را برمی‌گرداند.
     * فعلاً ثابت است (0.2 = 20%) ولی در آینده ممکن است قابل تنظیم شود.
     *
     * @returns {number} - نرخ پاداش (0.2 = 20%)
     */
    function getRewardRate() {
        return DEFAULT_REWARD_RATE;
    }

    /*
     * تابع setRewardRate - تنظیم نرخ پاداش
     *
     * این تابع فعلاً کاری نمی‌کند و همیشه نرخ پیش‌فرض را برمی‌گرداند.
     * در آینده ممکن است ادمین بتواند نرخ را تغییر دهد.
     *
     * @param {number} rate - نرخ جدید (نادیده گرفته می‌شود)
     * @returns {number} - نرخ پیش‌فرض (همیشه)
     */
    function setRewardRate(rate) {
        return DEFAULT_REWARD_RATE;
    }

    // ═══════════════════════════════════════════════════════════════════════
    //                    Export - رابط عمومی ماژول
    // ═══════════════════════════════════════════════════════════════════════

    /*
     * این object شامل تمام توابعی است که از بیرون قابل دسترسی هستند.
     *
     * دسته‌بندی:
     * - احراز هویت: register, login, logout, isAuthenticated, hasRole
     * - مدیریت session: getSession, getSessionSync, refreshSession
     * - مدیریت سکه: getCoinBalance, addCoin, subtractCoin, rewardWin
     * - توابع ادمین: listUsers, adminAddCoins, adminChangeRole, getAdminLogs
     * - کمکی: getRewardRate, setRewardRate
     *
     * یادآوری برای خودم:
     * - اگر تابع جدیدی اضافه کردم، باید اینجا export کنم
     * - توابع private (مثل api, getToken) export نمی‌شوند
     */
    return {
        // احراز هویت
        register,
        login,
        logout,
        isAuthenticated,

        // مدیریت session
        getSession,
        getSessionSync,
        refreshSession,
        hasRole,

        // مدیریت سکه
        getCoinBalance,
        addCoin,
        subtractCoin,
        rewardWin,

        // کمکی
        getRewardRate,
        setRewardRate,

        // توابع ادمین
        listUsers,
        adminAddCoins,
        adminChangeRole,
        getAdminLogs
    };
})();
