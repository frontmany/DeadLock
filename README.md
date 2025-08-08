
# Deadlock Messenger

Deadlock Messenger is a secure, privacy-focused messaging app designed for users who prioritize confidentiality and data protection. It offers end-to-end encrypted communication (AES-256 + RSA-4096), no metadata collection, and anonymous usage (no phone number required). Key features include dark mode, local message deletion and file transfers.

*"Secure by default, private by design."* 

![User interface](/user_interface.png)


## Web Page

 - [Deadlock](https://frontmany.github.io/index.html)


## Tech Stack

*Boost Asio, CryptoPP, Qt, sqlite*
## Encryption

All messages, files, and metadata in Deadlock are end-to-end encrypted (E2EE) using AES-256 and RSA-4096 with alghoritsm provided by CryptoPP library. Even data stored locally on your device remains encrypted with a unique key, part of which is securely retrieved from the server only during login. This means no one, even someone with your computer can access your conversations without physically unlocking your account through the app. Your chats always stays private.

![lock picture](/secure-chat-dark.png)




## Deadlock's Internal Architecture: Asynchronous Messaging

At the core of Deadlock lies a high-performance, thread-safe infrastructure designed for real-time communication. Here’s how it works:

How deadlock process arriving messages:
![lock picture](/incomes.png)

How deadlock process outgoing messages:
![lock picture](/outgoing.png)



### 1. ASIO in a Dedicated Thread (Network Layer)
The networking stack is powered by Boost.ASIO, running in its own isolated thread.

Received messages are parsed and pushed into a thread-safe `safe_deque` (a mutex-protected queue) to decouple network processing from UI/background tasks.

---

### 2. Message Processing Pipeline
A separate consumer thread polls the `safe_deque` and processes messages.

Processed messages are forwarded to the Worker subsystem, which handles:
- Local database (SQLite)
- Notifications (via Qt’s signal-slot system)
- Business logic

---

### 3. Thread-Safe UI Updates with Qt
The WorkerUI interface emits signals for UI updates (e.g., new messages, status changes).

These signals are connected via `Qt::QueuedConnection`, ensuring cross-thread safety:

```cpp
QObject::connect(mainWindow,
                 "displayMessage",  
                  Qt::QueuedConnection
                 );  
