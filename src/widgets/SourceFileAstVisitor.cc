#include "SourceFileAstVisitor"

namespace astgdb {

bool SourceFileAstVisitor::VisitCompoundStmt(CompoundStmt *S) {
  SourceLocation LBraceLoc =	S->getLBracLoc();
  SourceLocation RBraceLoc =	S->getRBracLoc();
  LBraceNode *lbnode = new BraceNode(S,
      SourceMgr->getSpellingLineNumber(LBraceLoc),
      SourceMgr->getSpellingColumnNumber(LBraceLoc));
  RBraceNode *rbnode = new BraceNode(S,
     SourceMgr->getSpellingLineNumber(RBraceLoc),
     SourceMgr->getSpellingColumnNumber(RBraceLoc));
  lbnode->set_pair(rbnode);
  rbnode->set_pair(lbnode);
  nodes->push_back(lbnode);
  nodes->push_back(rbnode);
  return true;
}

} //namespace astgdb
