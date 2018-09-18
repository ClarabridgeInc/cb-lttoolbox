package ltt;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.assertEquals;

public class LttTests {
    @Test
    void justAnExample() {
        try (LttLibrary.FSTProc l = new LttLibrary.FSTProc()) {
            assertTrue(l.init("../ben/ben.automorf.bin"));

            String actualRes = l.analyze("খালিদ  জামিলকে দেখে মনে হচ্ছে খাঁচাবন্দি ‘বাঘ’..!");

            String expectedRes = "CgYIARoCICAKYggCEAcaFeCmnOCmvuCmruCmv+CmsuCmleCnhyJFCg/gppzgpr7gpq7gpr/gprISMv7//////////wHz//////////8B4P//////////Adf//////////wHT//////////8BCgUQDhoBIAojIiEKCeCmpuCnh+CmlhIU9v//////////AcH//////////wEKNyI1Cgngpqbgp4fgppYSKPb//////////wHH//////////8ByP//////////Ac3//////////wEKUwgCEA8aDOCmpuCnh+CmluCnhyI/Cgngpqbgp4fgppYSMvb//////////wHH//////////8ByP//////////Ac7//////////wHi//////////8BCgUQExoBIApNCAIQFBoJ4Kau4Kao4KeHIjwKBuCmruCmqBIy////////////Ad7//////////wHc//////////8B1///////////AdH//////////wEKBRAXGgEgCjEiLwoD4Ka5Eij2//////////8Bx///////////Acb//////////wHN//////////8BClAIAhAYGg/gprngpprgp43gppvgp4ciOQoD4Ka5EjL2//////////8Bx///////////Acb//////////wHO//////////8B4v//////////AQoFEB0aASAKCggBEB4aBCDigJgKUAgCECoaCeCmrOCmvuCmmCI/Cgngpqzgpr7gppgSMv///////////wHg//////////8B2///////////Adf//////////wHU//////////8BCgcQLRoD4oCZChgIAhAuGgEuIg8KAS4SCr///////////wEKGAgCEC8aAS4iDwoBLhIKv///////////AQ==";

            assertEquals(expectedRes, actualRes);
        }
    }
}
