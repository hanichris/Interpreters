package com.craftinginterpreters.lox;

import java.util.HashMap;
import java.util.Map;

class Environment {
    private final Map<String, Object> values = new HashMap<>();

    /**
     * takes a 'Token' instead of a string to help report to the
     * user where the in their code they messed up.
     */
    Object get(Token name) {
        if (values.containsKey(name.lexeme)) return values.get(name.lexeme);
        throw new RuntimeError(name,
                "Undefined variable '" + name.lexeme + "'."
        );
    }

    /**
     * Not allowed to create a new variable unlike the 'define' method.
     * throws a RuntimeError if the key doesn't already exist in
     * the environment's variable map
     */
    void assign(Token name, Object value) {
        if (values.containsKey(name.lexeme)) {
            values.put(name.lexeme, value);
            return;
        }

        throw new RuntimeError(name,
                "Undefined variable '" + name.lexeme + "'.");
    }

    void define(String name, Object value) {
        values.put(name, value);
    }
}
