import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";

export default defineConfig({
  plugins: [react()],
  server: {
    port: 5173,
    host: true,
    proxy: {
      "/api": {
        target: "http://backend:8080", // must use backend service name instead of localhost, because it is inside container
        changeOrigin: true,
      },
    },
  },
});
