//
//  main.cpp
//  smartjson
//
//  Created by youlanhai on 16/1/1.
//  Copyright © 2016年 youlanhai. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>

#include "smartjson.hpp"

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cout << "usage: smartjson input [output]" << std::endl;
        return 0;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile;
    if(argc > 2)
    {
        outputFile = argv[2];
    }
    
    if(outputFile.empty())
    {
        outputFile = inputFile + ".out";
    }
    
    mjson::Parser parser;
    if(mjson::RC_OK != parser.parseFromFile(inputFile.c_str()))
    {
        std::cout << "Parse json Failed" << std::endl;
        return 0;
    }
    
    std::ofstream of(outputFile.c_str());
    mjson::Writer writer;
    writer.write(parser.getRoot(), of);
    of.close();
    return 0;
}
