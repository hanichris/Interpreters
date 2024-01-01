package com.craftinginterpreters.lox;

import java.util.HashMap;
import java.util.Map;

class Environment {
    final Environment enclosing; // Reference to the enclosing environment.
    private final Map<String, Object> values = new HashMap<>();

    /**
     * no-argument constructor for the global scope's environment which ends
     * the chain.
     */
    Environment() {
        enclosing = null;
    }

    /**
     * creates a new local scope nested inside the given outer one.
     */
    Environment(Environment enclosing) {
        this.enclosing = enclosing;
    }

    /**
     * Performs a variable lookup within the current innermost scope.
     * If it is not found, then search the enclosing scope for the variable
     * and recursively repeat until the global scope whereby a runtime error
     * is thrown if the variable was not found.
     * Takes a 'Token' instead of a string to help report to the
     * user where the in their code they messed up.
     */
    Object get(Token name) {
        if (values.containsKey(name.lexeme)) return values.get(name.lexeme);
        if (enclosing != null) return enclosing.get(name);
        throw new RuntimeError(name,
                "Undefined variable '" + name.lexeme + "'."
        );
    }

    /**
     * Support assignment in nested environments.
     * Not allowed to create a new variable unlike the 'define' method.
     * throws a RuntimeError if the key doesn't already exist in
     * the environment's variable map
     */
    void assign(Token name, Object value) {
        if (values.containsKey(name.lexeme)) {
            values.put(name.lexeme, value);
            return;
        }

        if (enclosing != null) {
            enclosing.assign(name, value);
            return;
        }

        throw new RuntimeError(name,
                "Undefined variable '" + name.lexeme + "'.");
    }

    /**
     * Declares a new variable in the current innermost scope.
     */
    void define(String name, Object value) {
        values.put(name, value);
    }
}
