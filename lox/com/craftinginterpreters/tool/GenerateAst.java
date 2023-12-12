package com.craftinginterpreters.tool;

public class GenerateAst {
    public static void main(String... args) {
        if (args.length != 1) {
            System.out.println("Usage: generate_ast <output directory>");
            System.exit(64);
        }
        String outputDir = args[0];
    }
}
