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

            BytePointer res = l.analyze("খালিদ  জামিলকে দেখে মনে হচ্ছে খাঁচাবন্দি ‘বাঘ’..!");
            assertNotNull(res);
            byte [] actualBytes = res.getStringBytes();
            assertNotNull(actualBytes);
            String actualRes = encoder.encode(actualBytes);

            String expectedRes = "CgYIARoCICAKYggCEAcaFeCmnOCmvuCmruCmv+CmsuCmleCnhyJFCg/gppzgpr7gpq7gpr/gprISMv7//////////wHz//////////8B4P//////////Adf//////////wHT//////////8BCgUQDhoBIAojIiEKCeCmpuCnh+CmlhIU9v//////////AcH//////////wEKNyI1Cgngpqbgp4fgppYSKPb//////////wHH//////////8ByP//////////Ac3//////////wEKUwgCEA8aDOCmpuCnh+CmluCnhyI/Cgngpqbgp4fgppYSMvb//////////wHH//////////8ByP//////////Ac7//////////wHi//////////8BCgUQExoBIApNCAIQFBoJ4Kau4Kao4KeHIjwKBuCmruCmqBIy////////////Ad7//////////wHc//////////8B1///////////AdH//////////wEKBRAXGgEgCjEiLwoD4Ka5Eij2//////////8Bx///////////Acb//////////wHN//////////8BClAIAhAYGg/gprngpprgp43gppvgp4ciOQoD4Ka5EjL2//////////8Bx///////////Acb//////////wHO//////////8B4v//////////AQoFEB0aASAKCggBEB4aBCDigJgKUAgCECoaCeCmrOCmvuCmmCI/Cgngpqzgpr7gppgSMv///////////wHg//////////8B2///////////Adf//////////wHU//////////8BCgcQLRoD4oCZChgIAhAuGgEuIg8KAS4SCr///////////wEKGAgCEC8aAS4iDwoBLhIKv///////////AQ==";

            assertEquals(expectedRes, actualRes);
        }
    }
}
