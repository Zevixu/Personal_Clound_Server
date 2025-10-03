import React, { useState } from "react";

const API = import.meta.env.VITE_API_BASE || "http://localhost:8080";

export default function App() {
  const [files, setFiles] = useState([]);
  const [selected, setSelected] = useState(null);
  const [message, setMessage] = useState("");

  async function health() {
    const r = await fetch(`${API}/healthz`);
    setMessage(await r.text());
  }

  async function startUpload() {
    if (!selected) return;
    const meta = {
      name: selected.name,
      size: selected.size,
      contentType: selected.type || "application/octet-stream",
      path: "/",
      sha256: "TODO",
    };
    const r = await fetch(`${API}/api/uploads/start`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(meta),
    });
    const data = await r.json();
    // PUT to presigned URL
    const put = await fetch(data.presignedUrl, {
      method: "PUT",
      body: selected,
      headers: { "Content-Type": meta.contentType },
    });
    if (!put.ok) {
      alert("Upload failed");
      return;
    }
    const etag = put.headers.get("ETag") || "";
    await fetch(`${API}/api/uploads/complete`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        uploadId: data.uploadId,
        s3Key: data.s3Key,
        etag,
      }),
    });
    alert("Uploaded!");
  }

  async function listFiles() {
    const r = await fetch(`${API}/api/files`);
    const data = await r.json();
    setFiles(data.files || []);
  }

  return (
    <div
      style={{ maxWidth: 800, margin: "40px auto", fontFamily: "sans-serif" }}
    >
      <h1>Cloud Drive</h1>
      <button onClick={health}>Ping API</button>
      <span style={{ marginLeft: 12 }}>{message}</span>

      <div
        style={{
          marginTop: 24,
          padding: 12,
          border: "1px solid #ddd",
          borderRadius: 8,
        }}
      >
        <input
          type="file"
          onChange={(e) => setSelected(e.target.files?.[0] || null)}
        />
        <button
          onClick={startUpload}
          disabled={!selected}
          style={{ marginLeft: 8 }}
        >
          Upload
        </button>
      </div>

      <div style={{ marginTop: 24 }}>
        <button onClick={listFiles}>List Files</button>
        <ul>
          {files.map((f) => (
            <li key={f.id}>
              {f.name} — {f.size} bytes
            </li>
          ))}
        </ul>
      </div>
    </div>
  );
}
