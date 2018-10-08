package com.clarabridge.ltt.service;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;

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

import static org.junit.jupiter.api.Assertions.assertEquals;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.CoreMatchers.equalTo;
import static org.hamcrest.CoreMatchers.is;
 
@ActiveProfiles("prod") // dev
@ExtendWith(SpringExtension.class)
@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
public class LttTests {
    @Autowired
    private TestRestTemplate restTemplate;

    @LocalServerPort
    private int port;
 
    @Test
    public void testAnalyze() throws Exception {
        String url = "http://localhost:" + this.port;
        URI uri = UriComponentsBuilder.fromHttpUrl(url).path("/analyze")
            .queryParam ("text", "খালিদ  জামিলকে দেখে মনে হচ্ছে খাঁচাবন্দি ‘বাঘ’!.")
            .build().encode().toUri();

        ResponseEntity<String> response = restTemplate.getForEntity(uri, String.class);
        assertThat(response.getStatusCode(), equalTo(HttpStatus.OK));

        String expectedRes = "ChMIARoP4KaW4Ka+4Kay4Ka/4KamCgYQBRoCICAKYggCEAcaFeCmnOCmvuCmruCmv+CmsuCmleCnhyJFCg/gppzgpr7gpq7gpr/gprISMv7//////////wHz//////////8B4P//////////Adf//////////wHT//////////8BCgUQDhoBIAqtAQgCEA8aDOCmpuCnh+CmluCnhyIhCgngpqbgp4fgppYSFPb//////////wHB//////////8BIjUKCeCmpuCnh+CmlhIo9v//////////Acf//////////wHI//////////8Bzf//////////ASI/Cgngpqbgp4fgppYSMvb//////////wHH//////////8ByP//////////Ac7//////////wHi//////////8BCgUQExoBIApNCAIQFBoJ4Kau4Kao4KeHIjwKBuCmruCmqBIy////////////Ad7//////////wHc//////////8B1///////////AdH//////////wEKBRAXGgEgCoEBCAIQGBoP4Ka54Kaa4KeN4Kab4KeHIi8KA+CmuRIo9v//////////Acf//////////wHG//////////8Bzf//////////ASI5CgPgprkSMvb//////////wHH//////////8Bxv//////////Ac7//////////wHi//////////8BCgUQHRoBIAokCAEQHhoe4KaW4Ka+4KaB4Kaa4Ka+4Kas4Kao4KeN4Kam4Ka/CggQKBoEIOKAmApQCAIQKhoJ4Kas4Ka+4KaYIj8KCeCmrOCmvuCmmBIy////////////AeD//////////wHb//////////8B1///////////AdT//////////wEKBxAtGgPigJkKGAgCEC4aASEiDwoBIRIKv///////////AQ==";

        assertEquals(expectedRes, response.getBody());
    }
}
