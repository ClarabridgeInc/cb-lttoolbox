package com.clarabridge.ltt.service;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.extension.ExtendWith;

import org.springframework.http.HttpEntity;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.util.UriComponentsBuilder;

import org.springframework.boot.web.server.LocalServerPort;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.client.TestRestTemplate;

import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.junit.jupiter.SpringExtension;

import java.net.URI;

import static org.assertj.core.api.Assertions.assertThat;
 
@ActiveProfiles("prod") // dev
@ExtendWith(SpringExtension.class)
@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
public class LttTests {
    @Autowired
    private TestRestTemplate restTemplate;

    @LocalServerPort
    private int port;
 
    // without space at the end
    // String expectedRes = "ChMIARoP4KaW4Ka+4Kay4Ka/4KamCgYQBRoCICAKYggCEAcaFeCmnOCmvuCmruCmv+CmsuCmleCnhyJFCg/gppzgpr7gpq7gpr/gprISMv7//////////wHz//////////8B4P//////////Adf//////////wHT//////////8BCgUQDhoBIAqtAQgCEA8aDOCmpuCnh+CmluCnhyIhCgngpqbgp4fgppYSFPb//////////wHB//////////8BIjUKCeCmpuCnh+CmlhIo9v//////////Acf//////////wHI//////////8Bzf//////////ASI/Cgngpqbgp4fgppYSMvb//////////wHH//////////8ByP//////////Ac7//////////wHi//////////8BCgUQExoBIApNCAIQFBoJ4Kau4Kao4KeHIjwKBuCmruCmqBIy////////////Ad7//////////wHc//////////8B1///////////AdH//////////wEKBRAXGgEgCoEBCAIQGBoP4Ka54Kaa4KeN4Kab4KeHIi8KA+CmuRIo9v//////////Acf//////////wHG//////////8Bzf//////////ASI5CgPgprkSMvb//////////wHH//////////8Bxv//////////Ac7//////////wHi//////////8BCgUQHRoBIAokCAEQHhoe4KaW4Ka+4KaB4Kaa4Ka+4Kas4Kao4KeN4Kam4Ka/CggQKBoEIOKAmApQCAIQKhoJ4Kas4Ka+4KaYIj8KCeCmrOCmvuCmmBIy////////////AeD//////////wHb//////////8B1///////////AdT//////////wEKBxAtGgPigJkKGAgCEC4aASEiDwoBIRIKv///////////AQ==";
    private static final String EXPECTED_RES = "ChMIARoP4KaW4Ka+4Kay4Ka/4KamCgYQBRoCICAKYggCEAcaFeCmnOCmvuCmruCmv+CmsuCmleCnhyJFCg/gppzgpr7gpq7gpr/gprISMv7//////////wHz//////////8B4P//////////Adf//////////wHT//////////8BCgUQDhoBIAqtAQgCEA8aDOCmpuCnh+CmluCnhyIhCgngpqbgp4fgppYSFPb//////////wHB//////////8BIjUKCeCmpuCnh+CmlhIo9v//////////Acf//////////wHI//////////8Bzf//////////ASI/Cgngpqbgp4fgppYSMvb//////////wHH//////////8ByP//////////Ac7//////////wHi//////////8BCgUQExoBIApNCAIQFBoJ4Kau4Kao4KeHIjwKBuCmruCmqBIy////////////Ad7//////////wHc//////////8B1///////////AdH//////////wEKBRAXGgEgCoEBCAIQGBoP4Ka54Kaa4KeN4Kab4KeHIi8KA+CmuRIo9v//////////Acf//////////wHG//////////8Bzf//////////ASI5CgPgprkSMvb//////////wHH//////////8Bxv//////////Ac7//////////wHi//////////8BCgUQHRoBIAokCAEQHhoe4KaW4Ka+4KaB4Kaa4Ka+4Kas4Kao4KeN4Kam4Ka/CggQKBoEIOKAmApQCAIQKhoJ4Kas4Ka+4KaYIj8KCeCmrOCmvuCmmBIy////////////AeD//////////wHb//////////8B1///////////AdT//////////wEKBxAtGgPigJkKGAgCEC4aASEiDwoBIRIKv///////////AQoYCAIQLxoBLiIPCgEuEgq///////////8BCgUQMBoBIA==";

    @Test
    public void testProcess() throws Exception {
        LttProcessRequest lttProcessRequest = new LttProcessRequest();
        lttProcessRequest.setText("খালিদ  জামিলকে দেখে মনে হচ্ছে খাঁচাবন্দি ‘বাঘ’!.");
        HttpEntity<LttProcessRequest> request = new HttpEntity<>(lttProcessRequest);

        String url = "http://localhost:" + this.port;
        URI uri = UriComponentsBuilder.fromHttpUrl(url).path("/process")
            .build().toUri();

        ResponseEntity<LttProcessResponse> response = this.restTemplate.postForEntity(uri, request, LttProcessResponse.class);
        assertThat(response.getStatusCode()).isEqualTo(HttpStatus.OK);
        assertThat(response.getBody().getData()).isEqualTo(EXPECTED_RES);
    }

    @Disabled
    public void testAnalyze() throws Exception {
        String url = "http://localhost:" + this.port;
        URI uri = UriComponentsBuilder.fromHttpUrl(url).path("/analyze")
            .queryParam ("text", "খালিদ  জামিলকে দেখে মনে হচ্ছে খাঁচাবন্দি ‘বাঘ’!.")
            .build().encode().toUri();

        ResponseEntity<String> response = restTemplate.getForEntity(uri, String.class);

        assertThat(response.getStatusCode()).isEqualTo(HttpStatus.OK);
        assertThat(response.getBody()).isEqualTo(EXPECTED_RES);
    }
}
