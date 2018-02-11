#ifndef __src_node_h__
#define __src_node_h__

namespace astgdb {

typedef Coord4 std::tuple<unsigned, unsigned, unsigned, unsigned>;
typedef Coord2 std::tuple<unsigned, unsigned>;

enum class Color {
  Default = 0,
  Red
};

class CharPlaceStyle {
  Color fg;
  Color bg;
  bool bold;
};

class SrcNode {
public:
  virtual Coord4 get_uv(void) = 0;
  virtual void click(unsigned u, unsigned v) {};
  virtual CharPlaceStyle *get_style(void) {
    return nullptr;
  }
};


class PairCollapsedNode: public SrcNode {
public:
  LBraceNode(CompoundStmt *S, unsigned u, unsigned v, SourceViewer *SrcViewer):
    S(S), u(u), v(v), SrcViewer(SrcViewer);
    
  void set_pair(BraceNode *braceNode) {
    pair_brace = braceNode;
    IsLeftBrace = (u<pair_brace->u || (u==pair_brace->u && v < pair_brace->v));
  }
    
  Coord4 get_uv(void) {
    return {u,v,u,v+1};
  }
  
  void click(unsigned u, unsigned v) {
    if (collapsed) {
      SrcViewer->RmOffsetOp(OffsetOpId);
    }
    else {
      Coord2 delta = get_delta_uv();
      Coord4 area = get_area_uv();
      OffsetOpId = SrcViewer->AddOffsetOp(area, delta);
    }
    collapsed = !collapsed
  }
  
  Coord2 get_delta_uv(void) {
    return IsLeftBrace ? {u - pair_brace->u, v - pair_brace->v} :
      {pair_brace->u - u, pair_brace->v - v};
  }
  
  Coord4 get_area_uv(void) {
    if (IsLeftBrace) {
      return {u, v, pair_brace->u, pair_brace->v+1};
    }
    else {
      return {pair_brace->u, pair_brace->v, u, v+1};
    }
  }
  
protected:
    bool IsLeftBrace;
    unsigned u,v;
    CompoundStmt S;
    BraceNode *pair_brace;
    bool collapsed {false};
    unsigned OffsetOpId;
    SourceViewer *SrcViewer;
};

class BracketNode: public PairCollapsedNode {
public:
  BracketNode(CompoundStmt *S, unsigned u, unsigned v, SrcViewer):
    PairCollapsedNode(S,u,v,SrvViewer) {}
  };
} /// namespace astgdb
#endif
