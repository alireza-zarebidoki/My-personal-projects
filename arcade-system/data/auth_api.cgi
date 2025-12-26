#!/usr/bin/env python3
# coding: utf-8
"""
File-based auth/coin backend with separated admin & user stores.

Data files (JSON):
- admins.json -> {"admins": [{username, passwordHash, role="admin", coins}]}
- users.json  -> {"users": [{username, passwordHash, role="user", coins}]}
- sessions.json -> {token: {username, role, createdAt}}
- admin_logs.json -> list of log entries
"""
import json
import os
import sys
import hashlib
import secrets
import time
from http import HTTPStatus
from urllib.parse import parse_qs

BASE_DIR = os.path.dirname(__file__)
DATA_DIR = os.path.abspath(os.path.join(BASE_DIR, "..", "data"))
LEGACY_DIR = BASE_DIR  # previous storage location
ADMINS_PATH = os.path.join(DATA_DIR, "admins.json")
USERS_PATH = os.path.join(DATA_DIR, "users.json")
SESSIONS_PATH = os.path.join(DATA_DIR, "sessions.json")
LOGS_PATH = os.path.join(DATA_DIR, "admin_logs.json")
STARTING_COINS = 100
DEFAULT_ADMIN_USER = "admin"
DEFAULT_ADMIN_PASS = "admin123"
DEFAULT_ADMIN_COINS = 9999


def read_body():
    length = int(os.environ.get("CONTENT_LENGTH") or 0)
    if length <= 0:
        return b""
    return sys.stdin.buffer.read(length)


def load_json(path, default):
    try:
        with open(path, "r", encoding="utf-8") as f:
            return json.load(f)
    except Exception:
        return default


