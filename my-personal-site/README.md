# Personal Website & Custom Blog Engine 🌐

Welcome to the source code of my personal portfolio and custom-built blogging platform. This project is a demonstration of my Full-Stack development skills, moving beyond static sites to a dynamic, database-driven content management system.

**Live Demo:**
- 🏠 **Portfolio:** [zarebidoki.com](https://zarebidoki.com)
- 📝 **Blog:** [blog.zarebidoki.com](https://blog.zarebidoki.com)

---

## 🚀 Project Overview

This repository contains two main integrated modules:

1.  **Interactive Portfolio (Frontend):** A single-page digital CV featuring a simulated Linux terminal, particle effects, and a responsive design.
2.  **Blog Engine (Full-Stack):** A completely custom-built CMS (Content Management System) written in native PHP and MySQL, featuring an Admin Panel, RESTful API, and a moder```

```n dark-themed UI.

## 🛠️ Tech Stack

### Frontend
![HTML5](https://img.shields.io/badge/HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white)
![CSS3](https://img.shields.io/badge/CSS3-1572B6?style=for-the-badge&logo=css3&logoColor=white)
![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black)
* **Particles.js:** For the interactive neural network background.
* **FontAwesome:** For UI icons.
* **Vazirmatn Font:** For Persian typography.

### Backend & Database
![PHP](https://img.shields.io/badge/PHP-777BB4?style=for-the-badge&logo=php&logoColor=white)
![MySQL](https://img.shields.io/badge/MySQL-4479A1?style=for-the-badge&logo=mysql&logoColor=white)
![REST API](https://img.shields.io/badge/REST%20API-005571?style=for-the-badge&logo=fastapi&logoColor=white)
* **Native PHP:** Handling API requests (`GET`, `POST`) and database connections (PDO).
* **MySQL:** Storing posts and user credentials.

### Admin Tools
* **Quill.js:** Integrated rich text editor with a custom Dark Mode theme.

---

## ✨ Key Features

### 1. 🖥️ Interactive Web Terminal
Located on the main page, this terminal mimics a Linux shell environment.
* **Command History:** Remembers previous commands.
* **Typewriter Effect:** Animated intro text.
* **Supported Commands:**
    * `help`: List all commands.
    * `about`: Summary about me.
    * `skills`: List of technical skills.
    * `contact`: Show email address.
    * `blog`: Opens the blog in a new tab.
    * `date`: Show current system date/time.
    * `clear`: Clear the terminal screen.
    * `sudo`: Try it and see what happens! 😉

### 2. 📝 Custom Blog System
A lightweight and fast blogging platform.
* **Dynamic Content:** Posts are fetched asynchronously via API (`get_posts.php`).
* **Hashtag System:** Automatically detects hashtags (e.g., `#Coding`) in articles and creates searchable links.
* **Smart Formatting:** Custom `linkify` function in JS to handle text formatting and links automatically.

### 3. 🛡️ Admin Panel & CMS
A secure dashboard to manage content without touching the database code.
* **Authentication:** Secure login system.
* **Rich Text Editor:** Full WYSIWYG editor (Quill.js) customized for the dark theme.
* **CRUD Operations:**
    * **Create:** Publish new articles via `add_post.php`.
    * **Read:** View list of published posts.
    * **Update:** Edit existing posts with pre-filled forms via `update_post.php`.
    * **Delete:** Remove posts securely via `delete_post.php`.

---

## 📂 Project Structure

```text
my-personal-site/
├── index.html            # Main Portfolio Entry
├── script.js             # Terminal & UI Logic
├── style.css             # Global Styles
├── images/               # Assets
└── blog/                 # Blog Sub-system
    ├── index.html        # Blog Home (Post List)
    ├── article.html      # Single Post View
    ├── css/              # Blog specific styles
    ├── js/               # Blog frontend logic (API calls)
    ├── api/              # Backend (REST API)
    │   ├── db_connect.php    # Database Connection
    │   ├── get_posts.php     # Fetch posts (All, Single, or by Tag)
    │   ├── add_post.php      # Create new post
    │   ├── update_post.php   # Edit existing post
    │   ├── delete_post.php   # Delete post
    │   └── login.php         # Admin authentication
    └── admin/            # Admin Panel
        ├── index.html    # Dashboard UI
        ├── script.js     # Admin logic & Quill.js setup
        └── style.css     # Admin dark theme

```

---

## 💻 Installation & Setup

To run this project locally, you need a server environment (like XAMPP, WAMP, or Docker) since it uses PHP and MySQL.

1. **Clone the repository:**
```bash
git clone [https://github.com/alireza-zarebidoki/My-personal-projects.git](https://github.com/alireza-zarebidoki/My-personal-projects.git)

```


2. **Database Setup:**
* Create a new MySQL database (e.g., `blog_db`).
* Import the SQL schema to create `users` and `posts` tables.
* Insert an admin user into the `users` table.


3. **Configuration:**
* Open `blog/api/db_connect.php`.
* Update the credentials:
```php
$host = "localhost";
$db_name = "YOUR_DB_NAME";
$username = "YOUR_DB_USER";
$password = "YOUR_DB_PASSWORD";

```




4. **Run:**
* Move the project folder to your server's root directory (e.g., `htdocs` or `/var/www/html`).
* Open `http://localhost/my-personal-site` in your browser.



---

## 📫 Contact Me

* **Email:** [alireza@zarebidoki.com](mailto:alireza@zarebidoki.com)
* **GitHub:** [@alireza-zarebidoki](https://github.com/alireza-zarebidoki)
* **Website:** [zarebidoki.com](https://zarebidoki.com)

---

*Developed with ❤️ by Alireza Zarebidoki*
