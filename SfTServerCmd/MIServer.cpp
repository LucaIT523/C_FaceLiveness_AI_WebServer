
#include <fstream>
#include "MIServer.h"
#include <facesdk/FaceSDK_C_Api.h>
#include "licenseproc.h"

#ifdef WIN64
#include <windows.h>
#else
#include <ctime>
#endif

uint64_t getMilliseconds() {
#ifdef WIN64
	return GetTickCount64();
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
}

typedef CPipeline_t* (*pipeline_create_t)(const char* name, const CInitConfig_t* config, int* err, char* msg);
typedef void (*pipeline_destroy_t)(CPipeline_t* engine);
typedef CImage_t* (*image_create_path_t)(const char* path, int* err, char* msg);
typedef CPipelineResult_t(*pipeline_check_liveness_t)(const CPipeline_t* engine, const CImage_t* image, const CMeta_t* meta, int* err, char* msg);
typedef void (*image_destroy_t)(CImage_t* image);

extern HMODULE      g_hFaceDll;
extern CPipeline_t* g_pPipeline;



ST_RESPONSE* lv_pstRes = NULL;
#define LD_MAX_TRIAL_COUNT 100
int lv_nTrialCount = 50 * 2;

std::string replaceAll(std::string original, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = original.find(search, pos)) != std::string::npos) {
		original.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return original;
}

// Critical section object
CRITICAL_SECTION g_cs;

unsigned int TF_READ_LIC(void*) {
	INT64 nSts = 0;
	ST_RESPONSE* p = (ST_RESPONSE*)malloc(sizeof(ST_RESPONSE));

	InitializeCriticalSection(&g_cs);

	while (TRUE)
	{
		Sleep(10 * 1000);

		memset(p, 0, sizeof(ST_RESPONSE));
		p->m_nProduct = GD_PRODUCT_LIVENESS_FACE;
		if ((nSts = mil_read_license(p)) <= 0) {
			if (lv_pstRes != NULL) free(lv_pstRes);
			lv_pstRes = NULL;
		}
		else {
			if (lv_pstRes == NULL) {
				lv_pstRes = (ST_RESPONSE*)malloc(sizeof(ST_RESPONSE));
			}
			memcpy(lv_pstRes, p, sizeof(ST_RESPONSE));
		}
	}
	free(p); // Clean up allocated memory
	DeleteCriticalSection(&g_cs);

}

void ClaHTTPServerWrapper::launch() {
	
	if (lv_pstRes == NULL) {
		lv_pstRes = (ST_RESPONSE*)malloc(sizeof(ST_RESPONSE));
		lv_pstRes->m_nProduct = GD_PRODUCT_LIVENESS_FACE;
		if (mil_read_license(lv_pstRes) <= 0) {
			free(lv_pstRes);
			lv_pstRes = NULL;
		}
	}

	DWORD dwTID = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_READ_LIC, NULL, 0, &dwTID);
	run();
}

