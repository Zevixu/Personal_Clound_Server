#include <drogon/HttpController.h>
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>
#include <string>
#include <memory>
#include "../services/S3Client.h"
#include <drogon/utils/Utilities.h>
#include <iostream>
#include <drogon/orm/DbClient.h>
#include "../plugins/RepoPlugin.h"

using namespace std;
using namespace drogon;
namespace fs = filesystem;

class UploadCtrl : public drogon::HttpController<UploadCtrl>
{
public:
    METHOD_LIST_BEGIN
    // ADD_METHOD_TO(UploadCtrl::start, "/api/uploads/start", Post);
    // ADD_METHOD_TO(UploadCtrl::complete, "/api/uploads/complete", Post);
    ADD_METHOD_TO(UploadCtrl::upload, "/api/uploads", Post);
    METHOD_LIST_END

    UploadCtrl()
    {
        m_s3client = make_shared<S3Client>(); // to-do: could we move this to the main.cpp file(like initialize() function)
        if (!m_s3client->initFromEnv())
        {
            LOG_ERROR << "S3Client initialization failed -- check S3_BUCKET env variable";
        }
    }

    // void start(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&cb)
    // {
    //     // NOTE: This is a stub. In real code, generate a presigned URL to S3/MinIO.
    //     Json::Value out;
    //     out["uploadId"] = "stub-upload-id";
    //     out["presignedUrl"] = "http://minio:9000/drive/stub-object"; // replace with real presign
    //     out["s3Key"] = "user/2025/stub-object";

    //     auto resp = HttpResponse::newHttpJsonResponse(out);
    //     cb(resp);
    // }

    // void complete(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&cb)
    // {
    //     // TODO: verify ETag / checksum and write metadata to DB
    //     auto resp = HttpResponse::newHttpResponse();
    //     resp->setStatusCode(k200OK);
    //     resp->setContentTypeCode(CT_TEXT_PLAIN);
    //     resp->setBody("completed");
    //     cb(resp);
    // }
    void upload(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&cb)
    {

        // const auto &files = req->getUploadedFiles();
        MultiPartParser parser;
        if (parser.parse(req) != 0)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setContentTypeCode(CT_TEXT_PLAIN);
            resp->setBody("Invalid multipart body");
            return cb(resp);
        }
        const auto &files = parser.getFiles();

        if (files.empty())
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            resp->setContentTypeCode(CT_TEXT_PLAIN);
            resp->setBody("missing file");
            return cb(resp);
        }

        const auto &f = files[0];
        const string fName = f.getFileName();
        const uint64_t size = f.fileLength();
        string realPath = "/";
        string contentType = "application/octet-stream";
        // Optional fields like path, contenType
        auto params = req->getParameters();
        if (auto it = params.find("path"); it != params.end())
        {
            realPath = it->second;
        }
        if (auto it = params.find("contentType"); it != params.end())
        {
            contentType = it->second;
        }

        // save the file to a tmp folder
        f.save();
        const string tmpPath = drogon::app().getUploadPath() + "/" + fName;

        // build file object key <yyyy/mm>/<uuid>-<filename>
        string uuid = drogon::utils::getUuid();
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        tm tm = *localtime(&t);
        char ym[16];
        snprintf(ym, sizeof(ym), "%04d/%02d", tm.tm_year + 1900, tm.tm_mon + 1);
        string key = ym;
        if (!realPath.empty() && realPath != "/")
        {
            key += "/" + realPath;
        }
        key += "/" + uuid + "-" + fName;

        // to-do: SHA-256

        // choose different upload methods depending on the file size
        string etag, errOut;
        bool rtn = false;
        if (size < m_s3client->conf().partSizeBytes)
        {
            rtn = m_s3client->putObjectFromFile(key, tmpPath, contentType, &etag, &errOut);
        }
        else
        {
            rtn = m_s3client->multiPartUploadFromFile(key, tmpPath, contentType, &etag, &errOut);
        }

        Json::Value respJson;
        respJson["result"] = "success";
        if (!rtn)
        {
            error_code ec;
            fs::remove(tmpPath, ec);

            LOG_ERROR << "File Upload Failed: " << errOut;
            respJson["result"] = "fail";
            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            resp->setStatusCode(k500InternalServerError);
            return cb(resp);
        }
        else
        {
            // get repo plugin
            auto repo = drogon::app().getPlugin<RepoPlugin>();
            // insert the file into database files table
            auto row = repo->insertFile(fName, realPath, size, key, contentType, etag); // for owner_id, use null pointer for now

            respJson["id"] = row.id;
            respJson["name"] = row.name;
            respJson["size"] = row.size;
            // to-do add sha-256 field
            respJson["s3key"] = row.s3Key;
            respJson["contentType"] = row.content_type;
            respJson["etag"] = row.etag;
            respJson["createdAt"] = row.created_at;

            // Clean up files in the temp folder
            error_code ec;
            fs::remove(tmpPath, ec);

            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            resp->setStatusCode(k200OK);
            cb(resp);
        }
    }

private:
    // use share pointer so that different controllers can share the same
    // s3 client. We want to keep one s3 client throught the lifetime of
    // this application
    // to-do: wrap this s3 client with a drogon plugin, so that each controller
    // will have access to it
    shared_ptr<S3Client> m_s3client;
};

// REGISTER_HTTP_CONTROLLER(UploadCtrl)
