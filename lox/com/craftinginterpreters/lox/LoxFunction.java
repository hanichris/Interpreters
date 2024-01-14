package com.craftinginterpreters.lox;

import java.util.List;

class LoxFunction implements LoxCallable {
    private final Stmt.Function declaration;

    LoxFunction(Stmt.Function declaration) {
        this.declaration = declaration;
    }

    @Override
    public Object call(Interpreter interpreter, List<Object> arguments) {
        Environment environment = new Environment(interpreter.globals);

        for (int i = 0; i < declaration.params.size(); i++) {
            environment.define(
                    declaration.params.get(i).lexeme,
                    arguments.get(i)
            );
        }

        // Call the executeBlock in a try-catch block. If it catches a return
        // exception, it pulls out the value and makes the return value from the call.
        // If it never catches one of those exceptions, it implies the function reaches
        // the end without hitting a return statement and thus implicitly returns nil.
        try {
            interpreter.executeBlock(declaration.body, environment);
        } catch (Return returnValue) {
            return returnValue.value;
        }
        return null;
    }

    @Override
    public int arity() {
        return declaration.params.size();
    }

    @Override
    public String toString() {
        return "<fn " + declaration.name.lexeme + ">";
    }
}
