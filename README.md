# WhatsApp Console — C++ OOP Project

A feature-complete WhatsApp-style **console chat application** written in C++, demonstrating core OOP principles: **encapsulation, inheritance, and polymorphism**.

---

## Features

| Category | Details |
|---|---|
| **Auth** | Sign up, login, logout, change password |
| **Private Chat** | 1-on-1 messaging, re-opens existing chat automatically |
| **Group Chat** | Multi-member groups, admin roles, remove participants, set description, join requests |
| **Messages** | Send, reply (quoted), add emoji, delete own message, status tracking (Sent/Delivered/Read) |
| **Utilities** | Keyword search, export chat to `.txt` file |
| **Account** | Update status, phone number, password |

---

## Class Design

```
Chat  (base)
├── PrivateChat   – 1-on-1 with typing indicator
└── GroupChat     – multi-user with admin management

User              – credentials, status, last-seen
Message           – content, timestamp, status, reply chain
WhatsApp          – application controller / main menu
```

---

## Build & Run

**Requirements:** any C++11-compliant compiler (g++, clang++, MSVC).

```bash
# Compile
g++ -std=c++11 -Wall -o whatsapp whatsapp.cpp

# Run
./whatsapp
```

---

## Usage Walkthrough

```
1. Sign Up  → create an account
2. Login    → enter credentials
3. Private Chat / Create Group → start chatting
   Inside a chat:
     1. Send message
     2. Reply to message  (quoted)
     3. Add emoji         (:heart:, :thumbsup:, …)
     4. Delete my message
     5. Search messages   (keyword)
     6. Export chat       (saves to .txt)
4. Account Settings → update status / phone / password
5. Logout
```

---

## Project Structure

```
whatsapp.cpp   ← single-file implementation (all classes + main)
README.md
```

---

## OOP Concepts Demonstrated

- **Encapsulation** — private members with public getters/setters in `User` and `Message`
- **Inheritance** — `PrivateChat` and `GroupChat` both extend `Chat`
- **Polymorphism** — `displayChat()` is virtual; `WhatsApp` holds a `vector<Chat*>` and dispatches dynamically
- **Composition** — `Message` holds a `Message*` for reply chains; `WhatsApp` owns `Chat*` objects
- **RAII** — `WhatsApp` destructor deletes all heap-allocated `Chat` objects

---

## License

MIT — free to use, modify, and distribute.