void MyRequestHandler::OnVersion(HTTPServerRequest& request, HTTPServerResponse& response)
{
	response.setStatus(HTTPResponse::HTTP_OK);
	response.setContentType("text/plain");

	response.set("Access-Control-Allow-Origin", "*");
	response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
	response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

	ostream& ostr = response.send();
	char szOut[MAX_PATH]; memset(szOut, 0, sizeof(szOut));
	sprintf_s(szOut, "Version : %s\nUpdate : %s", GD_ID_VERSION, GD_ID_UPDATE);
	ostr << szOut;
}
void MyRequestHandler::OnProcessProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, int base64)
{
	char        msg[MESSAGE_BUFFER_SIZE];
	int         err = OK;
	// Get the current time point
	auto now = std::chrono::system_clock::now();

	// Convert the time point to a time_t object
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

#ifdef NDEBUG
	if (lv_pstRes == NULL || lv_pstRes->m_lExpire < now_c) {
		OnNoLicense(request, response); 
		return;
	}
#endif
	//EnterCriticalSection(&g_cs);

	pipeline_destroy_t pipeline_destroy = (pipeline_destroy_t)(GetProcAddress(g_hFaceDll, "pipeline_destroy"));
	image_create_path_t image_create_path = (image_create_path_t)(GetProcAddress(g_hFaceDll, "image_create_path"));
	pipeline_check_liveness_t pipeline_check_liveness = (pipeline_check_liveness_t)(GetProcAddress(g_hFaceDll, "pipeline_check_liveness"));
	image_destroy_t image_destroy = (image_destroy_t)(GetProcAddress(g_hFaceDll, "image_destroy"));

	std::string FileImage = "";
	try {
		if (base64 == 0) {
			MyPartHandler hPart;
			Poco::Net::HTMLForm form(request, request.stream(), hPart);
			Object::Ptr jsonResponse = new Poco::JSON::Object();
			FileImage = hPart.fileData();
		}
		else {
			std::string encodedImage;
			std::istream& input = request.stream();
			std::ostringstream ss;
			StreamCopier::copyStream(input, ss);
			std::string data = ss.str();

			Parser parser;
			auto result = parser.parse(data);
			Object::Ptr object = result.extract<Object::Ptr>();
			encodedImage = object->getValue<std::string>("image");

			// Decode Base64
			std::istringstream istr(encodedImage);
			Poco::Base64Decoder decoder(istr);
			std::vector<char> decodedBytes((std::istreambuf_iterator<char>(decoder)), std::istreambuf_iterator<char>());

			// Print decoded bytes as a string (for demonstration purposes)
			std::string decodedImage(decodedBytes.begin(), decodedBytes.end());
			FileImage = decodedImage;
		}
	}
	catch (const Exception& ex)
	{
		FileImage = "";
	}
	uint64_t milliseconds = getMilliseconds();
	std::string millisecondsStr = std::to_string(milliseconds);
	std::string filePath = millisecondsStr + "output_file.dat";
	std::ofstream outFile(filePath, std::ios::binary);
	outFile.write(FileImage.c_str(), FileImage.size());
	outFile.close();

	try
	{
		// Send POST request
		std::string strRsp;
		HTTPServerResponse::HTTPStatus status = HTTPResponse::HTTP_OK;
		std::string out;
		std::ostringstream oss;
		CPipelineResult_t result;

		for (int i = 0; i < 2; i++) {
			pipeline_destroy_t pipeline_destroy = (pipeline_destroy_t)(GetProcAddress(g_hFaceDll, "pipeline_destroy"));
			image_create_path_t image_create_path = (image_create_path_t)(GetProcAddress(g_hFaceDll, "image_create_path"));
			pipeline_check_liveness_t pipeline_check_liveness = (pipeline_check_liveness_t)(GetProcAddress(g_hFaceDll, "pipeline_check_liveness"));
			image_destroy_t image_destroy = (image_destroy_t)(GetProcAddress(g_hFaceDll, "image_destroy"));

			CImage_t* image = image_create_path(filePath.c_str(), &err, msg);
			result = pipeline_check_liveness(g_pPipeline, image, NULL, &err, msg);
			image_destroy(image);

			char license[256] = "License error: license is not installed";
			if (_stricmp((const char*)msg, license) == 0) {
				pipeline_destroy(g_pPipeline);
				setting_init(1);
			}
			else {
				break;
			}
		}
		//.
		Object::Ptr root = new Object;
		root->set("score ", result.liveness_result.score);
		root->set("probability ", result.liveness_result.probability);
		root->set("quality ", result.quality_result.score);

		//.
		if (result.quality_result.score < 0.5) {
			root->set("liveness result ", "Image has a bad quality");
		}
		else if (result.liveness_result.probability >= 0.5) {
			root->set("liveness result ", "Image is genuine");
		}
		else {
			root->set("liveness result ", "Image is spoofed");
		}
		//.
		if (err == OK) {
			root->set("state ", "OK");
		}
		else {
			root->set("state ", msg);
		}
		
		Stringifier::stringify(root, oss);
		out = oss.str();

		//.
		std::remove(filePath.c_str());

		response.setStatus(status);
		response.setContentType("application/json");
		response.setContentLength(out.length());

		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

		response.send() << out.c_str();

	}
	catch (const Exception& ex)
	{
		response.setStatus(HTTPResponse::HTTP_CONFLICT);
		response.setContentType("application/json");

		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
		response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

		response.setContentLength(ex.displayText().length());
		response.send() << ex.displayText();
	}

	//LeaveCriticalSection(&g_cs);

}

void MyRequestHandler::OnUnknown(HTTPServerRequest& request, HTTPServerResponse& response)
{
	response.setStatus(HTTPResponse::HTTP_OK);
	response.setContentType("text/plain");

	response.set("Access-Control-Allow-Origin", "*");
	response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
	response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

	ostream& ostr = response.send();
	ostr << "Not found";
}

void MyRequestHandler::OnNoLicense(HTTPServerRequest& request, HTTPServerResponse& response)
{
	response.setStatus(HTTPResponse::HTTP_OK);
	response.setContentType("text/plain");

	response.set("Access-Control-Allow-Origin", "*");
	response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
	response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

	ostream& ostr = response.send();
	ostr << "Please input license.";
}

void MyRequestHandler::OnStatus(HTTPServerRequest& request, HTTPServerResponse& response)
{
	response.setStatus(HTTPResponse::HTTP_OK);
	response.setContentType("text/plain");

	response.set("Access-Control-Allow-Origin", "*");
	response.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
	response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");

	ostream& ostr = response.send();
	

	if (lv_pstRes == NULL) {
		//. no license
		ostr << "License not found";
	}
	else {
		time_t t = lv_pstRes->m_lExpire;
		struct tm timeinfo;
		localtime_s(&timeinfo, &t);
		char szTime[260]; memset(szTime, 0, sizeof(szTime));
		if (lv_pstRes->m_lExpire < 32503622400) {
			strftime(szTime, 260, "License valid : %Y-%m-%d", &timeinfo);
		}
		else {
			sprintf_s(szTime, 260, "License valid : NO LIMIT");
		}
		ostr << szTime;
	}
}