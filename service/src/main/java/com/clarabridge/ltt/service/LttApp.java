package com.clarabridge.ltt.service;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication // @Configuration, @EnableAutoConfiguration, @ComponentScan
public class LttApp {
    public static void main(String[] args) {
        new SpringApplication(LttApp.class).run(args);
    }
}
