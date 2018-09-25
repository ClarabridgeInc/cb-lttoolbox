#pragma once

#include <lttoolbox/fst_processor.h>
#include <lttoolbox/my_stdio.h>

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

public:
    bool
    init(const std::string &fstFileName)
    {
        std::ifstream ifs(fstFileName, std::ifstream::binary);
        fstp.load(ifs);

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
