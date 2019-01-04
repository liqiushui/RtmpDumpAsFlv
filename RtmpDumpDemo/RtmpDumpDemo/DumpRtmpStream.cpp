//
//  DumpRtmpStream.cpp
//  RtmpDumpDemo
//
//  Created by lunli on 2019/1/4.
//  Copyright © 2019年 lunli. All rights reserved.
//

#include "DumpRtmpStream.hpp"
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#include <librtmp/rtmp.h>
#include <librtmp/log.h>

RtmpStreamDumper::RtmpStreamDumper(std::string &url, std::string dump_flv_path)
{
    this->rtmp_rsource_url = url;
    if(dump_flv_path.length() == 0) {
        dump_flv_path = this->tempFlvPath();
    }
    this->dump_flv_path = dump_flv_path;
}


std::string RtmpStreamDumper::tempFlvPath()
{
    std::string result = "";
    char cCurrentPath[FILENAME_MAX] = {0};
    if(GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
    {
        
        time_t t = time(0);   // get time now
        srand((int)t);
        struct tm * now = localtime(&t);
        char buffer [256] = {0};
        strftime (buffer,80,"%Y_%m_%d_%H_%M_%S",now);
        sprintf(buffer, "%s_%d.flv", buffer, rand());
        result = std::string(cCurrentPath) + "//" + std::string(buffer);
    }
    return result;
}


bool RtmpStreamDumper::dumpBytesToFlv(const unsigned char *buffer, unsigned int size)
{
    
    FILE *fp = fopen(this->dump_flv_path.c_str(), "ab");
    if(fp != NULL)
    {
        fwrite(buffer, size, 1, fp);
        fflush(fp);
        fclose(fp);
        return true;
    }
    
    return false;
}


void RtmpStreamDumper::startDump()
{
    int readBytes = 0;
    bool bLiveStream = true;
    int bufsize = 1024*1024*10;
    long countbufsize = 0;
    char *buf  = (char*)calloc(sizeof(char), bufsize);
    char *path = (char*)calloc(sizeof(char), this->rtmp_rsource_url.size() + 1);
    strcpy(path, this->rtmp_rsource_url.c_str());
    
    RTMP_LogPrintf("Start Dump To %s", this->dump_flv_path.c_str());

    
    RTMP *rtmp = RTMP_Alloc();
    RTMP_Init(rtmp);
    rtmp->Link.timeout=10;
    
    if(!RTMP_SetupURL(rtmp, path))
    {
        RTMP_Log(RTMP_LOGERROR,"SetupURL Err\n");
        RTMP_Free(rtmp);
        return;
    }
    
    if (bLiveStream){
        rtmp->Link.lFlags|=RTMP_LF_LIVE;
    }

    RTMP_SetBufferMS(rtmp, 3600*1000);

    if(!RTMP_Connect(rtmp,NULL)){
        RTMP_Log(RTMP_LOGERROR,"Connect Err\n");
        RTMP_Free(rtmp);
        return ;
    }
    
    if(!RTMP_ConnectStream(rtmp,0)){
        RTMP_Log(RTMP_LOGERROR,"ConnectStream Err\n");
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
        return ;
    }
    
    while((readBytes = RTMP_Read(rtmp,buf,bufsize))){
        this->dumpBytesToFlv((const unsigned char *)buf, readBytes);
        countbufsize += readBytes;
        RTMP_LogPrintf("Receive: %5dByte, Total: %5.2fkB\n",readBytes,countbufsize*1.0/1024);
    }

    if(buf){
        free(buf);
    }
    
    if(rtmp){
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
        rtmp=NULL;
    }

}
