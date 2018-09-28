package com.clarabridge.ltt.service;

import ltt.LttLibrary;

import org.bytedeco.javacpp.BytePointer;

import org.springframework.stereotype.Service;

import org.apache.commons.io.IOUtils;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;

import java.io.InputStream;

// import lombok.val;
import lombok.extern.slf4j.Slf4j;

@Service
@Slf4j
public class LttService {
    private LttLibrary.FSTProc fstProc;

    @PostConstruct
    public void postConstruct() throws Exception {
        fstProc = new LttLibrary.FSTProc();

        InputStream resStream = getR("ben.automorf.bin");
        byte [] resByteArray = IOUtils.toByteArray(resStream);
        BytePointer resBytePtr = new BytePointer(resByteArray);
        
        // TODO: parameterize or embed!
        if (fstProc.initFromMemory(resBytePtr)) {
            log.info("cb-lttoolbox has been successfully initialized");
        } else {
            log.error("cb-lttoolbox is not initialized");
            throw new RuntimeException("cb-lttoolbox is not initialized!");
        }
    }
    
    @PreDestroy
    public void preDestroy() throws Exception {
        fstProc.close();
    }

    private static InputStream getR(final String resName) {
        return LttService.class.getResourceAsStream("/" + resName);
    }

    public byte [] analyze(String text) {
        BytePointer res = fstProc.analyze(text);
        byte [] result = res.getStringBytes();
        return result;
    }
}
