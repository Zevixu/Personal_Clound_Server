import React, { useState } from "react";

const API = import.meta.env.VITE_API_BASE || "http://localhost:8080";

export default function App() {
  const [files, setFiles] = useState([]);
  const [selectedFile, setSelectedFile] = useState(null);
  const [message, setMessage] = useState("");

  console.log(selectedFile);

  async function health() {
    const r = await fetch(`${API}/healthz`);
    setMessage(await r.text());
  }

  async function startUpload() {
    if (!selectedFile) return;
    // const meta = {
    //   name: selectedFile.name,
    //   size: selectedFile.size,
    //   contentType: selectedFile.type || "application/octet-stream",
    //   path: "/",
    //   sha256: "TODO",
    // };
    // const r = await fetch(`${API}/api/uploads/start`, {
    //   method: "POST",
    //   headers: { "Content-Type": "application/json" },
    //   body: JSON.stringify(meta),
    // });
    // const data = await r.json();
    // // PUT to presigned URL
    // const put = await fetch(data.presignedUrl, {
    //   method: "PUT",
    //   body: selectedFile,
    //   headers: { "Content-Type": meta.contentType },
    // });
    // if (!put.ok) {
    //   alert("Upload failed");
    //   return;
    // }
    // const etag = put.headers.get("ETag") || "";
    // await fetch(`${API}/api/uploads/complete`, {
    //   method: "POST",
    //   headers: { "Content-Type": "application/json" },
    //   body: JSON.stringify({
    //     uploadId: data.uploadId,
    //     s3Key: data.s3Key,
    //     etag,
    //   }),
    // });
    const form = new FormData();
    form.append("file", selectedFile);
    form.append("path", "/"); // optional
    form.append("contentType", selectedFile.type || "application/octet-stream");
    const res = await fetch(`${API}/api/uploads`, {
      method: "POST",
      body: form,
    });
    const jsonData = await res.json();
    if (jsonData?.result == "success") {
      alert("Upload success!");
    } else {
      alert("Upload fail!");
    }
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
          onChange={(e) => setSelectedFile(e.target.files?.[0] || null)}
        />
        <button
          onClick={startUpload}
          disabled={!selectedFile}
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
