// ============================================
// BACKEND - C++ HTTP Server (server.cpp)
// ============================================
// Compile: g++ -o server server.cpp -lpthread
// Run: ./server
// Server will run on: http://localhost:8080

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <thread>
#include <algorithm>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using socket_t = SOCKET;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using socket_t = int;
#endif

using namespace std;

// Simple hash function for password
string hashPassword(const string& password) {
    string hashed = password;
    int key = 123;
    for(size_t i = 0; i < hashed.length(); i++) {
        hashed[i] = hashed[i] ^ key;
    }
    return hashed;
}

// URL decode function
string urlDecode(const string& str) {
    string result;
    for(size_t i = 0; i < str.length(); i++) {
        if(str[i] == '%' && i + 2 < str.length()) {
            int value;
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &value);
            result += static_cast<char>(value);
            i += 2;
        } else if(str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

// Parse POST data
string getPostValue(const string& data, const string& key) {
    size_t pos = data.find(key + "=");
    if(pos == string::npos) return "";
    
    pos += key.length() + 1;
    size_t end = data.find("&", pos);
    if(end == string::npos) end = data.length();
    
    return urlDecode(data.substr(pos, end - pos));
}

// Register user function
string registerUser(const string& username, const string& password) {
    if(username.empty() || password.empty()) {
        return "ERROR|Username and password required";
    }
    
    if(password.length() < 4) {
        return "ERROR|Password must be at least 4 characters";
    }
    
    // Check if user exists
    ifstream checkFile("users.txt");
    string line, existingUser;
    while(getline(checkFile, line)) {
        size_t pos = line.find(':');
        if(pos != string::npos) {
            existingUser = line.substr(0, pos);
            if(existingUser == username) {
                checkFile.close();
                return "ERROR|Username already exists";
            }
        }
    }
    checkFile.close();
    
    // Save user
    ofstream file("users.txt", ios::app);
    if(!file) {
        return "ERROR|Server error: Could not save data";
    }
    
    file << username << ":" << hashPassword(password) << endl;
    file.close();
    
    cout << "[INFO] New user registered: " << username << endl;
    return "SUCCESS|Registration successful";
}

// Login user function
string loginUser(const string& username, const string& password) {
    if(username.empty() || password.empty()) {
        return "ERROR|Username and password required";
    }
    
    ifstream file("users.txt");
    if(!file) {
        return "ERROR|No users registered yet";
    }
    
    string line, storedUser, storedPass;
    
    while(getline(file, line)) {
        size_t pos = line.find(':');
        if(pos != string::npos) {
            storedUser = line.substr(0, pos);
            storedPass = line.substr(pos + 1);
            
            if(storedUser == username && storedPass == hashPassword(password)) {
                file.close();
                cout << "[INFO] User logged in: " << username << endl;
                return "SUCCESS|" + username;
            }
        }
    }
    
    file.close();
    return "ERROR|Invalid username or password";
}

// Get all users
string getAllUsers() {
    ifstream file("users.txt");
    if(!file) {
        return "ERROR|No users found";
    }
    
    string result = "SUCCESS|";
    string line, username;
    int count = 0;
    
    while(getline(file, line)) {
        size_t pos = line.find(':');
        if(pos != string::npos) {
            username = line.substr(0, pos);
            if(count > 0) result += ",";
            result += username;
            count++;
        }
    }
    
    file.close();
    
    if(count == 0) {
        return "ERROR|No users found";
    }
    
    return result;
}

// Handle client request
void handleClient(socket_t clientSocket) {
    char buffer[4096] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if(bytesRead <= 0) {
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
        return;
    }
    
    string request(buffer);
    string response;
    string corsHeaders = "Access-Control-Allow-Origin: *\r\n"
                        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                        "Access-Control-Allow-Headers: Content-Type\r\n";
    
    // Handle OPTIONS (CORS preflight)
    if(request.find("OPTIONS") == 0) {
        response = "HTTP/1.1 200 OK\r\n" + corsHeaders + "\r\n";
        send(clientSocket, response.c_str(), (int)response.length(), 0);
    #ifdef _WIN32
        closesocket(clientSocket);
    #else
        close(clientSocket);
    #endif
        return;
    }
    
    // Get request body
    size_t bodyPos = request.find("\r\n\r\n");
    string body = bodyPos != string::npos ? request.substr(bodyPos + 4) : "";
    
    string result;
    
    // Route handling
    if(request.find("POST /register") == 0) {
        string username = getPostValue(body, "username");
        string password = getPostValue(body, "password");
        result = registerUser(username, password);
    }
    else if(request.find("POST /login") == 0) {
        string username = getPostValue(body, "username");
        string password = getPostValue(body, "password");
        result = loginUser(username, password);
    }
    else if(request.find("GET /users") == 0) {
        result = getAllUsers();
    }
    else {
        result = "ERROR|Invalid endpoint";
    }
    
    // Send response
    response = "HTTP/1.1 200 OK\r\n" + corsHeaders + 
               "Content-Type: text/plain\r\n" +
               "Content-Length: " + to_string(result.length()) + "\r\n\r\n" +
               result;
    
    send(clientSocket, response.c_str(), (int)response.length(), 0);
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }
#endif

    socket_t serverSocket = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if(serverSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket" << endl;
        WSACleanup();
        return 1;
    }
#else
    if(serverSocket == -1) {
        cerr << "Failed to create socket" << endl;
        return 1;
    }
#endif

    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Bind failed! Port 8080 might be in use." << endl;
#ifdef _WIN32
        closesocket(serverSocket);
        WSACleanup();
#endif
        return 1;
    }

    if(listen(serverSocket, 10) < 0) {
        cerr << "Listen failed" << endl;
#ifdef _WIN32
        closesocket(serverSocket);
        WSACleanup();
#endif
        return 1;
    }

    cout << "========================================" << endl;
    cout << "    C++ AUTHENTICATION SERVER" << endl;
    cout << "========================================" << endl;
    cout << "\u2713 Server running on http://localhost:8080" << endl;
    cout << "\u2713 Data stored in: users.txt" << endl;
    cout << "\u2713 Ready to accept connections..." << endl;
    cout << "\u2713 Press Ctrl+C to stop" << endl;
    cout << "========================================\n" << endl;

    while(true) {
        sockaddr_in clientAddr;
#ifdef _WIN32
        int clientLen = sizeof(clientAddr);
#else
        socklen_t clientLen = sizeof(clientAddr);
#endif
        socket_t clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

#ifdef _WIN32
        if(clientSocket == INVALID_SOCKET) {
            cerr << "Accept failed" << endl;
            continue;
        }
#else
        if(clientSocket < 0) {
            cerr << "Accept failed" << endl;
            continue;
        }
#endif

        // Handle client synchronously (fallback if std::thread is unavailable)
        handleClient(clientSocket);
    }

#ifdef _WIN32
    closesocket(serverSocket);
    WSACleanup();
#else
    close(serverSocket);
#endif
    return 0;
}

