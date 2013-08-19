/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
 *
 * --------------------------------------------------------------------
 * libz80 - Z80 emulation library (Version 2.1.0)
 * (C) Gabriel Gambetta (gabriel.gambetta@gmail.com) 2000 - 2012
 * https://github.com/ggambetta/libz80
 * --------------------------------------------------------------------
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCoreApplication>

#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStringList>

static const int MAX_LINE = 100;        // maximum line length
static const int OPCODE_OFFSET = 11;    // Offset of an opcode from the list, from the start of the line
static const int MAX_MATCH = 5;         // Max parameters per matching opcode.


//static const char *OPCODES_SPEC	= "mktables.spec";
static const char *OPCODES_SPEC	= "mktables_v2.spec";
static const char *OPCODES_LIST	= "opcodes.lst";
static const char *OPCODES_HEADER = "opcodes_decl.h";
//static const char *OPCODES_IMPL	= "opcodes_impl.c";
//static const char *OPCODES_TABLE = "opcodes_table.h";
static const char *OPCODES_IMPL	= "opcodes_impl.cpp";
static const char *OPCODES_TABLE = "opcodes_table.cpp";

struct Z80OpcodeTable;

struct Z80OpcodeEntry
{
    Z80OpcodeEntry() : pre_skip(0), post_skip(0), operand_type(0), table(0) {}

    int pre_skip;
    int post_skip;
    QByteArray func;	/* Z80OpcodeFunc* func;*/

    int operand_type;
    QByteArray format;

    Z80OpcodeTable *table;
};

struct Z80OpcodeTable
{
    QByteArray name;
    int opcode_offset;
    Z80OpcodeEntry entries[256];
};


struct Item {
    QRegularExpression re;
    QString pattern;
    QStringList lines;
};

QList<Item> items;

/** Creates a function name from an opcode */
QByteArray fixName(const QByteArray &opc)
{
    QByteArray name = opc;

    name.replace(' ', '_');
    name.replace('+', '_');
    name.replace(',', '_');
    name.replace('\'', '_');

    name.replace("(", "off_");
    name.replace(")", "");

    return name;
}

QByteArray mkFormat(const QByteArray &src)
{
    QByteArray dst = src;
    dst.replace("d", "0%02Xh");
    dst.replace("e", "%d");
    dst.replace("nn", "0%04Xh");
    dst.replace("n", "0%02Xh");
    return dst;
}

bool readSpec(QFile &file)
{
    while (!file.atEnd()) {
        QByteArray line = file.readLine(MAX_LINE);

        // remove trailing line break
        line.chop(1);

        if (line.isEmpty() || line[0] == '#')     // empty line or comment
            continue;

        if (line[0] != '\t') {   // new pattern definition
            line.append('$');

            Item newItem;
            newItem.re.setPattern(line);
            if (!newItem.re.isValid())
                qFatal(line);

            newItem.pattern = line;

            items.append(newItem);
        } else {                 // A line in the current pattern
            if (line.length() > 0) {
                items.last().lines << line;
            }
        }
    }

    return !items.isEmpty();
}

bool printCall(const QString &s, QTextStream &code)
{
    if (!s.trimmed().startsWith('%'))
        return false;

    code << s.left(s.indexOf('%'))
         << fixName(s.mid(s.indexOf('%')+1).toLatin1())
//         << "(ctx);\n";
         << "();\n";

    return true;
}

void generateCodeTable(QFile &opcodes, QFile &code, QFile &header)
{
    qDebug() << "Generating opcode implementations...";

    QTextStream out(&code);
    QTextStream outHeader(&header);

    out << "#include \"alu.h\"\n"
        << "using namespace Z80;\n\n";

    out << "#include \"imemory.h\"\n";
    out << "#include \"registers.h\"\n\n";

    QByteArray last;

    while (!opcodes.atEnd()) {
        QByteArray line = opcodes.readLine(MAX_LINE);

        // remove trailing line break
        line.chop(1);

        // skip the hex part
        line = line.mid(OPCODE_OFFSET);

        // Avoid duplicate opcodes
        if (line == last)
            continue;
        last = line;

        Item item;
        bool found = false;
        foreach (item, items) {
            QRegularExpressionMatch match = item.re.match(line);
            if (match.hasMatch() && match.capturedStart(0) == 0) {
                found = true;
                break;
            }
        }

        if (!found)
            qFatal(qPrintable(QString("%1 didn't match anything").arg(line.data())));

//        out << "static void " << fixName(line) << "(Z80Context* ctx)\n{\n";
//        outHeader << "static void " << fixName(line) << "(Z80Context* ctx);\n\n";
        out << "void Alu::" << fixName(line) << "()\n{\n";
        outHeader << "void " << fixName(line) << "();\n\n";

        foreach (QString cmd, item.lines) {
            if (!printCall(cmd, out)) {

                QRegularExpressionMatch match = item.re.match(line);
                for (int i = 0; i < MAX_MATCH; ++i) {
                    cmd.replace(QString("%%1").arg(i), match.captured(i));
                }
                out << cmd << "\n";
            }
        }

        out << "}\n\n\n";

    }

    qDebug() << "done";
}

