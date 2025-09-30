#include <drogon/HttpController.h>
#include <json/value.h>
#include <vector>

using namespace drogon;

class FilesCtrl : public drogon::HttpController<FilesCtrl> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(FilesCtrl::list, "/api/files", Get);
        ADD_METHOD_TO(FilesCtrl::remove, "/api/files/{1}", Delete);
    METHOD_LIST_END

    void list(const HttpRequestPtr&, std::function<void (const HttpResponsePtr &)> &&cb) {
        Json::Value out;
        Json::Value arr(Json::arrayValue);
        // stub
        Json::Value f; f["id"]="1"; f["name"]="hello.txt"; f["size"]=5; arr.append(f);
        out["files"] = arr;
        cb(HttpResponse::newHttpJsonResponse(out));
    }

    void remove(const HttpRequestPtr&, std::function<void (const HttpResponsePtr &)> &&cb, std::string id) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k204NoContent);
        cb(resp);
    }
};

REGISTER_HTTP_CONTROLLER(FilesCtrl);
