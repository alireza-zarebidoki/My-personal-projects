// --- 1. تنظیمات Particles.js (پس‌زمینه متحرک) ---
particlesJS("particles-js", {
    "particles": {
        "number": { "value": 80, "density": { "enable": true, "value_area": 800 } },
        "color": { "value": "#10b981" },
        "shape": { "type": "circle" },
        "opacity": { "value": 0.5, "random": false },
        "size": { "value": 3, "random": true },
        "line_linked": {
            "enable": true,
            "distance": 150,
            "color": "#10b981",
            "opacity": 0.2,
            "width": 1
        },
        "move": { "enable": true, "speed": 2, "direction": "none", "random": false, "out_mode": "out" }
    },
    "interactivity": {
        "detect_on": "canvas",
        "events": {
            "onhover": { "enable": true, "mode": "grab" },
            "onclick": { "enable": true, "mode": "push" }
        },
        "modes": {
            "grab": { "distance": 140, "line_linked": { "opacity": 1 } }
        }
    },
    "retina_detect": true
});

// --- 2. تنظیمات Typewriter (تایپ خودکار) ---
const textElement = document.getElementById("typewriter-text");
const texts = [
    "Computer Science Student",
    "Programmer (C/C++, Java)",
    "Linux Enthusiast",
    "Tech Explorer"
];
let textIndex = 0;
let charIndex = 0;
let isDeleting = false;
let typeSpeed = 100;

function typeWriter() {
    const currentText = texts[textIndex];

    if (isDeleting) {
        textElement.innerText = currentText.substring(0, charIndex - 1);
        charIndex--;
        typeSpeed = 50; // سرعت پاک کردن
    } else {
        textElement.innerText = currentText.substring(0, charIndex + 1);
        charIndex++;
        typeSpeed = 100; // سرعت تایپ
    }

    if (!isDeleting && charIndex === currentText.length) {
        isDeleting = true;
        typeSpeed = 2000; // مکث بعد از تایپ کامل
    } else if (isDeleting && charIndex === 0) {
        isDeleting = false;
        textIndex = (textIndex + 1) % texts.length;
        typeSpeed = 500; // مکث قبل از شروع تایپ جدید
    }

    setTimeout(typeWriter, typeSpeed);
}

// شروع تایپ‌نویس بعد از لود صفحه
document.addEventListener('DOMContentLoaded', typeWriter);


// --- 3. تنظیمات Terminal (ترمینال تعاملی) ---
const terminalInput = document.getElementById('terminal-input');
const terminalBody = document.getElementById('terminal-body');
const commandLine = document.querySelector('.command-line');

// فوکس همیشگی روی اینپوت ترمینال وقتی روی باکس کلیک میشه
document.getElementById('terminal').addEventListener('click', () => {
    terminalInput.focus();
});

terminalInput.addEventListener('keypress', function (e) {
    if (e.key === 'Enter') {
        const command = this.value.trim().toLowerCase();

        // ایجاد خط دستور قبلی در تاریخچه
        const historyLine = document.createElement('div');
        historyLine.innerHTML = `<span class="prompt">➜  ~</span> ${this.value}`;
        historyLine.style.marginBottom = "5px";
        terminalBody.insertBefore(historyLine, commandLine);

        // پردازش دستور
        let response = "";
        let responseClass = "output";

        switch (command) {
            case 'help':
                response = "Available commands:\n" +
                           "  <span class='output-info'>about</span>    : Who am I?\n" +
                           "  <span class='output-info'>skills</span>   : My technical skills\n" +
                           "  <span class='output-info'>social</span>   : Social media links\n" +
                           "  <span class='output-info'>contact</span>  : Show email\n" +
                           "  <span class='output-info'>blog</span>     : My Personal Blog (NEW!)\n" +
                           "  <span class='output-info'>clear</span>    : Clear terminal\n" +
                           "  <span class='output-info'>date</span>     : Show current date";
                break;
            case 'about':
                response = "I am Alireza, a Computer Science student at FUM (Ferdowsi University of Mashhad). Passionate about low-level programming and OS.";
                break;
            case 'skills':
                response = "LANGUAGES: C, C++, Java, JavaScript\n" +
                           "TOOLS: Git, Linux (Bash), Vim\n" +
                           "WEB: HTML5, CSS3";
                break;
            case 'contact':
                response = "Email: alireza@zarebidoki.com";
                responseClass = "output-success";
                break;
            case 'social':
                response = "GitHub: github.com/alireza-zarebidoki";
                break;
            case 'blog':
                response = "Opening blog in a new tab...";
                responseClass = "output-success";
                // باز کردن وبلاگ در تب جدید
                window.open('https://blog.zarebidoki.com', '_blank');
                break;
            case 'clear':
                while (terminalBody.firstChild !== commandLine) {
                    terminalBody.removeChild(terminalBody.firstChild);
                }
                this.value = '';
                return; // خروج از تابع برای جلوگیری از چاپ پاسخ خالی
            case 'date':
                response = new Date().toString();
                break;
            case 'sudo':
                response = "Permission denied: You are not root!";
                responseClass = "output-error";
                break;
            case '':
                response = "";
                break;
            default:
                response = `zsh: command not found: ${command}`;
                responseClass = "output-error";
        }

        if (response) {
            const responseDiv = document.createElement('div');
            responseDiv.className = responseClass;
            responseDiv.innerHTML = response;
            responseDiv.style.marginBottom = "15px";
            terminalBody.insertBefore(responseDiv, commandLine);
        }

        this.value = '';
        terminalBody.scrollTop = terminalBody.scrollHeight;
    }
});
