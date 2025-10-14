#include "S3Client.h"
#include <aws/core/client/ClientConfiguration.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/CreateMultipartUploadRequest.h>
#include <aws/s3/model/UploadPartRequest.h>
#include <aws/s3/model/CompletedMultipartUpload.h>
#include <aws/s3/model/CompletedPart.h>
#include <aws/s3/model/CompleteMultipartUploadRequest.h>
#include <aws/core/utils/UUID.h>
#include <fstream>

using AwsStr = Aws::String;

static size_t envToSize(const char *v, size_t def)
{
    if (!v || !*v)
        return def;
    try
    {
        return static_cast<size_t>(std::stoull(v));
    }
    catch (...)
    {
        return def;
    }
}

S3Client::S3Client()
{
    m_options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Warn;
    Aws::InitAPI(m_options);
}

S3Client::~S3Client()
{
    // destroy m_s3 before shutting down
    m_s3.reset();
    Aws::ShutdownAPI(m_options);
}

bool S3Client::initFromEnv()
{
    const char *aep = getenv("S3_ENDPOINT");
    const char *rg = getenv("S3_REGION");
    const char *bc = getenv("S3_BUCKET");
    const char *fps = getenv("S3_FORCE_PATH_STYLE");
    const char *psb = getenv("S3_MULTIPART_PART_SIZE_MB");

    m_conf.APIEndPoint = aep ? aep : "";
    m_conf.region = rg ? rg : "us-east-1";
    m_conf.bucket = bc ? bc : "";
    m_conf.forcePathStyle = fps && (std::string(fps) == "1" || std::string(fps) == "true" || std::string(fps) == "TRUE");
    size_t mb = envToSize(psb, 8);
    m_conf.partSizeBytes = mb * 1024ULL * 1024ULL;

    if (m_conf.bucket.empty())
    {
        return false;
    }

    m_s3 = makeS3Client();
    return true;
}

bool S3Client::putObjectFromFile(const string &key, const string &localPath, const string &contentType, string *eTagOut, string *errOut)
{
    ifstream ifs(localPath, ios::binary);
    if (!ifs)
    {
        if (errOut)
        {
            *errOut = "Failed to open uploaded file from tmp folder";
            return false;
        }
    }

    // Read into memroy for simplicity
    ostringstream oss;
    oss << ifs.rdbuf();
    auto bodyStr = oss.str();

    Aws::S3::Model::PutObjectRequest req;
    req.SetBucket(AwsStr(m_conf.bucket.c_str()));
    req.SetKey(AwsStr(key.c_str()));
    if (!contentType.empty())
    {
        req.SetContentType(AwsStr(contentType.c_str()));
    }

    auto ss = Aws::MakeShared<Aws::StringStream>("PutBody");
    ss->write(bodyStr.data(), bodyStr.size());
    req.SetBody(ss);

    auto rtn = m_s3->PutObject(req);
    if (!rtn.IsSuccess())
    {
        if (errOut)
            *errOut = rtn.GetError().GetMessage().c_str();
        return false;
    }
    if (eTagOut)
    {
        *eTagOut = rtn.GetResult().GetETag().c_str();
    }

    return true;
}

bool S3Client::multiPartUploadFromFile(const string &key, const string &localPath, const string &contentType, string *eTagOut, string *errOut)
{
    // for large files
    cout << "DEBUG: Receiving reqeust for uploading large files..." << endl;
    return true;
}

unique_ptr<Aws::S3::S3Client> S3Client::makeS3Client()
{
    Aws::Client::ClientConfiguration cfg;
    cfg.region = m_conf.region.c_str();

    if (!m_conf.APIEndPoint.empty())
    {
        cfg.endpointOverride = AwsStr(m_conf.APIEndPoint.c_str());
        cfg.scheme = Aws::Http::Scheme::HTTP; // set HTTPS if using TLS
    }

    Aws::S3::S3ClientConfiguration s3cfg(cfg);
    s3cfg.region = cfg.region;
    s3cfg.endpointOverride = cfg.endpointOverride;
    s3cfg.useVirtualAddressing = !m_conf.forcePathStyle; // MinIO => false

    return make_unique<Aws::S3::S3Client>(s3cfg);
}