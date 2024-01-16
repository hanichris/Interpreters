package com.craftinginterpreters.lox;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

/**
 * Resolver needs to visit every kind of node in the syntax tree, hence
 * implements the visitor abstraction. Only 4 kinds of nodes are of interest:
 * a block statement, a function declaration, a variable declaration and variable
 * and assignment expressions.
 */
class Resolver implements Expr.Visitor<Void>, Stmt.Visitor<Void> {
    private final Interpreter interpreter;
    //Holds the chain of Environment objects. Only for block scopes.
    private final Stack<Map<String, Boolean>> scopes = new Stack<>();

    Resolver(Interpreter interpreter) {
        this.interpreter = interpreter;
    }

    void resolve(List<Stmt> statements) {
        for (Stmt statement : statements) resolve(statement);
    }

    private void beginScope() {
        scopes.push(new HashMap<String, Boolean>());
    }

    private void endScope() {
        scopes.pop();
    }

    /**
     * adds the variable to the innermost scope so that it shadows any
     * outer ones and mark it as not ready.
     */
    private void declare(Token name) {
        if (scopes.isEmpty()) return;
        Map<String, Boolean> scope = scopes.peek();
        scope.put(name.lexeme, false);
    }

    /**
     * define the variable by marking it available for use.
     */
    private void define(Token name) {
        if (scopes.isEmpty()) return;
        scopes.peek().put(name.lexeme, true);
    }

    /**
     * starts at the innermost scope and works outwards looking for a
     * matching variable name. If found, it is resolved passing in the
     * number of scopes between the current innermost scopes and the
     * scope where the variable was found.
     */
    private void resolveLocal(Expr expr, Token name) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes.get(i).containsKey(name.lexeme)) {
                interpreter.resolve(expr, scopes.size() - 1 - i);
                return;
            }
        }
    }

    @Override
    public Void visitBlockStmt(Stmt.Block stmt) {
        beginScope();
        resolve(stmt.statements);
        endScope();
        return null;
    }

    /**
     * splits binding into two steps: declaration and definition.
     */
    @Override
    public Void visitVarStmt(Stmt.Var stmt) {
        declare(stmt.name);
        if (stmt.initializer != null) resolve(stmt.initializer);
        define(stmt.name);
        return null;
    }

    @Override
    public Void visitAssignExpr(Expr.Assign expr) {
        resolve(expr.value);
        resolveLocal(expr, expr.name);
        return null;
    }

    @Override
    public Void visitVariableExpr(Expr.Variable expr) {
        if (!scopes.isEmpty() &&
                scopes.peek().get(expr.name.lexeme) == Boolean.FALSE)
            Lox.error(expr.name,
                    "Can't read local variable in its own initializer");
        resolveLocal(expr, expr.name);
        return null;
    }

    /**
     * Apply the visitor pattern to the statement syntax tree node.
     */
    private void resolve(Stmt stmt) {
        stmt.accept(this);
    }

    /**
     * Apply the visitor pattern to the expression syntax tree node.
     */
    private void resolve(Expr expr) {
        expr.accept(this);
    }
}
