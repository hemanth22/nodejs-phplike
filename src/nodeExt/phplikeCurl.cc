#include "common.h"
#include "../cpp/curl/src/phplikeCppCurl.h"
#include "node_buffer.h"

string resHeader;


//Handle<Value> node_curl_request(Handle<Object> args) {
NAN_METHOD(node_curl_request) {
    int i, n, curlType = 0;
    NanScope();
    phplikeCppCurl *pCurl = new phplikeCppCurl();
    String::Utf8Value method(args[0]); 
    String::Utf8Value url(args[1]);
    Handle<Object> jsParam;
    Handle<Object> jsHeader;
    Handle<Object> jsOptions;
    Handle<Object> jsFileUpload;



    string paramStr;
    map<string, string> param;
    map<string, string> header;
    map<string, string> options;
    map<string, vector<string> > fileUpload;


    Handle<Array> propertyNames; 

    // Handle parameter 
    if (args[2]->IsObject()) {
        jsParam = Handle<Object>::Cast(args[2]);
        propertyNames = jsParam->GetPropertyNames();
        n = propertyNames->Length();
        for (i = 0; i < n ; i++) {
            Handle<Value>  b = propertyNames->Get(NanNew<Integer>(i));
            string c = string(*String::Utf8Value(b));
            Handle<Value>  v = jsParam->Get(b);
            param[c] = string(*String::Utf8Value(v));
        }
    } else if (args[2]->IsString()) {
        paramStr = string(*String::Utf8Value(args[2]));
        curlType = 1;
    }

    // Handle header
    if (args[3]->IsObject()) {
        jsHeader = Handle<Object>::Cast(args[3]);
        propertyNames = jsHeader->GetPropertyNames();
        n = propertyNames->Length();
        for (i = 0; i < n ; i++) {
            Handle<Value>  b = propertyNames->Get(NanNew<Integer>(i));
            string c = string(*String::Utf8Value(b));
            Handle<Value>  v = jsHeader->Get(b);
            header[c] = string(*String::Utf8Value(v));
        }

    }

    // Handle options
    if (args[4]->IsObject()) {
        jsOptions = Handle<Object>::Cast(args[4]);
        propertyNames = jsOptions->GetPropertyNames();
        n = propertyNames->Length();
        for (i = 0; i < n ; i++) {
            Handle<Value>  b = propertyNames->Get(NanNew<Integer>(i));
            string c = string(*String::Utf8Value(b));
            Handle<Value>  v = jsOptions->Get(b);
            options[c] = string(*String::Utf8Value(v));
        }

    }

    // Handle fileUpload
    if (args[5]->IsObject()) {
        jsFileUpload = Handle<Object>::Cast(args[5]);
        propertyNames = jsFileUpload->GetPropertyNames();
        n = propertyNames->Length();
 
        for (i = 0; i < n ; i++) {
            vector<string> fileInfo;
            //Handle<Value>  jsFileInfo = propertyNames->Get(NanNew<Integer>(i));
            Handle<Value>  b = propertyNames->Get(NanNew<Integer>(i));
            string c = string(*String::Utf8Value(b));

            Handle<Array>  v = Handle<Array>::Cast(jsFileUpload->Get(b));

            // handle file array
            Handle<Value> fileName = v->Get(NanNew<Integer>(0));
            Handle<Value> filePath = v->Get(NanNew<Integer>(1));
            fileInfo.push_back(string(*String::Utf8Value(fileName)));
            fileInfo.push_back(string(*String::Utf8Value(filePath)));

            fileUpload[c] = fileInfo;
        }

    }


    if (curlType == 1) {
        pCurl->request(string(*method), string(*url), paramStr, header, options, fileUpload);
    } else {
        pCurl->request(string(*method), string(*url), param, header, options, fileUpload);
    }

    //string content = pCurl->resContent;

    resHeader = pCurl->resHeader;
    if (pCurl->contentLength <= 0) {
        NanReturnValue(NanNew<String>(""));
    }

    // save binary data into js string.
    if (
        jsOptions->Has(NanNew<String>("BINARY_RESPONSE")) 
        && string(*String::Utf8Value(jsOptions->Get(NanNew<String>("BINARY_RESPONSE")))) == "1"
       ) {
        //node::Buffer *buffer = node::Buffer::New(pCurl->contentLength);
        //NanNewBufferHandle((char*)value.data(), value.size());
        NanReturnValue(NanNewBufferHandle(pCurl->resContentPointer, pCurl->contentLength));
        //memcpy(node::Buffer::Data(buffer), pCurl->resContentPointer, pCurl->contentLength);
        //return buffer->handle_;
    }

    NanReturnValue(NanNew<String>(pCurl->resContentPointer, pCurl->contentLength));
}


//Handle<Value> nodeCurlGetHeader(Handle<Object> args) {
NAN_METHOD(nodeCurlGetHeader) {
    NanScope();
    NanReturnValue(NanNew<String>(resHeader.c_str()));
}


