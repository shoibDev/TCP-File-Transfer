# TCP File‑Transfer (Client / Server)

A minimal TCP client–server pair for uploading text (or any binary) files.
The client streams a file to the server; the server writes it to disk and, if a
duplicate name exists, appends `(2)`, `(3)`, … before the extension.

---

## 1. Protocol flow

```text
Client                         TCP stream                              Server
------                                                                 -------
1. connect  -------------------------------------------------------->  accept
2. send 2‑byte length (N) of file name ----------------------------->  recv N
3. send file‑name bytes ------------------------------------------->  recv name
4. loop:
     read ≤ bufSize from disk
     send chunk ---------------------------------------------------->  recv & fwrite
   (repeat until EOF, then close)                                     fclose / close
```

---

## 2. Directory layout

```text
/project-root
├── TCPserver.c
├── TCPclient.c
├── Makefile
├── spawn_clients.py   # optional stress‑test helper
└── smallTest.txt      # sample upload
```

---

## 3. Requirements

* GCC or Clang (C11)
* Python ≥ 3.6 (only for `spawn_clients.py`)
* POSIX‑like shell (Linux, macOS, WSL, etc.)

---

## 4. Build

```bash
make            # or: make all
```

Produces two executables:

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

* `port` – TCP port to listen on (e.g., `50555`)
* `bufSize` – chunk size for `recv()` in bytes (default **4096**)

Example:

```bash
./server 50555 4096
```

Server output:

```text
Server listening on port 50555 with buffer size 4096...
```

Leave this terminal open.

---

## 6. Run the client

```bash
./sendFile <fileName> <IP:port> [bufSize]
```

* `fileName` – path to the file to send
* `IP:port` – destination in `IP:port` form (e.g., `127.0.0.1:50555`)
* `bufSize` – (optional) chunk size for `send()`; defaults to **4096**

Example (same host):

```bash
./sendFile smallTest.txt 127.0.0.1:50555 4096
```

The server logs the transfer and writes the file in its own directory.
Sending the same name again produces `smallTest(2).txt`, `smallTest(3).txt`, etc.

---

## 7. Spawn multiple clients (optional)

`spawn_clients.py` fires off *N* parallel client processes for stress‑testing.

Edit the variables at the top of the script:

```python
client_program = "./sendFile"
server_address = "127.0.0.1:50555"
buf_size       = "4096"
num_clients    = 5
test_file      = "smallTest.txt"
```

Run:

```bash
python3 spawn_clients.py
```

---

## 8. Troubleshooting

| Message / Symptom              | Possible Fix                                                     |
| ------------------------------ | ---------------------------------------------------------------- |
| `bind: Address already in use` | Another process on that port; choose a new port or kill the old. |
| `connect: Connection refused`  | Server isn’t running or wrong IP/port supplied.                  |
| Server writes a 0‑byte file    | Client closed early or sent an empty file.                       |
| Server never exits             | Designed to run indefinitely; press **Ctrl‑C** to stop.          |

---
