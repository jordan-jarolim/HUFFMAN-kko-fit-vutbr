/*
 * Autor: Jordan Jarolim (xjarol03)
 * Datum: 16.2.2017
 * Soubor: main.c
 * Komentar:
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <signal.h>

#include "ahed.hpp"
#include "tree.hpp"

using namespace std;

/* name of temp file */
char buffer [80];

/* helper function to check if file exists */
inline bool fileExist (const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}

/* signal handler to close temp file */
void signalHandler( int signum ) {
    cout << "Interrupt signal (" << signum << ") received.\n";
    
    if (fileExist(buffer)){
        if( remove(buffer) != 0 ){
            perror( "Error removing file\n" );
            exit(-1);
        }
    }
    
    exit(signum);
    
}


/* main function */
int main(int argc, char **argv)
{
    int c;
    string inputFileName = "", outputFileName = "", logFileName = "";
    ifstream myReadFile;
    FILE * inputFile, * outputFile, *logFile;
    
    bool encode = false;
    bool decode = false;
    bool help = false;
    tAHED ahed;

    /* register all possible signals */
    signal(SIGINT, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);


    /* parse parameters */
    while ((c = getopt (argc, argv, "i:o:l:cxh")) != -1)
    
    switch (c){
        case 'i':
            inputFileName = optarg;
            break;
        case 'o':
            outputFileName = optarg;
            break;
        case 'l':
            logFileName = optarg;
            break;
        case 'c':
            encode = true;
            break;
        case 'x':
            decode = true;
            break;
        case 'h':
            help = true;
            break;
    }

    if (!encode && !decode && !help)
        exit(0);
    
    if(help){
        cout <<"aplikace pro demonstraci adaptivniho huffmanova kodovani\n";
        cout << "-i <input>: input file, -o <output>: output file, -c: coding, -x decoding, -h help, -l <logfile>: logfile\n";
        exit(0);
    }
    
    /* Get timestamp for helper temp file */
    time_t rawtime;
    struct tm * timeinfo;
    
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (buffer,80,"%Y%m%d%H%M%S",timeinfo);
    
    /* to confert filenames */
    char inputFileNameArr[1024];
    char outputFileNameArr[1024];
    char logFileNameArr[1024];

    
    /* We have to create temp file */
    if (inputFileName == ""){
        string input;
        char F;
        /* read input */
        while((F = getchar()) != EOF && !feof(stdin)){
            input += F;
        }

        /* cpy input to temp file */
        strncpy(inputFileNameArr, buffer, sizeof(inputFileNameArr));
        inputFileNameArr[sizeof(inputFileNameArr) - 1] = 0;
        inputFile = fopen (inputFileNameArr , "w");
         
        fprintf(inputFile, "%s", input.c_str());
        fclose(inputFile);


    }else{
        strncpy(inputFileNameArr, inputFileName.c_str(), sizeof(inputFileNameArr));
        inputFileNameArr[sizeof(inputFileNameArr) - 1] = 0;
    }
    
    inputFile = fopen (inputFileNameArr , "r");
    

    strncpy(outputFileNameArr, outputFileName.c_str(), sizeof(outputFileNameArr));
    outputFileNameArr[sizeof(outputFileNameArr) - 1] = 0;
    
   /* if (encode)
        outputFile = fopen (outputFileNameArr , "w");
    else{*
        if (fileExist(outputFileNameArr)){
            if( remove(outputFileNameArr) != 0 ){
                perror( "Error initializing file\n" );
                exit(-1);
            }
        }*/
        outputFile = fopen (outputFileNameArr , "w");
    //}
    
    if (encode){
        AHEDEncoding(&ahed, inputFile, outputFile);
        
    }else if (decode){
         AHEDDecoding(&ahed, inputFile, outputFile);
    }
    
    if (logFileName != ""){
        string log = "";
        log = "login = xjarol03\nuncodedSize = "+to_string(ahed.uncodedSize)+"\ncodedSize = "+to_string(ahed.codedSize);
        
        strncpy(logFileNameArr, logFileName.c_str(), sizeof(logFileNameArr));
        logFileNameArr[sizeof(logFileNameArr) - 1] = 0;
        logFile = fopen (logFileNameArr , "w");
        fprintf(logFile, "%s", log.c_str());
        fclose(logFile);

    }
    
    
    if (inputFile)
        fclose(inputFile);
    if (outputFile)
        fclose(outputFile);
    
    /* Delete temp file */
    if (inputFileName == ""){
        if (fileExist(buffer)){
            if( remove(buffer) != 0 ){
                perror( "Error removing file\n" );
                exit(-1);
            }
        }
    }

    exit(0);
}



