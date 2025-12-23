const API_BASE = '../api/';
let quill; // متغیر برای ذخیره ادیتور

document.addEventListener('DOMContentLoaded', () => {
    // 1. راه‌اندازی ادیتور متن پیشرفته
    quill = new Quill('#editor-container', {
        theme: 'snow',
        placeholder: 'متن مقاله خود را اینجا بنویسید...',
        modules: {
            toolbar: [
                [{ 'header': [1, 2, 3, false] }],
                ['bold', 'italic', 'underline', 'strike'],
                [{ 'list': 'ordered'}, { 'list': 'bullet' }],
                [{ 'direction': 'rtl' }, { 'align': [] }],
                ['link', 'image', 'code-block'],
                ['clean']
            ]
        }
    });

    // چک کردن لاگین
    const user = localStorage.getItem('admin_user');
    if (user) {
        showDashboard();
        loadAdminPosts();
    }
});

async function login() {
    const u = document.getElementById('username').value;
    const p = document.getElementById('password').value;
    const msg = document.getElementById('login-msg');

    if(!u || !p) return;

    try {
        const res = await fetch(API_BASE + 'login.php', {
            method: 'POST',
            body: JSON.stringify({ username: u, password: p })
        });
        const data = await res.json();

        if (data.message === 'login_success') {
            localStorage.setItem('admin_user', data.username);
            showDashboard();
            loadAdminPosts();
        } else {
            msg.innerText = data.message;
        }
    } catch (err) { msg.innerText = "خطا در اتصال"; }
}

function showDashboard() {
    document.getElementById('login-section').style.display = 'none';
    document.getElementById('dashboard-section').style.display = 'block';
}

function logout() {
    localStorage.removeItem('admin_user');
    location.reload();
}

// --- مدیریت ارسال / آپدیت ---
async function submitPost() {
    const id = document.getElementById('post-id').value; // اگر پر باشد یعنی در حال ویرایشیم
    const title = document.getElementById('post-title').value;
    const summary = document.getElementById('post-summary').value;
    const content = quill.root.innerHTML; // گرفتن متن از ادیتور Quill

    if (!title || content === '<p><br></p>') {
        alert("عنوان و متن مقاله الزامی است!");
        return;
    }

    const url = id ? 'update_post.php' : 'add_post.php'; // انتخاب API مناسب
    const payload = { title, summary, content };
    if (id) payload.id = id;

    const res = await fetch(API_BASE + url, {
        method: 'POST',
        body: JSON.stringify(payload)
    });
    const data = await res.json();

    if (data.message === 'post_created' || data.message === 'post_updated') {
        alert(id ? "✅ پست ویرایش شد!" : "✅ پست منتشر شد!");
        clearForm();
        loadAdminPosts();
    } else {
        alert("خطا: " + data.message);
    }
}

// --- پر کردن فرم برای ویرایش ---
async function editPost(id) {
    // گرفتن اطلاعات پست تکی
    const res = await fetch(API_BASE + `get_posts.php?id=${id}`);
    const post = await res.json();

    if (post) {
        document.getElementById('post-id').value = post.id;
        document.getElementById('post-title').value = post.title;
        document.getElementById('post-summary').value = post.summary;
        quill.root.innerHTML = post.content; // ریختن متن در ادیتور

        // تغییر متن دکمه
        document.getElementById('submit-btn').innerText = "ذخیره تغییرات";
        document.getElementById('submit-btn').style.background = "#facc15"; // زرد
        document.getElementById('submit-btn').style.color = "#000";

        // اسکرول به بالا
        window.scrollTo({ top: 0, behavior: 'smooth' });
    }
}

function clearForm() {
    document.getElementById('post-id').value = '';
    document.getElementById('post-title').value = '';
    document.getElementById('post-summary').value = '';
    quill.root.innerHTML = '';

    document.getElementById('submit-btn').innerText = "انتشار مقاله";
    document.getElementById('submit-btn').style.background = "#10b981"; // سبز
    document.getElementById('submit-btn').style.color = "#0f172a";
}

async function loadAdminPosts() {
    const res = await fetch(API_BASE + 'get_posts.php');
    const posts = await res.json();
    const list = document.getElementById('admin-posts-list');
    list.innerHTML = '';

    if(posts.length > 0 && !posts.message) {
        posts.forEach(post => {
            list.innerHTML += `
                <li>
                    <span>${post.title}</span>
                    <div>
                        <i class="fas fa-edit edit-btn" onclick="editPost(${post.id})" title="ویرایش"></i>
                        <i class="fas fa-trash delete-btn" onclick="deletePost(${post.id})" title="حذف"></i>
                    </div>
                </li>
            `;
        });
    } else {
        list.innerHTML = '<li style="justify-content:center; color:#64748b;">هیچ پستی یافت نشد.</li>';
    }
}

async function deletePost(id) {
    if(!confirm("⚠️ حذف پست غیرقابل برگشت است. ادامه می‌دهید؟")) return;
    const res = await fetch(API_BASE + 'delete_post.php', {
        method: 'POST',
        body: JSON.stringify({ id: id })
    });
    const data = await res.json();
    if (data.message === 'post_deleted') loadAdminPosts();
}
