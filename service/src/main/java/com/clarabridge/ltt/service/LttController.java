package com.clarabridge.ltt.service;

import org.springframework.beans.factory.annotation.Autowired;

import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.google.common.io.BaseEncoding;
import com.google.protobuf.util.JsonFormat;

import lombok.extern.slf4j.Slf4j;

@RestController
@RequestMapping(produces = "application/text")
@Slf4j
public class LttController {
    private BaseEncoding encoder = BaseEncoding.base64();

    @Autowired
    private LttService lttService;

    // Main analyze endpoint, returns BASE64 representation of protobuf response.
    @CrossOrigin
    @GetMapping("/analyze")
    public String analyze(@RequestParam(value = "text") String text) throws Exception {
        log.info("analyzing text: {}", text);
        byte [] resData = lttService.analyze(text);
        return encoder.encode(resData);
    }
}
