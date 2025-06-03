Below is an updated **README.md** you can drop straight into the project root.

```markdown
# TCP File-Transfer (Client / Server)

A minimal TCP client-server pair for uploading text (or any binary) files.  
The client streams a file to the server; the server writes it to disk and, if
a duplicate name exists, appends `(2)`, `(3)`, … before the extension.

---

## 1. How the protocol flows

```

Client                   TCP stream                    Server

---

1. connect -----------------------------------------> 2. accept
2. send 2-byte length (N) of file name --------------> 4. recv N
3. send file-name bytes -----------------------------> 6. recv file name
4. loop:
   read ≤ bufSize from disk
   send chunk ----------------------------------> 8. recv & fwrite
   (repeat until EOF, then close)                      9. fclose / close

```

---

## 2. Directory layout

```

/project-root
├── TCPserver.c
├── TCPclient.c
├── Makefile
├── spawn\_clients.py   # optional stress-test helper
└── smallTest.txt      # sample upload

````

---

## 3. Requirements

* GCC or Clang (C11)
* Python ≥ 3.6 (only for `spawn_clients.py`)
* POSIX-like shell (Linux, macOS, WSL, etc.)

---

## 4. Build

```bash
make            # or: make all
````

Creates two executables:

| Target     | Description           |
| ---------- | --------------------- |
| `server`   | TCP receiver / writer |
| `sendFile` | TCP sender / uploader |

Clean up build artifacts:

```bash
make clean
```

---

## 5. Run the server

```bash
./server <port> <bufSize>
```

* `port`      TCP port to listen on (e.g., `50555`)
* `bufSize`   Chunk size for `recv()` in bytes (default 4096)

Example:

```bash
./server 50555 4096
```

Output:

```
Server listening on port 50555 with buffer size 4096...
```

Leave this terminal open.

---

## 6. Run the client

```bash
./sendFile <fileName> <IP:port> [bufSize]
```

* `fileName`  Path to the file to send
* `IP:port`   Destination in `IP:port` form — e.g., `127.0.0.1:50555`
* `bufSize`   (Optional) chunk size for `send()`; defaults to 4096

Example (same host):

```bash
./sendFile smallTest.txt 127.0.0.1:50555 4096
```

The server will log the transfer and write the file in its own directory.

Uploading an existing name again produces `smallTest(2).txt`, `smallTest(3).txt`, etc.

---

## 7. Spawn multiple clients (optional)

`spawn_clients.py` fires off N parallel client processes for stress-testing.

Edit the variables at the top of the script:

```python
client_program = "./sendFile"
server_address = "127.0.0.1:50555"
buf_size       = "4096"
num_clients    = 5
test_file      = "smallTest.txt"
```

Then run:

```bash
python3 spawn_clients.py
```

---

## 8. Troubleshooting

| Message / Symptom              | Possible Fix                                                     |
| ------------------------------ | ---------------------------------------------------------------- |
| `bind: Address already in use` | Another process on that port; choose a new port or kill the old. |
| `connect: Connection refused`  | Server isn’t running or wrong IP/port supplied.                  |
| Server writes a 0-byte file    | Client closed early or sent an empty file.                       |
| Server never exits             | Designed to run indefinitely; press **Ctrl-C** to stop.          |

---

### Next steps (ideas)

* Spawn a thread or `fork()` per client for simultaneous transfers.
* Send an SHA-256 checksum after the payload and verify on the server.
* Add a progress bar on the client side.
* Replace the IPv4-only parsing with `getaddrinfo()` for IPv6/DNS support.

```

Feel free to tweak headings or formatting, but the core instructions, diagram, and extra “school-project” wording have been aligned to your new requirements.
```