void generateCode()
{
    qDebug() << "Reading specification...";

    QFile spec(OPCODES_SPEC);
    if (!spec.open(QIODevice::ReadOnly | QIODevice::Text))
        qFatal(qPrintable(QString("Can not open %1").arg(OPCODES_SPEC)));

    if (!readSpec(spec))
        qFatal("Error reading the specification");

    spec.close();
    qDebug() << "done";

    QFile opcodes(OPCODES_LIST);
    if (!opcodes.open(QIODevice::ReadOnly | QIODevice::Text))
        qFatal(qPrintable(QString("Can not open %1").arg(OPCODES_LIST)));

    QFile code(OPCODES_IMPL);
    if (!code.open(QIODevice::WriteOnly | QIODevice::Text))
        qFatal(qPrintable(QString("Can not open %1").arg(OPCODES_IMPL)));

    QFile header(OPCODES_HEADER);
    if (!header.open(QIODevice::WriteOnly | QIODevice::Text))
        qFatal(qPrintable(QString("Can not open %1").arg(OPCODES_HEADER)));

    generateCodeTable(opcodes, code, header);

    opcodes.close();
    code.close();
    header.close();
}

enum TokenType
{
    TT_OPCODE,
    TT_D,
    TT_E,
    TT_N,
    TT_NN,
    TT_END
};

enum Z80OperandType
{
    OP_NONE,
    OP_BYTE,
    OP_OFFSET,
    OP_WORD
};

const char* OpTypeName[] = { "OP_NONE", "OP_BYTE", "OP_OFFSET", "OP_WORD" };


int isHex(char c)
{
    return (isxdigit(c) && isupper(c)) || isdigit(c);
}

int hexVal(char c)
{
    if (isdigit(c))
        return c - '0';

    return c - 'A' + 10;
}

char *nextToken(char* cur, unsigned char *val, TokenType *ttype)
{
    *ttype = TT_END;

    while (*cur)
    {
        if (*cur == ' ') {
            cur++;
            continue;
        }
        else if (isHex(*cur) && isHex(*(cur+1))) {
            *val = (hexVal(*cur) << 4) | hexVal(*(cur+1));
            *ttype = TT_OPCODE;
            cur += 2;
            break;
        }
        else if (*cur == 'd') {
            *ttype = TT_D;
            cur++;
            break;
        }
        else if (*cur == 'e') {
            *ttype = TT_E;
            cur++;
            break;
        }
        else if ((*cur == 'n') && (*(cur+1) == 'n')) {
            *ttype = TT_NN;
            cur += 2;
            break;
        }
        else if (*cur == 'n') {
            *ttype = TT_N;
            cur++;
            break;
        }
        else {
            qFatal(qPrintable(QString("Unknown char at %1").arg(cur)));
        }
    }

    return cur;
}

Z80OpcodeTable* createTableTree(QFile &opcodes, QFile &table)
{
    Z80OpcodeTable *mainTable = new Z80OpcodeTable;
    mainTable->name = "";

    QTextStream out(&table);

//    out << "static struct Z80OpcodeTable opcodes_main;\n";
    out << "#include \"alu.h\"\n"
        << "using namespace Z80;\n\n";

    out << "#include \"opcodes_table.h\"\n\n";

    out << "void Alu::initOpcodeTables()\n{\n";
    out << "    opcodes_main = new OpcodeTable;\n";

    qDebug() << "Determining opcode prefixes...";

    unsigned char prefix[10];
    int prefixLen = 0;

    while (!opcodes.atEnd()) {
        QByteArray line = opcodes.readLine(MAX_LINE);

        // remove trailing line break
        line.chop(1);

        prefixLen = 0;
        line.truncate(OPCODE_OFFSET-1);

        bool wasOperand = false;
        int opcodeOffset = 0;
        char *cur = line.data();
        do {
            unsigned char code;
            TokenType tt;

            cur = nextToken(cur, &code, &tt);
            if (tt == TT_END) {
                break;
            } else if (tt == TT_OPCODE) {
                prefix[prefixLen++] = code;
                if (wasOperand) {
                    opcodeOffset++;
                }
            } else {
                wasOperand = true;
            }
        } while(1);

        Z80OpcodeTable *current = mainTable;
        for (int i = 0; i < prefixLen-1; ++i) {
            unsigned char code = prefix[i];

            if (!current->entries[code].table) {
                QString name = QString("%1%2").arg(current->name.data()).arg(code, 2, 16);
                current->entries[code].table = new Z80OpcodeTable;
                current = current->entries[code].table;
                current->name = name.toLatin1();
//                out << "static struct Z80OpcodeTable opcodes_" << name << ";\n";
                out << "    opcodes_" << name << " = new OpcodeTable;\n";

                qDebug() << name;
            } else {
                current = current->entries[code].table;
            }
        }

        current->opcode_offset = opcodeOffset;
    }

    qDebug() << "\n";

    mainTable->name = "main";
    return mainTable;
}

