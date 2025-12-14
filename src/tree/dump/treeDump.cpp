#include "D:\TX\TXLib-20251118.h"
#include "treeDump.h"

#include <cstdarg>

#include "input.h"
#include "stack.h"

static void addNodeInfo(FILE* file, int index, treeNode_t* node, const char* const fillColor) {
    fprintf(file, "Node_%d [shape=Mrecord; style=filled; fillcolor = \"%s\"; "
                  "color = \"black\"; "
                  "label = <{ptr: ",
            index, fillColor);
    $ptrHTML(file, node, BACK_COLOR);

    switch(getNodeType(node)) {
        case NUMBER_TYPE: {
            fprintf(file, "| type: NUMBER | val: %d | ", getData(node).number);
            break;
        }
        case OPERATION_TYPE: {
            fprintf(file, "| type: OPERATION | val: ");
            //TODO
            fprintf(file, "%ls", TD_TOKENS_INFO[getData(node).operation].representation);
            fprintf(file, " | ");
            break;
        }
        case PARAM_TYPE: {
            dslParameter_t* param = getData(node).parameter;
            fprintf(file, "| type: PARAMETER | %s = %d | ", param->name, param->value);
            break;
        }
        default: {
            PRINTERR("UNEXPECTED NODE TYPE: %d\n", getNodeType(node));
            fprintf(file, "UNEXPECTED NODE TYPE");
        }
    }

    if (getLeft(node) != NULL) {
        fprintf(file, "{L: ");
        $ptrHTML(file, getLeft(node), BACK_COLOR);
    }
    else {
        fprintf(file, "{L: <font face=\"monospace\" color=\"blue\">NULL</font>");
    }

    if (getRight(node) != NULL) {
        fprintf(file, " | R: ");
        $ptrHTML(file, getRight(node), BACK_COLOR);
        fprintf(file, "}}>; ]\n");
    }
    else {
        fprintf(file, " | R: <font face=\"monospace\" color=\"blue\"> NULL</font>}}>; ]\n");
    }
}

static void addNodesRec(FILE* file, treeNode_t* curNode, int* counter, const char* const fillColor) {
    int curIndex = *counter;

    treeNode_t* children[] = {getLeft(curNode), getRight(curNode)};
    for (int i = 0; i < 2; i++) {
        treeNode_t* child = children[i];
        if (child != NULL) {
            *counter = *counter + 1;
            addNodeInfo(file, *counter, child, fillColor);
            fprintf(file, "Node_%d->Node_%d\n", curIndex, *counter);
            addNodesRec(file, child, counter, fillColor);
        }
    }
}

static void generateDotFile(treeNode_t* node, FILE *dotFile, const char* const fillColor) {
    assert(dotFile);
    fprintf(dotFile, "digraph G {\n");

    int counter = 0;
    addNodeInfo(dotFile, 0, node, fillColor);
    addNodesRec(dotFile, node, &counter, fillColor);

    fprintf(dotFile, "}\n");
}

static void fillHtmlHeader(const char *desc, const char *fileName, const int line,
                           const char *func, int code, FILE* htmlFile, size_t counter) {
    assert(htmlFile);
    fprintf(htmlFile, "\n\n</p><h3>DUMP#%llu <font color=red>\n\t%s\n</font> \n\t%s "
                      "at (%s:%d) - \n\tSTATUS: %d\n</h3></p>\n",
            counter, desc, func, fileName, line, code);
}

static size_t counter = 0;
int treeLog(const char* message, ...) {
    assert(message);
    #ifdef DEBUG_TREE
        FILE* htmlFile = fopen(HTML_FILE_PATH, counter++ == 0 ? "w" : "a");
        if (!htmlFile) {
            RETURN_ERR(TR_CANT_OPEN_FILE, "Cannot open HTML log file");
        }

        // Стиль для всего сообщения
        fprintf(htmlFile, "\n<div style='font-size: 18px; line-height: 1.6; margin: 12px 0; padding: 12px; background: #f8f9fa; border-left: 5px solid #28a745; border-radius: 4px; font-family: \"Courier New\", monospace; font-weight: bold;'>\n\t");
        va_list args;
        va_start(args, message);
        vfprintf(htmlFile, message, args);
        va_end(args);

        fprintf(htmlFile, "\n</div>\n");
        fclose(htmlFile);
    #endif
    return TR_SUCCESS;
}


int treeDump(treeNode_t* node, const char* desc, const char* file,
             const int   line, const char* func, int code, const char* const fillColor) {
    FILE* htmlFile = fopen(HTML_FILE_PATH, counter++ == 0 ? "w" : "a");
    if (!htmlFile) {
        RETURN_ERR(TR_CANT_OPEN_FILE, "Cannot open HTML log file");
    }

    char svg_filename[MAX_LINE_LENGTH] = {};
    snprintf(svg_filename, sizeof(svg_filename), SVG_FORMAT, counter);
    fillHtmlHeader(desc, file, line, func, code, htmlFile, counter);

    // DOT файл
    FILE* dot_file = fopen(DOT_FILE_PATH, "w");
    if (!dot_file) {
        fclose(htmlFile);
        RETURN_ERR(TR_CANT_OPEN_FILE, "cannot create DOT file");
    }
    generateDotFile(node, dot_file, fillColor);
    fclose(dot_file);

    // Генерируем PNG
    char command[MAX_LINE_LENGTH];
    snprintf(command, sizeof(command), "dot -T svg %s -o %s",
             DOT_FILE_PATH, svg_filename);
    int result = system(command);

    if (result != 0) {
        fprintf(htmlFile, "\t<p style=\"color: red;\">Graphviz generation failed!</p>\n");
    } else {
        fprintf(htmlFile, "\t<img src=\"images/tree_%zu.svg\" "
                           "alt=\"List structure visualization\">\n",
                counter);
    }

    fprintf(htmlFile, "<hr>\n");

    fclose(htmlFile);

    return TR_SUCCESS;
}
