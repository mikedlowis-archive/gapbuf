/* Utility Functions
 *****************************************************************************/
/* Memory-mapped file representation */
typedef struct {
    uint8_t* buf; /* memory mapped byte buffer */
    size_t len;   /* length of the buffer */
} FMap;

/* Buffer management functions
 *****************************************************************************/
/* undo/redo list item */
typedef struct Log {
    struct Log* next;   /* pointer to next operation in the stack */
    bool insert;        /* whether this operation was an insert or delete */
    uint transid;       /* transaction id used to group related edits together */
    union {
        struct {
            size_t beg; /* offset in the file where insertion started */
            size_t end; /* offset in the file where insertion ended */
        } ins;
        struct {
            size_t off;  /* offset in the file where the deletion occurred */
            size_t len;  /* number of runes deleted */
            Rune* runes; /* array of runes containing deleted content */
        } del;
    } data;
} Log;

/* gap buffer main data structure */
typedef struct {
    char* path;           /* the path to the open file */
    uint64_t modtime;     /* modification time of the opened file */
    int crlf;             /* tracks whether the file uses dos style line endings */
    size_t bufsize;       /* size of the buffer in runes */
    char* bufstart;       /* start of the data buffer */
    char* bufend;         /* end of the data buffer */
    char* gapstart;       /* start of the gap */
    char* gapend;         /* end of the gap */
    Log* undo;            /* undo list */
    Log* redo;            /* redo list */
    bool modified;        /* tracks whether the buffer has been modified */
    bool expand_tabs;     /* tracks current mode */
    uint transid;         /* tracks the last used transaction id for log entries */
    void (*errfn)(char*); /* callback for error messages */
} Buf;

/* cursor/selection representation */
typedef struct {
    size_t beg;
    size_t end;
    size_t col;
} Sel;

enum {
    LEFT  = -1,
    RIGHT = +1,
    UP    = -1,
    DOWN  = +1
};

void buf_init(Buf* buf, void (*errfn)(char*));
void buf_load(Buf* buf, Sel* sel, char* path);
void buf_reload(Buf* buf);
void buf_save(Buf* buf);

Rune buf_getc(Buf* buf, Sel* sel);
void buf_putc(Buf* buf, Sel* sel, Rune rune, int fmtopts);
void buf_last(Buf* buf, Sel* sel);

#if 0
Rune buf_get(Buf* buf, size_t pos);
size_t buf_end(Buf* buf);
size_t buf_insert(Buf* buf, bool indent, size_t off, Rune rune);
size_t buf_delete(Buf* buf, size_t beg, size_t end);
size_t buf_change(Buf* buf, size_t beg, size_t end);

void buf_chomp(Buf* buf);
void buf_undo(Buf* buf, Sel* sel);
void buf_redo(Buf* buf, Sel* sel);
void buf_loglock(Buf* buf);
void buf_logclear(Buf* buf);
bool buf_iseol(Buf* buf, size_t pos);
size_t buf_bol(Buf* buf, size_t pos);
size_t buf_eol(Buf* buf, size_t pos);
size_t buf_bow(Buf* buf, size_t pos);
size_t buf_eow(Buf* buf, size_t pos);
size_t buf_lscan(Buf* buf, size_t pos, Rune r);
size_t buf_rscan(Buf* buf, size_t pos, Rune r);
void buf_getword(Buf* buf, bool (*isword)(Rune), Sel* sel);
void buf_getblock(Buf* buf, Rune beg, Rune end, Sel* sel);
size_t buf_byrune(Buf* buf, size_t pos, int count);
size_t buf_byword(Buf* buf, size_t pos, int count);
size_t buf_byline(Buf* buf, size_t pos, int count);
void buf_findstr(Buf* buf, int dir, char* str, size_t* beg, size_t* end);
void buf_lastins(Buf* buf, size_t* beg, size_t* end);
size_t buf_setln(Buf* buf, size_t line);
size_t buf_getln(Buf* buf, size_t off);
size_t buf_getcol(Buf* buf, size_t pos);
size_t buf_setcol(Buf* buf, size_t pos, size_t col);
#endif

