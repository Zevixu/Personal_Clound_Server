#include <drogon/HttpController.h>
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>
#include <string>
#include <memory>

using namespace drogon;

class UploadCtrl : public drogon::HttpController<UploadCtrl> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(UploadCtrl::start, "/api/uploads/start", Post);
        ADD_METHOD_TO(UploadCtrl::complete, "/api/uploads/complete", Post);
    METHOD_LIST_END

    void start(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&cb) {
        // NOTE: This is a stub. In real code, generate a presigned URL to S3/MinIO.
        Json::Value out;
        out["uploadId"] = "stub-upload-id";
        out["presignedUrl"] = "http://minio:9000/drive/stub-object"; // replace with real presign
        out["s3Key"] = "user/2025/stub-object";

        auto resp = HttpResponse::newHttpJsonResponse(out);
        cb(resp);
    }

    void complete(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&cb) {
        // TODO: verify ETag / checksum and write metadata to DB
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_TEXT_PLAIN);
        resp->setBody("completed");
        cb(resp);
    }
};

REGISTER_HTTP_CONTROLLER(UploadCtrl);
