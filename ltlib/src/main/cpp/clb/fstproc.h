#pragma once

#include "lttoolbox/fst_processor.h"
#include "lttoolbox/compression.h"
#include "lttoolbox/my_stdio.h"

#include <google/protobuf/stubs/strutil.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <locale>

namespace ltt
{

class FSTProc
{
private:
    FSTProcessor fstp; // -w option // on-by-default // fstp.setDictionaryCaseMode(true)

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
        std::wstring converted(L"");
        if (!simple_cvt::utf8_to_utf16(text, converted))
        {
            throw std::runtime_error("FSTProc::analyze: utf8_to_utf16: invalid utf-8 input found");
        }
        // simple_cvt::dump_hex("text->utf16", converted.c_str(), converted.size());

        // std::string convertedBack("");
        // if (!simple_cvt::utf16_to_utf8(converted, convertedBack))
        //{
        //    throw std::runtime_error("utf16_to_utf8: invalid utf-16 input found");
        //}
        // simple_cvt::dump_hex("text", text.c_str(), text.size());
        // simple_cvt::dump_hex("converted back", convertedBack.c_str(), convertedBack.size());

        clb_stream_wstring ins(converted); 
        clb_writer_proto w;
        fstp.analysis(ins, w);

        std::string result("");
        google::protobuf::Base64Escape(w.get_serialized(), &result);
        return result;
    }
};

} // ltt
