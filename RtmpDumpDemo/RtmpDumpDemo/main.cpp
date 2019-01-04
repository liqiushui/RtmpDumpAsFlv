//
//  main.cpp
//  RtmpDumpDemo
//
//  Created by lunli on 2019/1/4.
//  Copyright © 2019年 lunli. All rights reserved.
//

#include <iostream>
#include "DumpRtmpStream.hpp"

int main(int argc, const char * argv[]) {
    std::cout<<"use example :RtmpDumper [rtmp_live_url] [flv_save_path(default to excute folder)]"<<std::endl;
    std::string url((argc > 1)?argv[1]:"");
    std::string path((argc > 2)?argv[2]:"");
    RtmpStreamDumper *dp = new RtmpStreamDumper(url, path);
    dp->startDump();
    return 0;
}
