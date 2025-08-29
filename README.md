

---

```markdown
# Multi-Threaded Web Server (C++ with Docker)

## Overview
This project is a custom multi-threaded web server implemented in C++, designed to serve static and dynamic content.  
The development environment runs entirely inside Docker containers, including the server and database (MySQL).

---

## Features
- C++ multi-threaded web server core
- Thread pool, job queue, buffering, and caching support
- MySQL database for dynamic site support
- Fully Dockerized environment
- Easy to extend and configure

---

## Project Structure
```

project-root/
│
├── docker-compose.yml       # Defines cpp-server & mysql services
├── docker/
│   ├── cpp-server/
│   │   └── Dockerfile       # C++ dev container build instructions
│   └── mysql/
│       └── custom.cnf       # MySQL config (optional)
├── mysql\_data/              # MySQL persistent storage (auto-created)
├── .gitignore
└── README.md

````

---

## Development Environment Setup

### **1. Clone the repository**
```bash
git clone <your-repo-url>
cd multi-threaded-webserver
````

### **2. Build and start containers**

```bash
docker compose build
docker compose up -d
```

### **3. Verify containers are running**

```bash
docker ps
```

You should see:

```
cpp-server   ...   Up
mysql        ...   Up
```

---

## Testing MySQL Connection from cpp-server

1. **Enter the cpp-server container**:

```bash
docker compose exec cpp-server bash
```

2. **Check MySQL CLI is installed**:

```bash
mysql --version
```

3. **Connect to MySQL service**:

```bash
mysql -h mysql -u webuser -pwebpass webserver_db
```

* Host: `mysql` (service name from `docker-compose.yml`)
* User: `webuser`
* Password: `webpass`
* Database: `webserver_db`

4. **Run a test query**:

```sql
SHOW TABLES;
```

If everything is set up correctly, you’ll see:

```
Empty set (0.00 sec)
```

(That’s fine — no tables yet.)

5. **Exit**:

```sql
exit;
```

---

## Restarting the Environment

```bash
docker compose down       # Stop all containers
docker compose up -d      # Start containers again
```

---

## Services

* **cpp-server** — C++ development container with MySQL client & headers installed
* **mysql** — MySQL database container (with persistent volume at `mysql_data/`)

---

## License

MIT License

```

---


You’re right—my bad. Your flow is:

1. run the Makefile → 2) run `./server` → 3) hit it with curl.
   Here’s the exact, minimal sequence:

2. Build

```bash
make clean && make
```

2. Run the server

```bash
./server
```

You should see:

```
✅ Server listening on http://localhost:8080
```

3. Test from another terminal

```bash
curl -i http://localhost:8080
```

You should get a `200 OK` with your HTML body. The server terminal will print the request.

Quick fixes if something blocks you:

* **Address already in use**: a previous server is running. Kill it:

  ```bash
  pkill -f ./server
  ```

  then run `./server` again.
* **No response**: make sure you’re curling the same machine the server runs on (and port `8080`).

Once you confirm this works, say **“Step 1 complete”** and I’ll plug in the queue (no behavior change yet—just routing requests through FIFO).


If you push this version to GitHub now, anyone cloning your repo can bring up the environment and confirm MySQL connectivity without asking you how.  

Do you want me to also include **a small connection test C++ program** in `/src` so that the first time you run the container, it can already demonstrate talking to MySQL? That could make verification even easier before we start Step 5.
```
