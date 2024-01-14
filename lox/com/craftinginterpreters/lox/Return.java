package com.craftinginterpreters.lox;

class Return extends RuntimeException {
    final Object value;

    Return(Object value) {
        // disable some unneeded JVM machinery.
        // using the exception class for control flow and not error handling.
        super(null, null, false, false);
        this.value = value;
    }
}
