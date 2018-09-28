package ltt;

import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.BytePointer;
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

        public native boolean initFromFile(@StdString String fstFileName);
        public native boolean initFromMemory(@StdString BytePointer fstBytePtr);

        public native @StdString BytePointer analyze(@StdString String text);
    }
}
