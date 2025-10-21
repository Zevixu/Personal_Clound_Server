#pragma once
#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>
#include <optional>
#include <string>
#include <vector>

using namespace std;

struct FileRow
{
    string id;
    // optional<string> owner_id;
    string name;
    string path;
    uint64_t size{};
    // string sha256; to-do: add later
    string s3Key;
    string content_type;
    string etag;
    string created_at;
};

class RepoPlugin : public drogon::Plugin<RepoPlugin>
{
public:
    RepoPlugin() = default;

    // initialize and start the plugin
    void initAndStart(const Json::Value &config) override;

    // shut down the plugin
    void shutdown() override;

    FileRow insertFile(
        // const optional<string> &ownerId,
        const string &name,
        const string &path,
        uint64_t size,
        // const string &sha256,
        const string &s3Key,
        const string &contentType,
        const string &etag);

    vector<FileRow> listFiles(
        // const optional<string> &ownerId,
        const string &path,
        int limit = 100);

private:
    void runMigrations();

    drogon::orm::DbClientPtr m_dbclient;
};