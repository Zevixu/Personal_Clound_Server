import React, { useEffect, useMemo, useRef, useState } from "react";
import {
  Box,
  Button,
  Container,
  Dialog,
  DialogActions,
  DialogContent,
  DialogContentText,
  DialogTitle,
  IconButton,
  LinearProgress,
  Paper,
  Snackbar,
  Stack,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Tooltip,
  Typography,
} from "@mui/material";
import UploadIcon from "@mui/icons-material/Upload";
import RefreshIcon from "@mui/icons-material/Refresh";
import DeleteIcon from "@mui/icons-material/Delete";
import DownloadIcon from "@mui/icons-material/Download";

// Adjust these endpoints to match your Drogon backend
const API_BASE = import.meta.env.VITE_API_BASE ?? "http://localhost:8080";
const LIST_ENDPOINT = `${API_BASE}/api/files`;
const UPLOAD_ENDPOINT = `${API_BASE}/api/uploads`;
const DELETE_ENDPOINT = (id) => `${API_BASE}/api/files/${id}`;
const DOWNLOAD_ENDPOINT = (id) => `${API_BASE}/api/files/${id}/download`;

/* Utilities */
const fmtBytes = (bytes) => {
  if (bytes === 0 || bytes === undefined || bytes === null) return "-";
  const k = 1024;
  const sizes = ["B", "KB", "MB", "GB", "TB"];
  const i = Math.floor(Math.log(bytes) / Math.log(k));
  return `${(bytes / Math.pow(k, i)).toFixed(i === 0 ? 0 : 2)} ${sizes[i]}`;
};

const fmtDate = (iso) => {
  if (!iso) return "-";
  const d = new Date(iso);
  if (Number.isNaN(d.getTime())) return iso;
  return d.toLocaleString();
};
/* */

