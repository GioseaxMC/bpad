#define ETONIZER_HANDLE_INCLUDES

#define BPAD

#include "etonizer.c"

#include "betzy.c"

#include <time.h>

#include "bf.c"

enum Kind {
    KIND_NULL,
    KIND_INC, // +
    KIND_DEC, // -
    KIND_MOVL, // <
    KIND_MOVR, // >
    KIND_PUTC, // .
    KIND_GETC, // ,
    KIND_JMPR, // [
    KIND_JMPL, // ]
    KIND_PRNT, // ]
    KIND_STRL, // ]
    KIND_WORD, // ]
};

typedef struct Node {
    Token tk;
    int kind;
    // char* data;

    // wanted to use Nodes children but fuck me
    struct Node* items;
    size_t count;
    size_t capacity;
} Node;

typedef struct {
    Node* items;
    size_t count;
    size_t capacity;
} Nodes;

Nodes ast = {0};

Node parse_strlit(Token tk) {
    if (tk.token[0] != '"') {
        printf(TC_RED"ERROR"TC_RESET": "TK_LO": expected a string literal but got '%s'\n", tk_locator(tk), tk.token);
        exit(-1);
    }
    tk.token_len--;
    tk.token[tk.token_len] = 0;
    tk.token++;
    return (Node) { .tk = tk, .kind = KIND_STRL };
}

Node parse_primary(TokenList* tkns) {
    Token tk = consume(tkns);
    debug printf("Parsing tk: '%s', "TK_LO"\n", tk.token, tk_locator(tk));
    Node node = { .tk = tk};

    if /**/(streq(tk.token, "+")) {
        node.kind = KIND_INC;
    } elif (streq(tk.token, "-")) {
        node.kind = KIND_DEC;
    } elif (streq(tk.token, ">")) {
        node.kind = KIND_MOVR;
    } elif (streq(tk.token, ">")) {
        node.kind = KIND_MOVL;
    } elif (streq(tk.token, ".")) {
        node.kind = KIND_PUTC;
    } elif (streq(tk.token, ",")) {
        node.kind = KIND_GETC;
    } elif (streq(tk.token, "]")) {
        node.kind = KIND_JMPL;
    } elif (streq(tk.token, "[")) {
        node.kind = KIND_JMPR;
    } elif (*tk.token == '"') {
        node = parse_strlit(tk);
    } elif (streq(tk.token, "print")) {
        node.kind = KIND_PRNT;
        expect(tkns, "(");
        while (not streq(current(tkns).token, ")")) {
            da_append(node, parse_primary(tkns));
            if (not streq(current(tkns).token, ")")) {
                expect(tkns, ",");
            }
        }
        expect(tkns, ")");
    } elif (str_is_ualnum(tk.token)) {
        node.kind = KIND_WORD;
    } else {
        printf("ERROR"TC_RESET": "TK_LO": expected expression but got '%s'\n", tk_locator(tk), tk.token);
        scrash();
    }

    return node;
}

void parse(TokenList* tkns) {
    while (more(tkns)) {
        da_append(ast, parse_primary(tkns));
    }
}

FILE* tempfile;
#define compop(op) fprintf(tempfile, op)

void compile_node(Node node) {
    switch (node.kind) {
        when(KIND_INC, {
            compop("+");
        })
        when(KIND_DEC, {
            compop("-");
        })
        when(KIND_MOVL, {
            compop("<");
        })
        when(KIND_MOVR, {
            compop(">");
        })
        when(KIND_PUTC, {
            compop(".");
        })
        when(KIND_GETC, {
            compop(",");
        })
        when(KIND_JMPL, {
            compop("]");
        })
        when(KIND_JMPR, {
            compop("[");
        })
        when(KIND_PRNT, {
            bool current_cell_val = 0;
            foreach(node.items, node.count, Node subnode, {
                foreach(subnode.tk.token, subnode.tk.token_len, char c, {
                    int direction = getsign(current_cell_val - c);
                    int quantity = babs(current_cell_val - c);
                    current_cell_val -= current_cell_val - c;
                    for (int i=0; i < quantity; i++) {
                        if (direction > 0) {
                            compop("-");
                        } else {
                            compop("+");
                        }
                    }
                    compop(".");
                });
            })
        })
    }
}

void compile() {
    foreach(ast.items, ast.count, Node node, {
        compile_node(node);
    })
}

int main(int argc, char** argv) {
    char* outfile = argv[2];
    char* tempfile_n = ".temp.bf";
    int bf_argc = 3;
    char* bf_argv[] = {"bft", ".temp.bf", "main.exe", 0};

    if (argc < 2) {
        crash("Usage:\n\tbpad <infile> <outfile>");
    }

    foreach(argv, argc, char* arg, {
        debug printf("%s . ", arg);
        if (streq(arg, "-d")) {
            DEBUGGING = 1;
        }
    })

    char* source_code = NULL;
    char* filename = argv[1];
    read_file(filename, source_code);
    
    TokenList tkns;
    init_token_list(&tkns);
    tokenize(source_code, &tkns, filename);
    
    debug foreach(tkns.tokens, tkns.size, Token tk, {
        printf("Token: '%s', "TK_LO"\n", tk.token, tk_locator(tk));
    })
    
    parse(&tkns);


    tempfile = ofile_write(tempfile_n);

    
    compile();

    fflush(tempfile);
    fclose(tempfile);
    
    free_token_list(&tkns);
    return start_bf_compiler(bf_argc, bf_argv);
}