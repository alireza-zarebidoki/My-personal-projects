# Personal Website & Custom Blog Engine 🌐

Welcome to the source code of my personal portfolio and custom-built blogging platform. This project demonstrates my **Full-Stack development skills**, moving beyond static templates to a dynamic, database-driven Content Management System (CMS).

**Live Demo:**
- 🏠 **Portfolio:** [zarebidoki.com](https://zarebidoki.com)
- 📝 **Blog:** [blog.zarebidoki.com](https://blog.zarebidoki.com)

---

## 🚀 Project Highlights

This repository consists of two integrated modules:

1.  **Interactive Portfolio (Frontend):** A creative single-page application featuring a **simulated Linux terminal**, particle physics background, and responsive design.
2.  **Custom Blog Engine (Backend):** A lightweight CMS built from scratch using **Native PHP** (No WordPress/Frameworks). It features a secure Admin Panel, RESTful API, and a Markdown-like rendering system.

---

## 🛠️ Tech Stack

### Frontend
![HTML5](https://img.shields.io/badge/HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white)
![CSS3](https://img.shields.io/badge/CSS3-1572B6?style=for-the-badge&logo=css3&logoColor=white)
![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black)
* **Particles.js:** For the neural network background effect.
* **Quill.js:** Customized WYSIWYG editor with Dark Mode integration.
* **Vazirmatn Font:** Optimized typography for Persian content.

### Backend & Database
![PHP](https://img.shields.io/badge/PHP-777BB4?style=for-the-badge&logo=php&logoColor=white)
![MySQL](https://img.shields.io/badge/MySQL-4479A1?style=for-the-badge&logo=mysql&logoColor=white)
![REST API](https://img.shields.io/badge/REST%20API-005571?style=for-the-badge&logo=fastapi&logoColor=white)
* **Native PHP (PDO):** Secure database interactions and API endpoints.
* **Session Management:** Custom session handling logic for secure authentication.
* **Security:** Password hashing (`password_hash` / `password_verify`) and SQL Injection protection.

---

## ✨ Key Features

### 1. 🖥️ Interactive Web Terminal
A functional terminal simulator on the main page.
* **Commands:** `help`, `about`, `skills`, `contact`, `blog`, `date`, `clear`, `sudo`.
* **Features:** Command history, typing effects, and Easter eggs.

### 2. 🛡️ Secure Admin Panel (CMS)
A protected dashboard to manage content.
* **Authentication:** Secure login system with hashed passwords.
* **CRUD Operations:** Create, Read, Update, and Delete posts via API.
* **Rich Text Editor:** Integrated Quill.js editor that supports images, code blocks, and formatting.

### 3. 📝 Dynamic Blog System
* **Hashtag System:** Auto-detects hashtags (e.g., `#Linux`) in posts and generates searchable links.
* **Smart API:** Fetches content asynchronously without reloading the page.

---

## 📂 Project Structure

```text
my-personal-site/
├── index.html            # Main Portfolio Entry
├── script.js             # Terminal & UI Logic
├── style.css             # Global Styles
└── blog/                 # Blog Sub-system
    ├── index.html        # Blog Home (Post List)
    ├── article.html      # Single Post View
    ├── css/              # Blog specific styles
    ├── js/               # Blog frontend logic
    ├── api/              # PHP REST API
    │   ├── db_connect.php    # Database Connection (PDO)
    │   ├── login.php         # Auth Logic
    │   ├── get_posts.php     # Public API (Read)
    │   ├── add_post.php      # Protected API (Create)
    │   ├── update_post.php   # Protected API (Update)
    │   └── delete_post.php   # Protected API (Delete)
    └── admin/            # Admin Panel UI
        ├── index.html    # Dashboard
        └── script.js     # Admin Logic & Quill Setup

```

---

## 💻 Installation & Setup

To run this project locally, you need a server environment like XAMPP, WAMP, or Docker.

1. **Clone the repository:**
```bash
git clone [https://github.com/alireza-zarebidoki/My-personal-projects.git](https://github.com/alireza-zarebidoki/My-personal-projects.git)

```


2. **Database Setup:**
* Create a MySQL database (e.g., `blog_db`).
* Import the provided SQL schema (users & posts tables).
* Insert an admin user with a **hashed password**.


3. **Configuration:**
* Open `blog/api/db_connect.php`.
* Update the credentials:
```php
$host = "localhost";
$db_name = "YOUR_DB_NAME";
$username = "YOUR_DB_USER";
$password = "YOUR_DB_PASSWORD";

```




4. **Permissions:**
* Ensure the `blog/api/sessions` folder exists and is writable by the server.


5. **Run:**
* Serve the project via Apache/Nginx and open `http://localhost/my-personal-site`.



---

## 📫 Contact Me

* **Email:** [alireza@zarebidoki.com](mailto:alireza@zarebidoki.com)
* **GitHub:** [@alireza-zarebidoki](https://github.com/alireza-zarebidoki)
* **Website:** [zarebidoki.com](https://zarebidoki.com)

---

*Developed with ❤️ by Alireza Zarebidoki*
