
#include "httpsclient.h"
#include <iostream>
#include <assert.h>

struct test_base_t {
	test_base_t(https::connect_fun_t connect_fun,
		https::write_fun_t write_fun,
		https::recv_fun_t recv_fun,
		https::close_fun_t close_fun) {

		https::connect_fun = connect_fun != nullptr ? std::move(connect_fun) : []() {return 0; };
		https::write_fun = write_fun != nullptr ? std::move(write_fun) : [](const std::string& buf) { return buf.size(); };
		https::recv_fun = recv_fun != nullptr ? std::move(recv_fun) : [](unsigned char* buf, int len) { return 0; };
		https::close_fun = close_fun != nullptr ? std::move(close_fun) : []() { return 0; };
	}
	~test_base_t() {
		clear();
	}
	void clear() {
		https::connect_fun = nullptr;
		https::write_fun = nullptr;
		https::recv_fun = nullptr;
		https::close_fun = nullptr;
	}
};

void test_1() {
	size_t times = 0;
	std::vector<std::string> bufs{
		"HTTP/1.1 2",
		"00 OK\r",
		"\n",
		"\r",
		"\n"
	};

	test_base_t t(nullptr, nullptr, [&](unsigned char* buf, int len) -> int {
		if (times >= bufs.size()) {
			return 0;
		}
		auto& sbuf = bufs[times++];
		memcpy(buf, sbuf.c_str(), sbuf.size());
		return sbuf.size();
	}, nullptr);

	auto res = https::client().get("http://localhost");
	assert(res->statusText == "OK" && res->status == 200);
}

void test_2() {
	size_t times = 0;
	std::vector<std::string> bufs{
		"HTTP/1.1 2",
		"00 ok\r",
		"\n",
		"\r",
		"\n",
		"ab",
		"c"
	};

	test_base_t t(nullptr, nullptr, [&](unsigned char* buf, int len) -> int {
		if (times >= bufs.size()) {
			return 0;
		}
		auto& sbuf = bufs[times++];
		memcpy(buf, sbuf.c_str(), sbuf.size());
		return sbuf.size();
	}, nullptr);

	auto res = https::client().get("http://localhost");
	assert(res->statusText == "ok" && res->status == 200);
	assert(res->body.content_length == 3);
	assert(res->body.pos == 3);
	assert(strncmp(res->body.content.data(), "abc", res->body.content.size()) == 0);
	assert(res->body.content.size() == res->body.content_length);

}

void test_3() {
	size_t times = 0;
	std::vector<std::string> bufs{
		"HTTP/1.1 2",
		"00 ok\r",
		"\n",
		"Content-Length: 3\r\n",
		"\r",
		"\n",
		"ab",
		"cd",
		"ef"
	};

	test_base_t t(nullptr, nullptr, [&](unsigned char* buf, int len) -> int {
		if (times >= bufs.size()) {
			return 0;
		}
		auto& sbuf = bufs[times++];
		memcpy(buf, sbuf.c_str(), sbuf.size());
		return sbuf.size();
	}, nullptr);

	auto res = https::client().get("http://localhost");
	assert(res->statusText == "ok" && res->status == 200);
	assert(res->body.content_length == 3);
	assert(res->body.pos == 3);
	assert(std::string(res->body.content.data(), res->body.content.size()) == "abc");
}

void test_4() {
	size_t times = 0;
	std::vector<std::string> bufs{
		"HTTP/1.1 204 ok ok ok\r\n",
		"Transfer-Encoding: chunked\r\n"
		"\r",
		"\n",
		"A\r\n",
		"0123456789\r",
		"\n",
		"3\r\nabc\r\n2\r\nde\r\n",
		"0\r\n",
	};

	test_base_t t(nullptr, nullptr, [&](unsigned char* buf, int len) -> int {
		if (times >= bufs.size()) {
			return 0;
		}
		auto& sbuf = bufs[times++];
		memcpy(buf, sbuf.c_str(), sbuf.size());
		return sbuf.size();
	}, nullptr);

	auto res = https::client().get("http://localhost");
	assert(res->statusText == "ok ok ok" && res->status == 204);
	assert(res->body.content_length == 15);
	assert(res->body.pos == 15);
	std::string body_content = std::string(res->body.content.begin(), res->body.content.end());
	assert(body_content == "0123456789abcde");

}

void sample() {

	https::headers_t headers{
		{ "Accept-Encoding", "gzip" }
	};
	auto res = https::client().get("https://dllink.fygame.com/dlyy360/link_svrlist.php", headers);
	if (res->status == 200 || res->status == 204) {
		std::string body = std::string(res->body.content.begin(), res->body.content.end());
		std::cout << body;
	}
	else {
		res->statusText;
	}

}

int main() {

	test_1();
	test_2();
	test_3();
	test_4();

	sample();

	return 0;
}