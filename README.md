🚀 Login System (C++ + HTML)

A simple yet functional login and authentication system built using C++ for backend processing and HTML for the frontend UI.
Users can create accounts, log in securely, and view how many users have logged in before them.


---

🔥 Features

🔐 Account Creation
Users can register using a username and password.

🧾 Login Authentication
Credentials are validated against stored user data.

📁 Persistent Storage
All usernames and passwords are stored in users.txt.

📊 Login Statistics
After login, users see how many others logged in before them.



---

📂 Project Structure

File	Description

oopsproject.html	Frontend UI for registration and login
server.cpp	Backend logic for registration, validation, and tracking
users.txt	Stores usernames and passwords (plain text)
server.exe / server.err	Compiled executable and error output
server.log	Log file for backend activity



---

🛠️ How It Works

1️⃣ User Registration

User enters a name and password.

Stored line-by-line in users.txt.


2️⃣ User Login

Input credentials are compared with stored data.

If correct → login successful.


3️⃣ Login Counter

System counts how many users have logged in previously.

Displays the number after successful login.



---

▶️ Running the Project

Step 1: Compile the backend

g++ server.cpp -o server.exe

Step 2: Run the server

./server.exe

Step 3: Open the frontend

Open oopsproject.html in your browser to interact with the login system.


---

⚠️ Notes

Passwords are stored in plain text for simplicity.
➝ Use hashing (bcrypt, SHA-256) for real applications.

Visit server.log to review backend activity and logs.



---

🌱 Future Improvements

Here are some enhancements planned for the next versions:

🔒 1. Password Encryption

Implement hashing and salting to securely store passwords.


🎨 2. Improved UI/UX

Add CSS styling or use frameworks like Bootstrap/Tailwind for a cleaner interface.


🗄️ 3. Shift to Database

Replace users.txt with SQLite/MySQL/PostgreSQL for better data management.


👤 4. Session Management

Maintain active user sessions after login (cookies or tokens).


📈 5. Detailed Activity Logs

Track login attempts, timestamps, and errors more clearly.


🔁 6. Multi-user Dashboard

Show all registered users, latest logins, and advanced stats.


📱 7. Responsive Frontend

Make the HTML page accessible on mobile and tablet devices.


🛡️ 8. Brute-force Protection

Block repeated wrong login attempts for security.


💬 9. Admin Panel (Optional)

Add an admin interface to manage users and view logs.



---

🤝 Contributing

Got ideas or improvements?
Fork the repo → Make changes → Submit a pull request.
All contributions are welcome!


---

📄 License

This project is for educational purposes.
License details will be added soon.