export default function FileManager() {
  const [files, setFiles] = useState([]);
  const [loading, setLoading] = useState(false);
  const [uploading, setUploading] = useState(false);
  const [snack, setSnack] = useState({ open: false, message: "" });
  const [confirmId, setConfirmId] = useState(null);
  const inputRef = useRef(null);

  const hasRows = useMemo(() => files && files.length > 0, [files]);

  const showMessage = (message) => setSnack({ open: true, message });

  const fetchFiles = async () => {
    setLoading(true);
    try {
      const res = await fetch(LIST_ENDPOINT);
      const data = await res.json();
      if (data?.files.length) {
        setFiles(data.files);
      }
    } catch (err) {
      console.error(err);
      showMessage("Failed to fetch files");
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchFiles();
  }, []);

  const handleChooseFiles = () => {
    inputRef.current?.click();
  };

  const handleUpload = async (event) => {
    // limit to only uploading one file at a time
    // If backend supports multiple files in a single request, we can append all.
    const file = event.target.files[0];
    if (!file) return;
    setUploading(true);
    try {
      const form = new FormData();
      form.append("file", file);
      const res = await fetch(UPLOAD_ENDPOINT, {
        method: "POST",
        body: form,
      });
      if (!res.result == "success") {
        throw new Error(`Upload failed`);
      }

      showMessage("Upload complete");

      // update the file list on the UI after uploading
      await fetchFiles();
    } catch (err) {
      console.error(err);
      showMessage(err.message || "Upload failed");
    } finally {
      // Reset the input so selecting the same file again triggers onChange
      // event.target.value = ""; // NEED TO BE TESTED to see whether selecting the same file can trigger onChange
      setUploading(false);
    }
  };

  const confirmDelete = (id) => setConfirmId(id);
  const cancelDelete = () => setConfirmId(null);

  const handleDelete = async () => {
    // if (!confirmId) return;
    // try {
    //   const res = await fetch(DELETE_ENDPOINT(confirmId), { method: "DELETE" });
    //   if (!res.ok) throw new Error(`Delete failed: ${res.status}`);
    //   showMessage("File deleted");
    //   setConfirmId(null);
    //   await fetchFiles();
    // } catch (err) {
    //   console.error(err);
    //   showMessage(err.message || "Delete failed");
    // }
  };

  const handleDownload = async (row) => {
    // try {
    //   const res = await fetch(DOWNLOAD_ENDPOINT(row.id));
    //   if (!res.ok) throw new Error(`Download failed: ${res.status}`);
    //   const blob = await res.blob();
    //   const url = window.URL.createObjectURL(blob);
    //   const a = document.createElement("a");
    //   // Try to preserve filename
    //   a.download = row.name ?? `download-${row.id}`;
    //   a.href = url;
    //   document.body.appendChild(a);
    //   a.click();
    //   a.remove();
    //   window.URL.revokeObjectURL(url);
    // } catch (err) {
    //   console.error(err);
    //   showMessage(err.message || "Download failed");
    // }
  };

  return (
    <Container maxWidth="lg" className="p-6">
      <Box py={4}>
        <Stack
          direction="row"
          alignItems="center"
          justifyContent="space-between"
          spacing={2}
        >
          <Typography variant="h5" fontWeight={700}>
            My Drive
          </Typography>
          <Stack direction="row" spacing={1}>
            <input
              ref={inputRef}
              type="file"
              hidden
              multiple
              onChange={handleUpload}
            />
            <Tooltip title="Upload files">
              <span>
                <Button
                  variant="contained"
                  startIcon={<UploadIcon />}
                  onClick={handleChooseFiles}
                  disabled={uploading}
                >
                  Upload
                </Button>
              </span>
            </Tooltip>
            <Tooltip title="Refresh list">
              <span>
                <Button
                  variant="outlined"
                  startIcon={<RefreshIcon />}
                  onClick={fetchFiles}
                  disabled={loading}
                >
                  Refresh File List
                </Button>
              </span>
            </Tooltip>
          </Stack>
        </Stack>

        {(loading || uploading) && (
          <Box mt={2}>
            <LinearProgress />
          </Box>
        )}

        <Box mt={3}>
          <TableContainer component={Paper} elevation={1}>
            <Table size="small" aria-label="files table">
              <TableHead>
                <TableRow>
                  <TableCell>Name</TableCell>
                  <TableCell>Date Uploaded</TableCell>
                  <TableCell align="right">File Size</TableCell>
                  <TableCell align="center">Actions</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {hasRows ? (
                  files.map((row) => (
                    <TableRow key={row.id} hover>
                      <TableCell sx={{ maxWidth: 420 }}>
                        <Typography noWrap title={row.name}>
                          {row.name || "-"}
                        </Typography>
                      </TableCell>
                      <TableCell>{fmtDate(row.created_at)}</TableCell>
                      <TableCell align="right">{fmtBytes(row.size)}</TableCell>
                      <TableCell align="center">
                        <Stack
                          direction="row"
                          spacing={1}
                          justifyContent="center"
                        >
                          <Tooltip title="Download">
                            <span>
                              <IconButton
                                onClick={() => handleDownload(row)}
                                size="small"
                              >
                                <DownloadIcon />
                              </IconButton>
                            </span>
                          </Tooltip>
                          <Tooltip title="Delete">
                            <span>
                              <IconButton
                                onClick={() => handleDelete(row.id)}
                                size="small"
                                color="error"
                              >
                                <DeleteIcon />
                              </IconButton>
                            </span>
                          </Tooltip>
                        </Stack>
                      </TableCell>
                    </TableRow>
                  ))
                ) : (
                  <TableRow>
                    <TableCell colSpan={4} align="center">
                      <Typography variant="body2" color="text.secondary" py={3}>
                        No files yet. Click <strong>Upload</strong> to add some.
                      </Typography>
                    </TableCell>
                  </TableRow>
                )}
              </TableBody>
            </Table>
          </TableContainer>
        </Box>

        {/* Delete confirmation */}
        <Dialog open={!!confirmId} onClose={cancelDelete}>
          <DialogTitle>Delete file?</DialogTitle>
          <DialogContent>
            <DialogContentText>This action cannot be undone.</DialogContentText>
          </DialogContent>
          <DialogActions>
            <Button onClick={cancelDelete}>Cancel</Button>
            <Button onClick={handleDelete} color="error" variant="contained">
              Delete
            </Button>
          </DialogActions>
        </Dialog>

        <Snackbar
          open={snack.open}
          autoHideDuration={3000}
          onClose={() => setSnack((s) => ({ ...s, open: false }))}
          message={snack.message}
        />
      </Box>
    </Container>
  );
}
