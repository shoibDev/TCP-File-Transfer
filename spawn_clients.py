import subprocess

# Configuration
client_program = "./sendFile"
server_address = "127.0.0.1:50555"
buf_size = "4096"
num_clients = 5
test_file = "smallTest.txt"

# Start multiple clients
for i in range(num_clients):
    print(f"Starting client {i + 1}")
    subprocess.Popen([client_program, test_file, server_address, buf_size])

