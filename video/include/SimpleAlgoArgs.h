#ifndef SIMPLEALGOARGS_H
#define SIMPLEALGOARGS_H
#define MAXRECTCNT 8
typedef struct {
    int x;
    int y;
    int width;
    int height;
}T_Rect,*PT_Rect;

typedef struct{
    T_Rect   roiRect;
    int      redLines;
    T_Rect   redRects[MAXRECTCNT];
    int      blackLines;
    T_Rect   blackRects[MAXRECTCNT];

}AlgResData;

#endif // SIMPLEALGOARGS_H
