#ifndef __srcview_h__
#define __srcview_h__

namespace astgdb {

class SourceViewer {
  private:
    std::vector<std::stack> wz_to_uv_ops;
    ASTUnit *ast;
    StringRef fileBuffer;
    unsigned offset_x;
    unsigned offset_y;
  
    /// map some ast nodes to source
    void extractSrcNodes(void);
    
    /// y,x is display coordinates
    /// w,z is coords in source file (with collapsed braces)
    /// u,v is coords in original source file
    std::pair<unsigned, unsigned> yx_to_wz(unsigned y, unsigned x) {
      return {y+offset_y, x+offset_x};
    }
    std::pair<unsigned, unsigned> wz_to_uv(unsigned w, unsigned z);
    std::pair<unsigned, unsigned> yx_to_uv(unsigned y, unsigned x);

  public:
    SourceView (StringRef fileBuffer, ASTUnit *ast):
      ast(ast),
      fileBuffer(fileBuffer);
    
    /// y,x is display coordinates
    SrcNode *get_SrcNode(unsigned y, unsigned x);
    
    // char, fg, bg, bold
    std::tuple<int,int,int,bool> getChar(unsigned y, unsigned x);

    void mouseover(unsigned y, unsigned x);
    
    void click(unsigned y, unsigned x);

    void scroll_up(unsigned y, unsigned x);

    void scroll_down(unsigned y, unsigned x);

    void draw(DrawBuffer *buf, unsigned lines, unsigned cols);
};

}//namespace astgdb

#endif
