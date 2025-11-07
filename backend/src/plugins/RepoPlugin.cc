#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include "RepoPlugin.h"
#include <fstream>

using namespace std;

/* utils */
static string readAll(const string &path)
{
    ifstream in(path, ios::binary);
    if (!in)
        throw runtime_error("Missing 001_init sql script");
    ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

// very simple splitter: one statement per ';' (skips empty)
static vector<string> splitSql(const string &sql)
{
    vector<string> out;
    string cur;
    bool inS = false, inD = false, esc = false;
    for (char c : sql)
    {
        cur.push_back(c);
        if (esc)
        {
            esc = false;
            continue;
        }
        if (c == '\\')
        {
            esc = true;
            continue;
        }
        if (c == '\'' && !inD)
            inS = !inS;
        else if (c == '"' && !inS)
            inD = !inD;
        else if (c == ';' && !inS && !inD)
        {
            if (cur.find_first_not_of(" \t\r\n;") != string::npos)
                out.push_back(cur);
            cur.clear();
        }
    }
    if (cur.find_first_not_of(" \t\r\n;") != string::npos)
        out.push_back(cur);
    return out;
}
/**/

void RepoPlugin::initAndStart(const Json::Value &config)
{
    // Drogon has parsed config.json. Now created the db client
    m_dbclient = drogon::app().getDbClient();
    if (!m_dbclient)
    {
        cerr << "DB client is not available in RepoPlugin" << endl;
    }

    runMigrations();
    cout << "RepoPlugin initialized and database migrations applied" << endl;
}

void RepoPlugin::shutdown()
{
    m_dbclient.reset();
}

FileRow RepoPlugin::insertFile(
    // const optional<string> &ownerId,
    const string &name,
    const string &path,
    uint64_t size,
    // const string &sha256,
    const string &s3Key,
    const string &contentType,
    const string &etag)
{
    const char *query =
        "INSERT INTO files(name,path,size,s3_key,content_type,etag) "
        "VALUES($1,$2,$3,$4,$5,$6)"
        "RETURNING id, name, path, size, s3_key, content_type, etag, to_char(created_at,'YYYY-MM-DD\"T\"HH24:MI:SSZ') as created_at;";

    auto f = m_dbclient->execSqlAsyncFuture(query, name, path, size, s3Key, contentType, etag).get();
    if (f.size() != 1)
    {
        cerr << "insertFile: unexpected rows" << endl;
    }

    const auto &r = f[0];

    FileRow row;
    row.id = r["id"].as<string>();
    row.name = r["name"].as<string>();
    row.path = r["path"].as<string>();
    row.size = (uint64_t)r["size"].as<long long>();
    row.s3Key = r["s3_key"].as<string>();
    row.content_type = r["content_type"].as<string>();
    row.etag = r["etag"].as<string>();
    row.created_at = r["created_at"].as<string>();

    return row;
}

vector<FileRow> RepoPlugin::listFiles(const string &path, int limit)
{
    const char *query =
        "SELECT id, name, path, size, s3_key, content_type, etag, to_char(created_at,'YYYY-MM-DD\"T\"HH24:MI:SSZ') as created_at "
        "FROM files "
        "WHERE deleted_at IS NULL "
        "AND ($1::text IS NULL OR path = $1::text) "
        "ORDER BY created_at DESC "
        "LIMIT $2;";

    auto files = m_dbclient->execSqlAsyncFuture(query, path, static_cast<int64_t>(limit)).get(); // do we need a thow error exception check here?

    vector<FileRow> out;
    out.reserve(files.size());

    for (auto &f : files)
    {
        FileRow file;
        file.id = f["id"].as<string>();
        file.name = f["name"].as<string>();
        file.path = f["path"].as<string>();
        file.size = (uint64_t)f["size"].as<long long>();
        file.s3Key = f["s3_key"].as<string>();
        file.content_type = f["content_type"].as<string>();
        file.etag = f["etag"].as<string>();
        file.created_at = f["created_at"].as<string>();

        out.push_back(file);
    }

    return out;
}

bool RepoPlugin::getFileById(const string &id, FileRow *file)
{
    // find the file that matches the id
    const char *query =
        "SELECT id, name, path, size, s3_key, content_type, etag, to_char(created_at,'YYYY-MM-DD\"T\"HH24:MI:SSZ') as created_at "
        "FROM files WHERE id = $1::uuid;";

    auto rtnArr = m_dbclient->execSqlAsyncFuture(query, id).get();
    if (rtnArr.empty())
    {
        return false;
    }
    else
    {
        const auto &r = rtnArr[0];
        file->id = r["id"].as<string>();
        file->name = r["name"].as<string>();
        file->path = r["path"].as<string>();
        file->size = (uint64_t)r["size"].as<long long>();
        file->s3Key = r["s3_key"].as<string>();
        file->content_type = r["content_type"].as<string>();
        file->etag = r["etag"].as<string>();
        file->created_at = r["created_at"].as<string>();
        return true;
    }
}

bool RepoPlugin::deleteFileById(const string &id)
{
    const char *query =
        "DELETE FROM files WHERE id = $1::uuid;";

    auto rtn = m_dbclient->execSqlAsyncFuture(query, id).get();
    return rtn.affectedRows() > 0;
}

void RepoPlugin::runMigrations()
{
    // split the sql file into separate single statements and execuate them one by one
    //
    cout << "ready to execuate db migration script" << endl;
    const string sql = readAll("db/migrations/001_init.sql");

    auto txn = m_dbclient->newTransaction();
    for (const auto &stament : splitSql(sql))
    {
        txn->execSqlSync(stament);
    }
    cout << "db migration script completed" << endl;
}