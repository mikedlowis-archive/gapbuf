#define INCLUDE_DEFS
#include <atf.h>
#include <stdc.h>
#include <utf.h>
#include <edit.h>
#include <unistd.h>

extern int CfgTabWidth;
extern int CfgExpandTabs;
extern int CfgCopyIndent;


static Buf TestBuf;

static void onerror(char* msg) {
}

static void set_buffer_text(char* str) {
    int i = 0;
    buf_init(&TestBuf, onerror);
#if 0
    TestBuf.crlf = 1;
    for (Rune* curr = TestBuf.bufstart; curr < TestBuf.bufend; curr++)
        *curr = '-';
    while (*str)
        buf_putc(&TestBuf, false, i++, (Rune)*str++);
#endif
}

static bool buf_text_eq(char* str) {
#if 0
    for (unsigned i = 0; i < buf_last(&TestBuf); i++) {
        //printf("'%c'", buf_getc(&TestBuf, i));
        if ((Rune)*(str++) != buf_getc(&TestBuf, i)) {
            //printf("\n");
            return false;
        }
    }
    //printf("\n");
#endif
    return true;
}

TEST_SUITE(BufferTests) {
    /* Initializing
     *************************************************************************/
    TEST(buf_init should initialize an empty buffer) {
        Buf buf = {0};
        buf_init(&buf, (void*)0x12345678);
        CHECK(buf.modified    == false);
        CHECK(buf.expand_tabs == CfgExpandTabs);
        CHECK(buf.crlf        == 0);
        CHECK(buf.bufsize     == sysconf(_SC_PAGE_SIZE));
        CHECK(buf.bufstart    != NULL);
        CHECK(buf.bufend      == buf.bufstart + buf.bufsize);
        CHECK(buf.gapstart    == buf.bufstart);
        CHECK(buf.gapend      == buf.bufend);
        CHECK(buf.undo        == NULL);
        CHECK(buf.redo        == NULL);
        CHECK(buf.errfn       == (void*)0x12345678);
    }

    TEST(buf_init shoud free old buffer and reinitialize) {
        Buf buf = {0};
        buf_init(&buf, onerror);
        buf_putc(&buf, false, 0, 'a');
        buf_init(&buf, (void*)0x12345678);
        CHECK(buf.modified    == false);
        CHECK(buf.expand_tabs == CfgExpandTabs);
        CHECK(buf.crlf        == 0);
        CHECK(buf.bufsize     == sysconf(_SC_PAGE_SIZE));
        CHECK(buf.bufstart    != NULL);
        CHECK(buf.bufend      == buf.bufstart + buf.bufsize);
        CHECK(buf.gapstart    == buf.bufstart);
        CHECK(buf.gapend      == buf.bufend);
        CHECK(buf.undo        == NULL);
        CHECK(buf.redo        == NULL);
        CHECK(buf.errfn       == (void*)0x12345678);
    }

    /* Loading
     *************************************************************************/
    TEST(buf_load should load a UTF-8 file from disk) {
        Sel sel;
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, NULL, "testdocs/lorem.txt");
        CHECK(sel.end             == 0);
        CHECK(TestBuf.modified    == false);
        CHECK(TestBuf.expand_tabs == true);
        CHECK(TestBuf.crlf        == 0);
        CHECK(TestBuf.bufsize     == 61440);
        CHECK(TestBuf.undo        == NULL);
        CHECK(TestBuf.redo        == NULL);
        CHECK(TestBuf.errfn       == NULL);
        CHECK(!strcmp(TestBuf.path, "testdocs/lorem.txt"));
    }

    TEST(buf_load should load a file from disk and jump to a specific line) {
        IGNORE("Jumping to a specific line is not implemented yet.");
        Sel sel;
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, &sel, "testdocs/lorem.txt:2");
        CHECK(sel.end             == 70);
        CHECK(TestBuf.modified    == false);
        CHECK(TestBuf.expand_tabs == true);
        CHECK(TestBuf.crlf        == 0);
        CHECK(TestBuf.bufsize     == 61440);
        CHECK(TestBuf.undo        == NULL);
        CHECK(TestBuf.redo        == NULL);
        CHECK(TestBuf.errfn       == NULL);
        CHECK(!strcmp(TestBuf.path, "testdocs/lorem.txt"));
    }

    TEST(buf_load should remove ./ from file path) {
        Sel sel;
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, &sel, "./testdocs/lorem.txt");
        CHECK(sel.end             == 0);
        CHECK(TestBuf.modified    == false);
        CHECK(TestBuf.expand_tabs == true);
        CHECK(TestBuf.crlf        == 0);
        CHECK(TestBuf.bufsize     == 61440);
        CHECK(TestBuf.undo        == NULL);
        CHECK(TestBuf.redo        == NULL);
        CHECK(TestBuf.errfn       == NULL);
        CHECK(!strcmp(TestBuf.path, "testdocs/lorem.txt"));
    }

    TEST(buf_reload should reload the file from disk) {
        Sel sel;
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, &sel, "testdocs/waf");
        TestBuf.path = "testdocs/lorem.txt";
        buf_reload(&TestBuf);
        CHECK(sel.end             == 0);
        CHECK(TestBuf.modified    == false);
        CHECK(TestBuf.expand_tabs == true);
        CHECK(TestBuf.crlf        == 0);
        CHECK(TestBuf.bufsize     == 61440);
        CHECK(TestBuf.undo        == NULL);
        CHECK(TestBuf.redo        == NULL);
        CHECK(TestBuf.errfn       == NULL);
        CHECK(!strcmp(TestBuf.path, "testdocs/lorem.txt"));
    }

    TEST(buf_load should handle non-existent paths) {
        Sel sel;
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, &sel, "nonexistent.txt");
        buf_reload(&TestBuf);
        CHECK(sel.end             == 0);
        CHECK(TestBuf.modified    == false);
        CHECK(TestBuf.expand_tabs == true);
        CHECK(TestBuf.crlf        == 0);
        CHECK(TestBuf.bufsize     == sysconf(_SC_PAGE_SIZE));
        CHECK(TestBuf.undo        == NULL);
        CHECK(TestBuf.redo        == NULL);
        CHECK(TestBuf.errfn       == NULL);
        CHECK(!strcmp(TestBuf.path, "nonexistent.txt"));
    }

