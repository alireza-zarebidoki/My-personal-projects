/* File: public_html/script.js */

// تنظیمات ذرات (بدون تغییر)
if (typeof particlesJS !== 'undefined') {
    particlesJS("particles-js", {
        "particles": { "number": { "value": 80 }, "color": { "value": "#10b981" }, "shape": { "type": "circle" }, "opacity": { "value": 0.3 }, "size": { "value": 3, "random": true }, "line_linked": { "enable": true, "distance": 150, "color": "#10b981", "opacity": 0.15, "width": 1 }, "move": { "enable": true, "speed": 1 } },
        "interactivity": { "events": { "onhover": { "enable": true, "mode": "grab" } } }
    });
}

// --- منطق منوی چرخشی اصلاح شده ---

let selectedIndex = 0;
const games = document.querySelectorAll('.game-item');
const carousel = document.querySelector('.game-carousel');

// ابعاد دقیق (الان با CSS هماهنگ است)
const ITEM_WIDTH = 120; // عرض آیتم
const GAP = 50;         // فاصله بین آیتم‌ها
const TOTAL_UNIT = ITEM_WIDTH + GAP; // 170px

function updateSelection() {
    games.forEach((g, index) => {
        if (index === selectedIndex) {
            g.classList.add('active');
        } else {
            g.classList.remove('active');
        }
    });

    // فرمول حرکت:
    // آیتم‌ها به صورت خطی چیده شده‌اند. برای اینکه آیتم n وسط بیاید،
    // باید کل نوار را به اندازه (n * عرض واحد) به چپ بکشیم.
    // + نصف عرض آیتم برای اینکه مرکز آیتم روی مرکز صفحه بیفتد.
    const centerOffset = ITEM_WIDTH / 2;
    const translateValue = -(selectedIndex * TOTAL_UNIT) - centerOffset;

    // چون left: 50% در CSS داریم، این translateValue نوار را نسبت به وسط صفحه جابجا میکند
    carousel.style.transform = `translateX(${translateValue}px)`;
}

function selectGame() {
    const activeGame = games[selectedIndex];
    const url = activeGame.getAttribute('data-url');

    if (url) {
        const display = document.getElementById('game-display');
        display.innerHTML = '<h2 class="blink-text" style="color:#facc15; margin-top:0;">LOADING SYSTEM...</h2>';

        setTimeout(() => {
            display.innerHTML = `
                <iframe src="cgi-bin/${url}"
                        style="width:100%; height:100%; border:none; background: transparent;"
                        title="Game Frame">
                </iframe>
                /* دکمه قرمز EXIT از اینجا حذف شد تا فقط دکمه فیزیکی B مسئول خروج باشد */
            `;
        }, 800);
    } else {
        activeGame.style.animation = "shake 0.5s";
        setTimeout(() => activeGame.style.animation = "", 500);
    }
}

document.addEventListener('keydown', (e) => {
    if(["ArrowRight", "ArrowLeft", "ArrowUp", "ArrowDown"].indexOf(e.code) > -1) {
        e.preventDefault();
    }

    if (e.key === 'ArrowRight') {
        // راست = آیتم بعدی (چون چیدمان LTR است)
        selectedIndex = (selectedIndex + 1) % games.length;
        updateSelection();
    } else if (e.key === 'ArrowLeft') {
        // چپ = آیتم قبلی
        selectedIndex = (selectedIndex - 1 + games.length) % games.length;
        updateSelection();
    } else if (e.key === 'Enter') {
        selectGame();
    }
    if (e.key === 'Enter' || e.key === 'a' || e.key === 'A') {
        selectGame(); // کلید A یا اینتر برای شروع
    } else if (e.key === 'b' || e.key === 'B' || e.key === 'Escape') {
        location.reload(); // کلید B یا Escape برای خروج و بازگشت به منو
    }
});

window.onload = () => {
    updateSelection();
};
