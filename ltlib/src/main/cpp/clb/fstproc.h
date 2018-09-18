#pragma once

#include <lttoolbox/fst_processor.h>
#include <lttoolbox/my_stdio.h>
#include <lttoolbox/lt_locale.h>

#include <google/protobuf/stubs/strutil.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <locale>
#include <codecvt>

namespace ltt
{

class FSTProc
{
private:
    FSTProcessor fstp; // -w option // on-by-default // fstp.setDictionaryCaseMode(true)
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    /*
    void
    printWC(const void *pStr, size_t len, size_t size)
    {
        std::cout << std::hex << std::setfill('0');  // setfill needs to be set only once
        unsigned char *ptr = reinterpret_cast<unsigned char *>(const_cast<void *>(pStr));
        for (size_t i = 0; i < len * size; ++i, ptr++)
        {
            std::cout << std::setw(2) << static_cast<unsigned>(*ptr) << " ";
        }
        std::cout << std::endl;
    }
    */

public:
    FSTProc()
    {
        // std::cout << "FSTProc::FSTProc() !" << std::endl;
    }

    ~FSTProc()
    {
        // std::cout << "FSTProc::~FSTProc() !" << std::endl;
    }

    bool
    init(const std::string &fstFileName)
    {
        // LtLocale::tryToSetLocale();

        {
            std::ifstream ifs(fstFileName, std::ifstream::binary);
            fstp.load(ifs); // hFst.get()
        }

        fstp.initAnalysis();
        return fstp.valid();
    }

    std::string
    analyze(const std::string &text)
    {
        clb_writer_proto w;
        std::wstring convertedText = converter.from_bytes(text);
        clb_stream_wstring ins(convertedText); 

        fstp.analysis(ins, w);

        std::string result("");
        google::protobuf::Base64Escape(w.get_serialized(), &result);
        return result;
    }
};

} // ltt
