package ltt;

import org.junit.jupiter.api.Test;

import org.bytedeco.javacpp.BytePointer;

import org.apache.commons.io.IOUtils;

import com.google.common.io.BaseEncoding;

import java.io.InputStream;

import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;

public class LttTests {

    private static InputStream getR(final String resName) {
        return LttTests.class.getResourceAsStream("/" + resName);
    }

    @Test
    void justAnExample() throws Exception {
        BaseEncoding encoder = BaseEncoding.base64();
        InputStream resStream = getR("ben.automorf.bin");
        assertNotNull(resStream);
        byte [] resByteArray = IOUtils.toByteArray(resStream);
        assertNotNull(resByteArray);
        // assertEquals(91423, resByteArray.length);

        try (LttLibrary.FSTProc l = new LttLibrary.FSTProc()) {
            BytePointer resBytePtr = new BytePointer(resByteArray);
            assertNotNull(resBytePtr);

            assertTrue(l.initFromMemory(resBytePtr));

            BytePointer res = l.analyze("খালিদ  জামিলকে দেখে মনে হচ্ছে খাঁচাবন্দি ‘বাঘ’!.");

            assertNotNull(res);
            byte [] actualBytes = res.getStringBytes();
            assertNotNull(actualBytes);
            String actualRes = encoder.encode(actualBytes);

            String expectedRes = "ChMIARoP4KaW4Ka+4Kay4Ka/4KamCgYQBRoCICAKYggCEAcaFeCmnOCmvuCmruCmv+CmsuCmleCnhyJFCg/gppzgpr7gpq7gpr/gprISMv7//////////wHz//////////8B4P//////////Adf//////////wHT//////////8BCgUQDhoBIAqtAQgCEA8aDOCmpuCnh+CmluCnhyIhCgngpqbgp4fgppYSFPb//////////wHB//////////8BIjUKCeCmpuCnh+CmlhIo9v//////////Acf//////////wHI//////////8Bzf//////////ASI/Cgngpqbgp4fgppYSMvb//////////wHH//////////8ByP//////////Ac7//////////wHi//////////8BCgUQExoBIApNCAIQFBoJ4Kau4Kao4KeHIjwKBuCmruCmqBIy////////////Ad7//////////wHc//////////8B1///////////AdH//////////wEKBRAXGgEgCoEBCAIQGBoP4Ka54Kaa4KeN4Kab4KeHIi8KA+CmuRIo9v//////////Acf//////////wHG//////////8Bzf//////////ASI5CgPgprkSMvb//////////wHH//////////8Bxv//////////Ac7//////////wHi//////////8BCgUQHRoBIAokCAEQHhoe4KaW4Ka+4KaB4Kaa4Ka+4Kas4Kao4KeN4Kam4Ka/CggQKBoEIOKAmApQCAIQKhoJ4Kas4Ka+4KaYIj8KCeCmrOCmvuCmmBIy////////////AeD//////////wHb//////////8B1///////////AdT//////////wEKBxAtGgPigJkKGAgCEC4aASEiDwoBIRIKv///////////AQ==";

            assertEquals(expectedRes, actualRes);
        }
    }
}
