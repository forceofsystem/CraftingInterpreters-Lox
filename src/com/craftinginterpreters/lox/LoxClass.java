package com.craftinginterpreters.lox;

import java.util.List;
import java.util.Map;

public class LoxClass extends LoxInstance implements LoxCallable {
    final String name;
    final LoxClass superclass;
    private final Map<String, LoxFunction> methods;
    private final Map<String, LoxFunction> staticMethods;
    private final Map<String, LoxFunction> getters;

    LoxClass(String name, LoxClass superclass, Map<String, LoxFunction> methods,
             Map<String, LoxFunction> staticMethods, Map<String, LoxFunction> getters) {
        super(null);
        this.name = name;
        this.superclass = superclass;
        this.methods = methods;
        this.staticMethods = staticMethods;
        this.getters = getters;
    }

    LoxFunction findMethod(String name) {
        if (methods.containsKey(name)) {
            return methods.get(name);
        }
        if (superclass != null) {
            return superclass.findMethod(name);
        }

        return null;
    }

    LoxFunction findGetter(String name) {
        if (getters.containsKey(name)) {
            return getters.get(name);
        }
        return null;
    }

    @Override
    public String toString() {
        return name;
    }

    @Override
    public Object call(Interpreter interpreter, List<Object> arguments) {
        LoxInstance instance = new LoxInstance(this);
        LoxFunction initializer = methods.get("init");
        if (initializer != null) {
            initializer.bind(instance).call(interpreter, arguments);
        }
        return instance;
    }

    @Override
    public int arity() {
        LoxFunction initializer = findMethod("init");
        if (initializer == null) return 0;
        return initializer.arity();
    }

    // Implementing static methods for classes
    public Object get(Token name) {
        LoxFunction static_method = staticMethods.get(name.lexeme);
        if (static_method != null) return static_method;
        throw new RuntimeError(name,
                "Undefined property '" + name.lexeme + "'.");
    }

    @Override
    public void set(Token name, Object value) {
        throw new RuntimeError(name,
                "Cannot set property '" + name.lexeme + "'.");
    }
}