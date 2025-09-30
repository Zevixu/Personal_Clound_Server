#include <drogon/HttpController.h>
using namespace drogon;

class HealthCtrl : public drogon::HttpController<HealthCtrl> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(HealthCtrl::health, "/healthz", Get);
        ADD_METHOD_TO(HealthCtrl::options, "/{path}", Options);
    METHOD_LIST_END

    void health(const HttpRequestPtr&, std::function<void (const HttpResponsePtr &)> &&cb) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_TEXT_PLAIN);
        resp->setBody("ok");
        cb(resp);
    }

    void options(const HttpRequestPtr&, std::function<void (const HttpResponsePtr &)> &&cb, std::string path) {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k204NoContent);
        cb(resp);
    }
};

// Register explicitly so Drogon sees it in static builds
REGISTER_HTTP_CONTROLLER(HealthCtrl);
