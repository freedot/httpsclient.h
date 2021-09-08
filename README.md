# httpsclient.h
https client for c++


    //用法一
    https::headers_t headers{
        { "Accept-Encoding", "gzip" }
    };
    auto res = https::client().get("https://dllink.fygame.com/dlyy360/link_svrlist.php", headers);
    if (res->status == 200 || res->status == 204) {
        std::string body = std::string(res->body.content.begin(), res->body.content.end());
        std::cout << body;
    }