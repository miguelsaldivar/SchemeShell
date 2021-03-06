#include "scheme.h"

Shell::Shell() {
}

bool Shell::is_fixnum(std::unique_ptr<scheme_object> obj) {
    return obj->type == scheme_object::FIXNUM;
}

bool Shell::is_delimiter(int c) {
    return std::isspace(c) || c == EOF 
           || c == '(' || c == ')'
           || c == '"' || c == ';';
}

bool Shell::is_boolean(std::unique_ptr<scheme_object> obj) {
    return obj->type == scheme_object::BOOLEAN;
}

bool Shell::is_false(std::unique_ptr<scheme_object> obj) {
    return obj->boolean == false;
}

int Shell::peek(std::FILE* in) {
    int c;
    c = std::getc(in);
    std::ungetc(c, in);
    return c;
}

void Shell::eat_whitespace(std::FILE* in) {
    int c;
    while((c = std::getc(in)) != EOF) {
        if(isspace(c)) 
            continue;
        else if (c == ';') {
            while (((c = std::getc(in)) != EOF) && (c != '\n'));
            continue;
        }
        std::ungetc(c, in);
        break;
    }
}

std::unique_ptr<scheme_object> Shell::make_bool(bool val) {
    auto obj = std::make_unique<scheme_object>();
    obj->type = scheme_object::BOOLEAN;
    obj->boolean = val;
    return obj;
}

std::unique_ptr<scheme_object> Shell::make_fixnum(long num) {
    auto obj = std::make_unique<scheme_object>();
    obj->type = scheme_object::FIXNUM;
    obj->fixnum= num;
    return obj;
}

std::unique_ptr<scheme_object> Shell::read(std::FILE* in) {
    int c = 0;
    short sign = 1;
    long num = 0;
    eat_whitespace(in);

    c = std::getc(in);
    if (c == '#') {
        c = std::getc(in);
        switch(c) {
            case 't':
                return make_bool(1);
            case 'f':
                return make_bool(0);
            default:
                std::cerr << "unknown boolean literal\n";
                exit(1);
        }
    }
    if (std::isdigit(c) || (c == '-' && (std::isdigit(peek(in))))) {
        if (c == '-') sign = -1;
        else std::ungetc(c, in);
        while (std::isdigit(c = std::getc(in))) 
            num = (num * 10) + (c - '0');
        num *= sign;
        if (is_delimiter(c)) {
            std::ungetc(c, in);
            return make_fixnum(num);
        }
        std::cerr << "Bad input \n";
        exit(1);
    }
    std::cerr << "Bad input \n";
    exit(1);
}

std::unique_ptr<scheme_object> Shell::eval(std::unique_ptr<scheme_object> expression) {
    return expression;
}

void Shell::write(std::unique_ptr<scheme_object> obj) {
    switch (obj->type) {
        case scheme_object::BOOLEAN: {
            char c = is_false(std::move(obj)) ? 'f' : 't';
            std::cout << "#" << c;
            break;
        }
        case scheme_object::FIXNUM:
            std::cout << obj->fixnum;
            break;
        default:
            std::cerr << "Cannot write an unknown type\n";
            exit(1);
    }
}

void Shell::repl() {
    while (1) {
        std::cout << "> ";
        write(eval(read(stdin)));
        std::cout << "\n";
    }
}
