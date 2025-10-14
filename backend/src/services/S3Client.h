// For MinIO/AWS S3, use AWS SDK for C++ initially.
#pragma once
#include <string>
#include <aws/s3/S3Client.h>
#include <aws/core/Aws.h>

using namespace std;

struct S3Config
{
    string APIEndPoint;
    string region;
    string bucket;
    bool forcePathStyle{false};            // false by default
    size_t partSizeBytes{8 * 1024 * 1024}; // 8 MB  by default
};

class S3Client
{
public:
    S3Client();
    ~S3Client();

    bool initFromEnv(); // load variables from enviroment
    const S3Config &conf() const { return m_conf; }

    // uploading small files using PutObject
    bool putObjectFromFile(const string &key, const string &localPath, const string &contentType, string *eTagOut, string *errOut);

    // uploading large files using multipart
    bool multiPartUploadFromFile(const string &key, const string &localPath, const string &contentType, string *eTagOut, string *errOut);

private:
    S3Config m_conf;
    Aws::SDKOptions m_options;
    unique_ptr<Aws::S3::S3Client> m_s3;

    unique_ptr<Aws::S3::S3Client> makeS3Client();
};
