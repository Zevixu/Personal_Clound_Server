#include "FilesCtrl.h"
#include <json/value.h>
#include <vector>
#include "../plugins/RepoPlugin.h"
#include "../plugins/S3ClientPlugin.h"

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
}

void FilesCtrl::remove(const HttpRequestPtr &, std::function<void(const HttpResponsePtr &)> &&cb, std::string id)
{
    // look up db to make sure it exist before deleting
    auto repo = drogon::app().getPlugin<RepoPlugin>();
    FileRow deleteFile;
    if (!repo->getFileById(id, &deleteFile))
    {
        // the file doesn't exist, return "not found" response directly
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k404NotFound);
        r->setContentTypeCode(CT_TEXT_PLAIN);
        r->setBody("file not found");
        return cb(r);
    }

    // the file exists in the db, now delete this file from s3/minio storage
    auto s3client = drogon::app().getPlugin<S3ClientPlugin>();
    if (!s3client->deleteObjectFromFile(deleteFile.s3Key))
    {
        // if deleting file from s3/minio storage fails, return response
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k502BadGateway);
        r->setContentTypeCode(CT_TEXT_PLAIN);
        r->setBody("failed to delete from s3/minio storage");
        return cb(r);
    }

    // delete file from storage success, now delete record from db
    if (!repo->deleteFileById(id))
    {
        // fail to delete this record from db, return response
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k500InternalServerError);
        r->setContentTypeCode(CT_TEXT_PLAIN);
        r->setBody("failed to update database. Try again.");
        return cb(r);
    }

    auto r = HttpResponse::newHttpResponse();
    r->setStatusCode(k200OK);
    r->setContentTypeCode(CT_TEXT_PLAIN);
    r->setBody("delete success");
    cb(r);
}
