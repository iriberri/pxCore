#ifndef PX_FILE_DOWNLOADER_H
#define PX_FILE_DOWNLOADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rtString.h"
#include "rtCore.h"
#ifdef ENABLE_HTTP_CACHE
#include <pxFileCache.h>
#endif
using namespace std;

class pxFileDownloadRequest
{
public:
    pxFileDownloadRequest(const char* imageUrl, void* callbackData) 
      : mFileUrl(imageUrl), mProxyServer(),
    mErrorString(), mHttpStatusCode(0), mCallbackFunction(NULL),
    mDownloadedData(0), mDownloadedDataSize(), mDownloadStatusCode(0) ,mCallbackData(callbackData),
    mCallbackFunctionMutex(), mHeaderData(0), mHeaderDataSize(0), mHeaderOnly(false)
#ifdef ENABLE_HTTP_CACHE
    , mCacheEnabled(true)
#endif
  {
    mAdditionalHttpHeaders.clear();
  }
        
  ~pxFileDownloadRequest()
  {
    if (mDownloadedData  != NULL)
      free(mDownloadedData);
    mDownloadedData = NULL;
    if (mHeaderData != NULL)
      free(mHeaderData);
    mHeaderData = NULL;
    mAdditionalHttpHeaders.clear();
    mHeaderOnly = false;
  }
  
  void setFileUrl(const char* imageUrl) { mFileUrl = imageUrl; }
  rtString getFileUrl() const { return mFileUrl; }
    
  void setProxy(const char* proxyServer)
  {
    mProxyServer = proxyServer;
  }
    
  rtString getProxy() const
  {
    return mProxyServer;
  }
  
  void setErrorString(const char* errorString)
  {
    mErrorString = errorString;
  }
    
  rtString getErrorString()
  {
    return mErrorString;
  }
  
  void setCallbackFunction(void (*callbackFunction)(pxFileDownloadRequest*))
  {
    mCallbackFunction = callbackFunction;
  }

  void setCallbackFunctionThreadSafe(void (*callbackFunction)(pxFileDownloadRequest*))
  {
    mCallbackFunctionMutex.lock();
    mCallbackFunction = callbackFunction;
    mCallbackFunctionMutex.unlock();
  }
  
  long getHttpStatusCode()
  {
    return mHttpStatusCode;
  }
  
  void setHttpStatusCode(long statusCode)
  {
    mHttpStatusCode = statusCode;
  }
    
  bool executeCallback(int statusCode)
  {
    mDownloadStatusCode = statusCode;
    mCallbackFunctionMutex.lock();
    if (mCallbackFunction != NULL)
    {
      (*mCallbackFunction)(this);
      mCallbackFunctionMutex.unlock();
      return true;
    }
    mCallbackFunctionMutex.unlock();
    return false;
  }
  
  void setDownloadedData(char* data, size_t size)
  {
    mDownloadedData = data;
    mDownloadedDataSize = size;
  }
  
  void getDownloadedData(char*& data, size_t& size)
  {
    data = mDownloadedData;
    size = mDownloadedDataSize; 
  }
  
  char* getDownloadedData()
  {
    return mDownloadedData;
  }
  
  size_t getDownloadedDataSize()
  {
    return mDownloadedDataSize;
  }

  void setHeaderData(char* data, size_t size)
  {
    mHeaderData = data;
    mHeaderDataSize = size;
  }

  char* getHeaderData()
  {
    return mHeaderData;
  }

  size_t getHeaderDataSize()
  {
    return mHeaderDataSize;
  }

  /*  Function to set additional http headers */
  void setAdditionalHttpHeaders(vector<rtString>& additionalHeaders)
  {
    mAdditionalHttpHeaders = additionalHeaders;
  }

  vector<rtString>& getAdditionalHttpHeaders()
  {
    return mAdditionalHttpHeaders;
  }

  void setDownloadStatusCode(int statusCode)
  {
    mDownloadStatusCode = statusCode;
  }
  
  int getDownloadStatusCode()
  {
    return mDownloadStatusCode;
  }
  
  void* getCallbackData()
  {
    return mCallbackData;
  }
  
  void setCallbackData(void* callbackData)
  {
    mCallbackData = callbackData;
  }

  /* Function used to set to download only header or not */
  void setHeaderOnly(bool val)
  {
    mHeaderOnly = val;
  }

  bool getHeaderOnly()
  {
    return mHeaderOnly;
  }

#ifdef ENABLE_HTTP_CACHE
  /* Function used to enable or disable using file cache */
  void setCacheEnabled(bool val)
  {
    mCacheEnabled = val;
  }

  bool getCacheEnabled()
  {
    return mCacheEnabled;
  }
#endif
private:
  rtString mFileUrl;
  rtString mProxyServer;
  rtString mErrorString;
  long mHttpStatusCode;
  void (*mCallbackFunction)(pxFileDownloadRequest*);
  char* mDownloadedData;
  size_t mDownloadedDataSize;
  int mDownloadStatusCode;
  void* mCallbackData;
  rtMutex mCallbackFunctionMutex;
  char* mHeaderData;
  size_t mHeaderDataSize;
  vector<rtString> mAdditionalHttpHeaders;
  bool mHeaderOnly;
#ifdef ENABLE_HTTP_CACHE
  bool mCacheEnabled;
#endif
};

class pxFileDownloader
{
public:

    static pxFileDownloader* getInstance();

    virtual bool addToDownloadQueue(pxFileDownloadRequest* downloadRequest);
    virtual void raiseDownloadPriority(pxFileDownloadRequest* downloadRequest);
    virtual void removeDownloadRequest(pxFileDownloadRequest* downloadRequest);

    void clearFileCache();
    void downloadFile(pxFileDownloadRequest* downloadRequest);
    void setDefaultCallbackFunction(void (*callbackFunction)(pxFileDownloadRequest*));
    bool downloadFromNetwork(pxFileDownloadRequest* downloadRequest);

private:
    pxFileDownloader();
    ~pxFileDownloader();

    void startNextDownload(pxFileDownloadRequest* downloadRequest);
    pxFileDownloadRequest* getNextDownloadRequest();
    void startNextDownloadInBackground();
    void downloadFileInBackground(pxFileDownloadRequest* downloadRequest);
#ifdef ENABLE_HTTP_CACHE
    bool checkAndDownloadFromCache(pxFileDownloadRequest* downloadRequest,rtHttpCacheData& cachedData);
#endif
    //todo: hash mPendingDownloadRequests;
    //todo: string list mPendingDownloadOrderList;
    //todo: list mActiveDownloads;
    unsigned int mNumberOfCurrentDownloads;
    //todo: hash m_priorityDownloads;
    void (*mDefaultCallbackFunction)(pxFileDownloadRequest*);
    
    static pxFileDownloader* mInstance;
};

#endif //PX_FILE_DOWNLOADER_H
