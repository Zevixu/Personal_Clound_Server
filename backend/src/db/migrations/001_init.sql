CREATE TABLE IF NOT EXISTS files (
  id            UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  -- owner_id      UUID NULL,                       -- add later when auth completes
  name          TEXT NOT NULL,
  path          TEXT NOT NULL DEFAULT '/',
  size          BIGINT NOT NULL,
-- add later   sha256        TEXT NOT NULL, 
  s3_key        TEXT NOT NULL,
  content_type  TEXT NOT NULL,
  etag          TEXT NOT NULL,
  created_at    TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  updated_at    TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  deleted_at    TIMESTAMPTZ NULL
);

-- CREATE INDEX IF NOT EXISTS idx_files_owner_path ON files(owner_id, path); -- in order to query with owner_id and path
-- CREATE INDEX IF NOT EXISTS idx_files_created_at ON files(created_at DESC); -- in order to query with creation time, descending, 
--                                                                            -- used for retrieving recently uploading files