def save_json(path, data):
    with open(path, "w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False, indent=2)


def sha256(value: str) -> str:
    return hashlib.sha256(value.encode("utf-8")).hexdigest()


def ensure_data_dir():
    os.makedirs(DATA_DIR, exist_ok=True)


def migrate_legacy_file(name):
    legacy_path = os.path.join(LEGACY_DIR, name)
    new_path = os.path.join(DATA_DIR, name)
    if os.path.exists(new_path) or not os.path.exists(legacy_path):
        return
    try:
        data = load_json(legacy_path, None)
        if data is not None:
            save_json(new_path, data)
    except Exception:
        pass


def legacy_admin_to_list():
    legacy_path = os.path.join(LEGACY_DIR, "admin.json")
    data = load_json(legacy_path, None)
    if isinstance(data, dict) and data:
        return [
            {
                "username": data.get("username", DEFAULT_ADMIN_USER),
                "passwordHash": data.get("passwordHash") or data.get("password") or sha256(DEFAULT_ADMIN_PASS),
                "role": "admin",
                "coins": int(data.get("coins") or DEFAULT_ADMIN_COINS),
            }
        ]
    return []


def load_records(path, key):
    data = load_json(path, None)
    if isinstance(data, dict) and isinstance(data.get(key), list):
        return data.get(key) or []
    if isinstance(data, list):
        return data
    return []


def save_records(path, key, items):
    save_json(path, {key: items})


def ensure_files():
    ensure_data_dir()
    migrate_legacy_file("users.json")
    migrate_legacy_file("sessions.json")
    migrate_legacy_file("admin_logs.json")
    if not os.path.exists(ADMINS_PATH):
        migrated = legacy_admin_to_list()
        admins = migrated or [default_admin()]
        save_records(ADMINS_PATH, "admins", admins)
    if not os.path.exists(USERS_PATH):
        save_records(USERS_PATH, "users", [])
    if not os.path.exists(SESSIONS_PATH):
        save_json(SESSIONS_PATH, {})
    if not os.path.exists(LOGS_PATH):
        save_json(LOGS_PATH, [])


def respond(payload, status=HTTPStatus.OK):
    print(f"Status: {status.value} {status.phrase}")
    print("Content-Type: application/json; charset=utf-8")
    print()
    print(json.dumps(payload, ensure_ascii=False))


def error(message, status=HTTPStatus.BAD_REQUEST):
    respond({"ok": False, "error": message}, status)


def parse_json_body():
    try:
        return json.loads(read_body() or b"{}")
    except Exception:
        return {}


def default_admin():
    return {
        "username": DEFAULT_ADMIN_USER,
        "passwordHash": sha256(DEFAULT_ADMIN_PASS),
        "role": "admin",
        "coins": DEFAULT_ADMIN_COINS
    }


def load_admins():
    admins = load_records(ADMINS_PATH, "admins")
    if not isinstance(admins, list):
        admins = []
    if not admins:
        admins = [default_admin()]
        save_records(ADMINS_PATH, "admins", admins)
    return admins


def load_users():
    users = load_records(USERS_PATH, "users")
    if not isinstance(users, list):
        users = []
    return users


def load_state():
    admins = load_admins()
    users = load_users()
    sessions = load_json(SESSIONS_PATH, {}) or {}
    logs = load_json(LOGS_PATH, []) or []
    return admins, users, sessions, logs


def save_state(admins=None, users=None, sessions=None, logs=None):
    if admins is not None:
        save_records(ADMINS_PATH, "admins", admins)
    if users is not None:
        save_records(USERS_PATH, "users", users)
    if sessions is not None:
        save_json(SESSIONS_PATH, sessions)
    if logs is not None:
        save_json(LOGS_PATH, logs)


def create_session(username, role, sessions):
    token = secrets.token_hex(16)
    sessions[token] = {
        "username": username,
        "role": role,
        "createdAt": int(time.time())
    }
    save_state(sessions=sessions)
    return token


def update_sessions_role(sessions, username, role):
    changed = False
    for token, sess in sessions.items():
        if sess.get("username", "").lower() == username.lower():
            sess["role"] = role
            changed = True
    if changed:
        save_state(sessions=sessions)


def find_user(users, username):
    for u in users:
        if u.get("username", "").lower() == username.lower():
            return u
    return None


def find_admin(admins, username):
    for a in admins:
        if a.get("username", "").lower() == username.lower():
            return a
    return None


def get_password_hash(record):
    return record.get("passwordHash") or record.get("password") or ""


def require_session(sessions, token):
    if not token:
        return None
    return sessions.get(token)


def append_log(logs, entry):
    logs.insert(0, entry)
    logs[:] = logs[:200]
    save_state(logs=logs)


def handle_register(body, admins, users, sessions, logs):
    username = (body.get("username") or "").strip()
    password = body.get("password") or ""
    if len(username) < 3 or len(password) < 6:
        return error("نام کاربری یا رمز عبور نامعتبر است.")
    if find_user(users, username) or find_admin(admins, username):
        return error("این نام کاربری قبلاً ثبت شده است.")
    record = {
        "username": username,
        "passwordHash": sha256(password),
        "role": "user",
        "coins": STARTING_COINS,
        "createdAt": int(time.time())
    }
    users.append(record)
    save_state(users=users)
    token = create_session(username, "user", sessions)
    respond({"ok": True, "session": {"username": username, "role": "user", "coins": record["coins"], "token": token}})


def handle_login(body, admins, users, sessions, logs):
    username = (body.get("username") or "").strip()
    password = body.get("password") or ""
    hashed = sha256(password)

    admin = find_admin(admins, username)
    if admin and hashed == get_password_hash(admin):
        token = create_session(admin["username"], "admin", sessions)
        return respond({"ok": True, "session": {"username": admin["username"], "role": "admin", "coins": admin.get("coins", 0), "token": token}})

    user = find_user(users, username)
    if not user or get_password_hash(user) != hashed:
        return error("کاربر یا رمز عبور نادرست است.", HTTPStatus.UNAUTHORIZED)

    token = create_session(user["username"], user.get("role", "user"), sessions)
    respond({"ok": True, "session": {"username": user["username"], "role": user.get("role", "user"), "coins": user.get("coins", 0), "token": token}})


def handle_session(body, admins, users, sessions, logs):
    token = body.get("token") or (body.get("sessionToken"))
    sess = require_session(sessions, token)
    if not sess:
        return error("سشن نامعتبر است.", HTTPStatus.UNAUTHORIZED)
    user = find_user(users, sess["username"]) or find_admin(admins, sess["username"])
    coins = user.get("coins", 0) if user else 0
    sess["role"] = "admin" if find_admin(admins, sess["username"]) else "user"
    save_state(sessions=sessions)
    respond({"ok": True, "session": {"username": sess["username"], "role": sess["role"], "coins": coins, "token": token}})


def handle_logout(body, admin, users, sessions, logs):
    token = body.get("token")
    if token and token in sessions:
        sessions.pop(token, None)
        save_state(sessions=sessions)
    respond({"ok": True})


def ensure_user_session(body, admins, users, sessions):
    token = body.get("token")
    sess = require_session(sessions, token)
    if not sess:
        error("سشن نامعتبر است.", HTTPStatus.UNAUTHORIZED)
        return None, None
    user = find_user(users, sess["username"]) or find_admin(admins, sess["username"])
    if not user:
        error("کاربر یافت نشد.", HTTPStatus.UNAUTHORIZED)
        return None, None
    return sess, user


def handle_subtract_coin(body, admins, users, sessions, logs):
    sess, user = ensure_user_session(body, admins, users, sessions)
    if not sess:
        return
    delta = max(0, int(body.get("amount") or 0))
    balance = max(0, int(user.get("coins") or 0) - delta)
    user["coins"] = balance
    if sess.get("role") == "admin":
        save_state(admins=admins)
    else:
        save_state(users=users)
    respond({"ok": True, "coins": balance})


def handle_add_coin(body, admins, users, sessions, logs):
    sess, user = ensure_user_session(body, admins, users, sessions)
    if not sess:
        return
    delta = max(0, int(body.get("amount") or 0))
    user["coins"] = int(user.get("coins") or 0) + delta
    if sess.get("role") == "admin":
        save_state(admins=admins)
    else:
        save_state(users=users)
    respond({"ok": True, "coins": user["coins"]})


def handle_reward_win(body, admins, users, sessions, logs):
    sess, user = ensure_user_session(body, admins, users, sessions)
    if not sess:
        return
    base_price = max(0, int(body.get("basePrice") or 0))
    reward_rate = 0.2
    reward = int(base_price * reward_rate)
    if reward > 0:
        user["coins"] = int(user.get("coins") or 0) + reward
        if sess.get("role") == "admin":
            save_state(admins=admins)
        else:
            save_state(users=users)
    respond({"ok": True, "coins": user.get("coins", 0), "reward": reward})


def handle_admin_add_coins(body, admins, users, sessions, logs):
    token = body.get("token")
    sess = require_session(sessions, token)
    if not sess or sess.get("role") != "admin":
        return error("دسترسی ادمین ندارید.", HTTPStatus.FORBIDDEN)
    target = body.get("username") or ""
    amount = int(body.get("amount") or 0)
    if amount <= 0:
        return error("مقدار نامعتبر است.")
    user = find_user(users, target)
    container = "users"
    if not user:
        user = find_admin(admins, target)
        container = "admins" if user else None
    if not user:
        return error("کاربر یافت نشد.")
    user["coins"] = int(user.get("coins") or 0) + amount
    if container == "admins":
        save_state(admins=admins)
    else:
        save_state(users=users)
    append_log(logs, {"ts": int(time.time()), "admin": sess["username"], "target": target, "delta": amount, "coinsAfter": user["coins"]})
    respond({"ok": True, "coins": user["coins"]})


def handle_list_users(body, admins, users, sessions, logs):
    token = body.get("token")
    sess = require_session(sessions, token)
    if not sess or sess.get("role") != "admin":
        return error("دسترسی ادمین ندارید.", HTTPStatus.FORBIDDEN)
    payload = []
    for u in admins:
        payload.append({"username": u.get("username"), "role": "admin", "coins": u.get("coins", 0), "createdAt": u.get("createdAt", 0)})
    for u in users:
        payload.append({"username": u.get("username"), "role": "user", "coins": u.get("coins", 0), "createdAt": u.get("createdAt", 0)})
    respond({"ok": True, "users": payload})


def handle_admin_change_role(body, admins, users, sessions, logs):
    token = body.get("token")
    sess = require_session(sessions, token)
    if not sess or sess.get("role") != "admin":
        return error("دسترسی ادمین ندارید.", HTTPStatus.FORBIDDEN)

    target = (body.get("username") or "").strip()
    new_role = (body.get("role") or "user").lower()
    if new_role not in ("user", "admin"):
        return error("نقش نامعتبر است.")

    existing_admin = find_admin(admins, target)
    existing_user = find_user(users, target)

    if new_role == "admin":
        if existing_admin:
            return respond({"ok": True, "user": {"username": existing_admin["username"], "role": "admin", "coins": existing_admin.get("coins", 0)}})
        if not existing_user:
            return error("کاربر یافت نشد.")
        promoted = {
            "username": existing_user.get("username"),
            "passwordHash": get_password_hash(existing_user),
            "role": "admin",
            "coins": int(existing_user.get("coins", 0))
        }
        admins.append(promoted)
        users[:] = [u for u in users if u.get("username", "").lower() != target.lower()]
    else:  # new_role == 'user'
        if not existing_admin:
            return error("کاربر یافت نشد.")
        if existing_admin.get("username", "").lower() == DEFAULT_ADMIN_USER.lower():
            return error("ادمین اصلی قابل تغییر نیست.")
        demoted = {
            "username": existing_admin.get("username"),
            "passwordHash": get_password_hash(existing_admin),
            "role": "user",
            "coins": int(existing_admin.get("coins", 0))
        }
        users.append(demoted)
        admins[:] = [a for a in admins if a.get("username", "").lower() != target.lower()]

    save_state(admins=admins, users=users)
    update_sessions_role(sessions, target, new_role)
    append_log(logs, {"ts": int(time.time()), "admin": sess["username"], "target": target, "action": "role_change", "role": new_role})
    respond({"ok": True, "user": {"username": target, "role": new_role, "coins": (find_user(users, target) or find_admin(admins, target)).get("coins", 0)}})


def handle_logs(body, admins, users, sessions, logs):
    token = body.get("token")
    sess = require_session(sessions, token)
    if not sess or sess.get("role") != "admin":
        return error("دسترسی ادمین ندارید.", HTTPStatus.FORBIDDEN)
    respond({"ok": True, "logs": logs})


def main():
    ensure_files()
    admins, users, sessions, logs = load_state()
    qs = parse_qs(os.environ.get("QUERY_STRING", ""))
    action = (qs.get("action") or [None])[0]
    body = parse_json_body()

    if action == "register":
        return handle_register(body, admins, users, sessions, logs)
    if action == "login":
        return handle_login(body, admins, users, sessions, logs)
    if action == "session":
        return handle_session(body, admins, users, sessions, logs)
    if action == "logout":
        return handle_logout(body, admins, users, sessions, logs)
    if action == "add_coin":
        return handle_add_coin(body, admins, users, sessions, logs)
    if action == "subtract_coin":
        return handle_subtract_coin(body, admins, users, sessions, logs)
    if action == "reward_win":
        return handle_reward_win(body, admins, users, sessions, logs)
    if action == "admin_add_coins":
        return handle_admin_add_coins(body, admins, users, sessions, logs)
    if action == "admin_change_role":
        return handle_admin_change_role(body, admins, users, sessions, logs)
    if action == "list_users":
        return handle_list_users(body, admins, users, sessions, logs)
    if action == "logs":
        return handle_logs(body, admins, users, sessions, logs)

    error("action نامعتبر است.", HTTPStatus.NOT_FOUND)


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:  # fail-safe error reporting
        respond({"ok": False, "error": f"server error: {exc}"}, HTTPStatus.INTERNAL_SERVER_ERROR)
