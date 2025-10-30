#pragma once
#include <drogon/HttpController.h>
// #include "../services/S3Client.h"

using namespace drogon;

class UploadCtrl : public drogon::HttpController<UploadCtrl>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UploadCtrl::upload, "/api/uploads", Post);
    METHOD_LIST_END

    // UploadCtrl();

    void upload(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&cb);

    // private:
    //     // use share pointer so that different controllers can share the same
    //     // s3 client. We want to keep one s3 client throught the lifetime of
    //     // this application
    //     // to-do: wrap this s3 client with a drogon plugin, so that each controller
    //     // will have access to it
    //     shared_ptr<S3Client> m_s3client;
    //
};
