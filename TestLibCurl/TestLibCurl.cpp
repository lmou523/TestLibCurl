

#include <iostream>
#include <string>

#include "UseCurlDef.h"

using namespace std;


void TestRequestBaidu();
void TestRequest();
void GetHtmlAndWriteFile();


int main(int argc, char* argv[]) 
{
	GetHtmlAndWriteFile();

	return 0;
}

// 测试函数1
void TestRequestBaidu()
{
	CURL* curl = 0;
	CURLcode res;
	curl = curl_easy_init();
	if (curl != 0) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");

		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
}

// 测试函数2
void TestRequest()
{
	CURLcode return_code;
	return_code = curl_global_init(CURL_GLOBAL_WIN32);
	printf("初始化状态: %d \n", return_code);
	if (CURLE_OK != return_code)
		return ;

	struct curl_slist* headers = NULL;

	headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:76.0)");
	headers = curl_slist_append(headers, "Referer: https://www.baidu.com");

	CURL* easy_handle = curl_easy_init();

	if (NULL != easy_handle)
	{
		curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);        // 改协议头
		curl_easy_setopt(easy_handle, CURLOPT_URL, "https://cn.bing.com"); // 请求的网站
		return_code = curl_easy_perform(easy_handle);                      // 执行CURL

		long retcode = 0;
		return_code = curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &retcode);
		if ((CURLE_OK == return_code) && retcode)
			printf("请求状态码: %d \n", retcode);


		char* ipAddress = { 0 };
		return_code = curl_easy_getinfo(easy_handle, CURLINFO_PRIMARY_IP, &ipAddress);
		if ((CURLE_OK == return_code) && ipAddress)
			printf("目标IP地址: %s \n", ipAddress);

		char* contentType = { 0 };
		return_code = curl_easy_getinfo(easy_handle, CURLINFO_CONTENT_TYPE, &contentType);
		if ((CURLE_OK == return_code) && contentType)
			printf("请求的文件类型: %s \n", contentType);


		long requestSize = 0;
		return_code = curl_easy_getinfo(easy_handle, CURLINFO_REQUEST_SIZE, &requestSize);
		if ((CURLE_OK == return_code) && requestSize)
			printf("请求头大小: %d \n", requestSize);

		long headerSize = 0;
		return_code = curl_easy_getinfo(easy_handle, CURLINFO_HEADER_SIZE, &headerSize);
		if ((CURLE_OK == return_code) && headerSize)
			printf("响应头大小: %d \n", headerSize);
	}

	curl_easy_cleanup(easy_handle);
	curl_global_cleanup();

	system("pause");
}

// 测试函数3中注册的回调函数
size_t write_data1(void* ptr, size_t size, size_t nmemb, void* stream)
{
	FILE* fp;

	if ((fp = fopen("./baidu.html", "w")) == NULL)
	{
		return 0;
	}
	int written = fwrite(ptr, size, nmemb, fp);

	return written;
}

// 测试函数3
void GetHtmlAndWriteFile()
{
	const char* URL = "https://www.baidu.com";
	CURL* curl;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, URL);

	// CURLOPT_WRITEFUNCTION 将后继的动作交给write_data函数处理
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data1);
	curl_easy_perform(curl);

	curl_easy_cleanup(curl);
}

// 测试函数4
bool TestPost()
{
	const char* Url = "https://www.baidu.com/post.php";
	const char* Cookie = "exfffffx";
	const char* PostVal = "&logintype=uid&u=xieyan&psw=xxx86";

	CURL* curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, Url);                       // 指定url
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, Cookie);             // 指定cookie文件
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, PostVal);            // 指定post内容
		// curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");  // 是否代理
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return true;
}

// 测试函数5中注册的回调函数
size_t write_data2(void* ptr, size_t size, size_t nmemb, void* stream)
{
	FILE* fp;

	if ((fp = fopen("./baidu.html", "w")) == NULL)
	{
		return 0;
	}
	int written = fwrite(ptr, size, nmemb, fp);

	return written;
}

// 测试函数5
string postRequest(string strUrl, string strJson) {
	CURL* curl = curl_easy_init();
	if (NULL == curl) {
		// ERROR:
		return "ERROR: call curl_easy_init fail";
	}

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());

	struct curl_slist* headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strJson.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strJson.length());

	string strResp;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&strResp);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data2);

	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
	CURLcode res = curl_easy_perform(curl);
	bool bRet = false;
	if (res != CURLE_OK) { // connect fail
		// ERROR
		strResp = "ERROR: Connect fail";
	}
	else {
		long respCode;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respCode);
		if (respCode == 200) {
			bRet = true; // success
		}
		else {
			// error-info in strResp
		}
	}

	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	return strResp;
}
