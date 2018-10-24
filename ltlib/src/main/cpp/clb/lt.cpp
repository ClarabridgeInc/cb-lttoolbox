#include "clb_stream.h"

#include <lttoolbox/fst_processor.h>
#include <lttoolbox/my_stdio.h>
#include <lttoolbox/lt_locale.h>
#include <lttoolbox/exception.h>

#include <google/protobuf/stubs/strutil.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
//#include <thread>
#include <functional>

void
printArgs(int argc, char *argv[])
{
    std::wcout << "args:" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::wcout << argv[i] << std::endl;
    }
}

static std::string EXPECTED_RES("ChMIARoP4KaW4Ka+4Kay4Ka/4KamCgYQBRoCICAKYggCEAcaFeCmnOCmvuCmruCmv+CmsuCmleCnhyJFCg/gppzgpr7gpq7gpr/gprISMv7//////////wHz//////////8B4P//////////Adf//////////wHT//////////8BCgUQDhoBIAqtAQgCEA8aDOCmpuCnh+CmluCnhyIhCgngpqbgp4fgppYSFPb//////////wHB//////////8BIjUKCeCmpuCnh+CmlhIo9v//////////Acf//////////wHI//////////8Bzf//////////ASI/Cgngpqbgp4fgppYSMvb//////////wHH//////////8ByP//////////Ac7//////////wHi//////////8BCgUQExoBIApNCAIQFBoJ4Kau4Kao4KeHIjwKBuCmruCmqBIy////////////Ad7//////////wHc//////////8B1///////////AdH//////////wEKBRAXGgEgCoEBCAIQGBoP4Ka54Kaa4KeN4Kab4KeHIi8KA+CmuRIo9v//////////Acf//////////wHG//////////8Bzf//////////ASI5CgPgprkSMvb//////////wHH//////////8Bxv//////////Ac7//////////wHi//////////8BCgUQHRoBIAokCAEQHhoe4KaW4Ka+4KaB4Kaa4Ka+4Kas4Kao4KeN4Kam4Ka/CggQKBoEIOKAmApQCAIQKhoJ4Kas4Ka+4KaYIj8KCeCmrOCmvuCmmBIy////////////AeD//////////wHb//////////8B1///////////AdT//////////wEKBxAtGgPigJkKGAgCEC4aASEiDwoBIRIKv///////////AQ==");

void
doSingle(FSTProcessor &fstp, const char *inFileName)
{
    clb_writer_proto w;
    clb_stream_wif ins(inFileName);

    // clb_empty_writer w; //clb_writer w;
    fstp.analysis(ins.getStrStream(), w);

    std::string sOut("");
    google::protobuf::Base64Escape(w.get_serialized(), &sOut);

    /*if (sOut != EXPECTED_RES)
    {
        throw LttException("Invalid Res!");
    }*/

    std::wstring wOut(L"");
    simple_cvt::utf8_to_utf16(sOut, wOut);
    std::wcout << L"escaped result: " << wOut << std::endl;
}

/*
void
doMulti(FSTProcessor &fstp, const char *inFileName, size_t idx)
{
    std::wcout << L"thread idx: " << idx << std::endl;

    for (size_t i = 0; i < 100000 * 10; ++i)
    {
        doSingle(fstp, inFileName);
    }
}

void
runMulti(FSTProcessor &fstp, const char *inFileName, size_t num_threads)
{
    typedef std::vector<std::thread> t_thread_vec;

    t_thread_vec threads;
    for (size_t t = 0; t < num_threads; ++t)
    {
        auto r = std::bind(doMulti, fstp, inFileName, t);
        threads.emplace_back(r);
    }

    for (t_thread_vec::iterator itr = threads.begin(); itr != threads.end(); ++itr)
    {
        itr->join();
    }
}
*/

int
main(int argc, char *argv[])
{
/*
    const char *fstFileName = "ben/ben.automorf.bin";
    const char *inFileName  = "data/in.txt";
*/
    const char *fstFileName = argv[1];
    const char *inFileName  = argv[2];

    FSTProcessor fstp; // -w option // on-by-default // fstp.setDictionaryCaseMode(true)

    LtLocale::tryToSetLocale();

    {
        std::ifstream ifs(fstFileName, std::ifstream::binary);
        if (!ifs) {
            exit(EXIT_FAILURE);
        }
        fstp.load(ifs); // hFst.get()
    }

    try
    {
        /*
        do
        {
            wchar_t ch = ins.getWC();
            if (!ins.eof())
            {
                std::wcout << L": " << ch << std::endl;
            }
        }
        while (!ins.eof());
        */

        fstp.initAnalysis();
        if (!fstp.valid())
        {
            exit(EXIT_FAILURE);
        }

        //runMulti(fstp, inFileName, 4);
        doSingle(fstp, inFileName); 
    }
    catch (std::exception &e)
    {
        std::wcerr << e.what();
        exit(1);
    }

    return EXIT_SUCCESS;
}
