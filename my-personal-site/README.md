# Personal Website & Blog - Zarebidoki.com 🌐

This directory houses the source code for my personal portfolio website and my custom-built blogging platform, accessible at **[zarebidoki.com](https://zarebidoki.com)** and **[blog.zarebidoki.com](https://blog.zarebidoki.com)**.

Designed as a showcase of my skills, this project consists of a **lightweight, interactive portfolio** and a **full-stack blog engine** built from scratch.

![HTML5](https://img.shields.io/badge/HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white)
![CSS3](https://img.shields.io/badge/CSS3-1572B6?style=for-the-badge&logo=css3&logoColor=white)
![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black)
![PHP](https://img.shields.io/badge/PHP-777BB4?style=for-the-badge&logo=php&logoColor=white)
![MySQL](https://img.shields.io/badge/MySQL-4479A1?style=for-the-badge&logo=mysql&logoColor=white)

## 🚀 Overview

The project is divided into two main modules:

1.  **Main Portfolio (Frontend):** A single-page application acting as my digital CV.
2.  **Blog Engine (Full-Stack):** A custom CMS with an Admin Panel to publish articles about Computer Science.

## ✨ Key Features

### 🖥️ 1. Interactive Terminal
A simulated Linux terminal in the browser that accepts commands like `help`, `skills`, `contact`, and `blog`.
* **Tech:** Pure JavaScript.
* **Feature:** Supports command history, typing effects, and custom responses.

### 📝 2. Custom Blog System
Instead of using WordPress, I built a custom blog engine.
* **Dynamic Content:** Fetches posts via a RESTful API.
* **Hashtag System:** Auto-detects hashtags (e.g., `#Linux`) in posts and links them to search results.
* **Smart Formatting:** Converts plain text to HTML paragraphs automatically.

### 🛡️ 3. Admin Panel (CMS)
A secure area to manage content without touching the database.
* **Rich Text Editor:** Integrated **Quill.js** with a custom Dark Mode theme.
* **CRUD Operations:** Create, Read, Update, and Delete posts via API.
* **Security:** Session-based authentication.

## 📂 Architecture & Structure

```text
my-personal-site/
├── index.html            # Main Portfolio Entry
├── script.js             # Terminal logic & UI interactions
├── style.css             # Global styles & Animations
├── blog/                 # Blog Subsystem
│   ├── index.html        # Blog Home (List of posts)
│   ├── article.html      # Single Post View
│   ├── admin/            # Admin Panel (Protected)
│   ├── api/              # Backend Logic (PHP)
│   │   ├── db_connect.php
│   │   ├── get_posts.php
│   │   ├── add_post.php
│   │   ├── update_post.php
│   │   └── login.php
│   └── js/               # Blog Frontend Logic
└── images/               # Assets

```

## 🛠️ Built With

* **Frontend:** HTML5, CSS3, JavaScript (ES6+), Particles.js.
* **Backend:** Native PHP (REST API).
* **Database:** MySQL.
* **Editor:** Quill.js (customized).

## 💻 How to Run Locally

Since the project now includes a backend (PHP/MySQL), you need a local server environment (like XAMPP, WAMP, or Docker).

1. **Clone the repository:**
```bash
git clone [https://github.com/alireza-zarebidoki/My-personal-projects.git](https://github.com/alireza-zarebidoki/My-personal-projects.git)

```


2. **Setup Database:**
* Create a MySQL database named `blog`.
* Import the SQL schema (create `users` and `posts` tables).


3. **Configure Connection:**
* Update `blog/api/db_connect.php` with your local database credentials.


4. **Run:**
* Place the project in your server's root (e.g., `htdocs`).
* Open `http://localhost/my-personal-site` in your browser.



## 📫 Contact

* **Live Site:** [zarebidoki.com](https://zarebidoki.com)
* **Blog:** [blog.zarebidoki.com](https://www.google.com/url?sa=E&source=gmail&q=https://blog.zarebidoki.com)
* **GitHub:** [alireza-zarebidoki](https://github.com/alireza-zarebidoki)

---

*Created with ❤️ by Alireza Zarebidoki*
