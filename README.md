# httpsclient.h
https client for c++


    std::string url = "https://www.qq.com";

    // 用法一
    https::headers_t headers{
        { "Accept-Encoding", "gzip" }
    };
    auto res = https::client().get(url, headers);
    if (res->status == 200 || res->status == 204) {
        std::string body = std::string(res->body.content.begin(), res->body.content.end());
        std::cout << body;
    }

    // 用法二
    std::string body;
    auto res2 = https::client().get(url, headers, [&body](const char *data, size_t data_length) {
        body.append(data, data_length);
        return true;
    });
    if (res->status == 200 || res->status == 204) {
        std::cout << body;
    }