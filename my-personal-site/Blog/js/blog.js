const API_URL = 'api/get_posts.php';

// --- تابع کمکی: تبدیل هشتگ به لینک ---
function linkify(text) {
    // این کد تمام کلماتی که با # شروع میشن رو پیدا میکنه و لینک میکنه
    // ولی حواسش هست که کد رنگ‌ها (مثل #fff) رو خراب نکنه
    return text.replace(/(^|\s|>)(#([\w\u0600-\u06FF]+))/g,
        '$1<a href="index.html?tag=$3" class="hashtag">$2</a>');
}

// --- 1. لود کردن لیست پست‌ها (صفحه اصلی + جستجو) ---
async function loadPosts() {
    const container = document.getElementById('posts-container');
    if (!container) return;

    // چک کردن اینکه آیا دنبال تگ خاصی هستیم؟
    const urlParams = new URLSearchParams(window.location.search);
    const tag = urlParams.get('tag');

    let fetchUrl = API_URL;
    let pageTitle = "آخرین نوشته‌ها";

    if (tag) {
        fetchUrl = `${API_URL}?tag=${tag}`;
        pageTitle = `نتایج جستجو برای: <span style="color:#10b981">#${tag}</span>`;

        // اضافه کردن دکمه "پاک کردن فیلتر"
        pageTitle += ` <a href="index.html" class="btn-clear-filter" title="نمایش همه"><i class="fas fa-times"></i></a>`;
    }

    // اگر المنتی برای تایتل صفحه اصلی داری، آپدیتش کن (اختیاری)
    const heroSubtitle = document.querySelector('.hero-subtitle');
    if(heroSubtitle && tag) heroSubtitle.innerHTML = pageTitle;

    try {
        const response = await fetch(fetchUrl);
        const posts = await response.json();

        container.innerHTML = '';

        if (posts.length === 0) {
            container.innerHTML = `
                <div class="card" style="text-align:center;">
                    <i class="fas fa-search" style="font-size:3rem; color:#334155; margin-bottom:20px;"></i><br>
                    مطلبی با تگ <b>#${tag}</b> پیدا نشد.
                    <br><br>
                    <a href="index.html" class="btn btn-outline">بازگشت به همه مطالب</a>
                </div>`;
            return;
        }

        posts.forEach(post => {
            const date = new Date(post.created_at).toLocaleDateString('fa-IR');
            // خلاصه پست رو هم لینک‌دار میکنیم
            const linkedSummary = linkify(post.summary || '');

            const card = `
                <div class="card">
                    <h2 class="section-title" style="border:none; margin-bottom: 10px; font-size: 1.6rem;">
                        <a href="article.html?id=${post.id}" style="text-decoration:none; color:#fff;">${post.title}</a>
                    </h2>
                    <div class="article-meta"><i class="far fa-clock"></i> ${date}</div>

                    <p style="color: #cbd5e1; margin-bottom: 20px;">
                        ${linkedSummary || 'توضیحی ثبت نشده...'}
                    </p>

                    <div style="text-align: left;">
                        <a href="article.html?id=${post.id}" class="btn btn-outline" style="font-size: 0.9rem;">
                            ادامه مطلب <i class="fas fa-chevron-left"></i>
                        </a>
                    </div>
                </div>
            `;
            container.innerHTML += card;
        });

    } catch (error) {
        console.error(error);
        container.innerHTML = '<div class="card" style="color:#ff5f56;">خطا در ارتباط با سرور.</div>';
    }
}

// --- 2. لود کردن پست تکی ---
async function loadSinglePost() {
    const titleEl = document.getElementById('post-title');
    if (!titleEl) return;

    const urlParams = new URLSearchParams(window.location.search);
    const postId = urlParams.get('id');

    if (!postId) return;

    try {
        const response = await fetch(`${API_URL}?id=${postId}`);
        const post = await response.json();

        if (post.message) return;

        document.title = post.title;
        document.getElementById('post-title').innerText = post.title;
        document.getElementById('post-date').innerText = new Date(post.created_at).toLocaleDateString('fa-IR');

        // *** جادوی اصلی اینجاست: تبدیل متن ساده به لینک ***
        document.getElementById('post-body').innerHTML = linkify(post.content);

        document.getElementById('loading').style.display = 'none';
        document.getElementById('article-container').style.display = 'block';

    } catch (error) {
        console.error(error);
    }
}

document.addEventListener('DOMContentLoaded', () => {
    loadPosts();
    loadSinglePost();
});
