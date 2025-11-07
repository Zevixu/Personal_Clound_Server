# ☁️ Personal Cloud File Server

A full-stack cloud storage web application that lets users upload, list, and delete files — similar to Google Drive — built with **React**, **C++ (Drogon)**, **PostgreSQL**, and **MinIO/AWS S3**.  
It’s fully containerized using **Docker Compose** and ready to be deployed on **AWS EC2**.

---

## 🚀 Features

- 📤 **File Uploads**
  - Supports **server-proxy uploads**  
  - Real-time upload progress tracking in the React UI  
- 📄 **File Listing & Metadata**
  - Displays file name, upload date, file size and download/delete action buttons  
- 🗑️ **File Deletion**
  - Removes objects from S3/MinIO and hard-deletes metadata in PostgreSQL  
- 🧩 **Modular Architecture**
  - C++ backend built with the **Drogon** web framework  
  - File metadata stored in **PostgreSQL**  
  - Object storage provided by **MinIO** (S3-compatible, interchangeable with AWS S3)  
- 🐳 **Dockerized Setup**
  - One-command startup via Docker Compose (frontend, backend, database, MinIO)  
- ☁️ **Cloud-ready**
  - Tested and deployable on **AWS EC2**

---

## 🏗️ Tech Stack

| Layer | Technology |
|:------|:------------|
| **Frontend** | React.js + Vite Dev Server + Material UI |
| **Backend** | Drogon (C++ 20) |
| **Database** | PostgreSQL 16 |
| **Object Storage** | MinIO (S3-compatible) / AWS S3 |
| **Infrastructure** | Docker Compose / AWS EC2 |
| **Language Standards** | Modern C++ 20 |

---

## 🧩 Architecture Overview

```
+-------------+        HTTP          +-----------------+
|  Frontend   |  <-----------------> |     Nginx /     |
| React / Vite|                      | Drogon REST API |
+-------------+                      +-----------------+
       |                                         |
       |               DB (metadata)             |
       +---------------> PostgreSQL              |
       |                                         |
       |             Object storage              |
       +---------------> MinIO / AWS S3          |
```

Each file’s metadata (name, path, size, hash, content type, ETag, timestamps) is saved in PostgreSQL; file content itself is stored in S3/MinIO.

---

## ⚙️ Getting Started(Steps for local testing)

### 1️⃣ Clone the repository
```bash
git clone https://github.com/<your-username>/personal-cloud-file-server.git
cd personal-cloud-file-server
```

### 2️⃣ Build and run
```bash
docker compose up -d --build
```

This starts:
- **frontend** → React (Vite) on `http://localhost:5173`  
- **backend** → Drogon API on `http://localhost:8080`  
- **db** → PostgreSQL on port 5432  
- **minio** → Object storage server on `http://localhost:9000`

---

## 🧠 API Endpoints

| Method | Endpoint | Description |
|:-------|:----------|:-------------|
| `POST` | `/api/uploads` | Upload a file |
| `GET`  | `/api/files` | List all files |
| `DELETE` | `/api/files/:id` | Delete a file |
| `Download` | `/api/files/:id/download` | Download a file |
| `GET`  | `/api/health` *(optional)* | Health check |

Each response contains JSON metadata including file ID, path, size, ETag, and timestamps.

---

## 🗃️ Database Schema

```sql
CREATE TABLE files (
  id            UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  owner_id      UUID NULL,
  name          TEXT NOT NULL,
  path          TEXT NOT NULL DEFAULT '/',
  size          BIGINT NOT NULL,
  sha256        TEXT NOT NULL,
  s3_key        TEXT NOT NULL,
  content_type  TEXT NOT NULL,
  etag          TEXT NOT NULL,
  created_at    TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  updated_at    TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  deleted_at    TIMESTAMPTZ NULL
);
```

---

## 🧪 Development Notes

- Backend organized into **Drogon plugins**:  
  - `RepoPlugin` – handles DB migrations and metadata queries  
  - `S3Plugin` – handles S3/MinIO interaction (upload + delete + download)  
- Frontend communicates with the backend using a configurable `API` base URL (proxy via Vite during local dev).
- Uses **CORS middleware** for `http://localhost:5173` origin.

---

## 🧰 Useful Commands

```bash
# Rebuild all services
docker compose up -d --build

# Stop and remove all containers
docker compose down
```
---

## ☁️ Next Step: Deployment (AWS EC2)

1. Create an EC2 instance (Ubuntu 24.04 LTS recommended).  
2. Install Docker + Docker Compose.  
3. Copy project files to `/opt/cloud-drive`.  
4. Run:
   ```bash
   docker compose up -d --build
   ```
5. Configure EC2 security group to allow ports `80`/`443`.  
6. Place **Nginx** or **Caddy** in front for SSL and reverse proxy.

---

## 📄 Future Improvements

- 🔐 User authentication & access control  
- 🧾 Versioning & sharing links  
- 📦 Multi-part upload progress visualization  
- 🌐 HTTPS / TLS setup (Nginx + Let’s Encrypt)  
- 🧠 Background cleanup job for orphaned S3 objects  

---

## 🧑‍💻 Author

**Yi Xu**  
Embedded Linux / Full Stack Engineer  
[LinkedIn](https://www.linkedin.com/in/yi-xu-129a0622b/)
