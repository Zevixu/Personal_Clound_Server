#include <drogon/HttpController.h>

using namespace drogon;

class FilesCtrl : public drogon::HttpController<FilesCtrl>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FilesCtrl::list, "/api/files", Get);
    ADD_METHOD_TO(FilesCtrl::remove, "/api/files/{1}", Delete);
    METHOD_LIST_END

    void list(const HttpRequestPtr &, std::function<void(const HttpResponsePtr &)> &&cb);
    void remove(const HttpRequestPtr &, std::function<void(const HttpResponsePtr &)> &&cb, std::string id);
};
