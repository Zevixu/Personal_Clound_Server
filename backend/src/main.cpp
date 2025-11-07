#include <drogon/drogon.h>

using namespace std;

int main()
{
    // Load config.json automatically if present
    if (!filesystem::exists("/app/config.json"))
    {
        cerr << "[FATAL] /app/config.json not found in container\n";
        abort();
    }

    drogon::app().loadConfigFile("/app/config.json");

    // Simple CORS for local dev (tighten in prod)
    drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp)
                                             {
        resp->addHeader("Access-Control-Allow-Origin", "*"); // allow requests from any domain 
        resp->addHeader("Access-Control-Allow-Credentials", "true");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
        resp->addHeader("Access-Control-Allow-Methods", "GET,POST,DELETE,PUT,OPTIONS"); });

    // make sure drogon backend listens on 0.0.0.0 not 127.0.0.1
    // so that it will listen on all network interfaces, thus can be accessed from outside the container
    drogon::app().addListener("0.0.0.0", 8080);
    drogon::app().run();

    return 0;
}
