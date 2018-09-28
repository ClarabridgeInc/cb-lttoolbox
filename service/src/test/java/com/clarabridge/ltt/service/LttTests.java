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

import org.springframework.test.context.junit.jupiter.SpringExtension;

import java.net.URI;

import static org.junit.jupiter.api.Assertions.assertEquals;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.CoreMatchers.equalTo;
import static org.hamcrest.CoreMatchers.is;
 
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
            .queryParam("text", "খালিদ  জামিলকে দেখে মনে হচ্ছে খাঁচাবন্দি ‘বাঘ’..!")
            .build().encode().toUri();

        ResponseEntity<String> response = restTemplate.getForEntity(uri, String.class);
        assertThat(response.getStatusCode(), equalTo(HttpStatus.OK));

        String expectedRes = "CgYIARoCICAKYggCEAcaFeCmnOCmvuCmruCmv+CmsuCmleCnhyJFCg/gppzgpr7gpq7gpr/gprISMv7//////////wHz//////////8B4P//////////Adf//////////wHT//////////8BCgUQDhoBIAojIiEKCeCmpuCnh+CmlhIU9v//////////AcH//////////wEKNyI1Cgngpqbgp4fgppYSKPb//////////wHH//////////8ByP//////////Ac3//////////wEKUwgCEA8aDOCmpuCnh+CmluCnhyI/Cgngpqbgp4fgppYSMvb//////////wHH//////////8ByP//////////Ac7//////////wHi//////////8BCgUQExoBIApNCAIQFBoJ4Kau4Kao4KeHIjwKBuCmruCmqBIy////////////Ad7//////////wHc//////////8B1///////////AdH//////////wEKBRAXGgEgCjEiLwoD4Ka5Eij2//////////8Bx///////////Acb//////////wHN//////////8BClAIAhAYGg/gprngpprgp43gppvgp4ciOQoD4Ka5EjL2//////////8Bx///////////Acb//////////wHO//////////8B4v//////////AQoFEB0aASAKCggBEB4aBCDigJgKUAgCECoaCeCmrOCmvuCmmCI/Cgngpqzgpr7gppgSMv///////////wHg//////////8B2///////////Adf//////////wHU//////////8BCgcQLRoD4oCZChgIAhAuGgEuIg8KAS4SCr///////////wEKGAgCEC8aAS4iDwoBLhIKv///////////AQ==";

        assertEquals(expectedRes, response.getBody());
    }
}
