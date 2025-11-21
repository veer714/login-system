        const API_URL = 'http://localhost:8080';
        let serverOnline = false;

        // Check server status
        async function checkServer() {
            try {
                const response = await fetch(`${API_URL}/users`, {
                    method: 'GET',
                    signal: AbortSignal.timeout(3000)
                });
                serverOnline = response.ok;
            } catch(error) {
                serverOnline = false;
            }
            
            updateServerStatus();
        }

        function updateServerStatus() {
            const indicator = document.getElementById('statusIndicator');
            const text = document.getElementById('statusText');
            
            if(serverOnline) {
                indicator.className = 'status-indicator status-online';
                text.textContent = 'Server Online';
            } else {
                indicator.className = 'status-indicator status-offline';
                text.textContent = 'Server Offline';
            }
        }

        function showMessage(elementId, message, isSuccess) {
            const msgElement = document.getElementById(elementId);
            msgElement.textContent = message;
            msgElement.className = 'message ' + (isSuccess ? 'success' : 'error');
            msgElement.style.display = 'block';
            setTimeout(() => {
                msgElement.style.display = 'none';
            }, 5000);
        }

        function showLogin() {
            document.getElementById('loginForm').classList.remove('hidden');
            document.getElementById('registerForm').classList.add('hidden');
            document.getElementById('dashboard').classList.add('hidden');
        }

        function showRegister() {
            document.getElementById('loginForm').classList.add('hidden');
            document.getElementById('registerForm').classList.remove('hidden');
            document.getElementById('dashboard').classList.add('hidden');
        }

        function showDashboard(username) {
            document.getElementById('loginForm').classList.add('hidden');
            document.getElementById('registerForm').classList.add('hidden');
            document.getElementById('dashboard').classList.remove('hidden');
            document.getElementById('dashboardUsername').textContent = username;
            document.getElementById('userListContainer').style.display = 'none';
        }

        async function handleRegister(event) {
            event.preventDefault();
            
            const username = document.getElementById('registerUsername').value.trim();
            const password = document.getElementById('registerPassword').value;
            const confirmPassword = document.getElementById('confirmPassword').value;
            const btn = document.getElementById('registerBtn');
            
            if(password !== confirmPassword) {
                showMessage('registerMessage', 'Passwords do not match!', false);
                return;
            }
            
            btn.disabled = true;
            btn.textContent = 'Registering...';
            
            try {
                const response = await fetch(`${API_URL}/register`, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded',
                    },
                    body: `username=${encodeURIComponent(username)}&password=${encodeURIComponent(password)}`
                });
                
                const data = await response.text();
                const [status, message] = data.split('|');
                
                if(status === 'SUCCESS') {
                    showMessage('registerMessage', message, true);
                    document.getElementById('registerUsername').value = '';
                    document.getElementById('registerPassword').value = '';
                    document.getElementById('confirmPassword').value = '';
                    setTimeout(() => showLogin(), 2000);
                } else {
                    showMessage('registerMessage', message, false);
                }
            } catch(error) {
                showMessage('registerMessage', 'Connection error! Make sure C++ server is running on port 8080.', false);
                serverOnline = false;
                updateServerStatus();
            }
            
            btn.disabled = false;
            btn.textContent = 'Register';
        }

        async function handleLogin(event) {
            event.preventDefault();
            
            const username = document.getElementById('loginUsername').value.trim();
            const password = document.getElementById('loginPassword').value;
            const btn = document.getElementById('loginBtn');
            
            btn.disabled = true;
            btn.textContent = 'Logging in...';
            
            try {
                const response = await fetch(`${API_URL}/login`, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded',
                    },
                    body: `username=${encodeURIComponent(username)}&password=${encodeURIComponent(password)}`
                });
                
                const data = await response.text();
                const [status, message] = data.split('|');
                
                if(status === 'SUCCESS') {
                    showDashboard(message);
                } else {
                    showMessage('loginMessage', message, false);
                }
            } catch(error) {
                showMessage('loginMessage', 'Connection error! Make sure C++ server is running on port 8080.', false);
                serverOnline = false;
                updateServerStatus();
            }
            
            btn.disabled = false;
            btn.textContent = 'Login';
        }

        async function loadAllUsers() {
            try {
                const response = await fetch(`${API_URL}/users`);
                const data = await response.text();
                const [status, users] = data.split('|');
                
                if(status === 'SUCCESS') {
                    const userArray = users.split(',');
                    const userListDiv = document.getElementById('userList');
                    userListDiv.innerHTML = '';
                    
                    userArray.forEach((user, index) => {
                        const userItem = document.createElement('div');
                        userItem.className = 'user-item';
                        userItem.textContent = `${index + 1}. ${user}`;
                        userListDiv.appendChild(userItem);
                    });
                    
                    document.getElementById('userListContainer').style.display = 'block';
                } else {
                    alert('No users found!');
                }
            } catch(error) {
                alert('Error loading users!');
            }
        }

        function handleLogout() {
            showLogin();
            document.getElementById('loginUsername').value = '';
            document.getElementById('loginPassword').value = '';
        }

        // Check server status on load
        checkServer();
        setInterval(checkServer, 10000); // Check every 10 seconds
