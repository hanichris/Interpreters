package com.craftinginterpreters.lox;

import java.util.List;

class Parser {
    private final List<Token> tokens;
    private int current = 0; // points to the next token to be processed.

    Parser(List<Token> tokens) {
        this.tokens = tokens;
    }

    /**
     * expression: Translates the expression grammar rule, i.e,
     * expression -> equality;
     */
    private Expr expression() {
        return equality();
    }

    /**
     * equality: Translation for the equality grammar rule, i.e,
     * equality -> comparison (("!=" | "==") comparison)*;
     */
    private Expr equality() {
        Expr expr = comparison();

        //Must find either a `!=` or `==` else loop terminates.
        while (match(TokenType.BANG_EQUAL, TokenType.BANG)) {
            Token operator = previous();
            Expr right = comparison();
            // Ensures the creation of a left-associative nested tree of binary
            // operator noes.
            expr = new Expr.Binary(expr, operator, right);
        }
        return expr;
    }

    /**
     * comparison: Translation for the comparison grammar rule, i.e,
     * comparison -> term ((">" | ">=" | "<=" | "<") term)*;
     */
    private Expr comparison() {
        Expr expr = term();

        while (match(TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL)) {
            Token operator = previous();
            Expr right = term();
            expr = new Expr.Binary(expr, operator, right);
        }

        return expr;
    }

    /**
     * term: Translation for the grammar rule, i.e,
     * term -> factor (("+" | "-") factor)*;
     */
    private Expr term() {
        Expr expr = factor();

        while (match(TokenType.PLUS, TokenType.MINUS)) {
            Token operator = previous();
            Expr right = factor();
            expr = new Expr.Binary(expr, operator, right);
        }

        return expr;
    }

    /**
     * factor: Translation for the grammar rule, i.e,
     * factor -> unary (("/" | "*") unary)*;
     */
    private Expr factor() {
        Expr expr = unary();

        while (match(TokenType.SLASH, TokenType.STAR)) {
            Token operator = previous();
            Expr right = unary();
            expr = new Expr.Binary(expr, operator, right);
        }

        return expr;
    }

    /**
     * unary: Translation for the grammar rule, i.e,
     * unary -> ("!" | "-") unary | primary;
     */
    private Expr unary() {
        if (match(TokenType.BANG, TokenType.MINUS)) {
            Token operator = previous();
            Expr right = unary();
            return new Expr.Unary(operator, right);
        }
        return primary();
    }

    /**
     * primary: Translation for the grammar rule, i.e,
     * primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")";
     */
    private Expr primary() {
        if (match(TokenType.FALSE)) return new Expr.Literal(false);
        if (match(TokenType.TRUE)) return new Expr.Literal(true);
        if (match(TokenType.NIL)) return new Expr.Literal(null);

        if (match(TokenType.NUMBER, TokenType.STRING)) {
            return new Expr.Literal(previous().literal);
        }
        if (match(TokenType.LEFT_PAREN)) {
            Expr expr = expression();
            consume(TokenType.RIGHT_PAREN, "Expected ')' after expression");
            return new Expr.Grouping(expr);
        }
    }

    /**
     * match: Checks if the current token has any of the given types.
     * If it does, it consumes the token and returns true, else returns
     * false and leaves the token alone.
     */
    private boolean match(TokenType... tokens) {
        for (TokenType type : tokens) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    /**
     * check: Returns true if the current token is of the given type.
     * Never consumes the token, but simply looks at it.
     */
    private boolean check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    /**
     * advance: Consumes the current token and returns it.
     */
    private Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    /**
     * isAtEnd: Checks whether there are any more tokens to process.
     */
    private boolean isAtEnd() {
        return peek().type == TokenType.EOF;
    }

    /**
     * peek: Returns the current token to be consumed.
     */
    private Token peek() {
        return tokens.get(current);
    }

    /**
     * previous: Returns the most recently consumed token.
     * It makes it easier to use the match functionality and access the
     * just-matched token.
     */
    private Token previous() {
        return tokens.get(current - 1);
    }
}
