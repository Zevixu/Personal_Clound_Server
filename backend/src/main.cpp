#include <drogon/drogon.h>

int main() {
    // Load config.json automatically if present
    // drogon::app().loadConfigFile("config.json");

    // Simple CORS for local dev (tighten in prod)
    drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr &req, const drogon::HttpResponsePtr &resp){
        resp->addHeader("Access-Control-Allow-Origin", "*"); // allow requests from any domain 
        resp->addHeader("Access-Control-Allow-Credentials", "true");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
        resp->addHeader("Access-Control-Allow-Methods", "GET,POST,DELETE,PUT,OPTIONS");
    });

    drogon::app().addListener("0.0.0.0", 8080);
    drogon::app().run();
    return 0;
}