void scanOpcodes(QFile &opcodes, Z80OpcodeTable *mainTable)
{
    opcodes.seek(0);

    while (!opcodes.atEnd()) {
        QByteArray line = opcodes.readLine(MAX_LINE);

        // remove trailing line break
        line.chop(1);

        QByteArray name = fixName(line.mid(OPCODE_OFFSET));
        QByteArray fmt = mkFormat(line.mid(OPCODE_OFFSET));

        line.truncate(OPCODE_OFFSET-1);

        Z80OpcodeEntry *ent;
        Z80OpcodeTable *current = mainTable;
        char *cur = line.data();
        unsigned char code;
        TokenType tt;
        Z80OperandType opType = OP_NONE;
        do {
            cur = nextToken(cur, &code, &tt);
            if (tt == TT_END) {
                break;
            }
            else if (tt == TT_OPCODE) {
                if (current->entries[code].table) {
                    current = current->entries[code].table;
                    continue;
                }

                ent = &current->entries[code];
                ent->func = "&Z80::Alu::" + name;
                ent->format = fmt;
            }
            else if (tt == TT_NN) {
                opType = OP_WORD;
            }
            else if (tt == TT_N || tt == TT_D) {
                opType = OP_BYTE;
            }
            else if (tt == TT_E) {
                opType = OP_OFFSET;
            }
        } while(1);

        ent->operand_type = (int)opType;
    }
}

void outputTable(Z80OpcodeTable *table, QFile &file)
{
    qDebug() << "Outputting table" << table->name << "...";

    QTextStream out(&file);

//    out << "static struct Z80OpcodeTable opcodes_" << table->name << " = { " << table->opcode_offset << ", {\n";
    out << "    *opcodes_" << table->name << " = (OpcodeTable){ " << table->opcode_offset << ", {\n";

    int i;
    Z80OpcodeEntry *opc;
    Z80OpcodeTable *tbl;

    for (i = 0, opc = table->entries; i < 256; ++i, ++opc) {
        tbl = opc->table;

        QByteArray fmt = "NULL";
        if (!opc->format.isNull()) {
            fmt = "\"" + opc->format + "\"";
        }

//        QString o = QString("\t{ %1, %2, %3, %4%5 }%6\n")
//                .arg(!opc->func.isEmpty() ? opc->func.leftJustified(20).data() : QString("NULL").leftJustified(20),
//                     QString(OpTypeName[opc->operand_type]).leftJustified(9),
//                     fmt.leftJustified(20).data(),
//                     tbl ? "&opcodes_" : "",
//                     tbl ? tbl->name.data() : "NULL",
//                     i == 255 ? "" : ",");
        QString o = QString("\t\t{ %1, %2, %3, %4%5 }%6\n")
                .arg(!opc->func.isEmpty() ? opc->func.leftJustified(31).data() : QString("NULL").leftJustified(31),
                     QString(OpTypeName[opc->operand_type]).leftJustified(9),
                     fmt.leftJustified(20).data(),
                     tbl ? "opcodes_" : "",
                     tbl ? tbl->name.data() : "NULL",
                     i == 255 ? "" : ",");
        out << o;
    }

    out << "    } };\n";
    out << "\n\n";
    out.flush();

    qDebug() << "done";

    for (i = 0, opc = table->entries; i < 256; ++i, ++opc)
    {
        tbl = opc->table;
        if (tbl)
            outputTable(tbl, file);
    }
}

void generateParserTables(QFile &opcodes, QFile &table)
{
    Z80OpcodeTable* mainTable = createTableTree(opcodes, table);
    scanOpcodes(opcodes, mainTable);

    table.write("\n\n");

    outputTable(mainTable, table);

    table.write("}\n");
}

void generateParser()
{
    QFile opcodes(OPCODES_LIST);
    if (!opcodes.open(QIODevice::ReadOnly | QIODevice::Text))
        qFatal(qPrintable(QString("Can not open %1").arg(OPCODES_LIST)));

    QFile table(OPCODES_TABLE);
    if (!table.open(QIODevice::WriteOnly | QIODevice::Text))
        qFatal(qPrintable(QString("Can not open %1").arg(OPCODES_TABLE)));

    generateParserTables(opcodes, table);

    opcodes.close();
    table.close();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    generateCode();
    generateParser();

//    return app.exec();
}
