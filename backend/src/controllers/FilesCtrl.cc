#include "FilesCtrl.h"
#include <json/value.h>
#include <vector>
#include "../plugins/RepoPlugin.h"

using namespace drogon;

void FilesCtrl::list(const HttpRequestPtr &, std::function<void(const HttpResponsePtr &)> &&cb)
{
    auto repo = drogon::app().getPlugin<RepoPlugin>();
    auto files = repo->listFiles("/", 100); // put default value for now

    Json::Value resp;
    Json::Value fileArr(Json::arrayValue);
    for (auto &f : files)
    {
        Json::Value tmp;
        tmp["id"] = f.id;
        tmp["name"] = f.name;
        tmp["path"] = f.path;
        tmp["size"] = (Json::UInt64)f.size;
        tmp["s3Key"] = f.s3Key;
        tmp["contentType"] = f.content_type;
        tmp["etag"] = f.etag;
        tmp["created_at"] = f.created_at;
        fileArr.append(tmp);
    }

    resp["files"] = fileArr;
    cb(HttpResponse::newHttpJsonResponse(resp));

    // Json::Value out;
    // Json::Value arr(Json::arrayValue);
    // // stub

    // Json::Value f;
    // f["id"] = "1";
    // f["name"] = "hello.txt";
    // f["size"] = 5;
    // arr.append(f);
    // out["files"] = arr;
    // cb(HttpResponse::newHttpJsonResponse(out));
}

void FilesCtrl::remove(const HttpRequestPtr &, std::function<void(const HttpResponsePtr &)> &&cb, std::string id)
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k204NoContent);
    cb(resp);
}