#if 0
    /* Saving
     *************************************************************************/
    TEST(buf_save should save a UTF-8 file to disk) {
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, "testdocs/lorem.txt");
        TestBuf.modified = true;
        buf_save(&TestBuf);
        CHECK(TestBuf.modified == false);
    }

    TEST(buf_save should save a non UTF-8 file to disk) {
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, "testdocs/waf");
        TestBuf.modified = true;
        buf_save(&TestBuf);
        CHECK(TestBuf.modified == false);
    }

    TEST(buf_save should save a file to disk with unix line endings) {
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, "testdocs/lf.txt");
        TestBuf.modified = true;
        buf_save(&TestBuf);
        CHECK(TestBuf.modified == false);
    }

    TEST(buf_save should save a file to disk with dos line endings) {
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, "testdocs/crlf.txt");
        TestBuf.modified = true;
        buf_save(&TestBuf);
        CHECK(TestBuf.modified == false);
    }

    TEST(buf_save should make sure unix file ends witn newline) {
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, "testdocs/lf.txt");
        TestBuf.modified = true;
        size_t end = buf_last(&TestBuf);
        buf_delete(&TestBuf, end-1, end);
        CHECK(end-1 == buf_last(&TestBuf));
        buf_save(&TestBuf);
        CHECK(end == buf_last(&TestBuf));
        CHECK(TestBuf.modified == false);
    }

    TEST(buf_save should make sure dos file ends witn newline) {
        buf_init(&TestBuf, NULL);
        buf_load(&TestBuf, "testdocs/crlf.txt");
        TestBuf.modified = true;
        size_t end = buf_last(&TestBuf);
        buf_delete(&TestBuf, end-1, end);
        CHECK(end-1 == buf_last(&TestBuf));
        buf_save(&TestBuf);
        CHECK(end == buf_last(&TestBuf));
        CHECK(TestBuf.modified == false);
    }

    /* Insertions
     *************************************************************************/
    TEST(buf_putc should insert at 0 in empty buf) {
        buf_init(&TestBuf, onerror);
        buf_putc(&TestBuf, false, 0, 'a');
        CHECK(buf_text_eq("a"));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    TEST(buf_putc should insert at 0) {
        buf_init(&TestBuf, onerror);
        buf_putc(&TestBuf, false, 0, 'b');
        buf_putc(&TestBuf, false, 0, 'a');
        CHECK(buf_text_eq("ab"));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    TEST(buf_putc should insert at 1) {
        buf_init(&TestBuf, onerror);
        buf_putc(&TestBuf, false, 0, 'a');
        buf_putc(&TestBuf, false, 1, 'b');
        CHECK(buf_text_eq("ab"));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    TEST(buf_putc should insert at 1) {
        buf_init(&TestBuf, onerror);
        buf_putc(&TestBuf, false, 0, 'a');
        buf_putc(&TestBuf, false, 1, 'c');
        buf_putc(&TestBuf, false, 1, 'b');
        CHECK(buf_text_eq("abc"));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    TEST(buf_putc should sentence in larger text) {
        set_buffer_text(
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit." );
        buf_putc(&TestBuf, false, 5, ' ');
        buf_putc(&TestBuf, false, 6, 'a');
        CHECK(buf_text_eq(
            "Lorem a ipsum dolor sit amet, consectetur adipiscing elit." ));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    TEST(buf_putc should expand tabs) {
        set_buffer_text("");
        TestBuf.expand_tabs = true;
        buf_putc(&TestBuf, true, 0, '\t');
        CHECK(buf_text_eq("    "));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    TEST(buf_putc should copy indent) {
        set_buffer_text("    ");
        TestBuf.crlf = 0;
        buf_putc(&TestBuf, true, 4, '\n');
        CHECK(buf_text_eq("    \n    "));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    /* Deletions
     *************************************************************************/
    TEST(buf_delete should delete first char) {
        set_buffer_text("abc");
        buf_delete(&TestBuf, 0, 1);
        CHECK(buf_text_eq("bc"));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    TEST(buf_delete should delete second char) {
        set_buffer_text("abc");
        buf_delete(&TestBuf, 1, 2);
        CHECK(buf_text_eq("ac"));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    TEST(buf_delete should delete third char) {
        set_buffer_text("abc");
        buf_delete(&TestBuf, 2, 3);
        CHECK(buf_text_eq("ab"));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    TEST(buf_delete should delete more than one char) {
        set_buffer_text("abcdef");
        buf_delete(&TestBuf, 1, 5);
        CHECK(buf_text_eq("af"));
        CHECK(TestBuf.modified == true);
        CHECK(TestBuf.redo == NULL);
    }

    /* Undo/Redo
     *************************************************************************/
    TEST(buf_undo should undo an insert) {
        Sel sel;
        set_buffer_text("");
        buf_putc(&TestBuf, true, 0, 'a');
        CHECK(buf_text_eq("a"));
        CHECK(TestBuf.redo == NULL);
        CHECK(TestBuf.undo != NULL);
        buf_undo(&TestBuf, &sel);
        CHECK(buf_text_eq(""));
        CHECK(TestBuf.redo != NULL);
        CHECK(TestBuf.undo == NULL);
    }

    TEST(buf_undo should undo a delete) {
        Sel sel;
        set_buffer_text("a");
        buf_delete(&TestBuf, 0, 1);
        CHECK(buf_text_eq(""));
        CHECK(TestBuf.redo == NULL);
        CHECK(TestBuf.undo != NULL);
        buf_undo(&TestBuf, &sel);
        CHECK(buf_text_eq("a"));
        CHECK(TestBuf.redo != NULL);
        CHECK(TestBuf.undo != NULL);
    }

    /* Accessors
     *************************************************************************/
    // buf_getc
    TEST(buf_getc should return newline for indexes outside the buffer) {
        set_buffer_text("test");
        CHECK('\n' == buf_getc(&TestBuf, 4));
    }

    TEST(buf_getc should indexed character before the gap) {
        set_buffer_text("ac");
        buf_putc(&TestBuf, false, 1, 'b');
        CHECK('a' == buf_getc(&TestBuf, 0));
    }

    TEST(buf_getc should indexed character after the gap) {
        set_buffer_text("ac");
        buf_putc(&TestBuf, false, 1, 'b');
        CHECK('c' == buf_getc(&TestBuf, 2));
    }

    TEST(buf_last should return the index just after the last rune in the buffer) {
        set_buffer_text("abc");
        CHECK(3 == buf_last(&TestBuf));
    }

    TEST(buf_iseol should return true if index points to a newline rune) {
        set_buffer_text("abc\ncba");
        CHECK(buf_iseol(&TestBuf, 3));
    }

    TEST(buf_iseol should return true if index points to a crlf rune) {
        IGNORE("Test causes an assertion in the syncgap function. no idea why");
        set_buffer_text("abc\r\ncba");
        CHECK(buf_iseol(&TestBuf, 0));
    }

    TEST(buf_iseol should return false if index does not point to a newline rune) {
        set_buffer_text("abc\ncba");
        CHECK(!buf_iseol(&TestBuf, 2));
    }

    /* Movements
     *************************************************************************/
    // Start of Line
    TEST(buf_bol should return 0 if column 1 of first line) {
        set_buffer_text("ab\ncd");
        CHECK(0 == buf_bol(&TestBuf, 2));
    }

    TEST(buf_bol should return 0 if column 2 of first line) {
        set_buffer_text("ab\ncd");
        CHECK(0 == buf_bol(&TestBuf, 1));
    }

    TEST(buf_bol should return 0 if index points to newline) {
        set_buffer_text("ab\ncd");
        CHECK(0 == buf_bol(&TestBuf, 0));
    }

    TEST(buf_bol should return 3 if column 1 of second line) {
        set_buffer_text("ab\ncd");
        CHECK(3 == buf_bol(&TestBuf, 3));
    }

    TEST(buf_bol should return 3 if column 2 of second line) {
        set_buffer_text("ab\ncd");
        CHECK(3 == buf_bol(&TestBuf, 4));
    }

    TEST(buf_bol should return input if index is outside buffer) {
        set_buffer_text("ab\ncd");
        CHECK(6 == buf_bol(&TestBuf, 6));
    }

    // End of Line
    TEST(buf_eol should return 2 if column 1 of first line) {
        set_buffer_text("ab\ncd");
        CHECK(2 == buf_eol(&TestBuf, 0));
    }

    TEST(buf_eol should return 2 if column 2 of first line) {
        set_buffer_text("ab\ncd");
        CHECK(2 == buf_eol(&TestBuf, 1));
    }

    TEST(buf_eol should return 2 if column 3 of first line) {
        set_buffer_text("ab\ncd");
        CHECK(2 == buf_eol(&TestBuf, 2));
    }

    TEST(buf_eol should return 5 if column 1 of second line) {
        set_buffer_text("ab\ncd");
        CHECK(5 == buf_eol(&TestBuf, 3));
    }

    TEST(buf_eol should return 5 if column 2 of second line) {
        set_buffer_text("ab\ncd");
        CHECK(5 == buf_eol(&TestBuf, 4));
    }

    TEST(buf_eol should return 5 if column 3 of second line) {
        set_buffer_text("ab\ncd");
        CHECK(5 == buf_eol(&TestBuf, 5));
    }

    // Start of Word
    TEST(buf_bow should return input when pointing to whitespace before word) {
        set_buffer_text(" abc ");
        CHECK(0 == buf_bow(&TestBuf, 0));
    }

    TEST(buf_bow should return 1 when first rune of word) {
        set_buffer_text(" abc ");
        CHECK(1 == buf_bow(&TestBuf, 1));
    }

    TEST(buf_bow should return 1 when second rune of word) {
        set_buffer_text(" abc ");
        CHECK(1 == buf_bow(&TestBuf, 2));
    }

    TEST(buf_bow should return 1 when third rune of word) {
        set_buffer_text(" abc ");
        CHECK(1 == buf_bow(&TestBuf, 3));
    }

    TEST(buf_bow should return input when pointing to whitespace after word) {
        IGNORE("this may be correct but moving by word is janky right now. revisit later");
        set_buffer_text(" abc ");
        CHECK(4 == buf_bow(&TestBuf, 4));
    }

    // End of Word
    TEST(buf_eow should return input when pointing to whitespace before word) {
        IGNORE("this may be correct but moving by word is janky right now. revisit later");
        set_buffer_text(" abc ");
        CHECK(0 == buf_eow(&TestBuf, 0));
    }

    TEST(buf_eow should return 3 when first rune of word) {
        set_buffer_text(" abc ");
        CHECK(3 == buf_eow(&TestBuf, 1));
    }

    TEST(buf_eow should return 3 when second rune of word) {
        set_buffer_text(" abc ");
        CHECK(3 == buf_eow(&TestBuf, 2));
    }

    TEST(buf_eow should return 3 when third rune of word) {
        set_buffer_text(" abc ");
        CHECK(3 == buf_eow(&TestBuf, 3));
    }

    TEST(buf_eow should return input when pointing to whitespace after word) {
        IGNORE("this may be correct but moving by word is janky right now. revisit later");
        set_buffer_text(" abc ");
        CHECK(4 == buf_eow(&TestBuf, 4));
    }

    // Scan Left
    TEST(buf_lscan should return location of token to the left) {
        set_buffer_text("a{bc}");
        CHECK(1 == buf_lscan(&TestBuf, 4, '{'));
    }

    TEST(buf_lscan should return input location if token not found) {
        set_buffer_text("{ab}");
        CHECK(3 == buf_lscan(&TestBuf, 3, '['));
    }

    // Scan Right
    TEST(buf_rscan should return location of token to the right) {
        set_buffer_text("{ab}c");
        CHECK(3 == buf_rscan(&TestBuf, 0, '}'));
    }

    TEST(buf_rscan should return input location if token not found) {
        set_buffer_text("{ab}c");
        CHECK(0 == buf_rscan(&TestBuf, 0, ']'));
    }

    // By Rune
    TEST(buf_byrune should do nothing for -1 at beginning of file) {
        set_buffer_text("abc\n");
        CHECK(0 == buf_byrune(&TestBuf, 0, -1));
    }

    TEST(buf_byrune should move to first rune for -1 at second rune of file) {
        set_buffer_text("abc\n");
        CHECK(0 == buf_byrune(&TestBuf, 1, -2));
    }

    TEST(buf_byrune should move to just after last rune for +1 at end of file) {
        set_buffer_text("abc\n");
        CHECK(4 == buf_byrune(&TestBuf, 3, 2));
    }

    TEST(buf_byrune should move to just after last rune for +2 at second to last rune) {
        set_buffer_text("abc\n");
        CHECK(4 == buf_byrune(&TestBuf, 2, 3));
    }

    TEST(buf_byrune should move from blank line to non-blank line for +1) {
        set_buffer_text("ab\n\ncd\n");
        CHECK(4 == buf_byrune(&TestBuf, 3, 1));
    }

    // By Line
    TEST(buf_byline should not move before first line) {
        set_buffer_text("ab\n\ncd\n");
        CHECK(0 == buf_byline(&TestBuf, 0, -1));
    }

    TEST(buf_byline should not move before first line) {
        set_buffer_text("a\nb\nc\nd\n");
        CHECK(0 == buf_byline(&TestBuf, 7, -10));
    }

    TEST(buf_byline should move back multiple lines) {
        set_buffer_text("a\nb\nc\nd\n");
        CHECK(2 == buf_byline(&TestBuf, 7, -2));
    }

    TEST(buf_byline should move back a line) {
        set_buffer_text("abc\ndef");
        CHECK(0 == buf_byline(&TestBuf, 4, -1));
    }

    TEST(buf_byline should move forward a line) {
        set_buffer_text("abc\ndef");
        CHECK(4 == buf_byline(&TestBuf, 2, 1));
    }

    TEST(buf_byline should not move after last line) {
        set_buffer_text("abc\ndef");
        CHECK(6 == buf_byline(&TestBuf, 6, 1));
    }

    TEST(buf_byline should do nothing at end of buffer) {
        set_buffer_text("abc\ndef");
        CHECK(7 == buf_byline(&TestBuf, buf_last(&TestBuf), 1));
    }

    /* Cursor Column Tracking
     *************************************************************************/
    TEST(buf_getccol should return the column associated with the position) {
        set_buffer_text("abcdef");
        CHECK(4 == buf_getccol(&TestBuf, 4));
    }

    TEST(buf_getccol should return the column associated with the position on second line) {
        set_buffer_text("abcdef\nabcdef");
        CHECK(0 == buf_getccol(&TestBuf, 7));
    }

    TEST(buf_getccol should handle tab characters) {
        set_buffer_text("\tabcdef");
        CHECK(4 == buf_getccol(&TestBuf, 1));
    }

    TEST(buf_setcol should set the column to column 1 of second line) {
        set_buffer_text("abc\ndef");
        CHECK(4 == buf_setcol(&TestBuf, 4, 0));
    }

    TEST(buf_setcol should set the column to column 2 of second line) {
        set_buffer_text("abc\ndef");
        CHECK(5 == buf_setcol(&TestBuf, 4, 1));
    }

    TEST(buf_setcol should handle tabs) {
        set_buffer_text("abc\n\tdef");
        CHECK(5 == buf_setcol(&TestBuf, 4, 4));
    }

    TEST(buf_setcol should not set column past the last rune) {
        set_buffer_text("abc\n\tdef");
        CHECK(8 == buf_setcol(&TestBuf, 4, 100));
    }
#endif
}

int main(int argc, char** argv) {
    atf_init(argc,argv);
    RUN_EXTERN_TEST_SUITE(BufferTests);
    return atf_print_results();
}

