#pragma once

#include <lttoolbox/my_stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>

class clb_stream_stdio
{
private:
    FILE *pF;
public:
    clb_stream_stdio(const char *file_name, const char *mode)
    {
        pF = fopen(file_name, mode);
    }
    ~clb_stream_stdio()
    {
        fclose(pF);
    }
    bool eof()
    {
        return feof(pF);
    }
    wchar_t getWC()
    {
        wchar_t result = static_cast<wchar_t>(fgetwc_unlocked(pF));
        //std::wcout << L": " << result << std::endl;
        return result;
    }
};

class clb_stream_wstring
{
private:
    std::wstringstream s; 
public:
    clb_stream_wstring(const wchar_t *str)
    :
        s(str)
    {
    }
    bool eof()
    {
        return s.eof();
    }
    wchar_t getWC()
    {
        wchar_t result;
        // >>-operator can't be used - it eats spaces
        s.get(result); //std::wcout << L": " << result << std::endl;
        return result;
    }
};

class clb_stream_wif
{
private:
    wchar_t *pData;
    clb_stream_wstring *pS;
public:
    clb_stream_wif(const char *file_name)
    :
        pData(NULL),
        pS(NULL)
    {
        std::wifstream s(file_name, std::wifstream::binary);

        // std::locale loc("en_US.UTF8");
        std::locale loc(std::locale(std::locale::classic(), "", std::locale::ctype)); 
        s.imbue(loc);

        s.seekg(0, s.end);
        std::wifstream::pos_type file_size = s.tellg();
        s.seekg(0, s.beg);

        pData = new wchar_t[static_cast<std::size_t>(file_size) + 1];
        s.get(pData, file_size);
        pData[file_size] = 0;

        // (*pContents) << s.rdbuf();
        pS = new clb_stream_wstring(pData);
    }
    ~clb_stream_wif()
    {
        delete pS;
        delete pData;
    }
    clb_stream_wstring &getStrStream()
    {
        return *pS;
    }
};

typedef clb_stream_wstring &clb_stream_t;
