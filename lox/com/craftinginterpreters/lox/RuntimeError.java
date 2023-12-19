package com.craftinginterpreters.lox;

class RuntimeError extends RuntimeException {
    /**
     * identifies where in the user's code the runtime error came from
     */
    final Token token;

    RuntimeError(Token token, String message) {
        super(message);
        this.token = token;
    }
}
