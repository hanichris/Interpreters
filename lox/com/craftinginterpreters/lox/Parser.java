package com.craftinginterpreters.lox;

import java.util.ArrayList;
import java.util.List;

class Parser {
    private static class ParseError extends RuntimeException {
    }
    private final List<Token> tokens;
    private int current = 0; // points to the next token to be processed.

    Parser(List<Token> tokens) {
        this.tokens = tokens;
    }

    List<Stmt> parse() {
        List<Stmt> statements = new ArrayList<>();
        while (!isAtEnd()) statements.add(declaration());
        return statements;
    }

    /**
     * expression: Translates the expression grammar rule, i.e,
     * expression -> equality;
     */
    private Expr expression() {
        return assignment();
    }

    /**
     * Called repeatedly when parsing a series of statements in
     * a block or script and helps synchronize the parser if it
     * enters into a panic mode recovery.
     */
    private Stmt declaration() {
        try {
            if (match(TokenType.VAR)) return varDeclaration();
            return statement();
        } catch (ParseError error) {
            synchronize();
            return null;
        }
    }

    private Stmt statement() {
        if (match(TokenType.PRINT)) return printStatement();
        return expressionStatement();
    }

    /**
     * Implementation of the 'print' statement within the language.
     * It parses the subsequent expression, consumes the terminating
     * SEMICOLON and emits a syntax tree.
     */
    private Stmt printStatement() {
        Expr value = expression();
        consume(TokenType.SEMICOLON, "Expect ';' after value.");
        return new Stmt.Print(value);
    }

    /**
     * requires and consumes an 'IDENTIFIER' token for the variable name.
     * If an '=' token is found, it expects an initializer expression to
     * follow before finally consuming the expected 'SEMICOLON' token
     * at the end of the declaration.
     */
    private Stmt varDeclaration() {
        Token name = consume(TokenType.IDENTIFIER, "Expect variable name");

        Expr initializer = null;
        if (match(TokenType.EQUAL)) initializer = expression();
        consume(TokenType.SEMICOLON, "Expect a ';' after a variable declaration");
        return new Stmt.Var(name, initializer);
    }

    /**
     * Parses the expression, consumes the terminating SEMICOLON and wraps
     * the `Expr` in the appropriate Stmt before returning it.
     */
    private Stmt expressionStatement() {
        Expr expr = expression();
        consume(TokenType.SEMICOLON, "Expect ';' after expression.");
        return new Stmt.Expression(expr);
    }

    private Expr assignment() {
        Expr expr = equality();

        if (match(TokenType.EQUAL)) {
            Token equals = previous();
            Expr value = assignment();

            if (expr instanceof Expr.Variable) {
                Token name = ((Expr.Variable) expr).name;
                return new Expr.Assign(name, value);
            }

            /*
             * report an error if the left-hand side isn't a valid assignment
             * target. We don't throw the error since the parser is not in a
             * confused state where we need to go into panic mode and synchronize.
             * */
            error(equals, "Invalid assignment target.");
        }
        return expr;
    }

    /**
     * equality: Translation for the equality grammar rule, i.e,
     * equality -> comparison (("!=" | "==") comparison)*;
     */
    private Expr equality() {
        Expr expr = comparison();

        //Must find either a `!=` or `==` else loop terminates.
        while (match(TokenType.BANG_EQUAL, TokenType.EQUAL_EQUAL)) {
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

        if (match(TokenType.IDENTIFIER)) return new Expr.Variable(previous());
        if (match(TokenType.LEFT_PAREN)) {
            Expr expr = expression();
            consume(TokenType.RIGHT_PAREN, "Expected ')' after expression");
            return new Expr.Grouping(expr);
        }
        throw error(peek(), "Expect expression");
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
     * consume: Checks whether the next token is of the expected type.
     * If not, then we've hit an error.
     */
    private Token consume(TokenType type, String message) {
        if (check(type)) return advance();
        throw error(peek(), message);
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

    /**
     * returns the error instead of throwing it so that the calling method
     * gets to decide whether to unwind the parser. True for cases where it
     * is unlikely for the parser to get into a weird state and hence, there
     * is no need for synchronization.
     */
    private ParseError error(Token token, String message) {
        Lox.error(token, message);
        return new ParseError();
    }

    /**
     * Discards tokens until it finds a statement boundary. The statement is
     * demarcated by a semicolon and some specific language reserved keywords.
     */
    private void synchronize() {
        advance();

        while (!isAtEnd()) {
            if (previous().type == TokenType.SEMICOLON) return;

            switch (peek().type) {
                case TokenType.CLASS:
                case TokenType.FOR:
                case TokenType.WHILE:
                case TokenType.IF:
                case TokenType.VAR:
                case TokenType.RETURN:
                case TokenType.FUN:
                case TokenType.PRINT:
                    return;
            }
            advance();
        }
    }
}
