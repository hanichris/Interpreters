package com.craftinginterpreters.lox;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Scanner;

public class Lox {
    public static void main(String ...args) throws IOException {
        if (args.length > 1) {
            System.out.println("Usage: jlox <script>");
            System.exit(64);
        } else if (args.length == 1) {
            runFile(args[0]);
        } else {
            runPrompt();
        }
    }

    private static void runFile(String path) throws IOException {
        byte[] bytes = Files.readAllBytes(Path.of(path));
        run(new String(bytes, Charset.defaultCharset()));
    }

    private static void runPrompt() throws  IOException {
        InputStreamReader input = new InputStreamReader(System.in);
        BufferedReader reader = new BufferedReader(input);

        while(true) {
            System.out.print("> ");
            String line = reader.readLine();
            if (line == null || line.equals("exit")) break;
            run(line);
        }
        System.out.println();
    }

    private static void run(String source){
        Scanner scanner = new Scanner(source);

        while(scanner.hasNextLine()){
            System.out.println(scanner.nextLine());
        }
    }
}