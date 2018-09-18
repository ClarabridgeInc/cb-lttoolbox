package ltt;

import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.Platform;
import org.bytedeco.javacpp.annotation.Namespace;
import org.bytedeco.javacpp.annotation.StdString;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.CharBuffer;

@Platform(include="clb/fstproc.h")
@Namespace("ltt")
public class LttLibrary {

    public static class FSTProc extends Pointer {
        static { Loader.load(); }
        public FSTProc() { allocate(); }
        private native void allocate();

        public native boolean init(@StdString String fstFileName);
        public native @StdString String analyze(@StdString String text);
    }

    public static void main(String[] args) {
        try (FSTProc l = new FSTProc()) {
            if (!l.init("ben/ben.automorf.bin")) {
                throw new RuntimeException("abc def!");
            }
            String res = l.analyze("খালিদ  জামিলকে দেখে মনে হচ্ছে খাঁচাবন্দি ‘বাঘ’..!");
            System.out.println(res);
        }
    }
}
